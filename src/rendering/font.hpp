#pragma once

#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H

#include "stb_rect_pack.h"
#include <filesystem>
#include <unordered_map>

using Codepoint = FT_ULong;

struct GlyphGeometry {
    FT_Glyph_Metrics metrics;
    stbrp_rect* rect;
    uint8_t* bitmap;
};

struct GlyphPos {
    /// Atlas coordinates
    float al, at, ar, ab;
    /// World coordinates
    float pl, pt, pr, pb;
    FT_Pos advance;
};

class Font {
public:
    Font() = delete;
    Font(std::filesystem::path path, float size);
    ~Font();

    void createAtlas();
    GlyphPos getGlyphPos(Codepoint codepoint);
    const FT_Size_Metrics getMetrics() const;
    float getSize() const;

    static double fracToPx(double value);

private:
    const float ATLAS_SIZE = 2048;

    std::filesystem::path m_Path;
    float m_Size;
    FT_Library m_Lib;
    FT_Face m_Font;
    std::unordered_map<Codepoint, GlyphGeometry> m_Geometry;
    std::vector<stbrp_rect> m_Rects;
};
