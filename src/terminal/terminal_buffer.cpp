#include "terminal_buffer.hpp"

// Cell
Cell Cell::empty() {
    return Cell{.character = ' '};
}

// TerminalBuf
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

void TerminalBuf::deleteRow(size_t row) {
    m_Buf.erase(m_Buf.begin() + row);
}
void TerminalBuf::deleteRows(size_t row, size_t end) {
    if (row < end) {
        m_Buf.erase(m_Buf.begin() + row, m_Buf.begin() + end);
    }
}
