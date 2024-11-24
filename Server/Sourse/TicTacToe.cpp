#include"TicTacToe.h"

char Board::whoWin() {
	for (size_t i = 0; i < 3;++i) {
		if (m_board[i][0] == m_board[i][1] && m_board[i][1]==m_board[i][2] && m_board[i][0]!=' ') {
			return m_board[i][0];
		}
	}
	for (size_t i = 0; i < 3; ++i) {
		if (m_board[0][i] == m_board[1][i] && m_board[1][i] == m_board[2][i] && m_board[0][i] != ' ') {
			return m_board[0][i];
		}
	}
	if (m_board[0][0] == m_board[1][1] && m_board[1][1] == m_board[2][2] && m_board[0][0] != ' ') {
		return m_board[0][0];
	}
	if (m_board[0][2] == m_board[1][1] && m_board[1][1] == m_board[2][0] && m_board[0][2] != ' ') {
		return m_board[0][2];
	}
	return 'N';
}
bool Board::isEnd() {
	if (whoWin()=='O' || whoWin() == 'X') {
		return true;
	}
	for (size_t i = 0; i < 3;++i) {
		for (size_t j = 0; j < 3;++j) {
			if (m_board[i][j]==' ') {
				return false;
			}
		}
	}
	return true;
}
bool Board::put(size_t i, size_t j,char ch) {
	if (m_board[i][j] == 'X' || m_board[i][j] == 'O') {
		return false;
	}
	m_board[i][j] = ch;
	return true;
}
const char* Board::operator[](size_t i) const{
	return m_board[i];
}
std::string Board::toString() {
	std::string out;
	for (size_t i = 0; i < 3; ++i) {
		out+= '|';
		for (size_t j = 0; j < 3; ++j) {
			out += ' ';
			out += m_board[i][j];
			out += " |";
		}
		out+= "\n-------------\n";
	}
	return out;
}
