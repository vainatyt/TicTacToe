#ifndef _TICTACTOE_
#define _TICTACTOE_
#include<iostream>
class Board {
	char m_board[3][3] = { {' ',' ',' '},{' ',' ',' '},{' ',' ',' '} };
public:
	char whoWin();
	bool isEnd();
	bool put(size_t i, size_t j,char ch);
	const char* operator[](size_t i) const;
	std::string toString();
};
std::ostream& operator<<(std::ostream& os, const Board& b);
/*
| x | x | 0 |
-------------
| 0 | x |   |
-------------
|   | x | 0 |
*/
#endif