#ifndef __PIECES_H__
#define __PIECES_H__

#include "configuration.h"
#include "utils.h"

class Piece
{
public:
	Piece() : flags(0) {}

	Piece(Config::PieceType type, Config::PlayerColour col, coord pos) : flags(0)
	{
		flags |= ((short) type) << Config::PIECE_TYPE_LSHIFT;
		flags |= ((short) col) << Config::PIECE_COLOUR_LSHIFT;
		flags |= ((short) pos) << Config::PIECE_POSITION_LSHIFT;
	}

	Piece(Config::PieceType type, Config::PlayerColour col, ChessVector pos) : flags(0)
	{
		flags |= ((short) type) << Config::PIECE_TYPE_LSHIFT;
		flags |= ((short) col) << Config::PIECE_COLOUR_LSHIFT;
		flags |= ((short) pos.GetVectorCoord()) << Config::PIECE_POSITION_LSHIFT;
	}

	Piece(const Piece& copy) : flags(copy.flags) {}

	Piece& operator=(const Piece& assign)
	{
		flags = assign.flags;
		return *this;
	}

	inline friend bool operator==(const Piece& lhs, const Piece& rhs)
	{
		return lhs.flags == rhs.flags;
	}

	inline friend bool operator!=(const Piece& lhs, const Piece& rhs)
	{
		return lhs.flags != rhs.flags;
	}

	inline ChessVector GetPositionVector() const
	{
		return ChessVector( GetPositionCoord());
	}

	inline void SetPositionVector(ChessVector pos)
	{
		SetPositionCoord( pos.GetVectorCoord());
	}

	inline coord GetPositionCoord() const
	{
		return (coord) GetFlag(Config::PIECE_POSITION_MASK, Config::PIECE_POSITION_LSHIFT);
	}

	inline void SetPositionCoord(coord pos)
	{
		SetFlag( (short) pos, Config::PIECE_POSITION_MASK, Config::PIECE_POSITION_LSHIFT);
	}

	inline Config::PlayerColour GetColour() const
	{
		return (Config::PlayerColour) GetFlag(Config::PIECE_COLOUR_MASK, Config::PIECE_COLOUR_LSHIFT);
	}

	inline void SetColour(Config::PlayerColour col)
	{
		SetFlag( (short) col, Config::PIECE_COLOUR_MASK, Config::PIECE_COLOUR_LSHIFT);
	}

	inline Config::PieceType GetType() const
	{
		return (Config::PieceType) GetFlag(Config::PIECE_TYPE_MASK, Config::PIECE_TYPE_LSHIFT);
	}

	inline void SetType(Config::PieceType type)
	{
		SetFlag( (short) type, Config::PIECE_TYPE_MASK, Config::PIECE_TYPE_LSHIFT);
	}

	/** Returns the current piece worth
	* NOTE: The worth may not be flat, but depend on position, moves, etc.
	*/
	int GetWorth() const;

	// Return the worth only of the specified position if the current piece occupies it
	int GetPositionWorth(ChessVector pos) const;

	friend bool operator<(const Piece& lhs, const Piece& rhs)
	{
		return lhs.GetPositionCoord() < rhs.GetPositionCoord();
	}
private:

	inline void SetFlag(unsigned short flag, unsigned short mask, char offset)
	{
		flags = (flags & ~ mask) | (flag << offset);
	}

	inline unsigned short GetFlag(unsigned short mask, char offset) const
	{
		return (flags & mask) >> offset;
	}

	unsigned short flags;
};

#endif // __PIECES_H__