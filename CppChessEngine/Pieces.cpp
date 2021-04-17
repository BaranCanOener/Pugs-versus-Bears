#include "pieces.h"
#include "algorithm"

bool comp1(std::tuple<char, char, char, char, int> arg1, std::tuple<char, char, char, char, int> arg2) {
	return std::get<4>(arg1) < std::get<4>(arg2);
}

bool comp2(std::tuple<char, char, char, char, int> arg1, std::tuple<char, char, char, char, int> arg2) {
	return std::get<4>(arg1) > std::get<4>(arg2);
}

//Generic piece implementation - parent class of all pieces
Piece::Piece(Colour colour) {
	Piece::colour = colour;
	Piece::moveCount = 0;
	Piece::moveList.reserve(40);
	Piece::captureMoveList.reserve(40);
}

/*Returns a list of possible moves assuming the piece is at the (x, y) Position on the board
The board is needed to only generate moves within range of the piece (i.e.to avoid skipping for sliding pieces)
and that do not move to an occupied square of the same color
Legality of moves is handled in terms of checkmate is handled by the board class itself
This virtual method gets overridden in child classes.*/
std::vector<std::tuple<char, char>> Piece::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Piece::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	return Piece::captureMoveList;
}

char Piece::getSymbol() {
	return Piece::symbol;
}

int Piece::getValue() {
	return Piece::value;
}

PieceType Piece::getPieceType() {
	return Piece::pieceType;
}

int Piece::getPositionalScore(char x, char y) {
	return 0;
}

void Piece::setEndgameScoreboard()
{
}

void Piece::setNormalScoreboard()
{
}


std::vector<std::tuple<char, char, char, char, int>> Piece::getCaptureMoveListMVV_LVA(Piece* squares[8][8], char x, char y)
{
	return std::vector<std::tuple<char, char, char, char, int>>();
}

//PAWN IMPLEMENTATION
Pawn::Pawn(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Pawn::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	if (Pawn::colour == Colour::White) {
		if (y < 7) {
			//Move one square ahead
			if (squares[x][y + 1] == nullptr) {
				Piece::moveList.push_back(std::tuple<char, char>(x, y + 1));
			}
			//Move two squares ahead
			if (y == 1) {
				if ((squares[x][y + 1] == nullptr) && (squares[x][y + 2] == nullptr)) {
					Piece::moveList.push_back(std::tuple<char, char>(x, y + 2));
				}
			}
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y + 1] != nullptr) {
				if (squares[x + 1][y + 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y + 1] != nullptr) {
				if (squares[x - 1][y + 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 1));
				}
			}
			if (y == 4) {
				if ((x > 0) && (squares[x - 1][y] != nullptr) && (squares[x - 1][y + 1] == nullptr) && (squares[x - 1][y]->getPieceType() == PieceType::Pawn) && squares[x - 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 1)); //white left en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
				if ((x < 7) && (squares[x + 1][y] != nullptr) && (squares[x + 1][y + 1] == nullptr) && (squares[x + 1][y]->getPieceType() == PieceType::Pawn) && squares[x + 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 1)); //white right en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
			}
		}
	}
	else if (Pawn::colour == Colour::Black) {
		if (y > 0) {
			//Move one square ahead
			if (squares[x][y - 1] == nullptr) {
				Piece::moveList.push_back(std::tuple<char, char>(x, y - 1));
			}
			//Move two squares ahead
			if (y == 6) {
				if ((squares[x][y - 1] == nullptr) && (squares[x][y - 2] == nullptr)) {
					Piece::moveList.push_back(std::tuple<char, char>(x, y - 2));
				}
			}
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y - 1] != nullptr) {
				if (squares[x + 1][y - 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y - 1] != nullptr) {
				if (squares[x - 1][y - 1]->colour != Pawn::colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 1));
				}
			}
			if (y == 3) {
				if ((x > 0) && (squares[x - 1][y] != nullptr) && (squares[x - 1][y - 1] == nullptr) && (squares[x - 1][y]->getPieceType() == PieceType::Pawn) && squares[x - 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 1)); //black left en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
				if ((x < 7) && (squares[x + 1][y] != nullptr) && (squares[x + 1][y - 1] == nullptr) && (squares[x + 1][y]->getPieceType() == PieceType::Pawn) && squares[x + 1][y]->colour != colour) {
					Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 1)); //black right en passant possible; board needs to check if the pawn to be captured moved in the prev. round
				}
			}
		}
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Pawn::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	if (Pawn::colour == Colour::White) {
		if (y < 7) {
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y + 1] != nullptr) {
				if (squares[x + 1][y + 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y + 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y + 1] != nullptr) {
				if (squares[x - 1][y + 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y + 1));
				}
			}
		}
	}
	else if (Pawn::colour == Colour::Black) {
		if (y > 0) {
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y - 1] != nullptr) {
				if (squares[x + 1][y - 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y - 1));
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y - 1] != nullptr) {
				if (squares[x - 1][y - 1]->colour != Pawn::colour) {
					Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y - 1));
				}
			}
		}
	}
	return Piece::captureMoveList;
}

