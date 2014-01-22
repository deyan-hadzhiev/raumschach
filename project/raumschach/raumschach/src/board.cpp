#include "configuration.h"
#include "constants.h"
#include "utils.h"
#include "board.h"
#include "random_generator.h"

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
	for(int i = 0; i < COUNT_OF(hashTable); ++i)
	{
		hashTable[i] = new unsigned long long[Config::BOARD_SIZE];
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
	for(int i = 0; i < COUNT_OF(hashTable); ++i)
	{
		delete[] hashTable[i];
		hashTable[i] = nullptr;
	}
}

void BitBoardMovePool::Initalize(RandomGenerator * randGen)
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

	InitZobristHash(randGen);
}

void BitBoardMovePool::InitZobristHash(RandomGenerator * randGen)
{
	unsigned long long randLow = 0UL;
	unsigned long long randHigh = 0UL;
	const unsigned UNSIGNED_MAX = ~0U;

	for(int i = 0; i < COUNT_OF(hashTable); ++i)
	{
		for(int j = 0; j < Config::BOARD_SIZE; ++j)
		{
			randHigh = ((unsigned long long) randGen->GetRand(UNSIGNED_MAX)) << 32;
			randLow = randGen->GetRand(UNSIGNED_MAX);
			hashTable[i][j] = randHigh | randLow;
		}
	}

}

BitBoard BitBoardMovePool::CalculateBitBoard(ChessVector pos, const coord vectors[][3], int srcVectorSize, bool scaleable)
{
	BitBoard result;
	for(int i = 0; i < srcVectorSize; ++i)
	{
		ChessVector dest = pos + ChessVector(vectors[i]);
		if(Board::ValidVector(dest))
		{
			result.SetBit(true, dest.GetVectorCoord());
		}

		if( scaleable)
		{
			for(int scale = 2; scale <= Config::BOARD_SIDE; ++scale)
			{
				dest = pos + (ChessVector(vectors[i]) * scale);
				if(Board::ValidVector(dest))
				{
					result.SetBit(true, dest.GetVectorCoord());
				}
			}
		}
	}
	return result;
}

