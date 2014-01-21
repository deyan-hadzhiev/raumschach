#include "configuration.h"
#include "constants.h"
#include "utils.h"
#include "board.h"

BitBoardMovePool::BitBoardMovePool()
{
	for(int i = 0; i < COUNT_OF(pool); ++i)
	{
		pool[i] = new BitBoard[Config::BOARD_SIZE];
	}
	for(int i = 0; i < COUNT_OF(pawnCapturePool); ++i)
	{
		pawnCapturePool[i] = new BitBoard[Config::BOARD_SIZE];
	}
}

BitBoardMovePool::~BitBoardMovePool()
{
	for(int i = 0; i < COUNT_OF(pool); ++i)
	{
		delete[] pool[i];
		pool[i] = nullptr;
	}
	for(int i = 0; i < COUNT_OF(pawnCapturePool); ++i)
	{
		delete[] pawnCapturePool[i];
		pawnCapturePool[i] = nullptr;
	}
}

void BitBoardMovePool::Initalize()
{
	const int boardSize = Config::BOARD_SIZE;
	auto initPieceMoves = [boardSize] (BitBoard * dest, const coord srcVectors[][3], int srcVectorSize, bool scaleable)
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
	BitBoard currPosBitBoard;
	while(Board::ValidVector(currPos) && !intersected)
	{
		currPosBitBoard.Zero();
		currPosBitBoard.SetBits(Config::BITBOARD_BIT, currPos.GetVectorCoord());
		// if there is intersection, record it if we include intersection, otherwise just record this to the resulting vector
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

BitBoard BitBoardMovePool::GetPieceFullMoves(Piece p) const
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

BitBoard BitBoardMovePool::GetPieceMoves(Piece p, const BitBoard& friendlyPieces, const BitBoard& enemyPieces, const Board * board, bool includeFriendly) const
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
			friendlyResult |= VectorToIntersection(piecePosition, pieceVectorPool[i], friendlyPieces, false || includeFriendly);
			enemyResult |= VectorToIntersection(piecePosition, pieceVectorPool[i], enemyPieces, true);
		}
		result = friendlyResult & enemyResult;
	}
	else if(pType == Config::KNIGHT)
	{
		result = (pool[pType][p.GetPositionCoord()] & enemyPieces) | (pool[pType][p.GetPositionCoord()] & (~ (friendlyPieces | enemyPieces)));
		if(includeFriendly) result |= (pool[pType][p.GetPositionCoord()] & friendlyPieces);
	}
	else if(pType == Config::KING)
	{
		BitBoard tmp = (pool[pType][p.GetPositionCoord()] & enemyPieces) | (pool[pType][p.GetPositionCoord()] & (~ (friendlyPieces | enemyPieces)));
		if(includeFriendly) tmp |= pool[pType][p.GetPositionCoord()] & friendlyPieces;
		if(board)
		{
			DynamicArray<ChessVector> kingMoves;
			tmp.GetVectors(kingMoves);
			Config::PlayerColour oppositePlayer = Config::GetOppositePlayer(p.GetColour());
			for(int i = 0; i < kingMoves.Count(); ++i)
			{
				if(!board->TileThreatened(kingMoves[i], oppositePlayer))
				{
					result.SetBits(Config::BITBOARD_BIT, kingMoves[i].GetVectorCoord());
				}
			}
		}
	}
	else
	{
		result = ((~ (friendlyPieces | enemyPieces)) & pool[pType + p.GetColour()][p.GetPositionCoord()]) | (pawnCapturePool[p.GetColour()][p.GetPositionCoord()] & enemyPieces);
		if(includeFriendly) result |= pawnCapturePool[p.GetColour()][p.GetPositionCoord()] & friendlyPieces;
	}
	return result;
}

Board::Board()
	: pieces(Config::PLAYER_PIECES_COUNT * 2), movePool(nullptr)
{
	piecesBitBoards[Config::WHITE] = BitBoard(0ULL, 0ULL);
	piecesBitBoards[Config::BLACK] = BitBoard(0ULL, 0ULL);
}

