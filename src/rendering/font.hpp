#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "atlas.hpp"
#include <filesystem>
#include <glm/ext/vector_float2.hpp>
#include <stb_rect_pack.h>
#include <unordered_map>
#include <unordered_set>

using codepoint_t = FT_ULong;

struct GlyphGeometry {
    FT_Glyph_Metrics metrics;
    stbrp_rect rect;
    uint8_t* bitmap;
};

struct GlyphPos {
    /// Atlas coordinates
    float al, at, ar, ab;
    /// World coordinates
    float pl, pt, pr, pb;
};

class Font {
public:
    Font() = delete;
    Font(std::filesystem::path path, float size);
    ~Font();

    void updateAtlas(std::unordered_set<codepoint_t>& codepoints);
    GlyphPos getGlyphPos(codepoint_t codepoint, glm::vec2& pen);

    FT_Size_Metrics getMetrics() const;
    float getSize() const;
    static double fracToPx(double value);

    constexpr static codepoint_t REPLACEMENT_CHAR = 0xfffd;
    constexpr static size_t TAB_WIDTH = 2;

private:
    std::filesystem::path m_Path;
    float m_Size;
    FT_Library m_Lib;
    FT_Face m_Font;
    Atlas m_Atlas;
    // TODO: Dynamically choose atlas size
    const uint32_t atlasSize = 1024;
    std::unordered_map<codepoint_t, GlyphGeometry> m_CodepointToGeometry;
};
