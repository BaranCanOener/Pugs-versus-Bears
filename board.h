#pragma once
#include "pieces.h"
#include <tuple>

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
};

class ChessBoard
{
private:
	void initializeOriginalSquares();
	Piece* originalSquares[8][8];
	int plyCount;
	std::vector<Piece*> promotedQueens;
	std::tuple<char, char> kingWhiteLocation;
	std::tuple<char, char> kingBlackLocation;
	std::tuple<char, char> enPassantPawn = std::tuple<char,char>(127,127); //stores the location of a pawn allowing for an en passant capture
public:
	bool allowIllegalMoves = false;
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
	ChessBoard();
};
