#pragma once
#include "engine.h"
#include <algorithm>
#include <random>
#include <ctime>

int Partition(int start, int end, std::vector<std::tuple<char,char,char,char, int>>& moveList) {

	int pivot = end;
	int j = start;
	for (int i = start; i < end; ++i) {
		if (std::get<4>(moveList[i]) < std::get<4>(moveList[pivot])) {
			std::swap(moveList[i], moveList[j]);
			++j;
		}
	}
	std::swap(moveList[j], moveList[pivot]);
	return j;

}

void Quicksort(int start, int end, std::vector<std::tuple<char, char, char, char, int>>& moveList) {

	if (start < end) {
		int p = Partition(start, end, moveList);
		Quicksort(start, p - 1, moveList);
		Quicksort(p + 1, end, moveList);
	}

}

bool comp_2(std::tuple<char, char, char, char, int> arg1, std::tuple<char, char, char, char, int> arg2) {
	return std::get<4>(arg1) > std::get<4>(arg2);
}

bool comp_1(std::tuple<char, char, char, char, int> arg1, std::tuple<char, char, char, char, int> arg2) {
	return std::get<4>(arg1) < std::get<4>(arg2);
}

/*Performs a search of all capture moves up to the depth prescribed by quiescenceLimit*/
int Engine::quiescenceSearch_moveSorting(ChessBoard* board, Colour colour, char depth, int alpha, int beta) {
	Engine::updateSearchProgress(board);
	if (useHashtable) {
		HashEntry data = board->transpos_table.getHashEntry();
		if ((data.zobristKey == board->transpos_table.getHash())) {
			board->transpos_table.incrementHashHits();
			if ((data.distanceToLeaf >= (depthLimit + quiescenceLimit - depth))) {
				if ((data.type == NodeType::Exact))
					return data.value;
				else if ((data.type == NodeType::Lower) && (data.value < beta))
					beta = data.value;
				else if ((data.type == NodeType::Upper) && (data.value > alpha))
					alpha = data.value;
			}

			return Engine::quiescenceSearch2(board, colour, depth, alpha, beta, data.preferredMove);
		}
	}
	int value = Engine::evalHeuristic(board);
	if (depth >= Engine::depthLimit + Engine::quiescenceLimit)
		return value;
	else {


		MoveData move;
		std::vector<std::tuple<char, char, char, char, int>> moveList, pieceMoveList;
		//DEBUG - dummy variable
		std::tuple<char, char, char, char> preferredMove;
		if (colour == Colour::White) {
			if (value >= beta) return value;
			if (alpha < value) alpha = value;
			for (char x = 0; x <= 7; x++)
				for (char y = 0; y <= 7; y++)
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						pieceMoveList = board->squares[x][y]->getCaptureMoveListMVV_LVA(board->squares, x, y); //Only capture-moves are investigated
						while (!pieceMoveList.empty()) {

							std::pop_heap(pieceMoveList.begin(), pieceMoveList.end(), comp_2);
							moveList.push_back(pieceMoveList.back());
							std::push_heap(moveList.begin(), moveList.end(), comp_2);
							pieceMoveList.pop_back();
						}
					}
		
			while (!moveList.empty()) {
				std::pop_heap(moveList.begin(), moveList.end(), comp_2);
				std::tuple<char, char, char, char, int> coord = moveList.back();
				moveList.pop_back();
				Engine::quiescenceNodes++;
				char xFrom = std::get<0>(coord);
				char yFrom = std::get<1>(coord);
				char xTo = std::get<2>(coord);
				char yTo = std::get<3>(coord);
				move = board->moveTo(std::tuple<char, char>(xFrom, yFrom), std::tuple<char, char>(xTo, yTo));
				if (move.validMove) {
					
					if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
						board->undoMove(move);
						
						return Engine::BLACK_MAX;
					}
					int newValue = Engine::quiescenceSearch(board, Colour::Black, depth + 1, alpha, beta);
					if (newValue > value)
						value = newValue;

					board->undoMove(move);
					
					if (value >= beta)
						return value;

					if (alpha < value)
						alpha = value;
				}
			}
			return value;
		}
		else {
			if (value <= alpha) return value;
			if (value < beta) beta = value;
			for (char x = 0; x <= 7; x++)
				for (char y = 0; y <= 7; y++)
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						pieceMoveList = board->squares[x][y]->getCaptureMoveListMVV_LVA(board->squares, x, y); //Only capture-moves are investigated
						while (!pieceMoveList.empty()) {
							std::pop_heap(pieceMoveList.begin(), pieceMoveList.end(), comp_1);
							moveList.push_back(pieceMoveList.back());
							std::push_heap(moveList.begin(), moveList.end(), comp_1);
							pieceMoveList.pop_back();
						}

					}

			while (!moveList.empty()) {
				std::pop_heap(moveList.begin(), moveList.end(), comp_1);
				std::tuple<char, char, char, char, int> coord = moveList.back();
				moveList.pop_back();
				Engine::quiescenceNodes++;
				char xFrom = std::get<0>(coord);
				char yFrom = std::get<1>(coord);
				char xTo = std::get<2>(coord);
				char yTo = std::get<3>(coord);
				move = board->moveTo(std::tuple<char, char>(xFrom, yFrom), std::tuple<char, char>(xTo, yTo));
				if (move.validMove) {
					
					if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
						board->undoMove(move);
						
						return Engine::WHITE_MIN;
					}
					int newValue = Engine::quiescenceSearch(board, Colour::White, depth + 1, alpha, beta);
					if (newValue < value)
						value = newValue;
					board->undoMove(move);
					
					if (value <= alpha) 
						return value;

					if (value < beta)
						beta = value;
				}
			}
		}
	}
	return value;
}


