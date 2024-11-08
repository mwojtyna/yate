#include "font.hpp"
#include "../terminal/codes.hpp"
#include "../terminal/parser.hpp"
#include "../utils.hpp"
#include "opengl.hpp"
#include <freetype/freetype.h>
#include <freetype/ftmodapi.h>
#include <glm/ext/vector_float2.hpp>
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

    // TODO: Calculate dpi to hopefully fix wrong font size
    error = FT_Set_Char_Size(m_Font, 0, size * 64, 0, 0);

    auto initial = std::unordered_set<codepoint_t>();
    // Insert ASCII characters
    for (codepoint_t c = ' '; c <= '~'; c++) {
        initial.insert(c);
    }
    initial.insert(Font::REPLACEMENT_CHAR); // replacement character �
    updateAtlas(initial);
}

Font::~Font() {
    for (auto& g : m_CodepointToGeometry) {
        std::free(g.second.bitmap);
    }
    FT_Done_Face(m_Font);
    FT_Done_Library(m_Lib);
    SPDLOG_DEBUG("Destroyed font '{}'", m_Path.c_str());
}

/// Returns: true if any codepoints were new, false if not
bool Font::updateAtlas(std::unordered_set<codepoint_t>& codepoints) {
    bool anyNew = false;
    FT_Error error = 0;

    // Filter out already rendered glyphs
    std::unordered_set<codepoint_t> newCodepoints;
    for (codepoint_t c : codepoints) {
        if (!m_CodepointToGeometry.contains(c)) {
            newCodepoints.insert(c);
            anyNew = true;
        }
    }
    if (newCodepoints.empty()) {
        return false;
    }

    size_t numGlyphs = m_CodepointToGeometry.size() + newCodepoints.size();
    size_t rectIndex = m_CodepointToGeometry.size();
    // We have to pass an array of rects to stbrp_pack_rects later, so we can't just make a Codepoint->stbrp_rect map
    stbrp_rect rects[Atlas::CAPACITY];
    std::unordered_map<codepoint_t, size_t> codepointToRectIndex;
    std::unordered_map<codepoint_t, GlyphGeometry> codepointToGeometry;

    for (codepoint_t c : newCodepoints) {
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
            // Have to memcpy, because on the next iteration the bitmap buffer pointer is overwritten with the next glyph's bitmap
            .bitmap = (uint8_t*)std::malloc(bitmapSize),
        };
        std::memcpy(glyph.bitmap, m_Font->glyph->bitmap.buffer, bitmapSize);
        codepointToGeometry[c] = glyph;
    }

    if (!m_Atlas.initialized()) {
        m_Atlas.newTarget(atlasSize, atlasSize, m_Font->num_glyphs);
    }
    if (!m_Atlas.pack(rects, numGlyphs)) {
        FATAL("Failed to calculate glyph packing");
    }
    SPDLOG_DEBUG("Calculated glyph packing, {} new glyphs",
                 newCodepoints.size());

    for (auto& [codepoint, glyph] : codepointToGeometry) {
        glyph.rect = rects[codepointToRectIndex[codepoint]];
        if (!glyph.rect.was_packed) {
            FATAL("One or more glyphs weren't packed");
        }

        glCall(glTexSubImage2D(GL_TEXTURE_2D, 0, glyph.rect.x, glyph.rect.y,
                               glyph.rect.w, glyph.rect.h, GL_RED,
                               GL_UNSIGNED_BYTE, glyph.bitmap));

        m_CodepointToGeometry[codepoint] = glyph;
    }

    SPDLOG_DEBUG("Generated {}x{} font atlas", atlasSize, atlasSize);
    return anyNew;
}

GlyphPos Font::getGlyphPos(const Cell& cell, glm::vec2& pen) {
    bool found = m_CodepointToGeometry.contains(cell.character);

    GlyphPos gp{};
    GlyphGeometry& gg = found ? m_CodepointToGeometry[cell.character]
                              : m_CodepointToGeometry[REPLACEMENT_CHAR];

    gp.al = gg.rect.x / (float)atlasSize;
    gp.at = (gg.rect.y + gg.rect.h) / (float)atlasSize;
    gp.ar = (gg.rect.x + gg.rect.w) / (float)atlasSize;
    gp.ab = gg.rect.y / (float)atlasSize;

    gp.pl = fracToPx(gg.metrics.horiBearingX);
    gp.pt = -fracToPx(gg.metrics.height - gg.metrics.horiBearingY),
    gp.pr = gp.pl + gg.rect.w;
    gp.pb = gp.pt + gg.rect.h;

    glm::vec2 advance(fracToPx(gg.metrics.horiAdvance), 0);
    if (cell.character == c0::HT) {
        const GlyphGeometry& space = m_CodepointToGeometry[' '];
        // Align to next tab stop
        advance.x = fracToPx(space.metrics.horiAdvance) *
                    (TAB_WIDTH - (cell.offset % TAB_WIDTH));
        gp.pl = 0;
        gp.pt = 0;
        gp.pr = 0;
        gp.pb = 0;
    }
    pen += advance;

    return gp;
}

FT_Size_Metrics Font::getMetricsInPx() const {
    FT_Size_Metrics metrics = m_Font->size->metrics;
    metrics.ascender = fracToPx(metrics.ascender);
    metrics.descender = fracToPx(metrics.descender);
    metrics.height = fracToPx(metrics.height);
    metrics.max_advance = fracToPx(metrics.max_advance);

    return metrics;
}

float Font::getSize() const {
    return m_Size;
}

double Font::fracToPx(double value) {
    return value / 64.0;
}
