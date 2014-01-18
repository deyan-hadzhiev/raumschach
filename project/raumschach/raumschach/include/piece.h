#ifndef __PIECES_H__
#define __PIECES_H__

#include "configuration.h"
#include "utils.h"

class Piece
{
public:
	Piece();
	Piece(Config::PieceType type, Config::PlayerColour col, coord pos);
	Piece(Config::PieceType type, Config::PlayerColour col, ChessVector pos);
	Piece(const Piece& copy);

	Piece& operator=(const Piece& assign);

	friend bool operator==(const Piece& lhs, const Piece& rhs);
	friend bool operator!=(const Piece& lhs, const Piece& rhs);

	ChessVector GetPositionVector() const;
	void SetPositionVector(ChessVector pos);
	coord GetPositionCoord() const;
	void SetPositionCoord(coord pos);

	Config::PlayerColour GetColour() const;
	void SetColour(Config::PlayerColour col);

	Config::PieceType GetType() const;
	void SetType(Config::PieceType type);

	/** Returns the current piece worth
	* NOTE: The worth may not be flat, but depend on position, moves, etc.
	*/
	int GetWorth() const;

	// Return the worth only of the specified position if the current piece occupies it
	int GetPositionWorth(ChessVector pos) const;

	friend bool operator<(const Piece& lhs, const Piece& rhs);
private:

	void SetFlag(unsigned short flag, unsigned short mask, char offset)
	{
		flags = (flags & ~ mask) | (flag << offset);
	}

	unsigned short GetFlag(unsigned short mask, char offset) const
	{
		return (flags & mask) >> offset;
	}

	unsigned short flags;
};

#endif // __PIECES_H__