/*This routine is responsible for aborting the iterative deepening search after the time limit has been exceeded
and calls updateFct intermittently*/
void Engine::updateSearchProgress(ChessBoard* board) {
	if ((Engine::nodes+Engine::quiescenceNodes) % updateAfterNodes == 0) {
		if (Engine::updateFct!=nullptr)
			Engine::updateFct(this, board);
		if (Engine::iterativeDeepening) {
			double timePassed = (double(std::clock()) - double(Engine::startTime)) / CLOCKS_PER_SEC;
			if (timePassed > Engine::timeLimit) Engine::abortSearch = true;
		}
	}
}

/*Calibrates the frequency at which updateFct is called*/
void Engine::calculateUpdateThreshold() {
	int nodesPerSecond = (Engine::nodes + Engine::quiescenceNodes) / Engine::getTimePassed();
	//20 updates per Second
	Engine::updateAfterNodes = std::max(nodesPerSecond / 20, 10000);
}

/*Performs a search of all capture moves up to the depth prescribed by quiescenceLimit*/
int Engine::quiescenceSearch(ChessBoard* board, Colour colour, char depth, int alpha, int beta) {
	Engine::updateSearchProgress(board);
	if (useHashtable) {
		HashEntry data = board->transpos_table.getHashEntry();
		if ((data.zobristKey == board->transpos_table.getHash())) {
			board->transpos_table.incrementHashHits();
			if ((data.distanceToLeaf >= (depthLimit + quiescenceLimit - depth))) {
				if ((data.type == NodeType::Exact))
					return data.value;
				else if ((data.type == NodeType::Lower) && (data.value < beta))
					beta = data.value;
				else if ((data.type == NodeType::Upper) && (data.value > alpha))
					alpha = data.value;
			}

			return Engine::quiescenceSearch2(board, colour, depth, alpha, beta, data.preferredMove);
		}
	}
	int value = Engine::evalHeuristic(board);
	if (depth >= Engine::depthLimit + Engine::quiescenceLimit)
		return value;
	else {

		MoveData move;
		std::vector<std::tuple<char, char>> moveList;
		//DEBUG - dummy variable
		std::tuple<char, char, char, char> preferredMove;
		if (colour == Colour::White) {
			if (value >= beta) return value;
			if (alpha < value) alpha = value;
			for (char x = 0; x <= 7; x++) {
				for (char y = 0; y <= 7; y++) {
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						moveList = board->squares[x][y]->getCaptureMoveList(board->squares, x, y); //Only capture-moves are investigated
						for (unsigned int i = 0; i < moveList.size(); i++) {
							Engine::quiescenceNodes++;
							move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);
							if (move.validMove) {
								
								if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
									board->undoMove(move);
									
									return Engine::BLACK_MAX;
								}
								int newValue = Engine::quiescenceSearch(board, Colour::Black, depth + 1, alpha, beta);
								if (newValue > value) {
									value = newValue;
									preferredMove = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
								}
								board->undoMove(move);
								
								if (value >= beta)
									return value;
								if (alpha < value) alpha = value;
							}
						}
					}
				}
			}
			return value;
		}
		else {
			if (value <= alpha) return value;
			if (value < beta) beta = value;
			for (char x = 0; x <= 7; x++) {
				for (char y = 0; y <= 7; y++) {
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						moveList = board->squares[x][y]->getCaptureMoveList(board->squares, x, y);
						for (unsigned int i = 0; i < moveList.size(); i++) {
							Engine::quiescenceNodes++;
							move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);
							if (move.validMove) {
								
								if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
									board->undoMove(move);
									
									return Engine::WHITE_MIN;
								}
								int newValue = Engine::quiescenceSearch(board, Colour::White, depth + 1, alpha, beta);
								if (newValue < value) {
									value = newValue;
									preferredMove = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
								}
								board->undoMove(move);
								
								if (value <= alpha) 
									return value;
								if (value < beta) beta = value;
							}
						}
					}
				}
			}
		}
	}
	return value;
}