Board::Board(BitBoardMovePool * pool)
	: pieces(Config::PLAYER_PIECES_COUNT * 2), movePool(pool)
{
	piecesBitBoards[Config::WHITE] = BitBoard(0ULL, 0ULL);
	piecesBitBoards[Config::BLACK] = BitBoard(0ULL, 0ULL);
}

Board::Board(const DynamicArray< Piece >& pieceArray, BitBoardMovePool * pool)
	: pieces(pieceArray), movePool(pool)
{
	pieces.Sort();
	for(int i = 0; i < pieces.Count(); ++i)
	{
		piecesBitBoards[pieces[i].GetColour()].SetBits(Config::BITBOARD_BIT, pieces[i].GetPositionCoord());
	}
}

Board::Board(const Board& copy)
	: pieces(copy.pieces), movePool(copy.movePool)
{
	piecesBitBoards[Config::WHITE] = copy.piecesBitBoards[Config::WHITE];
	piecesBitBoards[Config::BLACK] = copy.piecesBitBoards[Config::BLACK];
}

Board& Board::operator=(const Board& assign)
{
	if(this != &assign)
	{
		pieces = assign.pieces;
		movePool = assign.movePool;
		piecesBitBoards[Config::WHITE] = assign.piecesBitBoards[Config::WHITE];
		piecesBitBoards[Config::BLACK] = assign.piecesBitBoards[Config::BLACK];
	}
	return *this;
}

void Board::GetPiecesArray(Config::PlayerColour colour, DynamicArray<Piece>& dest) const
{
	dest.Clear();
	if(colour == Config::BOTH_COLOURS)
	{
		dest = pieces;
		return;
	}

	for(int i = 0; i < pieces.Count(); ++i)
	{
		if(pieces[i].GetColour() == colour)
		{
			dest += pieces[i];
		}
	}
}

void Board::GetPossibleMoves(Config::PlayerColour colour, DynamicArray<Move>& moveArray)
{
	// first get the list of pieces with this colour
	DynamicArray<Piece> validPieces;
	GetPiecesArray(colour, validPieces);

	BitBoard friendlyPieces = GetPiecesBitBoard(colour);
	BitBoard enemyPieces = GetPiecesBitBoard(Config::GetOppositePlayer(colour));

	// then create a move for each piece
	for(int i = 0; i < validPieces.Count(); ++i)
	{
		DynamicArray<ChessVector> destinations(Const::PIECE_MAX_MOVE_COUNT[validPieces[i].GetType()]);

		BitBoard availableMoves = movePool->GetPieceMoves(validPieces[i], friendlyPieces, enemyPieces, this);
		availableMoves.GetVectors(destinations);

		// now for every possible move, push an object to the moveArray
		for(int j = 0; j < destinations.Count(); ++j)
		{
			// add the destination to the possible moves only if it is valid
			// this saves us several computations later and is assuring that
			// this are all valid moves that can be made
			if(ValidMove(validPieces[i], destinations[j], availableMoves))
				moveArray += Move(validPieces[i], destinations[j], availableMoves);
		}
	}
}

bool Board::ValidMove(Piece piece, ChessVector pos)
{
	bool result = false;
	if(movePool)
	{
		BitBoard availableMoves = movePool->GetPieceMoves(piece, GetPiecesBitBoard(piece.GetColour()), GetPiecesBitBoard(Config::GetOppositePlayer(piece.GetColour())), this); 
		result = ValidMove(piece, pos, availableMoves);
	}
	return result;
}

bool Board::ValidMove(Piece piece, ChessVector pos, const BitBoard& availableMoves)
{
	bool result = false;
	int pieceIndex = GetPieceIndex(piece.GetPositionVector());
	if(pieceIndex >= 0 && pieces[pieceIndex] == piece && (availableMoves & BitBoard(pos)))
	{
		MadeMove move = MovePiece(piece, pos, availableMoves, true);

		// finally we test the check state after the move
		result = ! KingInCheck(piece.GetColour());

		UndoMove(move);
	}
	return result;
}

