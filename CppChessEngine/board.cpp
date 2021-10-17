#include "board.h"
#include <random>

/*Class constructor.
Uses a Mersenne twister engine to assign uniformly distributed pseudorandom integers
to each of the piece/move characteristics used in generating a Zobrist hash.*/
Hashtable::Hashtable() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned long long> dis(0, ULLONG_MAX);
	for (int x = 0; x <= 7; x++)
		for (int y = 0; y <= 7; y++) {
			hk_whitePawn[x][y] = dis(gen);
			hk_blackPawn[x][y] = dis(gen);
			hk_whiteRook[x][y] = dis(gen);
			hk_blackRook[x][y] = dis(gen);
			hk_whiteBishop[x][y] = dis(gen);
			hk_blackBishop[x][y] = dis(gen);
			hk_whiteKnight[x][y] = dis(gen);
			hk_blackKnight[x][y] = dis(gen);
			hk_whiteQueen[x][y] = dis(gen);
			hk_blackQueen[x][y] = dis(gen);
			hk_whiteKing[x][y] = dis(gen);
			hk_blackKing[x][y] = dis(gen);
			hk_enPassantSquare[x][y] = dis(gen);
		}
	hk_sideToMove = dis(gen);
	hk_whiteLRookCastling = dis(gen);
	hk_blackLRookCastling = dis(gen);
	hk_whiteRRookCastling = dis(gen);
	hk_blackRRookCastling = dis(gen);
	hashHits = 0;
	hash = 0;
	table = new HashEntry[HASH_SIZE];
}

Hashtable::~Hashtable() {
	delete Hashtable::table;
}

void Hashtable::depreciateHashtable() {
	hashHits = 0;
	for (int i = 0; i < Hashtable::HASH_SIZE; i++) {
		Hashtable::table[i].distanceToLeaf = 0;
	}
}

HashEntry Hashtable::getHashEntry() {
	return Hashtable::table[Hashtable::hash % Hashtable::HASH_SIZE];
}

/*Writes an entry to the hashtable;
based on the valuation and the alpha/beta window, the type of node is stored as well.*/
void Hashtable::setHashEntry(int distanceToLeaf, int value, int alpha, int beta, std::tuple<char, char, char, char> preferredMove) {
	unsigned long index = Hashtable::hash % Hashtable::HASH_SIZE;

	if (((value > alpha) && (value < beta))) {
		Hashtable::table[index].type = NodeType::Exact;
		Hashtable::table[index].value = value;
	}
	else if (value >= beta) {
		Hashtable::table[index].type = NodeType::Upper;
		Hashtable::table[index].value = value;
	}
	else if (value <= alpha) {
		Hashtable::table[index].type = NodeType::Lower;
		Hashtable::table[index].value = value;
	}

	Hashtable::table[index].distanceToLeaf = distanceToLeaf;
	Hashtable::table[index].zobristKey = Hashtable::hash;
	Hashtable::table[index].preferredMove = preferredMove;

}

/*Resets the hashtable data*/
void Hashtable::clearHashtable() {
	hashHits = 0;
	for (int i = 0; i < Hashtable::HASH_SIZE; i++) {
		Hashtable::table[i].distanceToLeaf = 0;
		Hashtable::table[i].zobristKey = 0;
	}
}

/*Initializes a hash based on the current board*/
void Hashtable::initializeHash(ChessBoard* board, Colour colour) {
	hash = 0;
	for (int x = 0; x <= 7; x++)
		for (int y = 0; y <= 7; y++) {
			if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == Colour::White)) {
				if (board->squares[x][y]->getPieceType() == PieceType::Pawn)
					hash = hash ^ hk_whitePawn[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Knight)
					hash = hash ^ hk_whiteKnight[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Bishop)
					hash = hash ^ hk_whiteBishop[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Rook)
					hash = hash ^ hk_whiteRook[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Queen)
					hash = hash ^ hk_whiteQueen[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::King)
					hash = hash ^ hk_whiteKing[x][y];
			}
			else if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == Colour::Black)) {
				if (board->squares[x][y]->getPieceType() == PieceType::Pawn)
					hash = hash ^ hk_blackPawn[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Knight)
					hash = hash ^ hk_blackKnight[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Bishop)
					hash = hash ^ hk_blackBishop[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Rook)
					hash = hash ^ hk_blackRook[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::Queen)
					hash = hash ^ hk_blackQueen[x][y];
				else if (board->squares[x][y]->getPieceType() == PieceType::King)
					hash = hash ^ hk_blackKing[x][y];
			}
		}
	hash = hash ^ hk_sideToMove;
}

