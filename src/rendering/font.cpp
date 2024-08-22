#include "font.hpp"
#include "opengl.hpp"
#include <freetype/freetype.h>
#include <freetype/ftmodapi.h>
#include <spdlog/spdlog.h>
#include <stb_rect_pack.h>

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

// TODO: Create atlas with ASCII characters, then dynamically add more glyphs as needed
void Font::createAtlas() {
    const Codepoint startCodepoint = 0x20, endCodepoint = 0x7e;
    const size_t numGlyphs = endCodepoint - startCodepoint + 1;

    FT_Error error = 0;

    stbrp_context context;
    stbrp_node nodes[numGlyphs];

    // We have pass an array of rects to stbrp_pack_rects later, so we can't just make a Codepoint->stbrp_rect map
    stbrp_rect rects[numGlyphs];
    size_t rectIndex = 0;
    std::unordered_map<Codepoint, size_t> codepointToRectIndex;
    for (Codepoint c = startCodepoint; c <= endCodepoint; c++) {
        FT_UInt glyphIndex = FT_Get_Char_Index(m_Font, c);

        error = FT_Load_Glyph(m_Font, glyphIndex, FT_LOAD_DEFAULT);
        if (error) {
            FATAL("Failed to load glyph '{}'", c);
        }

        error = FT_Render_Glyph(m_Font->glyph, FT_RENDER_MODE_NORMAL);

        rects[rectIndex] = stbrp_rect{
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
        m_CodepointToGeometry[c] = glyph;
    }

    stbrp_init_target(&context, ATLAS_SIZE, ATLAS_SIZE, nodes, numGlyphs);
    const int success = stbrp_pack_rects(&context, rects, numGlyphs);
    if (!success) {
        FATAL("Failed to calculate glyph packing");
    }
    SPDLOG_DEBUG("Calculated glyph packing");

    if (m_AtlasId == 0) {
        glCall(glGenTextures(1, &m_AtlasId));
    }
    glCall(glBindTexture(GL_TEXTURE_2D, m_AtlasId));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_SIZE, ATLAS_SIZE, 0,
                        GL_RED, GL_UNSIGNED_BYTE, nullptr));

    for (auto& [codepoint, glyph] : m_CodepointToGeometry) {
        glyph.rect = rects[codepointToRectIndex[codepoint]];
        if (!glyph.rect.was_packed) {
            FATAL("One or more glyphs weren't packed. Probably an issue with "
                  "pointers");
        }

        glCall(glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.rect.x, glyph.rect.y,
                               glyph.rect.w, glyph.rect.h, GL_RED,
                               GL_UNSIGNED_BYTE, glyph.bitmap));
    }

    SPDLOG_DEBUG("Generated font atlas");
}

GlyphPos Font::getGlyphPos(Codepoint codepoint) {
    GlyphPos gp{};
    GlyphGeometry& gg = m_CodepointToGeometry[codepoint];

    gp.al = gg.rect.x / ATLAS_SIZE;
    gp.at = (gg.rect.y + gg.rect.h) / ATLAS_SIZE;
    gp.ar = (gg.rect.x + gg.rect.w) / ATLAS_SIZE;
    gp.ab = gg.rect.y / ATLAS_SIZE;

    gp.pl = fracToPx(gg.metrics.horiBearingX);
    gp.pt = -fracToPx(gg.metrics.height - gg.metrics.horiBearingY),
    gp.pr = gp.pl + gg.rect.w;
    gp.pb = gp.pt + gg.rect.h;

    gp.advance = fracToPx(gg.metrics.horiAdvance);

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
