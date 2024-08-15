#include "text_renderer.hpp"
#include "../error.hpp"
#include "freetype/freetype.h"
#include "freetype/fttypes.h"
#include "glad/glad.h"
#include "program.hpp"
#include "spdlog/spdlog.h"
#include <cstddef>
#include <filesystem>

FT_Library TextRenderer::m_FT = nullptr;
FT_Face TextRenderer::m_Face = nullptr;
FT_F26Dot6 TextRenderer::m_Size = 64 * 64;
std::unordered_map<uint64_t, Glyph> TextRenderer::m_GlyphCache;

GLuint TextRenderer::m_TextureArrayId = 0;
GLint TextRenderer::m_MaxTextureLayers = 0;

void TextRenderer::initialize() {
    FT_Error error = FT_Init_FreeType(&m_FT);
    if (error) {
        SPDLOG_ERROR("Failed initializing FreeType: {}",
                     FT_Error_String(error));
        std::exit(1);
    }
    SPDLOG_DEBUG("FreeType version: {}.{}.{}", FREETYPE_MAJOR, FREETYPE_MINOR,
                 FREETYPE_PATCH);

    glCall(glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &m_MaxTextureLayers));
    SPDLOG_DEBUG("Max texture array layers = {}", m_MaxTextureLayers);

    glCall(glGenTextures(1, &m_TextureArrayId));
    glCall(glActiveTexture(GL_TEXTURE0));
    glCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArrayId));
    glCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, 100, 100,
                        m_MaxTextureLayers, 0, GL_RED, GL_UNSIGNED_BYTE,
                        nullptr));

    glCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,
                           GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,
                           GL_CLAMP_TO_EDGE));
    glCall(
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCall(
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

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

    // Cache common printable characters
    // https://www.ascii-code.com/
    for (size_t c = ' '; c <= '~'; c++) {
        Glyph character = getGlyph(c);
        m_GlyphCache[c] = character;
    }
    SPDLOG_DEBUG("Cached common printable characters");

    SPDLOG_DEBUG("Loaded font '{}'", path.c_str());
}

Glyph TextRenderer::getGlyph(const FT_ULong charCode) {
    if (auto it = m_GlyphCache.find(charCode); it != m_GlyphCache.end()) {
        return it->second;
    }

    FT_UInt glyphIndex = FT_Get_Char_Index(m_Face, charCode);

    FT_Error error = FT_Load_Glyph(m_Face, glyphIndex, FT_LOAD_DEFAULT);
    if (error) {
        SPDLOG_ERROR("Failed to load char '{}': {}", charCode,
                     FT_Error_String(error));
    }

    error = FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        SPDLOG_ERROR("Failed to render char '{}': {}", charCode,
                     FT_Error_String(error));
    }

    FT_GlyphSlot& ftGlyph = m_Face->glyph;

    Glyph glyph = {
        .texId = static_cast<GLuint>(charCode),
        .offsetX = ftGlyph->metrics.horiBearingX,
        .offsety = ftGlyph->metrics.height - ftGlyph->metrics.horiBearingY,
        .advance = ftGlyph->metrics.horiAdvance,
        .bitmap = ftGlyph->bitmap.buffer,
        .bitmapWidth = ftGlyph->bitmap.width,
        .bitmapHeight = ftGlyph->bitmap.rows,
    };
    glCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, glyph.texId,
                           glyph.bitmapWidth, glyph.bitmapHeight, 1, GL_RED,
                           GL_UNSIGNED_BYTE, glyph.bitmap));

    m_GlyphCache[charCode] = glyph;

    // for (size_t y = 0; y < glyph.bitmapHeight; y++) {
    //     for (size_t x = 0; x < glyph.bitmapWidth; x++) {
    //         std::cout << (glyph.bitmap[y * glyph.bitmapWidth + x] > 0 ? "@"
    //                                                                   : " ");
    //     }
    //     std::cout << "\n";
    // }

    return glyph;
}
