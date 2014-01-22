#include "configuration.h"
#include "constants.h"
#include "player.h"
#include "board.h"
#include <cmath>

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

	// if we have iterative deepening
	if(iterations)
	{
		IterateAlphaBetaRoot(*board, iterations, possibleMoves);
	}
	else
	{
		AlphaBetaRoot(*board, playerColour, possibleMoves);
	}

	possibleMoves.Sort();

	printf("Best ai moves heuristics for %s player:\n", Const::COLOUR_NAMES[playerColour].GetPtr());
	for(int i = 0; i < possibleMoves.Count(); ++i)
	{
		printf("%s to (%d, %d, %d), h = %d\n", Const::PIECE_NAMES[possibleMoves[i].piece.GetType()].GetPtr(), possibleMoves[i].destination.x, possibleMoves[i].destination.y, possibleMoves[i].destination.z, possibleMoves[i].heuristic);
	}
	printf("\n");

	int selectedMove = (searchDepth & 1 ? 0 : possibleMoves.Count() - 1);

	piece = possibleMoves[selectedMove].piece;
	pos = possibleMoves[selectedMove].destination;

	return true;
}

void AIPlayer::IterateAlphaBetaRoot(const Board& board, int iteration, DynamicArray<Move>& generatedMoves) const
{
	// first the end of the recursion
	AlphaBetaRoot(board, playerColour, generatedMoves);
	
	// now the recursive calls
	if(iteration > 0)
	{
		Board copyBoard(board);

		// now sort them to have the best on top
		generatedMoves.Sort();

		// we take the square root of the generated moves as sample
		int samples = (int) ceilf(sqrtf(generatedMoves.Count()));

		samples = Utils::Min(samples, generatedMoves.Count());

		for(int i = 0; i < samples; ++i)
		{
			DynamicArray<Move> enemyMoves(Const::MAX_PIECES_MOVES);
			
			// make the move on the board
			MadeMove friendlyMove = copyBoard.MovePiece(generatedMoves[i].piece, generatedMoves[i].destination, generatedMoves[i].pieceMoves, true);

			// now get the array of moves for the enemy
			AlphaBetaRoot(copyBoard, Config::GetOppositePlayer(playerColour), enemyMoves);

			// sort the moves
			enemyMoves.Sort();

			// take a sample as before
			int enemySamples = (int) ceilf(sqrtf(enemyMoves.Count()));
			enemySamples = Utils::Min(enemySamples, enemyMoves.Count());

			for(int j = 0; j < enemySamples; ++j)
			{
				// Make a move for each of the best enemy moves and then assemble the result
				MadeMove enemyMove = copyBoard.MovePiece(enemyMoves[j].piece, enemyMoves[j].destination, enemyMoves[j].pieceMoves, true);

				DynamicArray<Move> endMoves(Const::MAX_PIECES_MOVES);

				// make the recursive call to the iterative function use the results
				IterateAlphaBetaRoot(copyBoard, iteration - 1, endMoves);

				// now copy the best evaluation to the current top move heuristics
				int maxEvaluation = generatedMoves[i].heuristic;
				for(int k = 0; k < endMoves.Count(); ++k)
				{
					maxEvaluation = Utils::Max(maxEvaluation, endMoves[k].heuristic);
				}

				generatedMoves[i].heuristic = maxEvaluation;

				// undo the move
				copyBoard.UndoMove(enemyMove);
			}

			// finally undo the move
			copyBoard.UndoMove(friendlyMove);
		}
	}
}

void AIPlayer::AlphaBetaRoot(const Board& board, Config::PlayerColour colour, DynamicArray<Move>& generatedMoves) const
{
	int alpha = Config::INT_NEGATIVE_INFINITY;

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

		int alphaBetaResult = AlphaBeta(boardCopy, searchDepth, alpha, Config::INT_POSITIVE_INFINITY, false, oppositeColour);

		boardCopy.UndoMove(move);

		generatedMoves[i].heuristic = alphaBetaResult;
	}

	// clear the transition table, it will not help us later
	transitionTable->Clear();
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
			if(!transitionTable->GetValue(depth - 1, boardHash, res))
			{
				res = AlphaBeta(board, depth - 1, alpha, beta, false, oppositePlayer);

				transitionTable->AddValue(depth - 1, boardHash, res);
			}

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
			if(!transitionTable->GetValue(depth - 1, boardHash, res))
			{
				res = AlphaBeta(board, depth - 1, alpha, beta, true, oppositePlayer);

				transitionTable->AddValue(depth - 1, boardHash, res);
			}

			board.UndoMove(move);

			beta = Utils::Min(beta, res);
			if(beta <= alpha)
				break;
		}
		return beta;
	}
}

int AIPlayer::MoveHeuristic(const Move& move, const Board& board) const
{	
	return move.piece.GetPositionWorth(move.destination) + board.GetPiece(move.destination).GetWorth();
}