#include "board.h"
#include <iostream>

void ChessBoard::resetToDebugBoard() {
	//placeholder used for debugging purposes
}

void ChessBoard::initializeOriginalSquares() {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			switch (y)
			{
			case 0:
				switch (x)
				{
				case 0:
					ChessBoard::originalSquares[x][y] = new Rook(Colour::White);
					break;
				case 1:
					ChessBoard::originalSquares[x][y] = new Knight(Colour::White);
					break;
				case 2:
					ChessBoard::originalSquares[x][y] = new Bishop(Colour::White);
					break;
				case 3:
					ChessBoard::originalSquares[x][y] = new Queen(Colour::White);
					break;
				case 4:
					ChessBoard::originalSquares[x][y] = new King(Colour::White);
					break;
				case 5:
					ChessBoard::originalSquares[x][y] = new Bishop(Colour::White);
					break;
				case 6:
					ChessBoard::originalSquares[x][y] = new Knight(Colour::White);
					break;
				case 7:
					ChessBoard::originalSquares[x][y] = new Rook(Colour::White);
					break;
				}
				break;
			case 1:
				ChessBoard::originalSquares[x][y] = new Pawn(Colour::White);
				break;
			case 6:
				ChessBoard::originalSquares[x][y] = new Pawn(Colour::Black);
				break;
			case 7:
				switch (x)
				{
				case 0:
					ChessBoard::originalSquares[x][y] = new Rook(Colour::Black);
					break;
				case 1:
					ChessBoard::originalSquares[x][y] = new Knight(Colour::Black);
					break;
				case 2:
					ChessBoard::originalSquares[x][y] = new Bishop(Colour::Black);
					break;
				case 3:
					ChessBoard::originalSquares[x][y] = new Queen(Colour::Black);
					break;
				case 4:
					ChessBoard::originalSquares[x][y] = new King(Colour::Black);
					break;
				case 5:
					ChessBoard::originalSquares[x][y] = new Bishop(Colour::Black);
					break;
				case 6:
					ChessBoard::originalSquares[x][y] = new Knight(Colour::Black);
					break;
				case 7:
					ChessBoard::originalSquares[x][y] = new Rook(Colour::Black);
					break;
				}
				break;
			default:
				ChessBoard::originalSquares[x][y] = nullptr;
			}
		}
	}
}


void ChessBoard::resetBoard() {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			ChessBoard::squares[x][y] = ChessBoard::originalSquares[x][y];
			if (ChessBoard::squares[x][y] != nullptr)
				ChessBoard::squares[x][y]->moveCount = 0;
		}
	}
	ChessBoard::kingWhiteLocation = std::tuple<char, char>(4, 0);
	ChessBoard::kingBlackLocation = std::tuple<char, char>(4, 7);
	for (auto p: ChessBoard::promotedQueens) {
		if (p != nullptr) delete p;
	}
	ChessBoard::promotedQueens.clear();
	ChessBoard::whiteCastled = false;
	ChessBoard::blackCastled = false;
	ChessBoard::plyCount = 0;
}

int ChessBoard::getPlyCount()
{
	return ChessBoard::plyCount;
}

