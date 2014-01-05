#include "configuration.h"
#include "constants.h"
#include "utils.h"
#include "board.h"

BitBoardMovePool::BitBoardMovePool()
{
	for(int i = 0; i < COUNT_OF(pool); ++i)
	{
		for(int j = 0; j < COUNT_OF(pool[i]); ++j)
		{
			pool[i][j] = BitBoard();
		}
	}
	for(int i = 0; i < COUNT_OF(pawnCapturePool); ++i)
	{
		for(int j = 0; j < COUNT_OF(pawnCapturePool[i]); ++j)
		{
			pawnCapturePool[i][j] = BitBoard();
		}
	}
}

void BitBoardMovePool::Initalize()
{
	const int boardSize = Config::BOARD_SIZE;
	auto initPieceMoves = [boardSize] (BitBoard dest[], const coord srcVectors[][3], int srcVectorSize, bool scaleable)
	{
		for(int i = 0; i < boardSize; ++i)
		{
			dest[i] = CalculateBitBoard( ChessVector(i), srcVectors, srcVectorSize, scaleable);
		}
	};

	// initialize move pool
	initPieceMoves( pool[Config::KING], Const::KING_MOVE_VECTORS, VECTORS_COUNT(Const::KING_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::KING]);
	initPieceMoves( pool[Config::QUEEN], Const::QUEEN_MOVE_VECTORS, VECTORS_COUNT(Const::QUEEN_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::QUEEN]);
	initPieceMoves( pool[Config::ROOK], Const::ROOK_MOVE_VECTORS, VECTORS_COUNT(Const::ROOK_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::ROOK]);
	initPieceMoves( pool[Config::BISHOP], Const::BISHOP_MOVE_VECTORS, VECTORS_COUNT(Const::BISHOP_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::BISHOP]);
	initPieceMoves( pool[Config::KNIGHT], Const::KNIGHT_MOVE_VECTORS, VECTORS_COUNT(Const::KNIGHT_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::KNIGHT]);
	initPieceMoves( pool[Config::UNICORN], Const::UNICORN_MOVE_VECTORS, VECTORS_COUNT(Const::UNICORN_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::UNICORN]);
	initPieceMoves( pool[Config::PAWN + Config::WHITE], Const::PAWN_MOVE_VECTORS_WHITE, VECTORS_COUNT(Const::PAWN_MOVE_VECTORS_WHITE), Const::PIECE_MOVE_SCALING[Config::PAWN]);
	initPieceMoves( pool[Config::PAWN + Config::BLACK], Const::PAWN_MOVE_VECTORS_BLACK, VECTORS_COUNT(Const::PAWN_MOVE_VECTORS_BLACK), Const::PIECE_MOVE_SCALING[Config::PAWN]);

	// initialize pawn capture pool
	initPieceMoves( pawnCapturePool[Config::WHITE], Const::PAWN_CAPTURE_VECTORS_WHITE, VECTORS_COUNT(Const::PAWN_CAPTURE_VECTORS_WHITE), Const::PIECE_MOVE_SCALING[Config::PAWN]);
	initPieceMoves( pawnCapturePool[Config::BLACK], Const::PAWN_CAPTURE_VECTORS_BLACK, VECTORS_COUNT(Const::PAWN_CAPTURE_VECTORS_BLACK), Const::PIECE_MOVE_SCALING[Config::PAWN]);

	// initialize vector pool
	auto initPieceVectors = [] (DynamicArray<ChessVector>& dest, const coord srcVectors[][3], int srcVectorSize)
	{
		for(int i = 0; i < srcVectorSize; ++i)
		{
			dest += ChessVector(srcVectors[i]);
		}
	};

	initPieceVectors( vectorPool[Config::KING], Const::KING_MOVE_VECTORS, VECTORS_COUNT(Const::KING_MOVE_VECTORS));
	initPieceVectors( vectorPool[Config::QUEEN], Const::QUEEN_MOVE_VECTORS, VECTORS_COUNT(Const::QUEEN_MOVE_VECTORS));
	initPieceVectors( vectorPool[Config::ROOK], Const::ROOK_MOVE_VECTORS, VECTORS_COUNT(Const::ROOK_MOVE_VECTORS));
	initPieceVectors( vectorPool[Config::BISHOP], Const::BISHOP_MOVE_VECTORS, VECTORS_COUNT(Const::BISHOP_MOVE_VECTORS));
	initPieceVectors( vectorPool[Config::KNIGHT], Const::KNIGHT_MOVE_VECTORS, VECTORS_COUNT(Const::KNIGHT_MOVE_VECTORS));
	initPieceVectors( vectorPool[Config::UNICORN], Const::UNICORN_MOVE_VECTORS, VECTORS_COUNT(Const::UNICORN_MOVE_VECTORS));
	initPieceVectors( vectorPool[Config::PAWN + Config::WHITE], Const::PAWN_MOVE_VECTORS_WHITE, VECTORS_COUNT(Const::PAWN_MOVE_VECTORS_WHITE));
	initPieceVectors( vectorPool[Config::PAWN + Config::BLACK], Const::PAWN_MOVE_VECTORS_BLACK, VECTORS_COUNT(Const::PAWN_MOVE_VECTORS_BLACK));
}

