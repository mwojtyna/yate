#pragma once

#include "ft2build.h"
#include FT_FREETYPE_H
#include <filesystem>

class TextRenderer {
public:
    TextRenderer() = delete;
    TextRenderer(TextRenderer& renderer) = delete;
    TextRenderer(TextRenderer&& renderer) = delete;

    static void initialize();
    static void destroy();

    static void loadFont(const std::filesystem::path path);
    static void renderChar(const FT_ULong charCode);

private:
    static FT_Library m_FT;
    static FT_Face m_Face;
    static FT_F26Dot6 m_Size;
};