/*Returns true if colour is checked*/
bool ChessBoard::isChecked(Colour colour) {
	for (int x = 0; x <= 7; x++) {
		for (int y = 0; y <= 7; y++) {
			if ((ChessBoard::squares[x][y] != nullptr) && (ChessBoard::squares[x][y]->colour != colour)) {
				std::vector<std::tuple<char, char>> moveList;
				moveList = ChessBoard::squares[x][y]->getCaptureMoveList(ChessBoard::squares, x, y);
				for (unsigned int i = 0; i < moveList.size(); i++) {
					if ((ChessBoard::kingBlackLocation == moveList[i]) && (colour == Colour::Black)) {
						return true;
					}
					if ((ChessBoard::kingWhiteLocation == moveList[i]) && (colour == Colour::White)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

/*RETURNS ALL POSSIBLE MOVES BY colour*/
std::vector<std::tuple<char, char, char, char>> ChessBoard::getPossibleMoves(Colour colour) {
	std::vector<std::tuple<char, char, char, char>> moveList;
	for (int x = 0; x <= 7; x++) {
		for (int y = 0; y <= 7; y++) {
			if ((ChessBoard::squares[x][y] != nullptr) && (ChessBoard::squares[x][y]->colour == colour)) {
				std::vector<std::tuple<char, char>> pieceMoveList;
				pieceMoveList = ChessBoard::squares[x][y]->getMoveList(ChessBoard::squares, x, y);
				for (unsigned int i = 0; i < pieceMoveList.size(); i++) {
					MoveData move = ChessBoard::moveTo(std::tuple<char, char>(x, y), pieceMoveList[i]);
					if (move.validMove) {
						moveList.push_back(std::tuple<char, char, char, char>(x, y, std::get<0>(pieceMoveList[i]), std::get<1>(pieceMoveList[i])));
						ChessBoard::undoMove(move);
					}
				}
			}
		}
	}
	return moveList;
}

/*RETURNS TRUE IF THE GIVEN SQUARE IS ATTACKED BY colour*/
bool ChessBoard::squareAttackedBy(std::tuple<char, char> square, Colour colour) {
	for (int x = 0; x <= 7; x++) {
		for (int y = 0; y <= 7; y++) {
			if ((ChessBoard::squares[x][y] != nullptr) && (ChessBoard::squares[x][y]->colour == colour)) {
				std::vector<std::tuple<char, char>> pieceMoveList;
				pieceMoveList = ChessBoard::squares[x][y]->getMoveList(ChessBoard::squares, x, y);
				for (unsigned int i = 0; i < pieceMoveList.size(); i++) {
					if (pieceMoveList[i] == square) {
						MoveData move = ChessBoard::moveTo(std::tuple<char, char>(x, y), pieceMoveList[i]);
						if (move.validMove) {
							ChessBoard::undoMove(move);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

/*RETURNS ALL POSSIBLE CAPTURES
Used for quiescence searches
*/
std::vector<std::tuple<char, char, char, char>> ChessBoard::getPossibleCaptures(Colour colour) {
	std::vector<std::tuple<char, char, char, char>> moveList;
	for (int x = 0; x <= 7; x++) {
		for (int y = 0; y <= 7; y++) {
			if ((ChessBoard::squares[x][y] != nullptr) && (ChessBoard::squares[x][y]->colour == colour)) {
				std::vector<std::tuple<char, char>> pieceMoveList;
				pieceMoveList = ChessBoard::squares[x][y]->getCaptureMoveList(ChessBoard::squares, x, y);
				for (unsigned int i = 0; i < pieceMoveList.size(); i++) {
					MoveData move = ChessBoard::moveTo(std::tuple<char, char>(x, y), pieceMoveList[i]);
					if (move.validMove) {
						moveList.push_back(std::tuple<char, char, char, char>(x, y, std::get<0>(pieceMoveList[i]), std::get<1>(pieceMoveList[i])));
						ChessBoard::undoMove(move);
					}
				}
			}
		}
	}
	return moveList;
}

/*MOVE-ROUTINE
Some checks for legality are skipped if allowIllegalMoves=true to speed up the engine.
This is unproblematic because engine-chosen moves are, by construction, within the permitted moveset
of each piece*/
MoveData ChessBoard::moveTo(std::tuple<char, char> orig, std::tuple<char, char> dest) {
	MoveData move;
	move.orig = orig;
	move.dest = dest;
	move.pieceMoved = ChessBoard::squares[std::get<0>(orig)][std::get<1>(orig)];
	move.pieceTaken = ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)];
	//attempting to move from an empty square => return invalid move
	if (move.pieceMoved == nullptr)
		return move;
	//CHECK IF DESTINATION WITHIN PERMITTED MOVESET
	if (!ChessBoard::allowIllegalMoves) {
		std::vector<std::tuple<char, char>> moveList = move.pieceMoved->getMoveList(ChessBoard::squares, std::get<0>(orig), std::get<1>(orig));
		bool foundMove = false;
		for (unsigned int i = 0; i < moveList.size(); i++) {
			if (moveList[i] == dest) {
				foundMove = true;
			}
		}
		if (!foundMove) {
			return move;
		}
	}
	//CASTLING LOGIC
	if (move.pieceMoved->getPieceType() == PieceType::King) {
		if ((std::get<0>(orig) == 4) && (std::get<1>(orig) == 0) && (std::get<0>(dest) == 2) && (std::get<1>(dest) == 0))
		{ //Attempted white queenside castling
			if ((ChessBoard::squares[0][0] != nullptr) 
				&& (ChessBoard::squares[0][0]->getPieceType() == PieceType::Rook)
				&& (ChessBoard::squares[0][0]->colour == Colour::White) 
				&& (ChessBoard::squares[0][0]->moveCount == 0)
				&& (move.pieceMoved->moveCount == 0) 
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(2, 0), Colour::Black) 
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(3, 0), Colour::Black)
				&& (!ChessBoard::squareAttackedBy(std::tuple<char, char>(4, 0), Colour::Black))) { //squares being empty is ensured by the generateMove-routine of the king
					move.isWhiteLRookCastling = true;
					move.validMove = true;
					ChessBoard::squares[3][0] = ChessBoard::squares[0][0];
					ChessBoard::squares[2][0] = ChessBoard::squares[4][0];
					ChessBoard::kingWhiteLocation = std::tuple<char, char>(2, 0);
					ChessBoard::squares[0][0] = nullptr;
					ChessBoard::squares[4][0] = nullptr;
					ChessBoard::squares[3][0]->moveCount++;
					ChessBoard::squares[2][0]->moveCount++;
					ChessBoard::whiteCastled = true;
					ChessBoard::plyCount++;
					move.pieceMoved->moveCount++;
			}
			else move.validMove = false; //invalid castling attempt
			return move;
		}
		if ((std::get<0>(orig) == 4) && (std::get<1>(orig) == 0) && (std::get<0>(dest) == 6) && (std::get<1>(dest) == 0))
		{ //Attempted white kingside castling
			if ((ChessBoard::squares[7][0] != nullptr)
				&& (ChessBoard::squares[7][0]->getPieceType() == PieceType::Rook)
				&& (ChessBoard::squares[7][0]->colour == Colour::White)
				&& (ChessBoard::squares[7][0]->moveCount == 0)
				&& (move.pieceMoved->moveCount == 0)
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(6, 0), Colour::Black)
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(5, 0), Colour::Black)
				&& (!ChessBoard::squareAttackedBy(std::tuple<char, char>(4, 0), Colour::Black))) { //squares being empty is ensured by the generateMove-routine of the king
				move.isWhiteRRookCastling = true;
				move.validMove = true;
				ChessBoard::squares[5][0] = ChessBoard::squares[7][0];
				ChessBoard::squares[6][0] = ChessBoard::squares[4][0];
				ChessBoard::kingWhiteLocation = std::tuple<char, char>(6, 0);
				ChessBoard::squares[7][0] = nullptr;
				ChessBoard::squares[4][0] = nullptr;
				ChessBoard::squares[6][0]->moveCount++;
				ChessBoard::squares[5][0]->moveCount++;
				ChessBoard::whiteCastled = true;
				ChessBoard::plyCount++;
				move.pieceMoved->moveCount++;
			}
			else move.validMove = false; //invalid castling attempt
			return move;
		}
		if ((std::get<0>(orig) == 4) && (std::get<1>(orig) == 7) && (std::get<0>(dest) == 6) && (std::get<1>(dest) == 7))
		{ //Attempted black kingside castling
			if ((ChessBoard::squares[7][7] != nullptr)
				&& (ChessBoard::squares[7][7]->getPieceType() == PieceType::Rook)
				&& (ChessBoard::squares[7][7]->colour == Colour::Black)
				&& (ChessBoard::squares[7][7]->moveCount == 0)
				&& (move.pieceMoved->moveCount == 0)
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(6, 7), Colour::White)
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(5, 7), Colour::White)
				&& (!ChessBoard::squareAttackedBy(std::tuple<char, char>(4, 7), Colour::White))) { //squares being empty is ensured by the generateMove-routine of the king
				move.isBlackRRookCastling = true;
				move.validMove = true;
				ChessBoard::squares[5][7] = ChessBoard::squares[7][7];
				ChessBoard::squares[6][7] = ChessBoard::squares[4][7];
				ChessBoard::kingBlackLocation = std::tuple<char, char>(6, 7);
				ChessBoard::squares[7][7] = nullptr;
				ChessBoard::squares[4][7] = nullptr;
				ChessBoard::squares[6][7]->moveCount++;
				ChessBoard::squares[5][7]->moveCount++;
				ChessBoard::blackCastled = true;
				ChessBoard::plyCount++;
				move.pieceMoved->moveCount++;
			}
			else move.validMove = false; //invalid castling attempt
			return move;
		}
		if ((std::get<0>(orig) == 4) && (std::get<1>(orig) == 7) && (std::get<0>(dest) == 2) && (std::get<1>(dest) == 7))
		{ //Attempted black queenside castling
			if ((ChessBoard::squares[0][7] != nullptr)
				&& (ChessBoard::squares[0][7]->getPieceType() == PieceType::Rook)
				&& (ChessBoard::squares[0][7]->colour == Colour::Black)
				&& (ChessBoard::squares[0][7]->moveCount == 0)
				&& (move.pieceMoved->moveCount == 0)
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(2, 7), Colour::White)
				&& !ChessBoard::squareAttackedBy(std::tuple<char, char>(3, 7), Colour::White)
				&& (!ChessBoard::squareAttackedBy(std::tuple<char, char>(4, 7), Colour::White))) { //squares being empty is ensured by the generateMove-routine of the king
				move.isBlackLRookCastling = true;
				move.validMove = true;
				ChessBoard::squares[3][7] = ChessBoard::squares[0][7];
				ChessBoard::squares[2][7] = ChessBoard::squares[4][7];
				ChessBoard::kingBlackLocation = std::tuple<char, char>(2, 7);
				ChessBoard::squares[0][7] = nullptr;
				ChessBoard::squares[4][7] = nullptr;
				ChessBoard::squares[3][7]->moveCount++;
				ChessBoard::squares[2][7]->moveCount++;
				ChessBoard::blackCastled = true;
				ChessBoard::plyCount++;
				move.pieceMoved->moveCount++;
			}
			else move.validMove = false; //invalid castling attempt
			return move;
		}
		//No castling attempts, ordinary move => update King location
		if (ChessBoard::kingBlackLocation == orig) {
			ChessBoard::kingBlackLocation = dest;
		}
		else if (ChessBoard::kingWhiteLocation == orig) {
			ChessBoard::kingWhiteLocation = dest;
		}
	}
	//update board: set squares to new values
	ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = move.pieceMoved;
	ChessBoard::squares[std::get<0>(orig)][std::get<1>(orig)] = nullptr;
	//DOES PLAYER CHECK HIMSELF
	if ((!ChessBoard::allowIllegalMoves) && (ChessBoard::isChecked(move.pieceMoved->colour))) {
		//revert the king's locations
		if (ChessBoard::kingBlackLocation == dest) {
			ChessBoard::kingBlackLocation = orig;
		}
		else if (ChessBoard::kingWhiteLocation == dest) {
			ChessBoard::kingWhiteLocation = orig;
		}
		//revert the board's squares
		ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = move.pieceTaken;
		ChessBoard::squares[std::get<0>(orig)][std::get<1>(orig)] = move.pieceMoved;
		return move;
	}
	//PAWN PROMOTIONS & EN PASSANT
	move.prevEnPassantPawn = ChessBoard::enPassantPawn;
	if (move.pieceMoved->getPieceType() == PieceType::Pawn) {
		if ((std::get<1>(dest) == 7) && (move.pieceMoved->colour == Colour::White)) { //Pawn Promotion
			ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = new Queen(Colour::White);
			ChessBoard::promotedQueens.push_back(ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)]);
			move.isPromotion = true;
		}
		else if ((std::get<1>(dest) == 0) && (move.pieceMoved->colour == Colour::Black)) { //Pawn Promotion
			ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = new Queen(Colour::Black);
			ChessBoard::promotedQueens.push_back(ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)]);
			move.isPromotion = true;
		}
		if (move.pieceMoved->colour == Colour::White) { //check for white en passant capture
			if ((std::get<0>(orig) - std::get<0>(dest) == 1) && (move.pieceTaken == nullptr)) { //potential left en passant
				if (ChessBoard::enPassantPawn == std::tuple<char, char>(std::get<0>(orig) - 1, std::get<1>(orig))) {
					move.isEnPassant = true;
					move.pieceTaken = ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) - 1];
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) - 1] = nullptr;
					ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
				}
				else {
					//invalid en passant, pawn attempted to be captured did not move in prev.turn - revert the board's squares
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = move.pieceTaken;
					ChessBoard::squares[std::get<0>(orig)][std::get<1>(orig)] = move.pieceMoved;
					return move;
				}
			}
			else if ((std::get<0>(orig) - std::get<0>(dest) == -1) && (move.pieceTaken == nullptr)) { //potential right en passant
				if (ChessBoard::enPassantPawn == std::tuple<char, char>(std::get<0>(orig) + 1, std::get<1>(orig))) {
					move.isEnPassant = true;
					move.pieceTaken = ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) - 1];
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) - 1] = nullptr;
					ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
				}
				else {
					//invalid en passant, pawn attempted to be captured did not move in prev.turn - revert the board's squares
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = move.pieceTaken;
					ChessBoard::squares[std::get<0>(orig)][std::get<1>(orig)] = move.pieceMoved;
					return move;
				}
			}
			else if (std::get<1>(orig) - std::get<1>(dest) == -2)
				ChessBoard::enPassantPawn = dest; //allow for en passant capture in the next turn
		}
		else {
			if ((std::get<0>(orig) - std::get<0>(dest) == 1) && (move.pieceTaken == nullptr)) { //potential left en passant
				if (ChessBoard::enPassantPawn == std::tuple<char, char>(std::get<0>(orig) - 1, std::get<1>(orig))) {
					move.isEnPassant = true;
					move.pieceTaken = ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) + 1];
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) + 1] = nullptr;
					ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
				}
				else {
					//invalid en passant, pawn attempted to be captured did not move in prev.turn - revert the board's squares
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = move.pieceTaken;
					ChessBoard::squares[std::get<0>(orig)][std::get<1>(orig)] = move.pieceMoved;
					return move;
				}
			}
			else if ((std::get<0>(orig) - std::get<0>(dest) == -1) && (move.pieceTaken == nullptr)) { //potential right en passant
				if (ChessBoard::enPassantPawn == std::tuple<char, char>(std::get<0>(orig) + 1, std::get<1>(orig))) {
					move.isEnPassant = true;
					move.pieceTaken = ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) + 1];
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest) + 1] = nullptr;
					ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
				}
				else {
					//invalid en passant, pawn attempted to be captured did not move in prev.turn - revert the board's squares
					ChessBoard::squares[std::get<0>(dest)][std::get<1>(dest)] = move.pieceTaken;
					ChessBoard::squares[std::get<0>(orig)][std::get<1>(orig)] = move.pieceMoved;
					return move;
				}
			}
			if (std::get<1>(orig) - std::get<1>(dest) == 2)
				ChessBoard::enPassantPawn = dest;
		}
	}
	else
		ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
	ChessBoard::plyCount++;
	move.pieceMoved->moveCount++;
	move.validMove = true;
	return move;
}

