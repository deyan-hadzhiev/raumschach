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
	const int boardSize = Config::BOARD_SIZE * Config::BOARD_SIZE * Config::BOARD_SIZE;
	auto initPieceMoves = [boardSize] (BitBoard dest[], const coord srcVectors[][3], int srcVectorSize, bool scaleable)
	{
		for(int i = 0; i < boardSize; ++i)
		{
			dest[i] = CalculateBitBoard( ChessVector(i), srcVectors, srcVectorSize, scaleable);
		}
	};

	initPieceMoves( pool[Config::KING], Const::KING_MOVE_VECTORS, VECTORS_COUNT(Const::KING_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::KING]);
	initPieceMoves( pool[Config::QUEEN], Const::QUEEN_MOVE_VECTORS, VECTORS_COUNT(Const::QUEEN_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::QUEEN]);
	initPieceMoves( pool[Config::ROOK], Const::ROOK_MOVE_VECTORS, VECTORS_COUNT(Const::ROOK_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::ROOK]);
	initPieceMoves( pool[Config::BISHOP], Const::BISHOP_MOVE_VECTORS, VECTORS_COUNT(Const::BISHOP_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::BISHOP]);
	initPieceMoves( pool[Config::KNIGHT], Const::KNIGHT_MOVE_VECTORS, VECTORS_COUNT(Const::KNIGHT_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::KNIGHT]);
	initPieceMoves( pool[Config::UNICORN], Const::UNICORN_MOVE_VECTORS, VECTORS_COUNT(Const::UNICORN_MOVE_VECTORS), Const::PIECE_MOVE_SCALING[Config::UNICORN]);
	initPieceMoves( pool[Config::PAWN + Config::PCOLOUR_WHITE], Const::PAWN_MOVE_VECTORS_WHITE, VECTORS_COUNT(Const::PAWN_MOVE_VECTORS_WHITE), Const::PIECE_MOVE_SCALING[Config::PAWN]);
	initPieceMoves( pool[Config::PAWN + Config::PCOLOUR_BLACK], Const::PAWN_MOVE_VECTORS_BLACK, VECTORS_COUNT(Const::PAWN_MOVE_VECTORS_BLACK), Const::PIECE_MOVE_SCALING[Config::PAWN]);

	initPieceMoves( pawnCapturePool[Config::PCOLOUR_WHITE], Const::PAWN_CAPTURE_VECTORS_WHITE, VECTORS_COUNT(Const::PAWN_CAPTURE_VECTORS_WHITE), Const::PIECE_MOVE_SCALING[Config::PAWN]);
	initPieceMoves( pawnCapturePool[Config::PCOLOUR_BLACK], Const::PAWN_CAPTURE_VECTORS_BLACK, VECTORS_COUNT(Const::PAWN_CAPTURE_VECTORS_BLACK), Const::PIECE_MOVE_SCALING[Config::PAWN]);
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
			for(int scale = 2; scale <= Config::BOARD_SIZE; ++scale)
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

BitBoard BitBoardMovePool::GetPieceMoves(Piece p) const
{
	if(p.GetType() == Config::PAWN)
	{
		return pool[Config::PAWN + p.GetColour()][p.GetPositionCoord()];
	}
	else
	{
		return pool[p.GetType()][p.GetPositionCoord()];
	}
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
	return vec.x >= 0 && vec.x < Config::BOARD_SIZE
		&& vec.y >= 0 && vec.y < Config::BOARD_SIZE
		&& vec.z >= 0 && vec.z < Config::BOARD_SIZE;
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