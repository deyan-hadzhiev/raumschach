#ifndef __BOARD_H__
#define __BOARD_H__

#include "utils.h"
#include "piece.h"

class Board;

class BitBoardMovePool
{
public:
	BitBoardMovePool();
	~BitBoardMovePool();
	
	void Initalize();
	// used only for slight optimizations - the proper moves should still be calculated by the GetPieceMoves(...)
	// but this is a lot faster and can be used just to check the visibility of piece and tile
	BitBoard GetPieceFullMoves(Piece p) const;
	// calculates the posible moves for the speicified piece by checking for blocking friendly or enemy pieces
	BitBoard GetPieceMoves(Piece p, const BitBoard& friendlyPieces, const BitBoard& enemyPieces, const Board * board) const;

private:
	// disable copy and assign
	BitBoardMovePool(const BitBoardMovePool&);
	BitBoardMovePool& operator=(const BitBoardMovePool&);

	static BitBoard CalculateBitBoard(ChessVector pos, const coord vectors[][3], int srcVectorSize, bool scaleable);
	static BitBoard VectorToIntersection(ChessVector pos, ChessVector vec, const BitBoard& intersection, bool including);

	BitBoard * pool[Config::PIECE_TYPE_COUNT + 1];
	BitBoard * pawnCapturePool[Config::PCOLOUR_COUNT];

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

	// set all the tiles flag to the specifed state
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
	Board(BitBoardMovePool * pool);
	Board(const DynamicArray< Piece >& piece, BitBoardMovePool * pool);
	Board(const Board& copy);

	Board& operator=(const Board& assign);

	static bool ValidVector(ChessVector vec);

	// returns a BitBoard with all the pieces of the speciefied colour's bits set to 1
	BitBoard GetPiecesBitBoard(Config::PlayerColour colour) const;

	// copies all the pieces with the specified colour to the destination array
	void GetPiecesArray(DynamicArray<Piece>& dest, Config::PlayerColour colour) const;

	// check if this move is valid (short and slower version)
	bool ValidMove(Piece piece, ChessVector pos) const;
	// check if this move is valid
	bool ValidMove(Piece piece, ChessVector pos, const BitBoard& availableMoves) const;

	/** Move a piece on the board (short and slow version, because it calculates the available move positions)
	* @param piece : The piece to be moved
	* @param pos : The destination position
	* @param pretested : If the validation was already done before
	* @retval : true if the move was done, false otherwise
	*/
	bool MovePiece(Piece piece, ChessVector pos, bool pretested = false);
	
	/** Move a piece on the board
	* @param piece : The piece to be moved
	* @param pos : The destination position
	* @param availableMoves : BitBoard containing all the available move positions
	* @param pretested : If the validation was already done before
	* @retval : true if the move was done, false otherwise
	*/
	bool MovePiece(Piece piece, ChessVector pos, const BitBoard& availableMoves, bool pretested = false);

	// returns true if the king with the specified colour is under check
	Config::KingState KingCheckState(Config::PlayerColour col) const;
	// returns true if the tile is threatened by any piece of the specified player colour
	bool TileThreatened(ChessVector pos, Config::PlayerColour col) const;

	// only the king is special enough to have a function getting it
	Piece GetKing(Config::PlayerColour col) const;

	Piece GetPiece(ChessVector pos) const;
	void AddPiece(Piece piece);
	void RemovePiece(ChessVector pos);

	void SetMovePool(BitBoardMovePool * pool);
private:
	// get the index of the piece at the specified position, or -1 if there is no piece there
	int GetPieceIndex(ChessVector pos) const;

	DynamicArray< Piece > pieces;
	BitBoardMovePool * movePool;
};
#endif // __BOARD_H__