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

const std::optional<size_t> TerminalBuf::getEolIndexInRow(size_t row) {
    if (m_LineEndings.contains(row)) {
        return m_LineEndings[row];
    } else {
        return std::nullopt;
    }
}

void TerminalBuf::setEolIndexInRow(size_t rowIdx, size_t cellIdx) {
    m_LineEndings[rowIdx] = cellIdx;
}

void TerminalBuf::deleteEolIndexInRow(size_t rowIdx) {
    m_LineEndings.erase(rowIdx);
}

const std::vector<std::vector<Cell>>& TerminalBuf::getRows() const {
    return m_Buf;
}

void TerminalBuf::pushRow(const std::vector<Cell>&& cells) {
    m_Buf.push_back(std::move(cells));
}

void TerminalBuf::deleteRow(size_t row) {
    m_Buf.erase(m_Buf.begin() + row);
    m_LineEndings.erase(row);
}
void TerminalBuf::deleteRows(size_t row, size_t end) {
    m_Buf.erase(m_Buf.begin() + row, m_Buf.begin() + end);
    for (size_t i = row; i < end; i++) {
        m_LineEndings.erase(i);
    }
}