/*Performs a search of all capture moves up to the depth prescribed by quiescenceLimit*/
int Engine::quiescenceSearch2(ChessBoard* board, Colour colour, char depth, int alpha, int beta, std::tuple<char, char, char, char> firstMove) {
	Engine::updateSearchProgress(board);
	int value = Engine::evalHeuristic(board);
	if (depth >= Engine::depthLimit + Engine::quiescenceLimit)
		return value;
	else {

		MoveData move;
		std::vector<std::tuple<char, char>> moveList;
		//DEBUG - dummy variable
		std::tuple<char, char, char, char> preferredMove;

		Engine::quiescenceNodes++;
		if (colour == Colour::White) {
			if (value >= beta) return value;
			if (alpha < value) alpha = value;

			std::tuple<char, char> orig = std::tuple<char, char>(std::get<0>(firstMove), std::get<1>(firstMove));
			std::tuple<char, char> dest = std::tuple<char, char>(std::get<2>(firstMove), std::get<3>(firstMove));
			move = board->moveTo(orig, dest);
			if (move.validMove) {
				
				if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
					board->undoMove(move);
					
					return Engine::BLACK_MAX;
				}
				value = Engine::quiescenceSearch(board, Colour::Black, depth + 1, alpha, beta);
				if (alpha < value) {
					alpha = value;
					Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(std::get<0>(firstMove), std::get<1>(firstMove), std::get<2>(firstMove), std::get<3>(firstMove));
					//Engine::optimalTurnSequence_moves[depth] = move;
				}
				board->undoMove(move);
				
				if (Engine::abortSearch) return value;
			}

			for (char x = 0; x <= 7; x++) {
				for (char y = 0; y <= 7; y++) {
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						moveList = board->squares[x][y]->getCaptureMoveList(board->squares, x, y); //Only capture-moves are investigated
						for (unsigned int i = 0; i < moveList.size(); i++) {
							if ((std::get<0>(moveList[i]) == std::get<2>(firstMove)) && (std::get<1>(moveList[i]) == std::get<3>(firstMove)) && (x == std::get<0>(firstMove)) && (y == std::get<1>(firstMove)))
								continue;
							Engine::quiescenceNodes++;
							move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);
							if (move.validMove) {
								
								if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
									board->undoMove(move);
									
									return Engine::BLACK_MAX;
								}
								value = Engine::quiescenceSearch(board, Colour::Black, depth + 1, alpha, beta);
								board->undoMove(move);
								
								if (value >= beta) {
									return value;
								}
								if (alpha < value) {
									alpha = value;
									//Engine::optimalTurnSequence_moves[depth] = move;
								}
							}
						}
					}
				}
			}
			return value;
		}
		else {
			if (value <= alpha) return value;
			if (value < beta) beta = value;

			std::tuple<char, char> orig = std::tuple<char, char>(std::get<0>(firstMove), std::get<1>(firstMove));
			std::tuple<char, char> dest = std::tuple<char, char>(std::get<2>(firstMove), std::get<3>(firstMove));
			move = board->moveTo(orig, dest);
			if (move.validMove) {
				
				if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
					board->undoMove(move);
					
					return Engine::WHITE_MIN;
				}
				value = Engine::quiescenceSearch(board, Colour::White, depth + 1, alpha, beta);
				if (value < beta) {
					Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(std::get<0>(firstMove), std::get<1>(firstMove), std::get<2>(firstMove), std::get<3>(firstMove));
					//Engine::optimalTurnSequence_moves[depth] = move;
					beta = value;
				}
				board->undoMove(move);
				
				if (Engine::abortSearch) {
					return value;
				}
			}

			for (char x = 0; x <= 7; x++) {
				for (char y = 0; y <= 7; y++) {
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						moveList = board->squares[x][y]->getCaptureMoveList(board->squares, x, y);
						for (unsigned int i = 0; i < moveList.size(); i++) {
							if ((std::get<0>(moveList[i]) == std::get<2>(firstMove)) && (std::get<1>(moveList[i]) == std::get<3>(firstMove)) && (x == std::get<0>(firstMove)) && (y == std::get<1>(firstMove)))
								continue;
							Engine::quiescenceNodes++;
							move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);
							if (move.validMove) {
								
								if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
									board->undoMove(move);
									
									return Engine::WHITE_MIN;
								}
								value = Engine::quiescenceSearch(board, Colour::White, depth + 1, alpha, beta);
								board->undoMove(move);
								
								if (value <= alpha) {
									return value;
								}
								if (value < beta) {
									//Engine::optimalTurnSequence_moves[depth] = move;
									beta = value;
								}
							}
						}
					}
				}
			}
		}
	}
	return value;
}

