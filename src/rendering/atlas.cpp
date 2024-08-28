#include "atlas.hpp"

Atlas::~Atlas() {
    glCall(glDeleteTextures(1, &m_TexId));
}

bool Atlas::initialized() const {
    return m_TexId != 0;
}

void Atlas::newTarget(size_t width, size_t height, size_t numRects) {
    if (initialized()) {
        glCall(glDeleteTextures(1, &m_TexId));
    }

    stbrp_init_target(&m_Context, width, height, m_Nodes, numRects);
    glCall(glGenTextures(1, &m_TexId));
    glCall(glBindTexture(GL_TEXTURE_2D, m_TexId));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                        GL_UNSIGNED_BYTE, nullptr));
}

bool Atlas::pack(stbrp_rect rects[], size_t numRects) {
    const int success = stbrp_pack_rects(&m_Context, rects, numRects);
    return success == 1;
}
