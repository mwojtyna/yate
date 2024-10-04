#pragma once

#include "../rendering/types.hpp"
#include <glm/ext/vector_float4.hpp>
#include <vector>

struct Cell {
    glm::vec4 bgColor;
    glm::vec4 fgColor;
    codepoint_t character;
    bool lineStart = false;
    bool lineEnd = false;
    bool isCursor = false;
    /// Offset since start of line
    size_t offset = 0;
};

// TODO: Ring buffer
class TerminalBuf {
public:
    TerminalBuf() = default;
    TerminalBuf(size_t capacity);

    Cell& getCell(size_t col, size_t row);
    std::vector<Cell>& getRow(size_t row);
    const std::vector<std::vector<Cell>>& getRows() const;

    void pushRow(const std::vector<Cell>&& cells);

private:
    std::vector<std::vector<Cell>> m_Buf;
    size_t m_Width;
    // ~1000 full pages on a 1440p screen with 11px font size
    size_t m_Capacity = 20'000'000;
    size_t m_AddedCount;
};
