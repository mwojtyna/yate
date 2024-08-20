#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "stb_rect_pack.h"
#include <filesystem>
#include <unordered_map>

using Codepoint = FT_ULong;

struct Glyph {
    float al;
    float ab;
    float ar;
    float at;

    FT_UInt pl;
    FT_UInt pb;
    FT_UInt pr;
    FT_UInt pt;

    FT_Pos advance;

    stbrp_rect rect;
    uint8_t* bitmap;
};

class Font {
public:
    Font() = delete;
    Font(std::filesystem::path path, float size);
    ~Font();

    void createAtlas();
    Glyph& getGlyph(Codepoint codepoint);
    const FT_Size_Metrics getMetrics() const;
    float getSize() const;

private:
    std::filesystem::path m_Path;
    float m_Size;

    FT_Library m_Lib;
    FT_Face m_Font;
    std::unordered_map<FT_UInt, Glyph> m_Glyphs;
};
