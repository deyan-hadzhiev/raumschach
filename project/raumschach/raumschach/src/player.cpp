#include "configuration.h"
#include "player.h"

Player::Player(int depth)
	:	searchDepth(depth)
{}

Player::~Player()
{}

int Player::GetDepth() const
{
	return searchDepth;
}

/********** class HumanPlayer ***********/

HumanPlayer::HumanPlayer()
	:	Player(0)
{}

Config::PlayerType HumanPlayer::GetType() const
{
	return Config::PLAYER_HUMAN;
}

bool HumanPlayer::GetMove(Piece& piece, ChessVector& pos, const Board * board, const BitBoardMovePool * movePool) const
{
	return false;
}

/*********** class AIPlayer *************/

AIPlayer::AIPlayer(int depth, RandomGenerator * gen)
	:	Player(depth), rgen(gen)
{}

Config::PlayerType AIPlayer::GetType() const
{
	return Config::PLAYER_AI;
}

bool AIPlayer::GetMove(Piece& piece, ChessVector& pos, const Board * board, const BitBoardMovePool * movePool) const
{
	if(!board || !movePool)
		return false;

	// TODO implenent AI algorithm
	return false;
}