char Pawn::getSymbol() {
	return Pawn::symbol;
}

int Pawn::getValue() {
	if (Pawn::colour == Colour::White) {
		return Pawn::value;
	}
	else {
		return -Pawn::value;
	}
}

PieceType Pawn::getPieceType() {
	return Pawn::pieceType;
}

int Pawn::getPositionalScore(char x, char y) {
	if (Pawn::colour == Colour::White) {
		return Pawn::scoreBoard[7 - y][x];
	}
	else {
		return -Pawn::scoreBoard[y][x];
	}
}

std::vector<std::tuple<char, char, char, char, int>> Pawn::getCaptureMoveListMVV_LVA(Piece* squares[8][8], char x, char y)
{
	Piece::captureMoveListMVV_LVA.clear();
	if (Pawn::colour == Colour::White) {
		if (y < 7) {
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y + 1] != nullptr) {
				if (squares[x + 1][y + 1]->colour != Pawn::colour) {
					Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 1, y + 1, squares[x + 1][y + 1]->getValue() + getValue()));
					std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y + 1] != nullptr) {
				if (squares[x - 1][y + 1]->colour != Pawn::colour) {
					Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 1, y + 1, squares[x - 1][y + 1]->getValue() + getValue()));
					std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
				}
			}
		}
	}
	else if (Pawn::colour == Colour::Black) {
		if (y > 0) {
			//Move that corresponds to taking a piece to the right
			if ((x < 7) && squares[x + 1][y - 1] != nullptr) {
				if (squares[x + 1][y - 1]->colour != Pawn::colour) {
					Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 1, y - 1, squares[x + 1][y - 1]->getValue() + getValue()));
					std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
				}
			}
			//Move that corresponds to taking a piece to the left
			if ((x > 0) && squares[x - 1][y - 1] != nullptr) {
				if (squares[x - 1][y - 1]->colour != Pawn::colour) {
					Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 1, y - 1, squares[x - 1][y - 1]->getValue() + getValue()));
					std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
				}
			}
		}
	}
	return captureMoveListMVV_LVA;
}