BitBoard BitBoardMovePool::CalculateBitBoard(ChessVector pos, const coord vectors[][3], int srcVectorSize, bool scaleable)
{
	BitBoard result;
	for(int i = 0; i < srcVectorSize; ++i)
	{
		ChessVector dest = pos + ChessVector(vectors[i]);
		if(Board::ValidVector(dest))
		{
			result.SetBits( Config::BITBOARD_BIT, dest.GetVectorCoord());
		}

		if( scaleable)
		{
			for(int scale = 2; scale <= Config::BOARD_SIDE; ++scale)
			{
				dest = pos + (ChessVector(vectors[i]) * scale);
				if(Board::ValidVector(dest))
				{
					result.SetBits( Config::BITBOARD_BIT, dest.GetVectorCoord());
				}
			}
		}
	}
	return result;
}

BitBoard BitBoardMovePool::VectorToIntersection(ChessVector pos, ChessVector vec, const BitBoard& intersection, bool including)
{
	BitBoard result;
	ChessVector currPos = pos + vec;
	bool intersected = false;
	while(Board::ValidVector(currPos) && !intersected)
	{
		BitBoard currPosBitBoard;
		currPosBitBoard.SetBits(Config::BITBOARD_BIT, currPos.GetVectorCoord());
		// if there is intersection record it if we include intersection, otherwise just record this to the resulting vector
		if(currPosBitBoard & intersection)
		{
			intersected = true;
			if(including)
			{
				result |= currPosBitBoard;
			}
		}
		else
		{
			result |= currPosBitBoard;
		}
		currPos += vec;
	}
	return result;
}

BitBoard BitBoardMovePool::GetPieceMoves(Piece p, const BitBoard& friendlyPieces, const BitBoard& enemyPieces) const
{
	BitBoard result;
	// if the piece has scalable vector movement, we must calculate all the vectors manually
	Config::PieceType pType = p.GetType();
	if(Const::PIECE_MOVE_SCALING[pType])
	{
		const DynamicArray<ChessVector>& pieceVectorPool = vectorPool[pType];
		BitBoard friendlyResult;
		BitBoard enemyResult;
		ChessVector piecePosition = p.GetPositionVector();
		for(int i = 0; i < pieceVectorPool.Count(); ++i)
		{
			friendlyResult |= VectorToIntersection(piecePosition, pieceVectorPool[i], friendlyPieces, false);
			enemyResult |= VectorToIntersection(piecePosition, pieceVectorPool[i], enemyPieces, true);
		}
		result = friendlyResult & enemyResult;
	}
	else if(pType != Config::PAWN)
	{
		result = (pool[pType][p.GetPositionCoord()] & enemyPieces) | (pool[pType][p.GetPositionCoord()] & (~ (friendlyPieces | enemyPieces)));
	}
	else
	{
		result = ((~ (friendlyPieces | enemyPieces)) & pool[pType + p.GetColour()][p.GetPositionCoord()]) | (pawnCapturePool[p.GetColour()][p.GetPositionCoord()] & enemyPieces);
	}
	return result;
}

Board::Board()
	: pieces(Config::PLAYER_PIECES_COUNT * 2)
{}

Board::Board(const DynamicArray< Piece >& pieceArray)
	: pieces(pieceArray)
{}

Board::Board(const Board& copy)
	: pieces(copy.pieces)
{}

