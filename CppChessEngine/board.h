#pragma once
#include "pieces.h"
#include <tuple>
#include <string>

enum class NodeType { Exact, Lower, Upper };

class ChessBoard;
struct MoveData;

struct HashEntry {
	unsigned long long zobristKey;
	int value;
	NodeType type;
	char distanceToLeaf = 0;
	std::tuple<char, char, char, char> preferredMove;
};

class Hashtable
{
private:
	static const int HASH_SIZE = 10000000;
	uint64_t hk_whitePawn[8][8];
	uint64_t hk_blackPawn[8][8];
	uint64_t hk_whiteRook[8][8];
	uint64_t hk_blackRook[8][8];
	uint64_t hk_whiteBishop[8][8];
	uint64_t hk_blackBishop[8][8];
	uint64_t hk_whiteKnight[8][8];
	uint64_t hk_blackKnight[8][8];
	uint64_t hk_whiteQueen[8][8];
	uint64_t hk_blackQueen[8][8];
	uint64_t hk_whiteKing[8][8];
	uint64_t hk_blackKing[8][8];
	uint64_t hk_whiteLRookCastling;
	uint64_t hk_blackLRookCastling;
	uint64_t hk_whiteRRookCastling;
	uint64_t hk_blackRRookCastling;
	uint64_t hk_enPassantSquare[8][8];
	uint64_t hk_sideToMove;
	HashEntry* table = new HashEntry[HASH_SIZE];
public:
	uint64_t hash;
	int hashHits;
	HashEntry getHashEntry();
	void setHashEntry(int depth, int value, int alpha, int beta, std::tuple<char, char, char, char> preferredMove);
	void initializeHash(ChessBoard* board, Colour colour);
	void updateHash(ChessBoard* board, MoveData move);
	Hashtable();
	~Hashtable();
};

struct MoveData
{
	std::tuple<char, char> orig;
	std::tuple<char, char> dest;
	std::tuple<char, char> prevEnPassantPawn;
	Piece* pieceMoved = nullptr;
	Piece* pieceTaken = nullptr;
	bool isWhiteLRookCastling = false;
	bool isWhiteRRookCastling = false;
	bool isBlackLRookCastling = false;
	bool isBlackRRookCastling = false;
	bool isPromotion = false;
	bool isEnPassant = false;
	bool validMove = false;
	int prevCounter_50move = 0;
};

class ChessBoard
{
private:
	void initializeOriginalSquares();
	Piece* originalSquares[8][8];
	int plyCount;
	std::vector<Piece*> promotedQueens;

	std::tuple<char, char> enPassantPawn = std::tuple<char,char>(127,127); //stores the location of a pawn allowing for an en passant capture
	int counter_50move = 0;
public:
	bool allowIllegalMoves = false;
	std::tuple<char, char> kingWhiteLocation;
	std::tuple<char, char> kingBlackLocation;
	bool improvedDrawDetection = true;

	void setKingScoreboard(bool endgame);
	Hashtable transpos_table;

	bool whiteCastled = false;
	bool blackCastled = false;
	Piece* squares[8][8];
	void resetToDebugBoard();
	void resetBoard();
	int getPlyCount();
	bool isChecked(Colour colour);
	bool squareAttackedBy(std::tuple<char, char> square, Colour colour);
	std::vector<std::tuple<char, char, char, char>> getPossibleCaptures(Colour colour);
    std::vector<std::tuple<char, char, char, char>> getPossibleMoves(Colour colour);
	MoveData moveTo(std::tuple<char, char> orig, std::tuple<char, char> dest);
	void undoMove(MoveData move);
	std::string getAlgebraicNotation(MoveData move);
	bool drawBy50Moves() { return counter_50move > 50; }
	Colour flip(Colour colour);
	ChessBoard(const ChessBoard& rhs) { //Copy constructor
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				Piece piece = *(rhs.squares[x][y]);
				ChessBoard::squares[x][y] = &piece;
			}
		}
		for (auto itr = rhs.promotedQueens.begin(); itr != promotedQueens.end(); ++itr) {
			Piece piece = **(itr);
			promotedQueens.push_back(&piece);
		}
		whiteCastled = rhs.whiteCastled;
		blackCastled = rhs.blackCastled;
		plyCount = rhs.plyCount;
		allowIllegalMoves = rhs.allowIllegalMoves;
		improvedDrawDetection = rhs.improvedDrawDetection;
		ChessBoard::transpos_table.initializeHash(this, Colour::White); //tbd
		initializeOriginalSquares();
	}
	ChessBoard();
};