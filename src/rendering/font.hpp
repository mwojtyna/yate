#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../terminal/terminal_buffer.hpp"
#include "atlas.hpp"
#include "types.hpp"
#include <filesystem>
#include <glm/ext/vector_float2.hpp>
#include <stb_rect_pack.h>
#include <unordered_map>
#include <unordered_set>

class Font {
public:
    Font() = delete;
    Font(std::filesystem::path path, float size);
    ~Font();

    void updateAtlas(std::unordered_set<codepoint_t>& codepoints);
    GlyphPos getGlyphPos(const Cell& cell, glm::vec2& pen);

    FT_Size_Metrics getMetrics() const;
    float getSize() const;
    static double fracToPx(double value);

    constexpr static codepoint_t REPLACEMENT_CHAR = 0xfffd;
    constexpr static uint8_t TAB_WIDTH = 8;

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
