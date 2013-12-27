#ifndef __BOARD_H__
#define __BOARD_H__

#include "utils.h"
#include "piece.h"

class BitBoardMovePool
{
public:
	BitBoardMovePool();
	
	void Initalize();
	BitBoard GetPieceMoves( Piece p) const;

private:
	// disable copy and assign
	BitBoardMovePool(const BitBoardMovePool&);
	BitBoardMovePool& operator=(const BitBoardMovePool&);

	static BitBoard CalculateBitBoard(ChessVector pos, const coord vectors[][3], int srcVectorSize, bool scaleable);

	BitBoard pool[Config::PIECE_TYPE_COUNT + 1][Config::BOARD_SIZE * Config::BOARD_SIZE * Config:: BOARD_SIZE];
	BitBoard pawnCapturePool[Config::PCOLOUR_COUNT][Config::BOARD_SIZE * Config::BOARD_SIZE * Config::BOARD_SIZE];
};

class Board
{
public:
	Board();
	Board(const DynamicArray< Piece >& piece);
	Board(const Board& copy);

	Board& operator=(const Board& assign);

	static bool ValidVector(ChessVector vec);
	
	Piece GetPiece(ChessVector pos) const;
	void AddPiece(Piece piece);
	void RemovePiece(ChessVector pos);
private:
	// get the index of the piece at the specified position, or -1 if there is no piece there
	int GetPieceIndex(ChessVector pos) const;

	DynamicArray< Piece > pieces;
};

#endif // __BOARD_H__