//KNIGHT IMPLEMENTATION
Knight::Knight(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}
std::vector<std::tuple<char, char>> Knight::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	if ((x - 1 >= 0) && (y + 2 <= 7)) {
		if ((squares[x - 1][y + 2] == nullptr) || squares[x - 1][y + 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 2));
		}
	}
	if ((x - 1 >= 0) && (y - 2 >= 0)) {
		if ((squares[x - 1][y - 2] == nullptr) || squares[x - 1][y - 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 2));
		}
	}
	if ((x + 1 <= 7) && (y + 2 <= 7)) {
		if ((squares[x + 1][y + 2] == nullptr) || squares[x + 1][y + 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 2));
		}
	}
	if ((x + 1 <= 7) && (y - 2 >= 0)) {
		if ((squares[x + 1][y - 2] == nullptr) || squares[x + 1][y - 2]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 2));
		}
	}
	if ((x - 2 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 2][y + 1] == nullptr) || squares[x - 2][y + 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 2, y + 1));
		}
	}
	if ((x - 2 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 2][y - 1] == nullptr) || squares[x - 2][y - 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 2, y - 1));
		}
	}
	if ((x + 2 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 2][y + 1] == nullptr) || squares[x + 2][y + 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 2, y + 1));
		}
	}
	if ((x + 2 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 2][y - 1] == nullptr) || squares[x + 2][y - 1]->colour != Knight::colour) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 2, y - 1));
		}
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Knight::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	if ((x - 1 >= 0) && (y + 2 <= 7)) {
		if ((squares[x - 1][y + 2] != nullptr) && squares[x - 1][y + 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y + 2));
		}
	}
	if ((x - 1 >= 0) && (y - 2 >= 0)) {
		if ((squares[x - 1][y - 2] != nullptr) && squares[x - 1][y - 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y - 2));
		}
	}
	if ((x + 1 <= 7) && (y + 2 <= 7)) {
		if ((squares[x + 1][y + 2] != nullptr) && squares[x + 1][y + 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y + 2));
		}
	}
	if ((x + 1 <= 7) && (y - 2 >= 0)) {
		if ((squares[x + 1][y - 2] != nullptr) && squares[x + 1][y - 2]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y - 2));
		}
	}
	if ((x - 2 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 2][y + 1] != nullptr) && squares[x - 2][y + 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 2, y + 1));
		}
	}
	if ((x - 2 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 2][y - 1] != nullptr) && squares[x - 2][y - 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 2, y - 1));
		}
	}
	if ((x + 2 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 2][y + 1] != nullptr) && squares[x + 2][y + 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 2, y + 1));
		}
	}
	if ((x + 2 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 2][y - 1] != nullptr) && squares[x + 2][y - 1]->colour != Knight::colour) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 2, y - 1));
		}
	}
	return Piece::captureMoveList;
}

char Knight::getSymbol() {
	return Knight::symbol;
}

int Knight::getValue() {
	if (Knight::colour == Colour::White) {
		return Knight::value;
	}
	else {
		return -Knight::value;
	}
}

PieceType Knight::getPieceType() {
	return Knight::pieceType;
}

int Knight::getPositionalScore(char x, char y) {
	if (Knight::colour == Colour::White) {
		return Knight::scoreBoard[7 - y][x];
	}
	else {
		return -Knight::scoreBoard[y][x];
	}
}

std::vector<std::tuple<char, char, char, char, int>> Knight::getCaptureMoveListMVV_LVA(Piece* squares[8][8], char x, char y)
{
	Piece::captureMoveListMVV_LVA.clear();
	if ((x - 1 >= 0) && (y + 2 <= 7)) {
		if ((squares[x - 1][y + 2] != nullptr) && squares[x - 1][y + 2]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 1, y + 2, squares[x - 1][y + 2]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x - 1 >= 0) && (y - 2 >= 0)) {
		if ((squares[x - 1][y - 2] != nullptr) && squares[x - 1][y - 2]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 1, y - 2, squares[x - 1][y - 2]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x + 1 <= 7) && (y + 2 <= 7)) {
		if ((squares[x + 1][y + 2] != nullptr) && squares[x + 1][y + 2]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 1, y + 2, squares[x + 1][y + 2]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x + 1 <= 7) && (y - 2 >= 0)) {
		if ((squares[x + 1][y - 2] != nullptr) && squares[x + 1][y - 2]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 1, y - 2, squares[x + 1][y - 2]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x - 2 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 2][y + 1] != nullptr) && squares[x - 2][y + 1]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 2, y + 1, squares[x - 2][y + 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x - 2 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 2][y - 1] != nullptr) && squares[x - 2][y - 1]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 2, y - 1, squares[x - 2][y - 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x + 2 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 2][y + 1] != nullptr) && squares[x + 2][y + 1]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 2, y + 1, squares[x + 2][y + 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x + 2 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 2][y - 1] != nullptr) && squares[x + 2][y - 1]->colour != Knight::colour) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 2, y - 1, squares[x + 2][y - 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	return captureMoveListMVV_LVA;
}

