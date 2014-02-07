#include "configuration.h"
#include "constants.h"
#include "player.h"
#include "board.h"
#include <cmath>
#include <ctime>

Player::Player(int depth, int iterations, Config::PlayerColour colour)
	:	searchDepth(depth), playerColour(colour), iterations(iterations)
{}

Player::~Player()
{}

int Player::GetDepth() const
{
	return searchDepth;
}

int Player::GetIterations() const
{
	return iterations;
}

/********** class HumanPlayer ***********/

HumanPlayer::HumanPlayer(Config::PlayerColour colour)
	:	Player(0, 0, colour)
{}

Config::PlayerType HumanPlayer::GetType() const
{
	return Config::PLAYER_HUMAN;
}

bool HumanPlayer::GetMove(Piece& piece, ChessVector& pos, const Board * board) const
{
	return false;
}

/*********** class AIPlayer *************/

AIPlayer::AIPlayer(int depth, int iterations, Config::PlayerColour colour, RandomGenerator * gen)
	:	Player(depth, iterations, colour), rgen(gen), transitionTable(nullptr)
{
	transitionTable = new TransitionTable(depth);
}

AIPlayer::~AIPlayer()
{
	delete transitionTable;
	transitionTable = nullptr;
}

Config::PlayerType AIPlayer::GetType() const
{
	return Config::PLAYER_AI;
}

bool AIPlayer::GetMove(Piece& piece, ChessVector& pos, const Board * board) const
{
	if(!board)
		return false;

	DynamicArray<Move> possibleMoves(Const::MAX_PIECES_MOVES);

	long long start = clock();

	/* Doesnt work at the moment ****/
	//// if we have iterative deepening
	//if(iterations)
	//{
	//	IterateAlphaBetaRoot(*board, iterations, possibleMoves);
	//}
	//else
	//{
	//	AlphaBetaRoot(*board, playerColour, possibleMoves);
	//}

	//possibleMoves.Sort();

	// This is needed because of the oddity reversal in the alpha-beta search
	//int index = ((searchDepth & 1) != 0 ? 0 : possibleMoves.Count() - 1);

	Move finalMove = AlphaBetaSingle(*board, searchDepth, playerColour); //possibleMoves[index];
	
	long long end = clock();

	float calculation = (float)(end - start)/ CLOCKS_PER_SEC;

	int ms = (calculation - floor( calculation)) * 1000;
	int second = floor( calculation);
	int sec = second % 60;
	int min = (second / 60) % 60;
	int hrs = second / 3600;

	printf("Calculation time : %dh %dm %d.%ds\n", hrs, min, sec, ms);

	//finalMove = GetRandomBestMove(possibleMoves);

	printf("Best ai move heuristics for %s player:\n", Const::COLOUR_NAMES[playerColour].GetPtr());
	printf("%s to (%d, %d, %d), h = %d\n", Const::PIECE_NAMES[finalMove.piece.GetType()].GetPtr(), finalMove.destination.x, finalMove.destination.y, finalMove.destination.z, finalMove.heuristic);
	/*for(int i = 0; i < possibleMoves.Count(); ++i)
	{
		printf("%s to (%d, %d, %d), h = %d\n", Const::PIECE_NAMES[possibleMoves[i].piece.GetType()].GetPtr(), possibleMoves[i].destination.x, possibleMoves[i].destination.y, possibleMoves[i].destination.z, possibleMoves[i].heuristic);
	}*/
	printf("\n");

	piece = finalMove.piece;
	pos = finalMove.destination;

	return true;
}

