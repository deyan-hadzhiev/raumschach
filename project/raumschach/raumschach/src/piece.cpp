#include "configuration.h"
#include "utils.h"
#include "piece.h"
#include "constants.h"

bool operator<(const Piece& lhs, const Piece& rhs)
{
	return lhs.GetPositionCoord() < rhs.GetPositionCoord();
}

Piece::Piece()
	: flags(0)
{}

Piece::Piece(Config::PieceType type, Config::PlayerColour col, coord pos)
	:	flags(0)
{
	flags |= ((short) type) << Config::PIECE_TYPE_LSHIFT;
	flags |= ((short) col) << Config::PIECE_COLOUR_LSHIFT;
	flags |= ((short) pos) << Config::PIECE_POSITION_LSHIFT;
}

Piece::Piece(Config::PieceType type, Config::PlayerColour col, ChessVector pos)
	:	flags(0)
{
	flags |= ((short) type) << Config::PIECE_TYPE_LSHIFT;
	flags |= ((short) col) << Config::PIECE_COLOUR_LSHIFT;
	flags |= ((short) pos.GetVectorCoord()) << Config::PIECE_POSITION_LSHIFT;
}

Piece::Piece(const Piece& copy)
	:	flags(copy.flags)
{}

Piece& Piece::operator=(const Piece& assign)
{
	flags = assign.flags;
	return *this;
}

bool operator==(const Piece& lhs, const Piece& rhs)
{
	return lhs.flags == rhs.flags;
}

bool operator!=(const Piece& lhs, const Piece& rhs)
{
	return lhs.flags != rhs.flags;
}

ChessVector Piece::GetPositionVector() const
{
	return ChessVector( GetPositionCoord());
}

void Piece::SetPositionVector(ChessVector pos)
{
	SetPositionCoord( pos.GetVectorCoord());
}

coord Piece::GetPositionCoord() const
{
	return (coord) GetFlag(Config::PIECE_POSITION_MASK, Config::PIECE_POSITION_LSHIFT);
}

void Piece::SetPositionCoord(coord pos)
{
	SetFlag( (short) pos, Config::PIECE_POSITION_MASK, Config::PIECE_POSITION_LSHIFT);
}

Config::PlayerColour Piece::GetColour() const
{
	return (Config::PlayerColour) GetFlag(Config::PIECE_COLOUR_MASK, Config::PIECE_COLOUR_LSHIFT);
}

void Piece::SetColour( Config::PlayerColour col)
{
	SetFlag( (short) col, Config::PIECE_COLOUR_MASK, Config::PIECE_COLOUR_LSHIFT);
}

Config::PieceType Piece::GetType() const
{
	return (Config::PieceType) GetFlag(Config::PIECE_TYPE_MASK, Config::PIECE_TYPE_LSHIFT);
}

void Piece::SetType( Config::PieceType type)
{
	SetFlag( (short) type, Config::PIECE_TYPE_MASK, Config::PIECE_TYPE_LSHIFT);
}

int Piece::GetWorth() const
{
	return Const::PIECE_WORTH[GetType()];
}

int Piece::GetPositionWorth(ChessVector pos) const
{
	Config::PieceType type = GetType();
	coord worth = 0;
	if(type != Config::PAWN)
	{
		worth = pos.GetManhattanDistance(Const::BOARD_CENTER);
	}
	else
	{
		worth = Const::PAWN_DISTANCE_TO_REPRODUCTION - Utils::Abs(pos.y - Const::PAWN_REPRODUCE_VECTORS[GetColour()].y) - Utils::Abs(pos.z - Const::PAWN_REPRODUCE_VECTORS[GetColour()].z);
	}
	return worth;
}