//ROOK IMPLEMENTATION
Rook::Rook(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Rook::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Rook::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Rook::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y - i));
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Rook::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y + i));
	}
	return Piece::captureMoveList;
}

char Rook::getSymbol() {
	return Rook::symbol;
}

int Rook::getValue() {
	if (Rook::colour == Colour::White) {
		return Rook::value;
	}
	else {
		return -Rook::value;
	}
}

PieceType Rook::getPieceType() {
	return Rook::pieceType;
}

int Rook::getPositionalScore(char x, char y) {
	if (Rook::colour == Colour::White) {
		return Rook::scoreBoard[7 - y][x];
	}
	else {
		return -Rook::scoreBoard[y][x];
	}
}

std::vector<std::tuple<char, char, char, char, int>> Rook::getCaptureMoveListMVV_LVA(Piece* squares[8][8], char x, char y)
{
	Piece::captureMoveListMVV_LVA.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Rook::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - i, y, squares[x - i][y]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Rook::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + i, y, squares[x + i][y]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Rook::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x, y - i, squares[x][y - i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Rook::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x, y + i, squares[x][y + i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	return Piece::captureMoveListMVV_LVA;
}

//BISHOP IMPLEMENTATION
Bishop::Bishop(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Bishop::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	int i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Bishop::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Bishop::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	int i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y - i));
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Bishop::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y - i));
	}
	return Piece::captureMoveList;
}

char Bishop::getSymbol() {
	return Bishop::symbol;
}

int Bishop::getValue() {
	if (Bishop::colour == Colour::White) {
		return Bishop::value;
	}
	else {
		return -Bishop::value;
	}
}

PieceType Bishop::getPieceType() {
	return Bishop::pieceType;
}

int Bishop::getPositionalScore(char x, char y) {
	if (Bishop::colour == Colour::White) {
		return Bishop::scoreBoard[7 - y][x];
	}
	else {
		return -Bishop::scoreBoard[y][x];
	}
}

std::vector<std::tuple<char, char, char, char, int>> Bishop::getCaptureMoveListMVV_LVA(Piece* squares[8][8], char x, char y)
{
	Piece::captureMoveListMVV_LVA.clear();
	int i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Bishop::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - i, y + i, squares[x - i][y + i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Bishop::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + i, y + i, squares[x + i][y + i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Bishop::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - i, y - i, squares[x - i][y - i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Bishop::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + i, y - i, squares[x + i][y - i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	return Piece::captureMoveListMVV_LVA;
}

//QUEEN IMPLEMENTATION
Queen::Queen(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

std::vector<std::tuple<char, char>> Queen::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y - i));
		i++;
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x, y + i));
		i++;
	}
	i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x - i, y - i));
		i++;
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Queen::colour)) {
		Piece::moveList.push_back(std::tuple<char, char>(x + i, y - i));
		i++;
	}
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> Queen::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y));
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y));
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y - i));
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y + i));
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y + i));
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x - i, y - i));
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Queen::colour)) {
		Piece::captureMoveList.push_back(std::tuple<char, char>(x + i, y - i));
	}
	return Piece::captureMoveList;
}

char Queen::getSymbol() {
	return Queen::symbol;
}

int Queen::getValue() {
	if (Queen::colour == Colour::White) {
		return Queen::value;
	}
	else {
		return -Queen::value;
	}
}

PieceType Queen::getPieceType() {
	return Queen::pieceType;
}

int Queen::getPositionalScore(char x, char y) {
	if (Queen::colour == Colour::White) {
		return Queen::scoreBoard[7 - y][x];
	}
	else {
		return -Queen::scoreBoard[y][x];
	}
}