int Engine::alphaBeta(ChessBoard* board, Colour colour, char depth, int alpha, int beta) {
	Engine::updateSearchProgress(board);
	if (board->drawBy50Moves())
		return 0;
	if (depth >= Engine::depthLimit) {
		if (MVV_LVAquiescence)
			return Engine::quiescenceSearch_moveSorting(board, colour, Engine::depthLimit, alpha, beta);
		else
			return Engine::quiescenceSearch(board, colour, Engine::depthLimit, alpha, beta);
	}
	else {
		MoveData move;
		std::vector<std::tuple<char, char>> moveList;
		std::tuple<char, char, char, char> preferredMove;
		int searchedMoves = 0;

		if (useHashtable) {
			HashEntry data = board->transpos_table.getHashEntry();
			if ((data.zobristKey == board->transpos_table.getHash())) {
				board->transpos_table.incrementHashHits();
				if ((data.distanceToLeaf >= (depthLimit + quiescenceLimit - depth))) {
					if ((data.type == NodeType::Exact))
						return data.value;
					else if ((data.type == NodeType::Lower) && (data.value < beta))
						beta = data.value;
					else if ((data.type == NodeType::Upper) && (data.value > alpha))
						alpha = data.value;

					if (alpha >= beta) return data.value;
				}

				return Engine::alphaBeta_preferredMove(board, colour, depth, alpha, beta, data.preferredMove);
			}
		}

		if (colour == Colour::White) {

			//NULLMOVE
			if (nullmove)
			{
				if ((allowNull) && !board->isChecked(colour)) {
					allowNull = false;
					int value = Engine::alphaBeta(board, Colour::Black, depth + 1 + Engine::R, beta - 1, beta);

					if (value >= beta)
						return value;

				}
			}
			allowNull = true;
			int value = Engine::WHITE_MIN;
			if (Engine::abortSearch) return value;
			for (char x = 0; x <= 7; x++) {
				for (char y = 0; y <= 7; y++) {
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						moveList = board->squares[x][y]->getMoveList(board->squares, x, y);
						for (unsigned int i = 0; i < moveList.size(); i++) {
							Engine::nodes++;

							move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);


							if (move.validMove) {
								searchedMoves++;

								if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
									board->undoMove(move);

									if (useHashtable) {
										preferredMove = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
										board->transpos_table.setHashEntry(depthLimit + quiescenceLimit - depth, Engine::BLACK_MAX, Engine::BLACK_MAX, Engine::BLACK_MAX, preferredMove);
									}

									return Engine::BLACK_MAX;
								}
								int newValue = Engine::alphaBeta(board, Colour::Black, depth + 1, alpha, beta);
								if (newValue > value) {
									value = newValue;
									preferredMove = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
								}

								if (value > alpha) {
									alpha = value;
									Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
									//Engine::optimalTurnSequence_moves[depth] = move;
								}

								board->undoMove(move);

								if (alpha >= beta) {
									if (useHashtable)
										board->transpos_table.setHashEntry(depthLimit + quiescenceLimit - depth, value, alpha, beta, preferredMove);
									return value;
								}
							}
						}
					}
				}
			}
			if (useHashtable) board->transpos_table.setHashEntry(depthLimit + quiescenceLimit - depth, value, alpha, beta, preferredMove);
			if ((searchedMoves == 0) && (!board->isChecked(colour))) //Draw: No legal moves while not checked
				return 0;
			else
				return value;
		}
		else {


			//NULLMOVE
			if (nullmove)
			{
				if ((allowNull) && !board->isChecked(colour)) {
					allowNull = false;
					int value = Engine::alphaBeta(board, Colour::White, depth + 1 + Engine::R, alpha, alpha + 1);

					if (value <= alpha)
						return value;

				}
			}
			allowNull = true;
			int value = Engine::BLACK_MAX;

			if (Engine::abortSearch) return value;
			for (char x = 0; x <= 7; x++) {
				for (char y = 0; y <= 7; y++) {
					if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
						moveList = board->squares[x][y]->getMoveList(board->squares, x, y);
						for (unsigned int i = 0; i < moveList.size(); i++) {
							Engine::nodes++;

							move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);


							if (move.validMove) {
								searchedMoves++;

								if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
									board->undoMove(move);

									if (useHashtable) {
										preferredMove = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
										board->transpos_table.setHashEntry(depthLimit + quiescenceLimit - depth, Engine::WHITE_MIN, Engine::WHITE_MIN, Engine::WHITE_MIN, preferredMove);
									}
									return Engine::WHITE_MIN;
								}
								int newValue = Engine::alphaBeta(board, Colour::White, depth + 1, alpha, beta);

								if (newValue < value) {
									value = newValue;
									preferredMove = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
								}

								if (value < beta) {
									Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
									//Engine::optimalTurnSequence_moves[depth] = move;
									beta = value;
								}

								board->undoMove(move);

								if (alpha >= beta) {
									if (useHashtable)
										board->transpos_table.setHashEntry(depthLimit + quiescenceLimit - depth, value, alpha, beta, preferredMove);
									return value;
								}
							}
						}
					}
				}

			}
			if (useHashtable) board->transpos_table.setHashEntry(depthLimit + quiescenceLimit - depth, value, alpha, beta, preferredMove);
			if ((searchedMoves == 0) && (!board->isChecked(colour))) //Draw: No legal moves while not checked
				return 0;
			else
				return value;
		}
	}
}