Board& Board::operator=(const Board& assign)
{
	if(this != &assign)
	{
		pieces = assign.pieces;
	}
	return *this;
}

bool Board::ValidVector(ChessVector vec)
{
	return vec.x >= 0 && vec.x < Config::BOARD_SIDE
		&& vec.y >= 0 && vec.y < Config::BOARD_SIDE
		&& vec.z >= 0 && vec.z < Config::BOARD_SIDE;
}

BitBoard Board::GetPiecesBitBoard(Config::PlayerColour colour) const
{
	BitBoard result;
	for(int i = 0; i < pieces.Count(); ++i)
	{
		if(pieces[i].GetColour() == colour || colour == Config::BOTH_COLOURS)
		{
			result.SetBits(Config::BITBOARD_BIT, pieces[i].GetPositionCoord());
		}
	}
	return result;
}

bool Board::MovePiece(Piece piece, ChessVector pos, const BitBoardMovePool * movePool)
{
	bool result = false;
	if(movePool)
	{
		BitBoard availableMoves = movePool->GetPieceMoves(piece, GetPiecesBitBoard(piece.GetColour()), GetPiecesBitBoard(Config::GetOppositePlayer(piece.GetColour())));
		result = MovePiece(piece, pos, availableMoves);
	}
	return result;
}

bool Board::MovePiece(Piece piece, ChessVector pos, const BitBoard& availableMoves)
{
	bool result = false;
	//check if this piece is on the board
	int pieceIndex = GetPieceIndex(piece.GetPositionVector());
	if(pieceIndex >= 0 && pieces[pieceIndex] == piece)
	{
		int destinationIndex = GetPieceIndex(pos);
		BitBoard destBitBoard(pos);
		if(destBitBoard & availableMoves)
		{
			pieces[pieceIndex].SetPositionVector(pos);
			if(destinationIndex >= 0)
			{
				pieces.RemoveItem(destinationIndex);
			}
			result = true;
		}
	}
	return result;
}

int Board::GetPieceIndex(ChessVector pos) const
{
	int index = -1;
	for(int i = 0; i < pieces.Count() && index == -1; ++i)
	{
		if(pieces[i].GetPositionVector() == pos)
			index = i;
	}
	return index;
}

Piece Board::GetPiece(ChessVector pos) const
{
	int index = GetPieceIndex(pos);
	return (index != -1 ? pieces[index] : Piece());
}

void Board::AddPiece(Piece piece)
{
	pieces += piece;
}

void Board::RemovePiece(ChessVector pos)
{
	int index = GetPieceIndex(pos);
	if( index != -1)
		pieces.RemoveItem(index);
}

BoardTileState::BoardTileState()
{
	for(int i = 0; i < COUNT_OF(tiles); ++i)
	{
		tiles[i] = Config::TILE_NORMAL;
		changed[i] = true;
	}
}

BoardTileState::BoardTileState(const BoardTileState& copy)
{
	for(int i = 0; i < COUNT_OF(tiles); ++i)
	{
		tiles[i] = copy.tiles[i];
		changed[i] = true;
	}
}

BoardTileState& BoardTileState::operator=(const BoardTileState& assign)
{
	for(int i = 0; i < COUNT_OF(tiles); ++i)
	{
		tiles[i] = assign.tiles[i];
		changed[i] = true;
	}
	return *this;
}

Config::TileType BoardTileState::GetBoardTileState(ChessVector pos) const
{
	return tiles[pos.GetVectorCoord()];
}

void BoardTileState::SetBoardTileState(Config::TileType state, ChessVector pos)
{
	coord index = pos.GetVectorCoord();
	if(tiles[index] != state)
	{
		tiles[index] = state;
		changed[index] = true;
	}
}

void BoardTileState::SetBoardTileState(Config::TileType state, const BitBoard& bb)
{
	for(coord pos = 0; pos < Config::BOARD_SIZE; ++pos)
	{
		if(bb.GetBits(pos, Config::BITBOARD_BIT) && tiles[pos] != state)
		{
			tiles[pos] = state;
			changed[pos] = true;
		}
	}
}

void BoardTileState::SetChanged(bool flag)
{
	for(int i = 0; i < COUNT_OF(changed); ++i)
	{
		changed[i] = flag;
	}
}

bool BoardTileState::GetChangedTile(ChessVector pos) const
{
	return changed[pos.GetVectorCoord()];
}