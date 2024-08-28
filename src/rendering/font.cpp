#include "font.hpp"
#include "glm/ext/vector_float2.hpp"
#include "opengl.hpp"
#include <freetype/freetype.h>
#include <freetype/ftmodapi.h>
#include <spdlog/spdlog.h>
#include <stb_rect_pack.h>
#include <unordered_set>

Font::Font(std::filesystem::path path, float size)
    : m_Path(path), m_Size(size) {
    FT_Error error = 0;

    error = FT_Init_FreeType(&m_Lib);
    if (error) {
        FATAL("Failed to initialize freetype");
    }

    error = FT_New_Face(m_Lib, path.c_str(), 0, &m_Font);
    if (error) {
        FATAL("Failed to load font '{}'", path.c_str());
    }

    error = FT_Set_Char_Size(m_Font, 0, size * 64, 0, 0);
}

Font::~Font() {
    glCall(glDeleteTextures(1, &m_AtlasId));
    for (auto& g : m_CodepointToGeometry) {
        std::free(g.second.bitmap);
    }
    FT_Done_Face(m_Font);
    FT_Done_Library(m_Lib);
    SPDLOG_DEBUG("Destroyed font '{}'", m_Path.c_str());
}

void Font::updateAtlas(std::unordered_set<Codepoint>& codepoints) {
    FT_Error error = 0;

    // Filter out already rendered glyphs
    std::unordered_set<Codepoint> newCodepoints;
    for (Codepoint c : codepoints) {
        if (!m_CodepointToGeometry.contains(c)) {
            newCodepoints.insert(c);
        }
    }
    if (newCodepoints.empty()) {
        return;
    }

    size_t numGlyphs = m_CodepointToGeometry.size() + newCodepoints.size();
    size_t rectIndex = m_CodepointToGeometry.size();
    // We have to pass an array of rects to stbrp_pack_rects later, so we can't just make a Codepoint->stbrp_rect map
    std::unordered_map<Codepoint, size_t> codepointToRectIndex;
    std::unordered_map<Codepoint, GlyphGeometry> codepointToGeometry;

    for (Codepoint c : newCodepoints) {
        FT_UInt glyphIndex = FT_Get_Char_Index(m_Font, c);

        error = FT_Load_Glyph(m_Font, glyphIndex, FT_LOAD_DEFAULT);
        if (error) {
            FATAL("Failed to load glyph '{}'", c);
        }

        error = FT_Render_Glyph(m_Font->glyph, FT_RENDER_MODE_NORMAL);

        m_StbRects[rectIndex] = stbrp_rect{
            .w = static_cast<stbrp_coord>(m_Font->glyph->bitmap.width),
            .h = static_cast<stbrp_coord>(m_Font->glyph->bitmap.rows)};
        codepointToRectIndex[c] = rectIndex;
        rectIndex++;

        size_t bitmapSize = sizeof(uint8_t) * m_Font->glyph->bitmap.rows *
                            m_Font->glyph->bitmap.width;
        GlyphGeometry glyph = {
            .metrics = m_Font->glyph->metrics,
            // Have to malloc, otherwise this pointer gets overwritten when loading next glyph
            .bitmap = (uint8_t*)std::malloc(bitmapSize),
        };
        std::memcpy(glyph.bitmap, m_Font->glyph->bitmap.buffer, bitmapSize);
        codepointToGeometry[c] = glyph;
    }

    if (m_AtlasId == 0) {
        stbrp_init_target(&m_StbContext, ATLAS_SIZE, ATLAS_SIZE, m_StbNodes,
                          numGlyphs);
    }
    const int success = stbrp_pack_rects(&m_StbContext, m_StbRects, numGlyphs);
    if (!success) {
        FATAL("Failed to calculate glyph packing");
    }
    SPDLOG_DEBUG("Calculated glyph packing, {} new glyphs",
                 newCodepoints.size());

    if (m_AtlasId == 0) {
        glCall(glGenTextures(1, &m_AtlasId));
        glCall(glBindTexture(GL_TEXTURE_2D, m_AtlasId));
        glCall(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        glCall(
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                               GL_CLAMP_TO_EDGE));
        glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                               GL_CLAMP_TO_EDGE));
        glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_SIZE, ATLAS_SIZE, 0,
                            GL_RED, GL_UNSIGNED_BYTE, nullptr));
    }

    for (auto& [codepoint, glyph] : codepointToGeometry) {
        glyph.rect = m_StbRects[codepointToRectIndex[codepoint]];
        if (!glyph.rect.was_packed) {
            FATAL("One or more glyphs weren't packed. Probably an issue with "
                  "pointers");
        }

        glCall(glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.rect.x, glyph.rect.y,
                               glyph.rect.w, glyph.rect.h, GL_RED,
                               GL_UNSIGNED_BYTE, glyph.bitmap));

        m_CodepointToGeometry[codepoint] = glyph;
    }

    SPDLOG_DEBUG("Generated {}x{} font atlas", ATLAS_SIZE, ATLAS_SIZE);
}

GlyphPos Font::getGlyphPos(Codepoint codepoint, glm::vec2& pen) {
    bool found = m_CodepointToGeometry.contains(codepoint);

    GlyphPos gp{};
    GlyphGeometry& gg = found ? m_CodepointToGeometry[codepoint]
                              : m_CodepointToGeometry[REPLACEMENT_CHAR];

    gp.al = gg.rect.x / (float)ATLAS_SIZE;
    gp.at = (gg.rect.y + gg.rect.h) / (float)ATLAS_SIZE;
    gp.ar = (gg.rect.x + gg.rect.w) / (float)ATLAS_SIZE;
    gp.ab = gg.rect.y / (float)ATLAS_SIZE;

    gp.pl = fracToPx(gg.metrics.horiBearingX);
    gp.pt = -fracToPx(gg.metrics.height - gg.metrics.horiBearingY),
    gp.pr = gp.pl + gg.rect.w;
    gp.pb = gp.pt + gg.rect.h;

    glm::vec2 advance(fracToPx(gg.metrics.horiAdvance), 0);
    if (codepoint == '\n') {
        pen.x = 0;
        advance.x = 0;
        advance.y = -fracToPx(m_Font->size->metrics.ascender);
        gp.pl = 0;
        gp.pt = 0;
        gp.pr = 0;
        gp.pb = 0;
    } else if (codepoint == '\t') {
        GlyphGeometry& space = m_CodepointToGeometry[' '];
        advance.x = fracToPx(space.metrics.horiAdvance) * TAB_WIDTH;
        gp.pl = 0;
        gp.pt = 0;
        gp.pr = 0;
        gp.pb = 0;
    }
    pen += advance;

    return gp;
}

const FT_Size_Metrics Font::getMetrics() const {
    return m_Font->size->metrics;
}

float Font::getSize() const {
    return m_Size;
}

double Font::fracToPx(double value) {
    return value / 64.0;
}
