#pragma once

#include "ft2build.h"
#include "program.hpp"
#include FT_FREETYPE_H
#include <unordered_map>

#include "glad/glad.h"
#include <cstdint>
#include <filesystem>

struct Glyph {
    GLuint texId;
    int64_t offsetX;
    int64_t offsety;
    int64_t advance;
    uint8_t* bitmap;
    uint32_t bitmapWidth;
    uint32_t bitmapHeight;
};

class TextRenderer {
public:
    TextRenderer() = delete;
    TextRenderer(TextRenderer& renderer) = delete;
    TextRenderer(TextRenderer&& renderer) = delete;

    static void initialize();
    static void destroy();

    static void loadFont(const std::filesystem::path path);
    static Glyph getGlyph(const FT_ULong charCode);

private:
    static FT_Library m_FT;
    static FT_Face m_Face;
    static FT_F26Dot6 m_Size;
    static std::unordered_map<uint64_t, Glyph> m_GlyphCache;

    static GLuint m_TextureArrayId;
    static GLint m_MaxTextureLayers;
    static GLuint m_Depth;
};