void AIPlayer::IterateAlphaBetaRoot(const Board& board, int iteration, DynamicArray<Move>& generatedMoves) const
{
	// first the end of the recursion
	AlphaBetaRoot(board, playerColour, generatedMoves);
	
	// now the recursive calls
	if(iteration > 0)
	{
		Board boardCopy(board);

		// now sort them to have the best on top
		generatedMoves.Sort();

		// we take the square root of the generated moves as sample
		int samples = (int) ceilf(sqrtf(generatedMoves.Count()));

		samples = Utils::Min(samples, generatedMoves.Count());

		// now make the moves and calculate similary to the AlphaBetaRoot
		Config::PlayerColour oppositeColour = Config::GetOppositePlayer(playerColour);

		BitBoard playerPieces = board.GetPiecesBitBoard(playerColour);
		BitBoard enemyPieces = board.GetPiecesBitBoard(oppositeColour);

		for(int i = 0; i < samples; ++i)
		{
			// make the move and start an Alpha Beta from it
			MadeMove move = boardCopy.MovePiece(generatedMoves[i].piece, generatedMoves[i].destination, generatedMoves[i].pieceMoves, true);

			int alphaBetaResult = AlphaBeta(boardCopy, searchDepth + iteration * 2, Config::INT_NEGATIVE_INFINITY, Config::INT_POSITIVE_INFINITY, false, oppositeColour);

			boardCopy.UndoMove(move);

			generatedMoves[i].heuristic = alphaBetaResult;
		}

		// clear the transition table, it will not help us later
		transitionTable->Clear();
	}
}

Move AIPlayer::AlphaBetaSingle(const Board& board, int depth, Config::PlayerColour colour) const
{
	Board boardCopy(board);

	DynamicArray<Move> availableMoves(Const::MAX_PIECES_MOVES);

	// first fill with some expected heuristic
	boardCopy.GetPossibleMoves(colour, availableMoves);
	for(int i = 0; i < availableMoves.Count(); ++i)
	{
		availableMoves[i].heuristic = MoveHeuristic(availableMoves[i], board);
	}

	// sort the moves so that the most valuable are first
	availableMoves.Sort();

	Config::PlayerColour oppositeColour = Config::GetOppositePlayer(colour);

	BitBoard playerPieces = board.GetPiecesBitBoard(colour);
	BitBoard enemyPieces = board.GetPiecesBitBoard(oppositeColour);

	int alpha = Config::INT_NEGATIVE_INFINITY;
	Move bestMove = availableMoves.Count() > 0 ? availableMoves[0] : Move();

	printf("Moves Count: %d\n", availableMoves.Count());

	for(int i = 0; i < availableMoves.Count(); ++i)
	{
		// make the move and start an Alpha Beta from it
		MadeMove move = boardCopy.MovePiece(availableMoves[i].piece, availableMoves[i].destination, availableMoves[i].pieceMoves, true);

		int alphaBetaResult = AlphaBeta(boardCopy, depth - 1, alpha, Config::INT_POSITIVE_INFINITY, false, oppositeColour);

		boardCopy.UndoMove(move);

		if(alphaBetaResult > alpha)
		{
			alpha = alphaBetaResult;
			bestMove = availableMoves[i];
			bestMove.heuristic = alpha;
		}
	}

	// clear the transition table, it will not help us later
	//transitionTable->Clear();

	return bestMove;
}

void AIPlayer::AlphaBetaRoot(const Board& board, Config::PlayerColour colour, DynamicArray<Move>& generatedMoves) const
{
	Board boardCopy(board);

	// first fill with some expected heuristic
	boardCopy.GetPossibleMoves(colour, generatedMoves);
	for(int i = 0; i < generatedMoves.Count(); ++i)
	{
		generatedMoves[i].heuristic = MoveHeuristic(generatedMoves[i], board);
	}

	// sort the moves so that the most valuable are first
	generatedMoves.Sort();

	Config::PlayerColour oppositeColour = Config::GetOppositePlayer(colour);

	BitBoard playerPieces = board.GetPiecesBitBoard(colour);
	BitBoard enemyPieces = board.GetPiecesBitBoard(oppositeColour);

	for(int i = 0; i < generatedMoves.Count(); ++i)
	{
		// make the move and start an Alpha Beta from it
		MadeMove move = boardCopy.MovePiece(generatedMoves[i].piece, generatedMoves[i].destination, generatedMoves[i].pieceMoves, true);

		int alphaBetaResult = AlphaBeta(boardCopy, searchDepth, Config::INT_NEGATIVE_INFINITY, Config::INT_POSITIVE_INFINITY, false, oppositeColour);

		boardCopy.UndoMove(move);

		generatedMoves[i].heuristic = alphaBetaResult;
	}

	// clear the transition table, it will not help us later
	//transitionTable->Clear();
}

