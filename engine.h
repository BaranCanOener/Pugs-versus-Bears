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

	const int WHITE_MIN = -999999; //white maximizes score, i.e. this is the worst case
	const int BLACK_MAX = 999999; //black minimizes score, i.e. this is the worst case
	void updateSearchProgress(ChessBoard* board);
	void calculateUpdateThreshold();
	int quiescenceSearch(ChessBoard* board, Colour colour, char depth, int alpha, int beta);
	int alphaBeta(ChessBoard* board, Colour colour, char depth, int alpha, int beta);
	int alphaBeta_depth0(ChessBoard* board, Colour colour, int alpha, int beta, std::tuple<char, char, char, char> firstMove);
public:
	int quiescenceLimit = 2;
	int depthLimit = 5;
	float timeLimit = 5;
	int randLimit = 50;
	bool randomness = true;
	std::vector<std::tuple<char, char, char, char>> optimalTurnSequence;
	int optimalValue;
	bool seachAborted();
	std::function<void(Engine*)> updateFct; //Function to call periodically while computing turn 
	int getNodes();
	int getQuiescenceNodes();
	double getTimePassed();
	int evalHeuristic(ChessBoard* board);
	int calculateMove_fixedDepth(ChessBoard* board, Colour colour);
	int calculateMove_iterativeDeepening(ChessBoard* board, Colour colour);
	void calculateMove_random(ChessBoard* board, Colour colour);
	Engine();
};

