#include "text_renderer.hpp"
#include "../error.hpp"
#include "freetype/freetype.h"
#include "freetype/ftimage.h"
#include "freetype/fttypes.h"
#include "glad/glad.h"
#include "spdlog/spdlog.h"
#include <filesystem>

FT_Library TextRenderer::m_FT = nullptr;
FT_Face TextRenderer::m_Face = nullptr;
FT_F26Dot6 TextRenderer::m_Size = 256 * 64;

void TextRenderer::initialize() {
    FT_Error error = FT_Init_FreeType(&m_FT);
    if (error) {
        SPDLOG_ERROR("Failed initializing FreeType: {}",
                     FT_Error_String(error));
        assert(false);
    }

    SPDLOG_DEBUG("FreeType version: {}.{}.{}", FREETYPE_MAJOR, FREETYPE_MINOR,
                 FREETYPE_PATCH);
    SPDLOG_DEBUG("Initialized text renderer");
}

void TextRenderer::destroy() {
    FT_Error error = FT_Done_FreeType(m_FT);
    if (error) {
        SPDLOG_ERROR("Failed deleting FreeType: {}", FT_Error_String(error));
    }
    SPDLOG_DEBUG("Shutdown text renderer");
}

void TextRenderer::loadFont(const std::filesystem::path path) {
    if (m_Face != nullptr) {
        FT_Error error = FT_Done_Face(m_Face);
        if (error) {
            SPDLOG_ERROR("Failed destroying font '{}': {}", path.c_str(),
                         FT_Error_String(error));
        }
    }

    FT_Error error = FT_New_Face(m_FT, path.c_str(), 0, &m_Face);
    if (error) {
        SPDLOG_ERROR("Failed loading font '{}': {}", path.c_str(),
                     FT_Error_String(error));
    }

    error = FT_Set_Char_Size(m_Face, 0, m_Size, 92, 92);
    if (error) {
        SPDLOG_ERROR("Failed setting font size: {}", FT_Error_String(error));
    }

    SPDLOG_DEBUG("Loaded font '{}'", path.c_str());
}

void TextRenderer::renderChar(const FT_ULong charCode) {
    FT_UInt glyph = FT_Get_Char_Index(m_Face, charCode);

    FT_Error error = FT_Load_Glyph(m_Face, glyph, FT_LOAD_DEFAULT);
    if (error) {
        SPDLOG_ERROR("Failed to load char '{}': {}", charCode,
                     FT_Error_String(error));
    }

    error = FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        SPDLOG_ERROR("Failed to render char '{}': {}", charCode,
                     FT_Error_String(error));
    }

    FT_Bitmap& bitmap = m_Face->glyph->bitmap;
    GLuint texture;
    glCall(glGenTextures(1, &texture));
    glCall(glBindTexture(GL_TEXTURE_2D, texture));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap.width, bitmap.rows, 0,
                        GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer));
}