MadeMove Board::MovePiece(Piece piece, ChessVector pos, bool pretested)
{
	MadeMove result;
	if(movePool)
	{
		BitBoard availableMoves = movePool->GetPieceMoves(piece, GetPiecesBitBoard(piece.GetColour()), GetPiecesBitBoard(Config::GetOppositePlayer(piece.GetColour())), this);
		result = MovePiece(piece, pos, availableMoves, pretested);
	}
	return result;
}

MadeMove Board::MovePiece(Piece piece, ChessVector pos, const BitBoard& availableMoves, bool pretested)
{
	bool validMove = true;
	//check if this piece is on the board
	if(! pretested)
	{
		validMove = ValidMove(piece, pos, availableMoves);
	}

	MadeMove move;
	if(validMove)
	{
		int pieceIndex = GetPieceIndex(piece.GetPositionVector());
		// we must obtain the destination before actually moving the piece
		int destinationIndex = GetPieceIndex(pos);

		//copy to the MadeMove
		Piece removedPiece(Config::NO_TYPE, Config::WHITE, pos);
		if(destinationIndex >= 0)
		{
			removedPiece = pieces[destinationIndex];
		}
		move = MadeMove(removedPiece, piece.GetPositionVector());

		// do the actual move

		// update piece bit boards
		piecesBitBoards[piece.GetColour()].SetBits(0ULL, piece.GetPositionCoord());
		piecesBitBoards[piece.GetColour()].SetBits(Config::BITBOARD_BIT, pos.GetVectorCoord());

		// set the new position of the piece
		pieces[pieceIndex].SetPositionVector(pos);

		Config::PlayerColour pieceColour = pieces[pieceIndex].GetColour();
		// if the piece is a pawn and has reached the ending tile, it is considered to become a queen
		if(pieces[pieceIndex].GetType() == Config::PAWN
			&& Const::PAWN_REPRODUCE_VECTORS[pieceColour].y == pos.y
			&& Const::PAWN_REPRODUCE_VECTORS[pieceColour].z == pos.z)
		{
			pieces[pieceIndex].SetType(Config::QUEEN);
		}

		if(destinationIndex >= 0)
		{
			// update the bit boards
			piecesBitBoards[pieces[destinationIndex].GetColour()].SetBits(0ULL, pos.GetVectorCoord());

			pieces.RemoveItem(destinationIndex);
		}

		pieces.InsertionSort();
	}

	return move;
}

void Board::UndoMove(const MadeMove& move)
{
	// first return the current piece occupying the tile to it's original place
	int pieceIndex = GetPieceIndex(move.removedPiece.GetPositionVector());
	// this shouldn't be -1, but just to be sure
	if(pieceIndex >= 0)
	{
		// update bit boards
		Piece& p = pieces[pieceIndex];

		piecesBitBoards[p.GetColour()].SetBits(0ULL, p.GetPositionCoord());
		piecesBitBoards[p.GetColour()].SetBits(Config::BITBOARD_BIT, move.sourcePosition.GetVectorCoord());

		p.SetPositionVector(move.sourcePosition);
	}

	// now if this wasn't a quiet move, we must add back the removed piece
	// NOTE: after the addition the pieces will be sorted, so we need to sort only if we don't add a piece
	if(move.removedPiece.GetType() != Config::NO_TYPE)
	{
		AddPiece(move.removedPiece);
	}
	else
	{
		pieces.InsertionSort();
	}
}

bool Board::KingInCheck(Config::PlayerColour colour) const
{
	bool check = false;
	int kingIndex = -1;
	bool found = false;
	for(int i = 0; i < pieces.Count() && !found; ++i)
	{
		if(pieces[i].GetType() == Config::KING && pieces[i].GetColour() == colour)
		{
			kingIndex = i;
			found = true;
		}
	}
	if(kingIndex >= 0)
	{
		check = TileThreatened(pieces[kingIndex].GetPositionVector(), Config::GetOppositePlayer(colour));
	}
	return check;
}