BitBoard BitBoardMovePool::GetPieceMoves(Piece p, const BitBoard& friendlyPieces, const BitBoard& enemyPieces, const Board * board, bool includeFriendly) const
{
	BitBoard result;
	// if the piece has scalable vector movement, we must calculate all the vectors manually
	Config::PieceType pType = p.GetType();
	if(Const::PIECE_MOVE_SCALING[pType])
	{
		BitBoard unoccupiedTiles = ~ (friendlyPieces | enemyPieces);
		const DynamicArray<ChessVector>& pieceVectorPool = vectorPool[pType];
		const ChessVector piecePos = p.GetPositionVector();
		for(int i = 0; i < pieceVectorPool.Count(); ++i)
		{
			ChessVector testedPos = piecePos + pieceVectorPool[i];
			bool intersected = false;
			while(Board::ValidVector(testedPos) && !intersected)
			{
				const coord testedCoord = testedPos.GetVectorCoord();
				if(unoccupiedTiles.GetBit(testedCoord))
				{
					result.SetBit(true, testedCoord);
				}
				else // this has to be occupied by a friendly or enemy piece
				{
					intersected = true;
					if(includeFriendly || enemyPieces.GetBit(testedCoord))
					{
						result.SetBit(true, testedCoord);
					}
				}

				testedPos += pieceVectorPool[i];
			}
		}
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
					result.SetBit(true, kingMoves[i].GetVectorCoord());
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
	: movePool(nullptr)
{
	piecesBitBoards[Config::WHITE] = BitBoard(0ULL, 0ULL);
	piecesBitBoards[Config::BLACK] = BitBoard(0ULL, 0ULL);
}

Board::Board(BitBoardMovePool * pool)
	: movePool(pool)
{
	piecesBitBoards[Config::WHITE] = BitBoard(0ULL, 0ULL);
	piecesBitBoards[Config::BLACK] = BitBoard(0ULL, 0ULL);
}

Board::Board(const DynamicArray< Piece >& pieceArray, BitBoardMovePool * pool)
	: movePool(pool)
{
	for(int i = 0; i < pieceArray.Count(); ++i)
	{
		pieces[pieceArray[i].GetColour()] += pieceArray[i];
	}

	for(int i = 0; i < pieces[Config::WHITE].Count(); ++i)
	{
		piecesBitBoards[Config::WHITE].SetBit(true, pieces[Config::WHITE][i].GetPositionCoord());
	}

	for(int i = 0; i < pieces[Config::BLACK].Count(); ++i)
	{
		piecesBitBoards[Config::BLACK].SetBit(true, pieces[Config::BLACK][i].GetPositionCoord());
	}
}

Board::Board(const Board& copy)
	: movePool(copy.movePool)
{
	pieces[Config::WHITE] = copy.pieces[Config::WHITE];
	pieces[Config::BLACK] = copy.pieces[Config::BLACK];
	piecesBitBoards[Config::WHITE] = copy.piecesBitBoards[Config::WHITE];
	piecesBitBoards[Config::BLACK] = copy.piecesBitBoards[Config::BLACK];
}

Board& Board::operator=(const Board& assign)
{
	if(this != &assign)
	{
		pieces[Config::WHITE] = assign.pieces[Config::WHITE];
		pieces[Config::BLACK] = assign.pieces[Config::BLACK];
		movePool = assign.movePool;
		piecesBitBoards[Config::WHITE] = assign.piecesBitBoards[Config::WHITE];
		piecesBitBoards[Config::BLACK] = assign.piecesBitBoards[Config::BLACK];
	}
	return *this;
}

void Board::GetPossibleMoves(Config::PlayerColour colour, DynamicArray<Move>& moveArray)
{
	// first get the list of pieces with this colour
	const DynamicArray<Piece>& validPieces = pieces[colour];

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
		BitBoard availableMoves = movePool->GetPieceMoves(piece, piecesBitBoards[piece.GetColour()], piecesBitBoards[Config::GetOppositePlayer(piece.GetColour())], this); 
		result = ValidMove(piece, pos, availableMoves);
	}
	return result;
}

bool Board::ValidMove(Piece piece, ChessVector pos, const BitBoard& availableMoves)
{
	bool result = false;
	int pieceIndex = GetPieceIndex(piece.GetPositionVector());
	if(pieceIndex >= 0 && pieces[piece.GetColour()][pieceIndex] == piece && (availableMoves & BitBoard(pos)))
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
		BitBoard availableMoves = movePool->GetPieceMoves(piece, piecesBitBoards[piece.GetColour()], piecesBitBoards[Config::GetOppositePlayer(piece.GetColour())], this);
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
		const Config::PlayerColour pieceColour = piece.GetColour();
		const Config::PlayerColour oppositeColour = Config::GetOppositePlayer(pieceColour);

		int pieceIndex = GetPieceIndex(piece.GetPositionVector());
		// we must obtain the destination before actually moving the piece
		int destinationIndex = GetPieceIndex(pos);

		//copy to the MadeMove
		Piece removedPiece(Config::NO_TYPE, oppositeColour, pos);
		if(destinationIndex >= 0)
		{
			removedPiece = pieces[oppositeColour][destinationIndex];
		}
		move = MadeMove(removedPiece, piece.GetPositionVector());

		// do the actual move

		// update piece bit boards
		piecesBitBoards[pieceColour].SetBit(false, piece.GetPositionCoord());
		piecesBitBoards[pieceColour].SetBit(true, pos.GetVectorCoord());

		// set the new position of the piece
		pieces[piece.GetColour()][pieceIndex].SetPositionVector(pos);


		// if the piece is a pawn and has reached the ending tile, it is considered to become a queen
		if(pieces[pieceColour][pieceIndex].GetType() == Config::PAWN
			&& Const::PAWN_REPRODUCE_VECTORS[pieceColour].y == pos.y
			&& Const::PAWN_REPRODUCE_VECTORS[pieceColour].z == pos.z)
		{
			pieces[pieceColour][pieceIndex].SetType(Config::QUEEN);
		}

		if(destinationIndex >= 0)
		{
			// update the bit boards
			piecesBitBoards[oppositeColour].SetBit(false, pos.GetVectorCoord());

			pieces[oppositeColour].RemoveItem(destinationIndex);
		}
	}

	return move;
}

void Board::UndoMove(const MadeMove& move)
{
	// first return the current piece occupying the tile to it's original place
	int pieceIndex = GetPieceIndex(move.removedPiece.GetPositionVector());

	const Config::PlayerColour oppositeColour = Config::GetOppositePlayer(move.removedPiece.GetColour());
	// this shouldn't be -1, but just to be sure
	if(pieceIndex >= 0)
	{
		// update bit boards
		Piece& p = pieces[oppositeColour][pieceIndex];

		piecesBitBoards[oppositeColour].SetBit(false, p.GetPositionCoord());
		piecesBitBoards[oppositeColour].SetBit(true, move.sourcePosition.GetVectorCoord());

		p.SetPositionVector(move.sourcePosition);
	}

	// now if this wasn't a quiet move, we must add back the removed piece
	if(move.removedPiece.GetType() != Config::NO_TYPE)
	{
		AddPiece(move.removedPiece);
	}
}

