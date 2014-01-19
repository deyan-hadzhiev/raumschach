#include "configuration.h"
#include "utils.h"
#include "piece.h"
#include "constants.h"

int Piece::GetWorth() const
{
	return Const::PIECE_WORTH[GetType()];
}

int Piece::GetPositionWorth(ChessVector pos) const
{
	coord worth = 0;
	if(GetType() != Config::PAWN)
	{
		worth = pos.GetManhattanDistance(Const::BOARD_CENTER);
	}
	else
	{
		worth = Const::PAWN_DISTANCE_TO_REPRODUCTION - Utils::Abs(pos.y - Const::PAWN_REPRODUCE_VECTORS[GetColour()].y) - Utils::Abs(pos.z - Const::PAWN_REPRODUCE_VECTORS[GetColour()].z);
	}
	return worth;
}