/*The first level of alpha beta search takes a preferred firstMove to potentially improve pruning;
firstMove is a result of previous searches when using iterative deepening*/
int Engine::alphaBeta_preferredMove(ChessBoard* board, Colour colour, char depth, int alpha, int beta, std::tuple<char, char, char, char> firstMove) {
	if (depth >= Engine::depthLimit) {
		if (MVV_LVAquiescence)
			return Engine::quiescenceSearch_moveSorting(board, colour, Engine::depthLimit, alpha, beta);
		else
			return Engine::quiescenceSearch(board, colour, Engine::depthLimit, alpha, beta);
	}
	MoveData move;
	int searchedMoves = 0;
	if (colour == Colour::White) {
		

		//NULLMOVE
		if (nullmove)
		{
			if ((allowNull) && !board->isChecked(colour)) {
				allowNull = false;
				int value = Engine::alphaBeta(board, Colour::Black, depth + 1 + Engine::R, beta - 1, beta);
				
				if (value >= beta)
					return value;
				
			}

		}
		allowNull = true;
		int value = Engine::WHITE_MIN;

		Engine::nodes++;
		std::tuple<char, char> orig = std::tuple<char, char>(std::get<0>(firstMove), std::get<1>(firstMove));
		std::tuple<char, char> dest = std::tuple<char, char>(std::get<2>(firstMove), std::get<3>(firstMove));
		move = board->moveTo(orig, dest);
		if (move.validMove) {
			
			if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
				board->undoMove(move);
				
				return Engine::BLACK_MAX;
			}
			value = std::max(value, Engine::alphaBeta(board, Colour::Black, depth + 1, alpha, beta));
			if (value > alpha)
			{
				Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(std::get<0>(firstMove), std::get<1>(firstMove), std::get<2>(firstMove), std::get<3>(firstMove));
				//Engine::optimalTurnSequence_moves[depth] = move;
				alpha = value;
			}
			board->undoMove(move);
			
			if (Engine::abortSearch) return value;
		}
		for (char x = 0; x <= 7; x++) {
			for (char y = 0; y <= 7; y++) {
				if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
					std::vector<std::tuple<char, char>> moveList;
					moveList = board->squares[x][y]->getMoveList(board->squares, x, y);
					for (unsigned int i = 0; i < moveList.size(); i++) {
						if ((std::get<0>(moveList[i]) == std::get<2>(firstMove)) && (std::get<1>(moveList[i]) == std::get<3>(firstMove)) && (x == std::get<0>(firstMove)) && (y == std::get<1>(firstMove)))
							continue;
						Engine::nodes++;
						move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);
						if (move.validMove) {
							searchedMoves++;
							
							if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
								board->undoMove(move);
								
								return Engine::BLACK_MAX;
							}
							int newValue = Engine::alphaBeta(board, Colour::Black, depth+1, alpha, beta);
							board->undoMove(move);
							
							if (newValue > value) {
								value = newValue;
							}
							if (Engine::abortSearch) return value; //since the search tree was not fully traversed, the result is discarded
							if (value > alpha) {
								alpha = value;
								Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
								//Engine::optimalTurnSequence_moves[depth] = move;
							}			
							if (alpha >= beta) return value;
						}
					}
				}
			}
		}
		if ((searchedMoves == 0) && (!board->isChecked(colour))) //Draw: No legal moves while not checked
			return 0;
		else
			return value;
	}
	else {
		

		//NULLMOVE
		if (nullmove)
		{
			if ((allowNull) && !board->isChecked(colour)) {
				allowNull = false;
				int value = Engine::alphaBeta(board, Colour::White, depth + 1 + Engine::R, alpha, alpha + 1);
				
				if (value <= alpha)
					return value;
				
			}
		}
		allowNull = true;
		int value = Engine::BLACK_MAX;

		Engine::nodes++;
		std::tuple<char, char> orig = std::tuple<char, char>(std::get<0>(firstMove), std::get<1>(firstMove));
		std::tuple<char, char> dest = std::tuple<char, char>(std::get<2>(firstMove), std::get<3>(firstMove));
		move = board->moveTo(orig, dest);
		if (move.validMove) {
			
			if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
				board->undoMove(move);
				
				return Engine::WHITE_MIN;
			}
			value = std::min(value, Engine::alphaBeta(board, Colour::White, depth + 1, alpha, beta));
			if (value < beta) {
				Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(std::get<0>(firstMove), std::get<1>(firstMove), std::get<2>(firstMove), std::get<3>(firstMove));
				//Engine::optimalTurnSequence_moves[depth] = move;
				beta = value;
			}
			board->undoMove(move);
			
			if (Engine::abortSearch) {
				return value;
			}
		}
		for (char x = 0; x <= 7; x++) {
			for (char y = 0; y <= 7; y++) {
				if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->colour == colour)) {
					std::vector<std::tuple<char, char>> moveList;
					moveList = board->squares[x][y]->getMoveList(board->squares, x, y);
					for (unsigned int i = 0; i < moveList.size(); i++) {
						if ((std::get<0>(moveList[i]) == std::get<2>(firstMove)) && (std::get<1>(moveList[i]) == std::get<3>(firstMove)) && (x == std::get<0>(firstMove)) && (y == std::get<1>(firstMove)))
							continue;
						Engine::nodes++;
						move = board->moveTo(std::tuple<char, char>(x, y), moveList[i]);
						if (move.validMove) {
							searchedMoves++;
							
							if ((move.pieceTaken != nullptr) && (move.pieceTaken->getPieceType() == PieceType::King)) {
								board->undoMove(move);
								
								return Engine::WHITE_MIN;
							}
							int newValue = Engine::alphaBeta(board, Colour::White, depth + 1, alpha, beta);
							board->undoMove(move);
							
							if (newValue < value) {
								value = newValue;
							}
							if (Engine::abortSearch) return value; //the move might have been better than a previous choice, but since the search tree was not fully traversed, the result is discarded
							if (value < beta) {
								Engine::optimalTurnSequence[depth] = std::tuple<char, char, char, char>(x, y, std::get<0>(moveList[i]), std::get<1>(moveList[i]));
								//Engine::optimalTurnSequence_moves[depth] = move;
								beta = value;
							}
							if (alpha >= beta) return value;
						}
					}
				}
			}
		}
		if ((searchedMoves == 0) && (!board->isChecked(colour))) //Draw: No legal moves while not checked
			return 0;
		else
			return value;
	}
}

