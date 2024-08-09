#pragma once

#include "object.hpp"
#include <vector>

class Renderer {
public:
    void draw() const;
    void addObject(Object& object);
    void setWireframe(const bool enabled) const;

    static glm::mat4 PROJECTION;

private:
    std::vector<Object> m_Objects;
};
