#pragma once
#include "board.h"
#include "pieces.h"
#include <ctime>
#include <functional>

class Engine
{
private:
	//control variables for iterative deepening
	std::clock_t startTime = 0;
	int updateAfterNodes = 50000;
	bool iterativeDeepening = false;
	bool abortSearch = false;

	int randDouble;

	//tracks the amount of nodes searched
	int nodes = 0;
	int quiescenceNodes = 0;
	
	bool allowNull = true;

	const int WHITE_MIN = -999999; //white maximizes score, i.e. this is the worst case
	const int BLACK_MAX = 999999; //black minimizes score, i.e. this is the worst case
	void updateSearchProgress(ChessBoard* board);
	void calculateUpdateThreshold();
	int quiescenceSearch(ChessBoard* board, Colour colour, char depth, int alpha, int beta);
	int quiescenceSearch2(ChessBoard* board, Colour colour, char depth, int alpha, int beta, std::tuple<char, char, char, char> firstMove);
	int quiescenceSearch_moveSorting(ChessBoard* board, Colour colour, char depth, int alpha, int beta);
	int alphaBeta(ChessBoard* board, Colour colour, char depth, int alpha, int beta);
	int alphaBeta_preferredMove(ChessBoard* board, Colour colour, char depth, int alpha, int beta, std::tuple<char, char, char, char> firstMove);
public:
	int quiescenceLimit = 2;

	bool nullmove = false;
	int R = 2;

	bool useKingEndgameScoreboard = true;

	bool MVV_LVAquiescence = true;

	int endgameThreshold = 1300;
	bool isEndgame(ChessBoard* board);

	int depthLimit = 5;
	float timeLimit = 5;
	int randLimit = 20;
	bool randomness = true;
	bool useHashtable = true;
	std::vector<std::tuple<char, char, char, char>> optimalTurnSequence;
	std::vector<MoveData> optimalTurnSequence_moves;
	int optimalValue;
	bool seachAborted();
	std::function<void(Engine*, ChessBoard*)> updateFct; //Function to call periodically while computing turn 
	int getNodes();
	int getQuiescenceNodes();
	double getTimePassed();
	int evalHeuristic(ChessBoard* board);
	int calculateMove_fixedDepth(ChessBoard* board, Colour colour);
	int calculateMove_iterativeDeepening(ChessBoard* board, Colour colour);
	void calculateMove_random(ChessBoard* board, Colour colour);
	Engine();
};