/*The eval function currently accounts for piece values, positioning according to the scoreBoards, and castling*/
int Engine::evalHeuristic(ChessBoard* board) {
	int evaluation = 0;

	for (char x = 0; x <= 7; x++)
		for (char y = 0; y <= 7; y++)
			if (board->squares[x][y] != nullptr)
				evaluation += board->squares[x][y]->getValue() + board->squares[x][y]->getPositionalScore(x, y);

	if (board->whiteCastled) evaluation = evaluation + 50;
	if (board->blackCastled) evaluation = evaluation - 50;

	if (Engine::randomness) {
		evaluation += (std::rand() % Engine::randDouble)-Engine::randLimit;
	}

	return evaluation;
}

//TODO: To Do: Rewrite this
int Engine::calculateMove_fixedDepth(ChessBoard* board, Colour colour) {
	Engine::abortSearch = false;
	Engine::iterativeDeepening = false;
	Engine::nodes = 0;
	Engine::quiescenceNodes = 0;
	Engine::optimalTurnSequence = std::vector<std::tuple<char, char, char, char>>(Engine::depthLimit + Engine::quiescenceLimit + 1);
	Engine::startTime = std::clock();
	board->allowIllegalMoves = true;
	Engine::optimalValue = Engine::alphaBeta(board, colour, 0, Engine::WHITE_MIN, Engine::BLACK_MAX);
	board->allowIllegalMoves = false;
	if ((Engine::optimalValue == Engine::WHITE_MIN) && (colour == Colour::White)) Engine::optimalTurnSequence[0] = board->getPossibleMoves(colour).at(0);
	if ((Engine::optimalValue == Engine::BLACK_MAX) && (colour == Colour::Black)) Engine::optimalTurnSequence[0] = board->getPossibleMoves(colour).at(0);
	if (Engine::updateFct != nullptr)
		Engine::updateFct(this, board);
	Engine::calculateUpdateThreshold();
	return Engine::optimalValue;
}

