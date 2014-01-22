#ifndef __BOARD_H__
#define __BOARD_H__

#include "utils.h"
#include "piece.h"
#include "random_generator.h"

class Move
{
public:
	Move() : piece(), destination(0, 0, 0), heuristic(0), pieceMoves() {}
	Move(Piece p, ChessVector dest, const BitBoard& moves, int h = 0)
		:	piece(p), destination(dest), pieceMoves(moves), heuristic(h)
	{}
	Move(const Move& copy)
		:	piece(copy.piece), destination(copy.destination), pieceMoves(copy.pieceMoves), heuristic(copy.heuristic)
	{}
	inline Move& operator=(const Move& assign)
	{
		if(this != &assign)
		{
			piece = assign.piece;
			destination = assign.destination;
			pieceMoves = assign.pieceMoves;
			heuristic = assign.heuristic;
		}
		return *this;
	}

	inline friend bool operator<(const Move& rhs, const Move& lhs)
	{
		return rhs.heuristic > lhs.heuristic;
	}

	Piece piece;
	ChessVector destination;
	int heuristic;
	BitBoard pieceMoves;
};

class MadeMove
{
public:
	MadeMove() : removedPiece(), sourcePosition() {}
	MadeMove(Piece p, ChessVector src) : removedPiece(p), sourcePosition(src) {}
	MadeMove(const MadeMove& copy) : removedPiece(copy.removedPiece), sourcePosition(copy.sourcePosition) {}
	inline MadeMove& operator=(const MadeMove& assign)
	{
		removedPiece = assign.removedPiece;
		sourcePosition = assign.sourcePosition;
		return *this;
	}

	Piece removedPiece;
	ChessVector sourcePosition;
};

class Board;

class BitBoardMovePool
{
public:
	BitBoardMovePool();
	~BitBoardMovePool();

	void Initalize(RandomGenerator * randGen);
	// used only for slight optimizations - the proper moves should still be calculated by the GetPieceMoves(...)
	// but this is a lot faster and can be used just to check the visibility of piece and tile
	inline BitBoard GetPieceFullMoves(Piece p) const
	{
		const Config::PieceType pieceType = p.GetType();
		const coord piecePos = p.GetPositionCoord();
		const Config::PlayerColour pieceColour = p.GetColour();
		if(pieceType != Config::PAWN)
		{
			return pool[pieceType][piecePos];
		}
		else
		{
			return pool[pieceType + pieceColour][piecePos] | pawnCapturePool[pieceColour][piecePos];
		}
	}

	// returns the piece hash value based on its position and type
	inline unsigned long long GetPieceHash(Piece p) const
	{
		if(p.GetType() != Config::NO_TYPE)
		{
			return hashTable[p.GetType() + (p.GetColour() * Config::PIECE_TYPE_COUNT)][p.GetPositionCoord()];
		}
		return 0ULL;
	}

	/** calculates the posible moves for the speicified piece by checking for blocking friendly or enemy pieces
	@param p: The piece which moves we are checking
	@param friendlyPieces: The BitBoard of the friendly pieces, we could obtain it but this is easier
	@param enemyPieces: The BitBoard of the enemy pieces, we could obtain it but this is easier
	@param board: A pointer to a board, from which we will take the pieces, we need it for kings
	@param includeFriendly[optional]: To specify, whether we shall include friendly pieces, this is used when we check if a piece is protected by another one.
	*/
	BitBoard GetPieceMoves(Piece p, const BitBoard& friendlyPieces, const BitBoard& enemyPieces, const Board * board, bool includeFriendly = false) const;

private:
	// initializes the zobrist hash table
	void InitZobristHash(RandomGenerator * randGen);

	// disable copy and assign
	BitBoardMovePool(const BitBoardMovePool&);
	BitBoardMovePool& operator=(const BitBoardMovePool&);

	static BitBoard CalculateBitBoard(ChessVector pos, const coord vectors[][3], int srcVectorSize, bool scaleable);

	BitBoard * pool[Config::PIECE_TYPE_COUNT + 1];
	BitBoard * pawnCapturePool[Config::PCOLOUR_COUNT];

	unsigned long long * hashTable[Config::PIECE_TYPE_COUNT * 2];

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
	// set a specific tile to the specified state
	void SetChanged(ChessVector pos, bool flag = true);
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

	inline static bool ValidVector(ChessVector vec)
	{
		return vec.x >= 0 && vec.x < Config::BOARD_SIDE
			&& vec.y >= 0 && vec.y < Config::BOARD_SIDE
			&& vec.z >= 0 && vec.z < Config::BOARD_SIDE;
	}

	inline static bool ValidCoord(coord pos)
	{
		return pos >= 0 && pos < ((coord) Config::BOARD_SIZE);
	}

	// returns a BitBoard with all the pieces of the speciefied colour's bits set to 1
	inline BitBoard GetPiecesBitBoard(Config::PlayerColour colour) const
	{
		if(colour != Config::BOTH_COLOURS)
		{
			return piecesBitBoards[colour];
		}
		else
		{
			return piecesBitBoards[Config::WHITE] | piecesBitBoards[Config::BLACK];
		}
	}

