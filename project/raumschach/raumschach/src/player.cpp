#include "configuration.h"
#include "constants.h"
#include "player.h"
#include "board.h"

Player::Player(int depth, Config::PlayerColour colour)
	:	searchDepth(depth), playerColour(colour)
{}

Player::~Player()
{}

int Player::GetDepth() const
{
	return searchDepth;
}

/********** class HumanPlayer ***********/

HumanPlayer::HumanPlayer(Config::PlayerColour colour)
	:	Player(0, colour)
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

AIPlayer::AIPlayer(int depth, Config::PlayerColour colour, BitBoardMovePool * movePool, RandomGenerator * gen)
	:	Player(depth, colour), movePool(movePool), rgen(gen)
{}

Config::PlayerType AIPlayer::GetType() const
{
	return Config::PLAYER_AI;
}

bool AIPlayer::GetMove(Piece& piece, ChessVector& pos, const Board * board) const
{
	if(!board || !movePool)
		return false;

	int alpha = Config::INT_NEGATIVE_INFINITY;

	Board boardCopy(*board);

	DynamicArray<Move> moves(Const::MAX_PIECES_MOVES);
	boardCopy.GetPossibleMoves(playerColour, moves);
	for(int i = 0; i < moves.Count(); ++i)
	{
		moves[i].heuristic = MoveHeuristic(moves[i], *board);
	}

	// sort the moves so that the most valuable are first
	moves.Sort();

	Config::PlayerColour oppositeColour = Config::GetOppositePlayer(playerColour);

	BitBoard playerPieces = board->GetPiecesBitBoard(playerColour);
	BitBoard enemyPieces = board->GetPiecesBitBoard(oppositeColour);

	for(int i = 0; i < moves.Count(); ++i)
	{
		// make the move and start an Alpha Beta from it
		MadeMove move = boardCopy.MovePiece(moves[i].piece, moves[i].destination, moves[i].pieceMoves, true);

		int alphaBetaResult = AlphaBeta(boardCopy, searchDepth, alpha, Config::INT_POSITIVE_INFINITY, false, oppositeColour);

		boardCopy.UndoMove(move);

		if(alphaBetaResult > alpha)
		{
			alpha = alphaBetaResult;
			piece = moves[i].piece;
			pos = moves[i].destination;
		}
	}

	return true;
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
		for(int i = 0; i < moves.Count(); ++i)
		{
			MadeMove move = board.MovePiece(moves[i].piece, moves[i].destination, moves[i].pieceMoves, true);

			int res = AlphaBeta(board, depth - 1, alpha, beta, false, oppositePlayer);

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
		for(int i = 0; i < moves.Count(); ++i)
		{
			MadeMove move = board.MovePiece(moves[i].piece, moves[i].destination, moves[i].pieceMoves, true);

			int res = AlphaBeta(board, depth - 1, alpha, beta, true, oppositePlayer);

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