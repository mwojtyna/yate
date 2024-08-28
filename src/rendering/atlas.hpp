#pragma once

#include "opengl.hpp"
#include <cstddef>
#include <stb_rect_pack.h>

class Atlas {
public:
    ~Atlas();

    bool initialized() const;
    void newTarget(size_t width, size_t height, size_t numRects);
    bool pack(stbrp_rect rects[], size_t numRects);

    static constexpr size_t CAPACITY = 16384;

private:
    GLuint m_TexId = 0;
    stbrp_context m_Context;
    // Can't do it with a vector because stb keeps raw pointers to memory and when resizing a vector they become invalid
    stbrp_node m_Nodes[CAPACITY];
};