std::vector<std::tuple<char, char, char, char, int>> Queen::getCaptureMoveListMVV_LVA(Piece* squares[8][8], char x, char y)
{
	Piece::captureMoveListMVV_LVA.clear();
	int i = 1;
	while ((x - i >= 0) && (squares[x - i][y] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (squares[x - i][y]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - i, y, squares[x - i][y]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x + i <= 7) && (squares[x + i][y] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (squares[x + i][y]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + i, y, squares[x + i][y]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((y - i >= 0) && (squares[x][y - i] == nullptr)) {
		i++;
	}
	if ((y - i >= 0) && (squares[x][y - i]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x, y - i, squares[x][y - i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((y + i <= 7) && (squares[x][y + i] == nullptr)) {
		i++;
	}
	if ((y + i <= 7) && (squares[x][y + i]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x, y + i, squares[x][y + i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y + i <= 7) && (squares[x - i][y + i]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - i, y + i, squares[x - i][y + i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y + i <= 7) && (squares[x + i][y + i]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + i, y + i, squares[x + i][y + i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i] == nullptr)) {
		i++;
	}
	if ((x - i >= 0) && (y - i >= 0) && (squares[x - i][y - i]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - i, y - i, squares[x - i][y - i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	i = 1;
	while ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i] == nullptr)) {
		i++;
	}
	if ((x + i <= 7) && (y - i >= 0) && (squares[x + i][y - i]->colour != Queen::colour)) {
		Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + i, y - i, squares[x + i][y - i]->getValue() + getValue()));
		std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
	}
	return Piece::captureMoveListMVV_LVA;
}

//KING IMPLEMENTATION
King::King(Colour colour) :Piece(colour) {
	Piece::colour = colour;
}

void King::setEndgameScoreboard()
{
	std::copy(&scoreBoard_endgame[0][0], &scoreBoard_endgame[0][0] + 64, &scoreBoard[0][0]);
}

void King::setNormalScoreboard()
{
	std::copy(&scoreBoard_normal[0][0], &scoreBoard_normal[0][0] + 64, &scoreBoard[0][0]);
}

std::vector<std::tuple<char, char>> King::getMoveList(Piece* squares[8][8], char x, char y) {
	Piece::moveList.clear();
	if ((x - 1 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 1][y + 1] == nullptr) || (King::colour != squares[x - 1][y + 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y + 1));
		}
	}
	if ((x - 1 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 1][y - 1] == nullptr) || (King::colour != squares[x - 1][y - 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y - 1));
		}
	}
	if ((x + 1 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 1][y + 1] == nullptr) || (King::colour != squares[x + 1][y + 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y + 1));
		}
	}
	if ((x + 1 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 1][y - 1] == nullptr) || (King::colour != squares[x + 1][y - 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y - 1));
		}
	}
	if ((x + 1 <= 7)) {
		if ((squares[x + 1][y] == nullptr) || (King::colour != squares[x + 1][y]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x + 1, y));
		}
	}
	if ((x - 1 >= 0)) {
		if ((squares[x - 1][y] == nullptr) || (King::colour != squares[x - 1][y]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x - 1, y));
		}
	}
	if ((y + 1 <= 7)) {
		if ((squares[x][y + 1] == nullptr) || (King::colour != squares[x][y + 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x, y + 1));
		}
	}
	if ((y - 1 >= 0)) {
		if ((squares[x][y - 1] == nullptr) || (King::colour != squares[x][y - 1]->colour)) {
			Piece::moveList.push_back(std::tuple<char, char>(x, y - 1));
		}
	}
	//White queenside castling
	if ((King::colour == Colour::White) && (x == 4) && (y == 0) && (squares[3][0] == nullptr) && (squares[2][0] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(2, 0));
	//White kingside castling
	if ((King::colour == Colour::White) && (x == 4) && (y == 0) && (squares[6][0] == nullptr) && (squares[5][0] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(6, 0));
	//Black queenside castling
	if ((King::colour == Colour::Black) && (x == 4) && (y == 7) && (squares[3][7] == nullptr) && (squares[2][7] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(2, 7));
	//Black kingside castling
	if ((King::colour == Colour::Black) && (x == 4) && (y == 7) && (squares[6][7] == nullptr) && (squares[5][7] == nullptr))
		Piece::moveList.push_back(std::tuple<char, char>(6, 7));
	return Piece::moveList;
}

std::vector<std::tuple<char, char>> King::getCaptureMoveList(Piece* squares[8][8], char x, char y) {
	Piece::captureMoveList.clear();
	if ((x - 1 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 1][y + 1] != nullptr) && (King::colour != squares[x - 1][y + 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y + 1));
		}
	}
	if ((x - 1 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 1][y - 1] != nullptr) && (King::colour != squares[x - 1][y - 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y - 1));
		}
	}
	if ((x + 1 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 1][y + 1] != nullptr) && (King::colour != squares[x + 1][y + 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y + 1));
		}
	}
	if ((x + 1 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 1][y - 1] != nullptr) && (King::colour != squares[x + 1][y - 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y - 1));
		}
	}
	if ((x + 1 <= 7)) {
		if ((squares[x + 1][y] != nullptr) && (King::colour != squares[x + 1][y]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x + 1, y));
		}
	}
	if ((x - 1 >= 0)) {
		if ((squares[x - 1][y] != nullptr) && (King::colour != squares[x - 1][y]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x - 1, y));
		}
	}
	if ((y + 1 <= 7)) {
		if ((squares[x][y + 1] != nullptr) && (King::colour != squares[x][y + 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x, y + 1));
		}
	}
	if ((y - 1 >= 0)) {
		if ((squares[x][y - 1] != nullptr) && (King::colour != squares[x][y - 1]->colour)) {
			Piece::captureMoveList.push_back(std::tuple<char, char>(x, y - 1));
		}
	}
	return Piece::captureMoveList;
}

