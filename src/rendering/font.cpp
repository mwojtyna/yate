#include "font.hpp"
#include "../error.hpp"
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

Font::Font(std::filesystem::path path) : m_Path(path) {
    m_FT = msdfgen::initializeFreetype();
    if (m_FT == nullptr) {
        SPDLOG_ERROR("Failed to initialize freetype");
        exit(1);
    }

    m_Font = msdfgen::loadFont(m_FT, path.c_str());
    if (m_Font == nullptr) {
        SPDLOG_ERROR("Failed to load font '{}'", path.c_str());
        exit(1);
    }

    m_Geometry = std::make_unique<msdf_atlas::FontGeometry>(&m_Glyphs);
    int amountLoaded =
        m_Geometry->loadCharset(m_Font, 2, msdf_atlas::Charset::ASCII);
    SPDLOG_DEBUG("Loaded {} glyphs from font '{}'", amountLoaded, path.c_str());
}

Font::~Font() {
    msdfgen::destroyFont(m_Font);
    msdfgen::deinitializeFreetype(m_FT);
    SPDLOG_DEBUG("Destroyed font '{}'", m_Path.c_str());
}

void Font::createAtlas() {
    for (msdf_atlas::GlyphGeometry& glyph : m_Glyphs) {
        glyph.edgeColoring(&msdfgen::edgeColoringSimple, 3.0, 0);
    }

    msdf_atlas::TightAtlasPacker packer;
    packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
    packer.setScale(32.0); // px dimensions for each glyph in atlas
    packer.setPixelRange(2.0);
    packer.setMiterLimit(1.0);
    int remaining = packer.pack(m_Glyphs.data(), m_Glyphs.size());
    if (remaining > 0) {
        SPDLOG_ERROR("Packing failed; {} glyphs left to pack", remaining);
        exit(1);
    }

    int width = 0, height = 0;
    packer.getDimensions(width, height);
    msdf_atlas::ImmediateAtlasGenerator<
        float, 3, &msdf_atlas::msdfGenerator,
        msdf_atlas::BitmapAtlasStorage<uint8_t, 3>>
        generator(width, height);
    msdf_atlas::GeneratorAttributes attributes;
    generator.setAttributes(attributes);
    generator.setThreadCount(std::thread::hardware_concurrency() / 2);
    generator.generate(m_Glyphs.data(), m_Glyphs.size());
    m_Atlas = generator.atlasStorage();
    SPDLOG_DEBUG("Generated font atlas");

    GLuint textureId = 0;
    glCall(glGenTextures(1, &textureId));
    glCall(glBindTexture(GL_TEXTURE_2D, textureId));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Atlas.width,
                        m_Atlas.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                        m_Atlas.pixels));
}

Glyph Font::getGlyph(msdfgen::unicode_t codepoint) const {
    Glyph glyph;

    const msdf_atlas::GlyphGeometry* glyphInfo =
        m_Geometry->getGlyph(codepoint);
    int _ = 0;
    glyphInfo->getBoxRect(_, _, glyph.w, glyph.h);

    glyphInfo->getQuadAtlasBounds(glyph.l, glyph.b, glyph.r, glyph.t);
    glyph.l /= m_Atlas.width;
    glyph.r /= m_Atlas.width;
    glyph.b /= m_Atlas.height;
    glyph.t /= m_Atlas.height;
    glyph.advance = glyphInfo->getAdvance();

    return glyph;
}