	/** Copies all the pieces with the specified colour to the destination array
	* @param colour[in] : The colour of the player which pieces will be pushed to the array
	* @param dest[out] : The destination array in which the pieces will be pushed
	*/
	inline void GetPiecesArray(Config::PlayerColour colour, DynamicArray<Piece>& dest) const
	{
		dest.Clear();
		if(colour != Config::BOTH_COLOURS)
		{
			dest = pieces[colour];
		}
		else
		{
			dest = pieces[Config::WHITE];
			dest.Append(pieces[Config::BLACK]);
		}
	}

	/** Retrieves all the possible moves a player can make on the given board
	* @param colour[in] : The colour of the player, which moves we're getting
	* @param moveArray[out] : The destination array in which the moves will be pushed
	*/
	void GetPossibleMoves(Config::PlayerColour colour, DynamicArray<Move>& moveArray);

	// check if this move is valid (short and slower version)
	bool ValidMove(Piece piece, ChessVector pos);
	// check if this move is valid
	bool ValidMove(Piece piece, ChessVector pos, const BitBoard& availableMoves);

	// check if this move is possible, i.e. the piece can move there and no check is revealed with it
	bool PossibleMove(Piece piece, ChessVector pos, const BitBoard& availableMoves) const;

	/** Move a piece on the board (short and slow version, because it calculates the available move positions)
	* @param piece : The piece to be moved
	* @param pos : The destination position
	* @param pretested : If the validation was already done before
	* @retval : class representing a made move, so it can be saved and undone later
	*/
	MadeMove MovePiece(Piece piece, ChessVector pos, bool pretested = false);

	/** Move a piece on the board
	* @param piece : The piece to be moved
	* @param pos : The destination position
	* @param availableMoves : BitBoard containing all the available move positions
	* @param pretested : If the validation was already done before
	* @retval : class representing a made move, so it can be saved and undone later
	*/
	MadeMove MovePiece(Piece piece, ChessVector pos, const BitBoard& availableMoves, bool pretested = false);

	/** Undoes a previously made move by adding the removed piece (if any) and returning the moved piece to it's original position
	* @param lastMove : The last done move, that has to be undone
	*/
	void UndoMove(const MadeMove& move);

	// a simplified version of KingCheckState(...) just for checking whether the king is under check
	bool KingInCheck(Config::PlayerColour col) const;
	// returns true if the king with the specified colour is under check
	Config::KingState KingCheckState(Config::PlayerColour col);
	// returns true if the game is stalemate by pieces
	bool PieceStalemate() const;
	// returns true if the tile is threatened by any piece of the specified player colour
	bool TileThreatened(ChessVector pos, Config::PlayerColour col) const;

	// only the king is special enough to have a function getting it
	Piece GetKing(Config::PlayerColour col) const;

	/** Returns the current material balance
	* NOTE: The value is positive if the White pieces have more material and negative if the black do.
	*/
	int GetMaterialBalance() const;

	// Returns the hash of the current board
	inline unsigned long long GetHash() const
	{
		unsigned long long hash = 0ULL;

		for(int i = 0; i < pieces[Config::WHITE].Count(); ++i)
		{
			hash ^= movePool->GetPieceHash(pieces[Config::WHITE][i]);
		}

		for(int i = 0; i < pieces[Config::BLACK].Count(); ++i)
		{
			hash ^= movePool->GetPieceHash(pieces[Config::BLACK][i]);
		}

		return hash;
	}

	// Returns the worth of the tile
	int GetTileWorth(ChessVector pos) const;

	Piece GetPiece(ChessVector pos) const
	{
		const Config::PlayerColour colour = GetPieceColour(pos);
		if(colour == Config::BOTH_COLOURS)
			return Piece();

		const int index = GetPieceIndex(pos);

		return (index != -1 ? pieces[colour][index] : Piece());
	}

	void AddPiece(Piece piece);
	void RemovePiece(ChessVector pos);

	void SetMovePool(BitBoardMovePool * pool);
private:
	inline Config::PlayerColour GetPieceColour(ChessVector pos) const
	{
		Config::PlayerColour colour = Config::BOTH_COLOURS;
		if(piecesBitBoards[Config::WHITE].GetBit(pos.GetVectorCoord()))
		{
			colour = Config::WHITE;
		}
		else if(piecesBitBoards[Config::BLACK].GetBit(pos.GetVectorCoord()))
		{
			colour = Config::BLACK;
		}
		return colour;
	}

	// get the index of the piece at the specified position, or -1 if there is no piece there
	inline int GetPieceIndex(ChessVector pos) const
	{
		const coord destCoord = pos.GetVectorCoord();
		int index = -1;
		const Config::PlayerColour colour = GetPieceColour(pos);
		if(colour != Config::BOTH_COLOURS)
		{
			for(int i = 0; i < pieces[colour].Count() && index == -1; ++i)
			{
				if(pieces[colour][i].GetPositionCoord() == destCoord)
					index = i;
			}
		}
		return index;
	}

	BitBoard piecesBitBoards[Config::PCOLOUR_COUNT];

	DynamicArray< Piece > pieces[Config::PCOLOUR_COUNT];
	BitBoardMovePool * movePool;
};
#endif // __BOARD_H__