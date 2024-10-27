#include "terminal_buffer.hpp"

TerminalBuf::TerminalBuf(size_t capacity) : m_Capacity(capacity) {}

Cell& TerminalBuf::getCell(size_t col, size_t row) {
    return m_Buf[row][col];
}

std::vector<Cell>& TerminalBuf::getRow(size_t row) {
    return m_Buf[row];
}

const std::vector<std::vector<Cell>>& TerminalBuf::getRows() const {
    return m_Buf;
}

void TerminalBuf::pushRow(const std::vector<Cell>&& cells) {
    m_Buf.push_back(std::move(cells));
}