/*Incremental Zobrish hash update function:
Based on the random quantities assigned in the constructor and stored in the hk_-variables,
the hash is updated.*/
void Hashtable::updateHash(ChessBoard* board, MoveData move) {
	hash ^= hk_sideToMove;
	if (move.pieceMoved->colour == Colour::White) {
		switch (move.pieceMoved->getPieceType()) {
		case PieceType::Pawn:
			if (move.isPromotion)
				hash ^= hk_whitePawn[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_whiteQueen[std::get<0>(move.dest)][std::get<1>(move.dest)];
			else
				hash ^= hk_whitePawn[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_whitePawn[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Knight:
			hash ^= hk_whiteKnight[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_whiteKnight[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Bishop:
			hash ^= hk_whiteBishop[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_whiteBishop[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Rook:
			hash ^= hk_whiteRook[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_whiteRook[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Queen:
			hash ^= hk_whiteQueen[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_whiteQueen[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::King:
			hash ^= hk_whiteKing[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_whiteKing[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		}
		if (move.pieceTaken != nullptr) {
			switch (move.pieceTaken->getPieceType()) {
			case PieceType::Pawn:
				hash ^= hk_blackPawn[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Knight:
				hash ^= hk_blackKnight[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Bishop:
				hash ^= hk_blackBishop[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Rook:
				hash ^= hk_blackRook[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Queen:
				hash ^= hk_blackQueen[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::King:
				hash ^= hk_blackKing[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			}
		}
		if (move.isWhiteLRookCastling)
			hash ^= hk_whiteLRookCastling;
		else if (move.isWhiteRRookCastling)
			hash ^= hk_whiteRRookCastling;
	}
	else {
		switch (move.pieceMoved->getPieceType()) {
		case PieceType::Pawn:
			if (move.isPromotion)
				hash ^= hk_blackPawn[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_blackQueen[std::get<0>(move.dest)][std::get<1>(move.dest)];
			else
				hash ^= hk_blackPawn[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_blackPawn[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Knight:
			hash ^= hk_blackKnight[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_blackKnight[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Bishop:
			hash ^= hk_blackBishop[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_blackBishop[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Rook:
			hash ^= hk_blackRook[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_blackRook[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::Queen:
			hash ^= hk_blackQueen[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_blackQueen[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		case PieceType::King:
			hash ^= hk_blackKing[std::get<0>(move.orig)][std::get<1>(move.orig)] ^ hk_blackKing[std::get<0>(move.dest)][std::get<1>(move.dest)];
			break;
		}
		if (move.pieceTaken != nullptr) {
			switch (move.pieceTaken->getPieceType()) {
			case PieceType::Pawn:
				hash ^= hk_whitePawn[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Knight:
				hash ^= hk_whiteKnight[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Bishop:
				hash ^= hk_whiteBishop[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Rook:
				hash ^= hk_whiteRook[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::Queen:
				hash ^= hk_whiteQueen[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			case PieceType::King:
				hash ^= hk_whiteKing[std::get<0>(move.dest)][std::get<1>(move.dest)];
				break;
			}
		}
		if (move.isBlackLRookCastling)
			hash = hash ^ hk_blackLRookCastling;
		else if (move.isBlackRRookCastling)
			hash = hash ^ hk_blackRRookCastling;
	}
	if (move.prevEnPassantPawn != std::tuple<char, char>(127, 127))
		hash = hash ^ hk_enPassantSquare[std::get<0>(move.prevEnPassantPawn)][std::get<1>(move.prevEnPassantPawn)];
}

/**/
void ChessBoard::setKingScoreboard(bool endgame)
{
	if (endgame) {
		(squares[std::get<0>(kingWhiteLocation)][std::get<1>(kingWhiteLocation)])->setEndgameScoreboard();
		(squares[std::get<0>(kingBlackLocation)][std::get<1>(kingBlackLocation)])->setEndgameScoreboard();
	}
	else {
		(squares[std::get<0>(kingWhiteLocation)][std::get<1>(kingWhiteLocation)])->setNormalScoreboard();
		(squares[std::get<0>(kingBlackLocation)][std::get<1>(kingBlackLocation)])->setNormalScoreboard();
	}
}

void ChessBoard::resetToDebugBoard() {
	//placeholder used for debugging purposes
	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++) 
			ChessBoard::squares[x][y] = nullptr;
	ChessBoard::squares[0][0] = new King(Colour::Black);
	kingBlackLocation = std::tuple<char, char>(0, 0);
	ChessBoard::squares[1][1] = new Bishop(Colour::Black);
	ChessBoard::squares[0][2] = new Knight(Colour::Black);
	ChessBoard::squares[6][6] = new King(Colour::White);
	kingWhiteLocation = std::tuple<char, char>(6, 6);
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
	ChessBoard::promotedQueens.clear();
	ChessBoard::whiteCastled = false;
	ChessBoard::blackCastled = false;
	ChessBoard::plyCount = 0;
	ChessBoard::counter_50move = 0;
	ChessBoard::transpos_table.initializeHash(this, Colour::White);
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
				&& (!ChessBoard::squareAttackedBy(std::tuple<char, char>(4, 0), Colour::Black)
				&& (ChessBoard::squares[1][0] == nullptr) && (ChessBoard::squares[2][0] == nullptr) && (ChessBoard::squares[3][0] == nullptr))) { //squares being empty is ensured by the generateMove-routine of the king
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
				&& (!ChessBoard::squareAttackedBy(std::tuple<char, char>(4, 7), Colour::White)
				&& (ChessBoard::squares[1][7] == nullptr) && (ChessBoard::squares[2][7] == nullptr) && (ChessBoard::squares[3][7] == nullptr))) { //squares being empty is ensured by the generateMove-routine of the king
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
	//IMPROVED DRAW DETECTION
	//Second condition to improve draw detection - if the king is moved, check if it moves itself into an illegal check
	if (((!ChessBoard::allowIllegalMoves) && (ChessBoard::isChecked(move.pieceMoved->colour))) || 
		(improvedDrawDetection && (move.pieceMoved->getPieceType() == PieceType::King) && (ChessBoard::isChecked(move.pieceMoved->colour)))) {
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
	transpos_table.updateHash(this, move);
	move.prevCounter_50move = counter_50move;
	if ((move.pieceTaken != nullptr) || (move.pieceMoved->getPieceType() == PieceType::Pawn))
		counter_50move = 0; 
	else
		counter_50move++;
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
	transpos_table.updateHash(this, move);
	counter_50move = move.prevCounter_50move;
}

char numToLetter_(char coord) {
	switch (coord)
	{
	case 0:
		return 'a';
		break;
	case 1:
		return 'b';
		break;
	case 2:
		return 'c';
		break;
	case 3:
		return 'd';
		break;
	case 4:
		return 'e';
		break;
	case 5:
		return 'f';
		break;
	case 6:
		return 'g';
		break;
	case 7:
		return 'h';
		break;
	default:
		return 127;
	}
}

Colour ChessBoard::flip(Colour colour) {
	if (colour == Colour::Black)
		return Colour::White;
	else if (colour == Colour::White)
		return Colour::Black;
	else
		return colour;
}

std::string ChessBoard::getAlgebraicNotation(MoveData move)
{
	std::string moveStr = "";
	if (move.pieceMoved == nullptr)
		return moveStr;
	if (move.isWhiteLRookCastling || move.isBlackLRookCastling) {
		moveStr = "O-O-O";
	}
	else if (move.isWhiteRRookCastling || move.isBlackRRookCastling) {
		moveStr = "O-O";
	}
	else {
		if (move.pieceMoved->getPieceType() != PieceType::Pawn) {
			moveStr += move.pieceMoved->getSymbol();
			moveStr += numToLetter_(std::get<0>(move.orig)) + std::to_string(std::get<1>(move.orig)+1);
		}
		else if (move.pieceTaken == nullptr) {
			moveStr += numToLetter_(std::get<0>(move.orig)) + std::to_string(std::get<1>(move.dest) + 1);
		}
		else
			moveStr += numToLetter_(std::get<0>(move.orig)) + std::to_string(std::get<1>(move.orig) + 1);
		if (move.pieceTaken != nullptr)
			moveStr += "x";
		if ((move.pieceMoved->getPieceType() != PieceType::Pawn) || (move.pieceTaken != nullptr))
			moveStr += numToLetter_(std::get<0>(move.dest)) + std::to_string(std::get<1>(move.dest) + 1);
		if (move.isPromotion)
			moveStr += "Q";
		if (move.isEnPassant)
			moveStr += " e.p.";
	}
	if (isChecked(flip(move.pieceMoved->colour))) {
		if (getPossibleMoves(flip(move.pieceMoved->colour)).empty())
			moveStr += "#";
		else
			moveStr += "+";
	}

	return moveStr;
}

ChessBoard::ChessBoard() {
	ChessBoard::initializeOriginalSquares();
	ChessBoard::resetBoard();
	ChessBoard::promotedQueens.reserve(16);
}