void ChessBoard::undoMove(MoveData move) {
	if (move.isWhiteLRookCastling) {
		ChessBoard::squares[3][0]->moveCount--;
		ChessBoard::squares[2][0]->moveCount--;
		ChessBoard::squares[0][0] = ChessBoard::squares[3][0];
		ChessBoard::squares[4][0] = ChessBoard::squares[2][0];
		ChessBoard::kingWhiteLocation = std::tuple<char, char>(4, 0);
		ChessBoard::squares[3][0] = nullptr;
		ChessBoard::squares[2][0] = nullptr;
		ChessBoard::whiteCastled = false;
	}
	else if (move.isWhiteRRookCastling) {
		ChessBoard::squares[5][0]->moveCount--;
		ChessBoard::squares[6][0]->moveCount--;
		ChessBoard::squares[7][0] = ChessBoard::squares[5][0];
		ChessBoard::squares[4][0] = ChessBoard::squares[6][0];
		ChessBoard::kingWhiteLocation = std::tuple<char, char>(4, 0);
		ChessBoard::squares[5][0] = nullptr;
		ChessBoard::squares[6][0] = nullptr;
		ChessBoard::whiteCastled = false;
	}
	else if (move.isBlackLRookCastling) {
		ChessBoard::squares[3][7]->moveCount--;
		ChessBoard::squares[2][7]->moveCount--;
		ChessBoard::squares[0][7] = ChessBoard::squares[3][7];
		ChessBoard::squares[4][7] = ChessBoard::squares[2][7];
		ChessBoard::kingBlackLocation= std::tuple<char, char>(4,7);
		ChessBoard::squares[3][7] = nullptr;
		ChessBoard::squares[2][7] = nullptr;
		ChessBoard::blackCastled = false;
	}
	else if (move.isBlackRRookCastling) {
		ChessBoard::squares[5][7]->moveCount--;
		ChessBoard::squares[6][7]->moveCount--;
		ChessBoard::squares[7][7] = ChessBoard::squares[5][7];
		ChessBoard::squares[4][7] = ChessBoard::squares[6][7];
		ChessBoard::kingBlackLocation = std::tuple<char, char>(4, 7);
		ChessBoard::squares[5][7] = nullptr;
		ChessBoard::squares[6][7] = nullptr;
		ChessBoard::blackCastled = false;
	}
	else if (move.isEnPassant) {
		if ((move.pieceMoved->colour == Colour::White) && (std::get<0>(move.orig) - std::get<0>(move.dest) == 1)) { //white left en passant
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest) - 1] = move.pieceTaken;
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest)] = nullptr;
			ChessBoard::squares[std::get<0>(move.orig)][std::get<1>(move.orig)] = move.pieceMoved;
			ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
		}
		if ((move.pieceMoved->colour == Colour::White) && (std::get<0>(move.orig) - std::get<0>(move.dest) == -1)) { //white right en passant
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest) - 1] = move.pieceTaken;
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest)] = nullptr;
			ChessBoard::squares[std::get<0>(move.orig)][std::get<1>(move.orig)] = move.pieceMoved;
			ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
		}
		if ((move.pieceMoved->colour == Colour::Black) && (std::get<0>(move.orig) - std::get<0>(move.dest) == 1)) { //black left en passant
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest) + 1] = move.pieceTaken;
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest)] = nullptr;
			ChessBoard::squares[std::get<0>(move.orig)][std::get<1>(move.orig)] = move.pieceMoved;
			ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
		}
		if ((move.pieceMoved->colour == Colour::Black) && (std::get<0>(move.orig) - std::get<0>(move.dest) == -1)) { //black right en passant
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest) + 1] = move.pieceTaken;
			ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest)] = nullptr;
			ChessBoard::squares[std::get<0>(move.orig)][std::get<1>(move.orig)] = move.pieceMoved;
			ChessBoard::enPassantPawn = std::tuple<char, char>(127, 127);
		}
	}
	else {
		if ((ChessBoard::kingBlackLocation == move.dest) && (move.pieceMoved->getPieceType() == PieceType::King) && (move.pieceMoved->colour == Colour::Black)) {
			ChessBoard::kingBlackLocation = move.orig;
		}
		else if ((ChessBoard::kingWhiteLocation == move.dest) && (move.pieceMoved->getPieceType() == PieceType::King) && (move.pieceMoved->colour == Colour::White)) {
			ChessBoard::kingWhiteLocation = move.orig;
		}
		if (move.isPromotion) {
			delete ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest)];
			ChessBoard::promotedQueens.pop_back();
		}
		ChessBoard::squares[std::get<0>(move.dest)][std::get<1>(move.dest)] = move.pieceTaken;
		ChessBoard::squares[std::get<0>(move.orig)][std::get<1>(move.orig)] = move.pieceMoved;
	}
	move.pieceMoved->moveCount--;
	ChessBoard::plyCount--;
	ChessBoard::enPassantPawn = move.prevEnPassantPawn;
}

ChessBoard::ChessBoard() {
	ChessBoard::initializeOriginalSquares();
	ChessBoard::resetBoard();
	ChessBoard::promotedQueens.reserve(16);
	ChessBoard::resetBoard();
}