int AIPlayer::AlphaBeta(Board& board, int depth, int alpha, int beta, bool maximizing, Config::PlayerColour colour) const
{
	if(depth <= 0)
	{
		return board.GetMaterialBalance() * (colour == Config::WHITE ? 1 : -1);
	}

	DynamicArray<Move> moves(Const::MAX_PIECES_MOVES);
	board.GetPossibleMoves(colour, moves);
	for(int i = 0; i < moves.Count(); ++i)
	{
		moves[i].heuristic = MoveHeuristic(moves[i], board);
	}

	// sort the moves so that the most valuable are first
	moves.Sort();

	Config::PlayerColour oppositePlayer = Config::GetOppositePlayer(colour);

	BitBoard playerPieces = board.GetPiecesBitBoard(colour);
	BitBoard enemyPieces = board.GetPiecesBitBoard(oppositePlayer);

	if(maximizing)
	{
		unsigned long long boardHash = 0ULL;
		for(int i = 0; i < moves.Count(); ++i)
		{
			MadeMove move = board.MovePiece(moves[i].piece, moves[i].destination, moves[i].pieceMoves, true);

			boardHash = board.GetHash();

			int res = 0;
			/*if(!transitionTable->GetValue(depth - 1, boardHash, res))
			{*/
				res = AlphaBeta(board, depth - 1, alpha, beta, false, oppositePlayer);

				/*transitionTable->AddValue(depth - 1, boardHash, res);
			}*/

			board.UndoMove(move);

			alpha = Utils::Max(alpha, res);
			if( beta <= alpha)
			{
				break;
			}
		}
		return alpha;
	}
	else
	{
		unsigned long long boardHash = 0ULL;
		for(int i = 0; i < moves.Count(); ++i)
		{
			MadeMove move = board.MovePiece(moves[i].piece, moves[i].destination, moves[i].pieceMoves, true);

			boardHash = board.GetHash();

			int res = 0;
			/*if(!transitionTable->GetValue(depth - 1, boardHash, res))
			{*/
				res = AlphaBeta(board, depth - 1, alpha, beta, true, oppositePlayer);

			/*	transitionTable->AddValue(depth - 1, boardHash, res);
			}*/

			board.UndoMove(move);

			beta = Utils::Min(beta, res);
			if(beta <= alpha)
				break;
		}
		return beta;
	}
}

Move AIPlayer::GetRandomBestMove(DynamicArray<Move>& moves) const
{
	moves.Sort();
	
	DynamicArray<Move> bestCandidates;

	// this is necessary because of the oddity changes from the AlphaBeta algorithm
	const int startingIndex = (searchDepth & 1 ? 0 : moves.Count() - 1);
	int incrementation = (searchDepth & 1 ? 1 : -1);

	bool inThreshold = true;
	
	// add the first element, because it is always the best choice by alpha beta
	bestCandidates += moves[startingIndex];

	int index = startingIndex + incrementation;

	while(index >= 0 && index < moves.Count() && inThreshold)
	{
		if(Utils::Abs(moves[startingIndex].heuristic - moves[index].heuristic) < Const::BEST_MOVE_THRESHOLD)
		{
			bestCandidates += moves[index];
		}
		else
		{
			inThreshold = false;
		}

		index += incrementation;
	}

	printf("Best ai moves heuristics for %s player:\n", Const::COLOUR_NAMES[playerColour].GetPtr());
	for(int i = 0; i < bestCandidates.Count(); ++i)
	{
		printf("%s to (%d, %d, %d), h = %d\n", Const::PIECE_NAMES[bestCandidates[i].piece.GetType()].GetPtr(), bestCandidates[i].destination.x, bestCandidates[i].destination.y, bestCandidates[i].destination.z, bestCandidates[i].heuristic);
	}
	printf("\n");

	int random = rgen->GetRand(bestCandidates.Count());
	return bestCandidates[random];
}

int AIPlayer::MoveHeuristic(const Move& move, const Board& board) const
{	
	return move.piece.GetPositionWorth(move.destination) + board.GetPiece(move.destination).GetWorth();
}