Config::KingState Board::KingCheckState(Config::PlayerColour colour)
{
	using Config::KingState;
	KingState kingState = KingState::NORMAL;
	int kingIndex = -1;
	bool found = false;
	for(int i = 0; i < pieces.Count() && !found; ++i)
	{
		if(pieces[i].GetType() == Config::KING && pieces[i].GetColour() == colour)
		{
			kingIndex = i;
			found = true;
		}
	}

	Piece king;

	if(kingIndex >= 0)
	{
		king = pieces[kingIndex];
		if(TileThreatened(king.GetPositionVector(), Config::GetOppositePlayer(king.GetColour())))
		{
			kingState = KingState::CHECK;
		}
	}
	else
	{
		return Config::NO_KING;
	}

	Config::PlayerColour oppositeColour = Config::GetOppositePlayer(colour);
	BitBoard friendlyPiecesBitBoard = GetPiecesBitBoard(colour);
	BitBoard enemyPiecesBitBoard = GetPiecesBitBoard(oppositeColour);

	BitBoard kingMoves = movePool->GetPieceMoves( king, friendlyPiecesBitBoard, enemyPiecesBitBoard, this);
	DynamicArray<ChessVector> kingMovesArray;
	kingMoves.GetVectors(kingMovesArray);

	bool hasMoveableTile = false;
	// for every moveable tile, check if it isn't under check
	for(int i = 0; i < kingMovesArray.Count() && !hasMoveableTile; ++i)
	{
		hasMoveableTile |= ! TileThreatened(kingMovesArray[i], oppositeColour);
	}

	// if the king has no moveable tiles, we must check the other figures
	// this is slow operation, but will be rarely encountered
	if(!hasMoveableTile)
	{
		DynamicArray<Piece> friendlyPieces;
		GetPiecesArray(king.GetColour(), friendlyPieces);
		bool foundMove = false;
		// check every piece for available moves, until at least one available move is found
		for(int i = 0; i < friendlyPieces.Count() && !foundMove; ++i)
		{
			if(friendlyPieces[i].GetType() == Config::KING) continue;

			BitBoard pieceMovesBitBoard = movePool->GetPieceMoves(friendlyPieces[i], friendlyPiecesBitBoard, enemyPiecesBitBoard, this);

			DynamicArray<ChessVector> currentPieceMoves;
			pieceMovesBitBoard.GetVectors(currentPieceMoves);

			for(int move = 0; move < currentPieceMoves.Count() && !foundMove; ++move)
			{
				foundMove |= ValidMove(friendlyPieces[i], currentPieceMoves[move], pieceMovesBitBoard);
			}
		}

		// if no moves were found, then this is STALEMATE
		if((kingState != KingState::CHECK && !foundMove))
		{
			kingState = KingState::STALEMATE;
		}
		else if(!foundMove && kingState == KingState::CHECK) // if no moves were found and the king is under a check, this is a checkmate
		{
			kingState = KingState::CHECKMATE;
		}

	}
	else if(PieceStalemate())
	{
		kingState = KingState::STALEMATE;
	}

	return kingState;
}

bool Board::PieceStalemate() const
{
	int pieceCount[Config::PCOLOUR_COUNT][Config::PLAYER_PIECES_COUNT] = {0};
	for(int i = 0; i < pieces.Count(); ++i)
	{
		pieceCount[pieces[i].GetColour()][pieces[i].GetType()]++;
	}

	bool noStalemate = pieceCount[Config::WHITE][Config::QUEEN] > 0 || pieceCount[Config::BLACK][Config::QUEEN] > 0 // if there is at least one queen
		|| pieceCount[Config::WHITE][Config::PAWN] > 0 || pieceCount[Config::BLACK][Config::PAWN] > 0 // if there is at least one pawn (pawns reproduce)
		|| (pieceCount[Config::WHITE][Config::BISHOP] + pieceCount[Config::WHITE][Config::KNIGHT] + pieceCount[Config::WHITE][Config::ROOK] + pieceCount[Config::WHITE][Config::UNICORN]) > 2 // if we have more than two of the other figures
		|| (pieceCount[Config::BLACK][Config::BISHOP] + pieceCount[Config::BLACK][Config::KNIGHT] + pieceCount[Config::BLACK][Config::ROOK] + pieceCount[Config::BLACK][Config::UNICORN]) > 2; // if we have more than two of the other figures

	return !noStalemate;
}