int Engine::calculateMove_iterativeDeepening(ChessBoard* board, Colour colour) {
	Engine::abortSearch = false;
	Engine::iterativeDeepening = true;
	Engine::nodes = 0;
	Engine::quiescenceNodes = 0;
	Engine::optimalTurnSequence = std::vector<std::tuple<char, char, char, char>>(Engine::depthLimit + Engine::quiescenceLimit + 1);
	Engine::optimalTurnSequence_moves = std::vector<MoveData>(Engine::depthLimit + Engine::quiescenceLimit + 1);
	
	board->transpos_table.depreciateHashtable();
	//board->transpos_table.clearHashtable();
	//board->transpos_table.initializeHash(board, colour);

	//King endgame table
	if (useKingEndgameScoreboard)
		board->setKingScoreboard(isEndgame(board));
	else
		board->setKingScoreboard(false);

	board->allowIllegalMoves = true;
	Engine::startTime = std::clock();

	calculateMove_random(board, colour);
	std::tuple<char, char, char, char> firstMove = Engine::optimalTurnSequence.at(0);
	Engine::depthLimit = 0;
	//subsequent searches while time limit not reached
	while ((!Engine::abortSearch) && (depthLimit < 60)) {
		Engine::depthLimit++;
		Engine::optimalTurnSequence.resize(Engine::depthLimit + Engine::quiescenceLimit + 1);
		int newValue = Engine::alphaBeta_preferredMove(board, colour, 0, Engine::WHITE_MIN, Engine::BLACK_MAX,firstMove);
		if (!Engine::abortSearch) Engine::optimalValue = newValue;
		firstMove = Engine::optimalTurnSequence.at(0);
		double timePassed = (double(std::clock()) - double(Engine::startTime)) / CLOCKS_PER_SEC;
		if (timePassed > Engine::timeLimit) Engine::abortSearch = true;
	}
	board->allowIllegalMoves = false;
	if (Engine::updateFct!=nullptr)
		Engine::updateFct(this, board);
	Engine::calculateUpdateThreshold();
	return Engine::optimalValue;
}

