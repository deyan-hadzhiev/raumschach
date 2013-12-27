#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "configuration.h"
#include "utils.h"

namespace Const
{
	static const int PIECE_WORTH[Config::PIECE_TYPE_COUNT] =
	{
		0, // no type
		80, // king
		14, // queen
		6, // rook
		4, // bishop
		4, // knight
		3, // unicorn
		1, // pawn
	};

#define VECTORS_COUNT(x) sizeof((x)) / (sizeof(coord) * 3)

	static const bool PIECE_MOVE_SCALING[Config::PIECE_TYPE_COUNT] =
	{
		false, // no type
		false, // king
		true, // queen
		true, // rook
		true, // bishop
		false, // knight
		true, // unicorn
		false, // pawn
	};

	static const coord PAWN_MOVE_VECTORS_WHITE[][3] = {{0, 1, 0}, {0, 0, 1}};
	static const coord PAWN_CAPTURE_VECTORS_WHITE[][3] = {{1, 1, 0}, {-1, 1, 0}, {1, 0, 1}, {-1, 0, 1}, {0, 1, 1}};

	static const coord PAWN_MOVE_VECTORS_BLACK[][3] = {{0, -1, 0}, {0, 0, -1}};
	static const coord PAWN_CAPTURE_VECTORS_BLACK[][3] = {{1, -1, 0}, {-1, -1, 0}, {1, 0, -1}, {-1, 0, -1}, {0, -1, -1}};

	static const coord UNICORN_MOVE_VECTORS[][3] =
	{
		{1, 1, 1},
		{-1, 1, 1}, {1, -1, -1},
		{1, -1, 1}, {-1, 1, -1},
		{1, 1, -1}, {-1, -1, 1},
		{-1, -1, -1},
	};

	static const coord ROOK_MOVE_VECTORS[][3] =
	{
		{1, 0, 0}, {-1, 0, 0},
		{0, 1, 0}, {0, -1, 0},
		{0, 0, 1}, {0, 0, -1},
	};

	static const coord BISHOP_MOVE_VECTORS[][3] =
	{
		{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
		{1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
		{0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},
	};

	static const coord KNIGHT_MOVE_VECTORS[][3] =
	{
		{0, 1, 2}, {0, -1, 2}, {0, 1, -2}, {0, -1, -2},
		{1, 0, 2}, {-1, 0, 2}, {1, 0, -2}, {-1, 0, -2},
		{1, 2, 0}, {-1, 2, 0}, {1, -2, 0}, {-1, -2, 0},
		{2, 1, 0}, {-2, 1, 0}, {2, -1, 0}, {-2, -1, 0},
		{2, 0, 1}, {-2, 0, 1}, {2, 0, -1}, {-2, 0, -1},
		{0, 2, 1}, {0, -2, 1}, {0, 2, -1}, {0, -2, -1},
	};

	static const coord QUEEN_MOVE_VECTORS[][3] =
	{
		// rook move vectors
		{1, 0, 0}, {-1, 0, 0},
		{0, 1, 0}, {0, -1, 0},
		{0, 0, 1}, {0, 0, -1},

		// bishop move vectors
		{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
		{1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
		{0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},

		// unicorn move vectors
		{1, 1, 1},
		{-1, 1, 1}, {1, -1, -1},
		{1, -1, 1}, {-1, 1, -1},
		{1, 1, -1}, {-1, -1, 1},
		{-1, -1, -1},
	};

	static const coord KING_MOVE_VECTORS[][3] =
	{
		// rook move vectors
		{1, 0, 0}, {-1, 0, 0},
		{0, 1, 0}, {0, -1, 0},
		{0, 0, 1}, {0, 0, -1},

		// bishop move vectors
		{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
		{1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
		{0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},

		// unicorn move vectors
		{1, 1, 1},
		{-1, 1, 1}, {1, -1, -1},
		{1, -1, 1}, {-1, 1, -1},
		{1, 1, -1}, {-1, -1, 1},
		{-1, -1, -1},
	};
};

#endif // __CONSTANTS_H__