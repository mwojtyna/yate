#pragma once

#include <filesystem>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <optional>

using AtlasRef = msdfgen::BitmapConstRef<uint8_t, 3>;

struct GlyphInfo {
    double al = 0;
    double ab = 0;
    double ar = 0;
    double at = 0;

    double pl = 0;
    double pb = 0;
    double pr = 0;
    double pt = 0;

    double advance = 0;
};

class Font {
public:
    Font() = delete;
    Font(std::filesystem::path path, float size);
    ~Font();

    void createAtlas();
    GlyphInfo getGlyph(msdfgen::unicode_t codepoint1,
                       std::optional<msdfgen::unicode_t> codepoint2) const;

private:
    std::filesystem::path m_Path;
    float m_Size;
    std::unique_ptr<msdf_atlas::FontGeometry> m_Geometry = nullptr;
    std::vector<msdf_atlas::GlyphGeometry> m_Glyphs;
    AtlasRef m_Atlas;

    msdfgen::FreetypeHandle* m_FT;
    msdfgen::FontHandle* m_Font;
};