bool Board::KingInCheck(Config::PlayerColour colour) const
{
	bool check = false;
	int kingIndex = -1;
	bool found = false;
	for(int i = 0; i < pieces[colour].Count() && !found; ++i)
	{
		if(pieces[colour][i].GetType() == Config::KING)
		{
			kingIndex = i;
			found = true;
		}
	}
	if(kingIndex >= 0)
	{
		check = TileThreatened(pieces[colour][kingIndex].GetPositionVector(), Config::GetOppositePlayer(colour));
	}
	return check;
}

Config::KingState Board::KingCheckState(Config::PlayerColour colour)
{
	using Config::KingState;
	KingState kingState = KingState::NORMAL;
	int kingIndex = -1;
	bool found = false;
	for(int i = 0; i < pieces[colour].Count() && !found; ++i)
	{
		if(pieces[colour][i].GetType() == Config::KING)
		{
			kingIndex = i;
			found = true;
		}
	}

	Piece king;

	if(kingIndex >= 0)
	{
		king = pieces[colour][kingIndex];
		if(TileThreatened(king.GetPositionVector(), Config::GetOppositePlayer(king.GetColour())))
		{
			kingState = KingState::CHECK;
		}
	}
	else
	{
		return Config::NO_KING;
	}

	const Config::PlayerColour oppositeColour = Config::GetOppositePlayer(colour);
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
		const DynamicArray<Piece>& friendlyPieces = pieces[colour];
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
	for(int i = 0; i < pieces[Config::WHITE].Count(); ++i)
	{
		pieceCount[Config::WHITE][pieces[Config::WHITE][i].GetType()]++;
	}
	for(int i = 0; i < pieces[Config::BLACK].Count(); ++i)
	{
		pieceCount[Config::BLACK][pieces[Config::BLACK][i].GetType()]++;
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
		const DynamicArray< Piece >& passivePieces = pieces[Config::GetOppositePlayer(colour)];
		const DynamicArray< Piece >& activePieces = pieces[colour];

		BitBoard activeBitBoard = GetPiecesBitBoard(colour);

		BitBoard passiveBitBoard = GetPiecesBitBoard(Config::GetOppositePlayer(colour));

		// we set the position bit of the checked tile, se we can get proper results from pool->GetPieceMoves(...) for pawns
		passiveBitBoard.SetBit(true, pos.GetVectorCoord());

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
		for(int i = 0; i < pieces[col].Count(); ++i)
		{
			if(pieces[col][i].GetType() == Config::KING)
			{
				return pieces[col][i];
			}
		}
	}
	return Piece();
}

int Board::GetMaterialBalance() const
{
	int balance[Config::PCOLOUR_COUNT] = {0};
	const BitBoard unoccupiedTiles = ~ (piecesBitBoards[Config::WHITE] | piecesBitBoards[Config::BLACK]);
	BitBoard moveableTiles;
	for(int i = 0; i < pieces[Config::WHITE].Count(); ++i)
	{
		moveableTiles = movePool->GetPieceFullMoves(pieces[Config::WHITE][i]) & unoccupiedTiles;
		balance[Config::WHITE] += pieces[Config::WHITE][i].GetWorth() + pieces[Config::WHITE][i].GetPositionWorth(pieces[Config::WHITE][i].GetPositionVector()) + moveableTiles.GetBitCount();
	}
	for(int i = 0; i < pieces[Config::BLACK].Count(); ++i)
	{
		moveableTiles = movePool->GetPieceFullMoves(pieces[Config::WHITE][i]) & unoccupiedTiles;
		balance[Config::BLACK] += pieces[Config::BLACK][i].GetWorth() + pieces[Config::BLACK][i].GetPositionWorth(pieces[Config::BLACK][i].GetPositionVector()) + moveableTiles.GetBitCount();
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

void Board::AddPiece(Piece piece)
{
	pieces[piece.GetColour()] += piece;
	piecesBitBoards[piece.GetColour()].SetBit(true, piece.GetPositionCoord());
}

void Board::RemovePiece(ChessVector pos)
{
	const int index = GetPieceIndex(pos);
	const Config::PlayerColour colour = GetPieceColour(pos);
	if( index != -1 && colour != Config::BOTH_COLOURS)
	{
		piecesBitBoards[colour].SetBit(false, pieces[colour][index].GetPositionCoord());
		pieces[colour].RemoveItem(index);
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
		if(bb.GetBit(pos) && tiles[pos] != state)
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