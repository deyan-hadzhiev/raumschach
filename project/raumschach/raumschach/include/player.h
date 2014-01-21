#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "board.h"
#include "piece.h"
#include "utils.h"
#include <unordered_map>

class RandomGenerator;

class TransitionTable
{
public:
	TransitionTable(int depth) : table(nullptr), tableSize(depth)
	{
		table = new std::unordered_map<unsigned long long, int>[tableSize];
	}

	~TransitionTable()
	{
		delete[] table;
		table = nullptr;
	}

	/** Searches for the hash in the transition table
	* @param depth : the depth of the value to be searched
	* @param hash : the hash to be searched for in the transition table
	* @param value[out] : the value in the transition table if it was found, 0 otherwise
	* @return : true if the hash is found, or false otherwise.
	*/
	bool GetValue(int depth, unsigned long long hash, int& value) const
	{
		auto got = table[depth].find(hash);
		bool found = false;
		if(got != table[depth].end())
		{
			value = got->second;
			found = true;
		}
		return found;
	}

	// Adds a value with the specified hash at the specified depth
	void AddValue(int depth, unsigned long long hash, int value)
	{
		table[depth][hash] = value;
	}

	// Clears the current table and allocates another
	void Clear()
	{
		delete[] table;
		table = nullptr;
		table = new std::unordered_map<unsigned long long, int>[tableSize];
	}

private:
	// disable copy and assignment
	TransitionTable(const TransitionTable& copy);
	TransitionTable& operator=(const TransitionTable& assign);

	int tableSize;
	std::unordered_map<unsigned long long, int> * table;
};

class Player
{
public:
	Player(int depth, Config::PlayerColour colour);
	virtual ~Player();

	int GetDepth() const;
	Config::PlayerColour GetColour() const;

	virtual Config::PlayerType GetType() const = 0;

	/** Gets the player move based on the board it has been passed
	* @param piece[out] : The piece that will be moved by the player
	* @param pos[out] : The position, to which the piece shall be moved
	* @param board[in] : A board, based on which the move is calculated
	* @retval : true if a move was calculated and the result written to the proper output variables
	*/
	virtual bool GetMove(Piece& piece, ChessVector& pos, const Board * board) const = 0;

protected:
	// disable copy and assign
	Player(const Player& copy);
	Player& operator=(const Player& assign);

	const int searchDepth;
	Config::PlayerColour playerColour;
};

class HumanPlayer : public Player
{
public:
	HumanPlayer(Config::PlayerColour colour);

	Config::PlayerType GetType() const;

	bool GetMove(Piece& piece, ChessVector& pos, const Board * board) const;
};

class AIPlayer : public Player
{
public:
	AIPlayer(int depth, Config::PlayerColour colour, BitBoardMovePool * movePool, RandomGenerator * rgen);
	~AIPlayer();

	Config::PlayerType GetType() const;

	bool GetMove(Piece& piece, ChessVector& pos, const Board * board) const;

private:
	/** The main algorithm for decision making of moves
	* @param board : The current board for which we search for best move
	* @param depth : The current search depth
	* @param alpha : The heuristic of the best found move to the moment
	* @param beta : The hauristic of the best found enemy move for the moment
	* @param maximizing : If this is a maximizin call or a minimizing one
	* @param colour : The colour of the player that makes the move
	* @retval : The best calculated heuristic
	*/
	int AlphaBeta(Board& board, int depth, int alpha, int beta, bool maximizing, Config::PlayerColour colour) const;

	// The main heuristic of move function
	int MoveHeuristic(const Move& move, const Board& board) const;

	RandomGenerator * rgen;
	BitBoardMovePool * movePool;

	TransitionTable * transitionTable;
};

#endif // __PLAYER_H__