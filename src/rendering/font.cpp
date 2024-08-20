#include "font.hpp"
#include "../error.hpp"
#include <cassert>
#include <freetype/ftmodapi.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#define STB_RECT_PACK_IMPLEMENTATION

Font::Font(std::filesystem::path path, float size)
    : m_Path(path), m_Size(size * 64) {
    // m_FT = msdfgen::initializeFreetype();
    // if (m_FT == nullptr) {
    //     SPDLOG_ERROR("Failed to initialize freetype");
    //     exit(1);
    // }
    //
    // m_Font = msdfgen::loadFont(m_FT, path.c_str());
    // if (m_Font == nullptr) {
    //     SPDLOG_ERROR("Failed to load font '{}'", path.c_str());
    //     exit(1);
    // }
    //
    // m_Geometry = std::make_unique<msdf_atlas::FontGeometry>(&m_Glyphs);
    //
    // msdf_atlas::Charset charset = msdf_atlas::Charset::ASCII;
    // charset.add(0xfffd); // replacement character
    //
    // int amountLoaded = m_Geometry->loadCharset(m_Font, 1, charset);
    // SPDLOG_DEBUG("Loaded {} glyphs from font '{}'", amountLoaded, path.c_str());

    FT_Error error = 0;

    error = FT_Init_FreeType(&m_Lib);
    if (error) {
        FATAL("Failed to initialize freetype");
    }

    error = FT_New_Face(m_Lib, path.c_str(), 0, &m_Font);
    if (error) {
        FATAL("Failed to load font '{}'", path.c_str());
    }

    error = FT_Set_Char_Size(m_Font, 0, m_Size, 0, 0);
}

Font::~Font() {
    FT_Done_Face(m_Font);
    FT_Done_Library(m_Lib);
    SPDLOG_DEBUG("Destroyed font '{}'", m_Path.c_str());
}

// TODO: Create atlas with ASCII characters, then dynamically add more glyphs as needed
// TODO: Create atlas with antialiased bitmap glyphs
void Font::createAtlas() {
    FT_Error error = 0;
    for (Codepoint c = 0x61; c <= 0x61; c++) {
        FT_UInt glyphIndex = FT_Get_Char_Index(m_Font, c);

        error = FT_Load_Glyph(m_Font, glyphIndex, FT_LOAD_DEFAULT);
        if (error) {
            FATAL("Failed to load glyph '{}'", c);
        }

        error = FT_Render_Glyph(m_Font->glyph, FT_RENDER_MODE_NORMAL);

        Glyph g = {
            .al = 0,
            .ab = 1,
            .ar = 1,
            .at = 0,
            .pl = 0,
            .pb = 0,
            .pr = m_Font->glyph->bitmap.width,
            .pt = m_Font->glyph->bitmap.rows,
            .advance = m_Font->glyph->advance.x,
            .bitmap = m_Font->glyph->bitmap.buffer,
        };
        m_Glyphs[c] = g;
    }

    GLuint textureId = 0;
    glCall(glActiveTexture(GL_TEXTURE0));
    glCall(glGenTextures(1, &textureId));
    glCall(glBindTexture(GL_TEXTURE_2D, textureId));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Font->glyph->bitmap.width,
                        m_Font->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                        m_Font->glyph->bitmap.buffer));

    SPDLOG_DEBUG("Generated {}x{} font atlas");
}

Glyph& Font::getGlyph(Codepoint codepoint) {
    // if (m_Atlas.width == 0 && m_Atlas.height == 0) {
    //     assert(false && "Atlas not generated");
    // }
    //
    // GlyphPos gi;
    // msdf_atlas::GlyphGeometry* glyph =
    //     (msdf_atlas::GlyphGeometry*)m_Geometry->getGlyph(codepoint1);
    //
    // // If glyph not found, set it to � (replacement character)
    // if (glyph == nullptr) {
    //     glyph = (msdf_atlas::GlyphGeometry*)m_Geometry->getGlyph(0xFFFD);
    // }
    //
    // glyph->getQuadAtlasBounds(gi.al, gi.ab, gi.ar, gi.at);
    // glyph->getQuadPlaneBounds(gi.pl, gi.pb, gi.pr, gi.pt);
    //
    // gi.al /= m_Atlas.width;
    // gi.ar /= m_Atlas.width;
    // gi.ab /= m_Atlas.height;
    // gi.at /= m_Atlas.height;
    //
    // gi.pl *= m_Size;
    // gi.pr *= m_Size;
    // gi.pb *= m_Size;
    // gi.pt *= m_Size;
    //
    // if (codepoint2.has_value()) {
    //     gi.advance = glyph->getAdvance() * m_Size;
    // }
    //
    // return gi;

    return m_Glyphs[codepoint];
}

const FT_Size_Metrics Font::getMetrics() const {
    return m_Font->size->metrics;
}

float Font::getSize() const {
    return m_Size;
}
