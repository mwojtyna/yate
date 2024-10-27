#pragma once

#include <cstdint>
#include <freetype/freetype.h>
#include <stb_rect_pack.h>

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