void Engine::calculateMove_random(ChessBoard* board, Colour colour) {
	srand(time(NULL));
	Engine::nodes = 0;
	Engine::quiescenceNodes = 0;
	Engine::startTime = std::clock();
	bool illegalMoves = board->allowIllegalMoves;
	board->allowIllegalMoves = false;
	std::vector<std::tuple<char, char, char, char>> moveList = board->getPossibleMoves(colour);
	Engine::nodes = moveList.size();
	board->allowIllegalMoves = illegalMoves;
	if (moveList.size() != 0) {
		int rnd = std::rand() % moveList.size();
		Engine::optimalTurnSequence = std::vector<std::tuple<char, char, char, char>>(1);
		Engine::optimalTurnSequence[0] = moveList.at(rnd);
		Engine::optimalValue = Engine::evalHeuristic(board);
	}
}

int Engine::getNodes() {
	return Engine::nodes;
}

int Engine::getQuiescenceNodes() {
	return Engine::quiescenceNodes;
}

double Engine::getTimePassed() {
	return (double(std::clock()) - double(Engine::startTime)) / CLOCKS_PER_SEC;
}

bool Engine::isEndgame(ChessBoard* board)
{
	int matWhite = 0;
	int matBlack = 0;
	for (char x = 0; x <= 7; x++) {
		for (char y = 0; y <= 7; y++) {
			if ((board->squares[x][y] != nullptr) && (board->squares[x][y]->getPieceType() != PieceType::King)) {
				if (board->squares[x][y]->colour == Colour::White)
					matWhite = matWhite + board->squares[x][y]->getValue();
				else
					matBlack = matBlack - board->squares[x][y]->getValue();
			}
		}
	}

	return ((matWhite < endgameThreshold) && (matBlack < endgameThreshold));
}

bool Engine::seachAborted() {
	return Engine::abortSearch;
}

Engine::Engine() {
	srand(time(NULL));
	Engine::randDouble = Engine::randLimit * 2;
}