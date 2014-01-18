#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "configuration.h"
#include "utils.h"
#include "piece.h"

namespace Const
{
	static const ChessVector INVALID_CHESS_VECTOR = ChessVector(-1, -1, -1);

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

	static const Piece INITIAL_PIECES[] =
	{
		Piece(Config::KING, Config::WHITE, ChessVector(2, 0, 0)),
		Piece(Config::KING, Config::BLACK, ChessVector(2, 4, 4)),

		Piece(Config::QUEEN, Config::WHITE, ChessVector(2, 0, 1)),
		Piece(Config::QUEEN, Config::BLACK, ChessVector(2, 4, 3)),

		Piece(Config::ROOK, Config::WHITE, ChessVector(0, 0, 0)),
		Piece(Config::ROOK, Config::WHITE, ChessVector(4, 0, 0)),
		
		Piece(Config::ROOK, Config::BLACK, ChessVector(0, 4, 4)),
		Piece(Config::ROOK, Config::BLACK, ChessVector(4, 4, 4)),

		Piece(Config::KNIGHT, Config::WHITE, ChessVector(1, 0, 0)),
		Piece(Config::KNIGHT, Config::WHITE, ChessVector(3, 0, 0)),

		Piece(Config::KNIGHT, Config::BLACK, ChessVector(1, 4, 4)),
		Piece(Config::KNIGHT, Config::BLACK, ChessVector(3, 4, 4)),

		Piece(Config::BISHOP, Config::WHITE, ChessVector(0, 0, 1)),
		Piece(Config::BISHOP, Config::WHITE, ChessVector(3, 0, 1)),

		Piece(Config::BISHOP, Config::BLACK, ChessVector(0, 4, 3)),
		Piece(Config::BISHOP, Config::BLACK, ChessVector(3, 4, 3)),

		Piece(Config::UNICORN, Config::WHITE, ChessVector(1, 0, 1)),
		Piece(Config::UNICORN, Config::WHITE, ChessVector(4, 0, 1)),

		Piece(Config::UNICORN, Config::BLACK, ChessVector(1, 4, 3)),
		Piece(Config::UNICORN, Config::BLACK, ChessVector(4, 4, 3)),

		Piece(Config::PAWN, Config::WHITE, ChessVector(0, 1, 0)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(1, 1, 0)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(2, 1, 0)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(3, 1, 0)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(4, 1, 0)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(0, 1, 1)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(1, 1, 1)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(2, 1, 1)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(3, 1, 1)),
		Piece(Config::PAWN, Config::WHITE, ChessVector(4, 1, 1)),

		Piece(Config::PAWN, Config::BLACK, ChessVector(0, 3, 4)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(1, 3, 4)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(2, 3, 4)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(3, 3, 4)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(4, 3, 4)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(0, 3, 3)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(1, 3, 3)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(2, 3, 3)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(3, 3, 3)),
		Piece(Config::PAWN, Config::BLACK, ChessVector(4, 3, 3)),
	};

	// the line at which the pawns reproduce themselves
	static const ChessVector PAWN_REPRODUCE_VECTORS[Config::PCOLOUR_COUNT] =
	{
		ChessVector(0, 4, 4), // for the white pawns
		ChessVector(0, 0, 0), // for the black pawns
	};
};

#endif // __CONSTANTS_H__