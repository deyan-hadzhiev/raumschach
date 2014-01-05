#ifndef __BOARD_H__
#define __BOARD_H__

#include "utils.h"
#include "piece.h"

class BitBoardMovePool
{
public:
	BitBoardMovePool();
	
	void Initalize();
	BitBoard GetPieceMoves(Piece p, const BitBoard& friendlyPieces, const BitBoard& enemyPieces) const;

private:
	// disable copy and assign
	BitBoardMovePool(const BitBoardMovePool&);
	BitBoardMovePool& operator=(const BitBoardMovePool&);

	static BitBoard CalculateBitBoard(ChessVector pos, const coord vectors[][3], int srcVectorSize, bool scaleable);
	static BitBoard VectorToIntersection(ChessVector pos, ChessVector vec, const BitBoard& intersection, bool including);

	BitBoard pool[Config::PIECE_TYPE_COUNT + 1][Config::BOARD_SIZE];
	BitBoard pawnCapturePool[Config::PCOLOUR_COUNT][Config::BOARD_SIZE];

	DynamicArray<ChessVector> vectorPool[Config::PIECE_TYPE_COUNT + 1];
};

class BoardTileState
{
public:
	BoardTileState();
	BoardTileState(const BoardTileState& copy);

	BoardTileState& operator= (const BoardTileState& assign);

	Config::TileType GetBoardTileState(ChessVector pos) const;
	void SetBoardTileState(Config::TileType state, ChessVector pos);
	void SetBoardTileState(Config::TileType state, const BitBoard& bb);

	void SetChanged(bool flag);
	bool GetChangedTile(ChessVector pos) const;

private:
	bool changed[Config::BOARD_SIZE];
	Config::TileType tiles[Config::BOARD_SIZE];
};

class Board
{
public:
	Board();
	Board(const DynamicArray< Piece >& piece);
	Board(const Board& copy);

	Board& operator=(const Board& assign);

	static bool ValidVector(ChessVector vec);

	BitBoard GetPiecesBitBoard(Config::PlayerColour colour) const;

	// returns true if the move is valid and is done, false otherwise
	bool MovePiece(Piece piece, ChessVector pos, const BitBoardMovePool * movePool);
	bool MovePiece(Piece piece, ChessVector pos, const BitBoard& availableMoves);
	
	Piece GetPiece(ChessVector pos) const;
	void AddPiece(Piece piece);
	void RemovePiece(ChessVector pos);
private:
	// get the index of the piece at the specified position, or -1 if there is no piece there
	int GetPieceIndex(ChessVector pos) const;

	DynamicArray< Piece > pieces;
};
#endif // __BOARD_H__