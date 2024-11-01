#pragma once

#include "../rendering/types.hpp"
#include <glm/ext/vector_float4.hpp>
#include <vector>

struct Cell {
    glm::vec4 bgColor;
    glm::vec4 fgColor;
    codepoint_t character;
    /// Offset since start of line
    size_t offset = 0;
};

// TODO: Ring buffer
class TerminalBuf {
public:
    Cell& getCell(size_t col, size_t row);
    std::vector<Cell>& getRow(size_t row);
    const std::vector<std::vector<Cell>>& getRows() const;

    void pushRow(const std::vector<Cell>&& cells);
    void deleteRow(size_t row);
    void deleteRow(size_t row, size_t end);

private:
    std::vector<std::vector<Cell>> m_Buf;
    size_t m_Width;
    size_t m_AddedCount;
};
