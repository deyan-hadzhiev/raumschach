#ifndef __PLAYER_H__
#define __PLAYER_H__

class Board;
class BitBoardMovePool;
class Piece;
class ChessVector;
class RandomGenerator;

class Player
{
public:
	Player(int depth);
	virtual ~Player();

	int GetDepth() const;

	virtual Config::PlayerType GetType() const = 0;

	/** Gets the player move based on the board it has been passed
	* @param piece[out] : The piece that will be moved by the player
	* @param pos[out] : The position, to which the piece shall be moved
	* @param board[in] : A board, based on which the move is calculated
	* @param movePool[in] : A move pool for taking possible moves, etc.
	* @retval : true if a move was calculated and the result written to the proper output variables
	*/
	virtual bool GetMove(Piece& piece, ChessVector& pos, const Board * board, const BitBoardMovePool * movePool) const = 0;

protected:
	// disable copy and assign
	Player(const Player& copy);
	Player& operator=(const Player& assign);

	const int searchDepth;
};

class HumanPlayer : public Player
{
public:
	HumanPlayer();

	Config::PlayerType GetType() const;

	bool GetMove(Piece& piece, ChessVector& pos, const Board * board, const BitBoardMovePool * movePool) const;
};

class AIPlayer : public Player
{
public:
	AIPlayer(int depth, RandomGenerator * rgen);

	Config::PlayerType GetType() const;

	bool GetMove(Piece& piece, ChessVector& pos, const Board * board, const BitBoardMovePool * movePool) const;

private:
	RandomGenerator * rgen;
};

#endif // __PLAYER_H__