char King::getSymbol() {
	return King::symbol;
}

int King::getValue() {
	if (King::colour == Colour::White) {
		return King::value;
	}
	else {
		return -King::value;
	}
}


PieceType King::getPieceType() {
	return King::pieceType;
}

int King::getPositionalScore(char x, char y) {
	if (King::colour == Colour::White) {
		return King::scoreBoard[7 - y][x];
	}
	else {
		return -King::scoreBoard[y][x];
	}
}

std::vector<std::tuple<char, char, char, char, int>> King::getCaptureMoveListMVV_LVA(Piece* squares[8][8], char x, char y)
{
	Piece::captureMoveListMVV_LVA.clear();
	if ((x - 1 >= 0) && (y + 1 <= 7)) {
		if ((squares[x - 1][y + 1] != nullptr) && (King::colour != squares[x - 1][y + 1]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 1, y + 1, squares[x - 1][y + 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x - 1 >= 0) && (y - 1 >= 0)) {
		if ((squares[x - 1][y - 1] != nullptr) && (King::colour != squares[x - 1][y - 1]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 1, y - 1, squares[x - 1][y - 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x + 1 <= 7) && (y + 1 <= 7)) {
		if ((squares[x + 1][y + 1] != nullptr) && (King::colour != squares[x + 1][y + 1]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 1, y + 1, squares[x + 1][y + 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x + 1 <= 7) && (y - 1 >= 0)) {
		if ((squares[x + 1][y - 1] != nullptr) && (King::colour != squares[x + 1][y - 1]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 1, y - 1, squares[x + 1][y - 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x + 1 <= 7)) {
		if ((squares[x + 1][y] != nullptr) && (King::colour != squares[x + 1][y]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x + 1, y, squares[x + 1][y]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((x - 1 >= 0)) {
		if ((squares[x - 1][y] != nullptr) && (King::colour != squares[x - 1][y]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x - 1, y, squares[x - 1][y]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((y + 1 <= 7)) {
		if ((squares[x][y + 1] != nullptr) && (King::colour != squares[x][y + 1]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x, y + 1, squares[x][y + 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	if ((y - 1 >= 0)) {
		if ((squares[x][y - 1] != nullptr) && (King::colour != squares[x][y - 1]->colour)) {
			Piece::captureMoveListMVV_LVA.push_back(std::tuple<char, char, char, char, int>(x, y, x, y - 1, squares[x][y - 1]->getValue() + getValue()));
			std::push_heap(captureMoveListMVV_LVA.begin(), captureMoveListMVV_LVA.end(), (colour == Colour::White) ? comp2 : comp1);
		}
	}
	return Piece::captureMoveListMVV_LVA;
}