bool Board::TileThreatened(ChessVector pos, Config::PlayerColour colour) const
{
	bool threatened = false;
	if( movePool)
	{
		DynamicArray< Piece > passivePieces;
		DynamicArray< Piece > activePieces;
		for(int i = 0; i < pieces.Count(); ++i)
		{
			if(pieces[i].GetColour() == colour)
			{
				activePieces += pieces[i];
			}
			else
			{
				passivePieces += pieces[i];
			}
		}

		BitBoard activeBitBoard = GetPiecesBitBoard(colour);

		BitBoard passiveBitBoard = GetPiecesBitBoard(Config::GetOppositePlayer(colour));

		// we set the position bit of the checked tile, se we can get proper results from pool->GetPieceMoves(...) for pawns
		passiveBitBoard.SetBits(Config::BITBOARD_BIT, pos.GetVectorCoord());

		BitBoard positionBitBoard(pos);
		for(int i = 0; i < activePieces.Count() && !threatened; ++i)
		{
			BitBoard pieceVisibility(movePool->GetPieceFullMoves(activePieces[i]));
			// if this tile is visible, we need to make better check, by getting the proper move bboard
			if(positionBitBoard & pieceVisibility)
			{
				Config::PieceType type = activePieces[i].GetType();
				if(! Const::PIECE_MOVE_SCALING[type]) // if this piece moves are not scaleable, then the tile is threatened
				{
					threatened = true;
					continue;
				}
				BitBoard pieceMoves(movePool->GetPieceMoves(activePieces[i], activeBitBoard, passiveBitBoard, this, true));
				if(positionBitBoard & pieceMoves)
				{
					threatened = true;
				}
			}
		}
	}
	return threatened;
}

Piece Board::GetKing(Config::PlayerColour col) const
{
	if(col == Config::WHITE || col == Config::BLACK)
	{
		for(int i = 0; i < pieces.Count(); ++i)
		{
			if( pieces[i].GetColour() == col && pieces[i].GetType() == Config::KING)
			{
				return pieces[i];
			}
		}
	}
	return Piece();
}

int Board::GetMaterialBalance() const
{
	int balance[Config::PCOLOUR_COUNT] = {0};
	for(int i = 0; i < pieces.Count(); ++i)
	{
		balance[pieces[i].GetColour()] += pieces[i].GetWorth() + pieces[i].GetPositionWorth(pieces[i].GetPositionVector());
	}
	return balance[Config::WHITE] - balance[Config::BLACK];
}

int Board::GetTileWorth(ChessVector pos) const
{
	Piece tilePiece = GetPiece(pos);
	int tileWorth = 0;
	if(tilePiece.GetType() != Config::NO_TYPE)
	{
		tileWorth = tilePiece.GetWorth();
	}
	else
	{
		tileWorth = (int) pos.GetManhattanDistance(Const::BOARD_CENTER);
	}
	return tileWorth;
}

int Board::GetPieceIndex(ChessVector pos) const
{
	const coord destCoord = pos.GetVectorCoord();
	int index = -1;
	int left = 0;
	int right = pieces.Count() - 1;
	int half;
	while(left <= right && index == -1)
	{
		half = (left + right) >> 1;
		if(pieces[half].GetPositionCoord() == destCoord)
		{
			index = half;
		}
		else if(pieces[half].GetPositionCoord() < destCoord)
		{
			left = half + 1;
		}
		else
		{
			right = half - 1;
		}
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
	piecesBitBoards[piece.GetColour()].SetBits(Config::BITBOARD_BIT, piece.GetPositionCoord());
	pieces.InsertionSort();
}

void Board::RemovePiece(ChessVector pos)
{
	int index = GetPieceIndex(pos);
	if( index != -1)
	{
		piecesBitBoards[pieces[index].GetColour()].SetBits(0ULL, pieces[index].GetPositionCoord());
		pieces.RemoveItem(index);
		pieces.InsertionSort();
	}
}

void Board::SetMovePool(BitBoardMovePool * pool)
{
	movePool = pool;
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

void BoardTileState::SetChanged(ChessVector pos, bool flag)
{
	changed[pos.GetVectorCoord()] = flag;
}

bool BoardTileState::GetChangedTile(ChessVector pos) const
{
	return changed[pos.GetVectorCoord()];
}