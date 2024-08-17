#pragma once

#include <filesystem>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

using AtlasRef = msdfgen::BitmapConstRef<uint8_t, 3>;

struct Glyph {
    double l;
    double b;
    double r;
    double t;
    int w;
    int h;
    double advance;
};

class Font {
public:
    Font() = delete;
    Font(std::filesystem::path path);
    ~Font();

    void createAtlas();
    Glyph getGlyph(msdfgen::unicode_t codepoint) const;

private:
    std::filesystem::path m_Path;
    msdfgen::FreetypeHandle* m_FT;
    msdfgen::FontHandle* m_Font;
    std::unique_ptr<msdf_atlas::FontGeometry> m_Geometry = nullptr;
    std::vector<msdf_atlas::GlyphGeometry> m_Glyphs;
    AtlasRef m_Atlas;
};
