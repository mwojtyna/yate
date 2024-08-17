#include "font.hpp"
#include "../error.hpp"
#include <cassert>
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

Font::Font(std::filesystem::path path, float size)
    : m_Path(path), m_Size(size) {
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
    msdf_atlas::Charset charset;
    for (size_t cp = 0; cp <= 0x10FFFF; cp++) {
        charset.add(cp);
    }
    int amountLoaded = m_Geometry->loadCharset(m_Font, 2, charset);
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

GlyphInfo Font::getGlyph(msdfgen::unicode_t codepoint1,
                         std::optional<msdfgen::unicode_t> codepoint2) const {
    if (m_Atlas.width == 0 && m_Atlas.height == 0) {
        assert(false && "Atlas not generated");
    }

    GlyphInfo gi;
    msdf_atlas::GlyphGeometry* glyph =
        (msdf_atlas::GlyphGeometry*)m_Geometry->getGlyph(codepoint1);

    // If glyph not found, set it to � (replacement character)
    if (glyph == nullptr) {
        glyph = (msdf_atlas::GlyphGeometry*)m_Geometry->getGlyph(0xFFFD);
    }

    glyph->getQuadAtlasBounds(gi.al, gi.ab, gi.ar, gi.at);
    glyph->getQuadPlaneBounds(gi.pl, gi.pb, gi.pr, gi.pt);

    gi.al /= m_Atlas.width;
    gi.ar /= m_Atlas.width;
    gi.ab /= m_Atlas.height;
    gi.at /= m_Atlas.height;

    gi.pl *= m_Size;
    gi.pr *= m_Size;
    gi.pb *= m_Size;
    gi.pt *= m_Size;

    if (codepoint2.has_value()) {
        gi.advance = glyph->getAdvance() * m_Size;
    }

    return gi;
}

const msdfgen::FontMetrics& Font::getMetrics() const {
    return m_Geometry->getMetrics();
}

float Font::getSize() const {
    return m_Size;
}
