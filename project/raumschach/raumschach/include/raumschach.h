#ifndef __RAUMSCHACH_H__
#define __RAUMSCHACH_H__

#include "configuration.h"
#include "piece.h"

class Player;
class Render;
class Board;
class GraphicBoard;
class BitBoardMovePool;
class BoardTileState;

class Raumschach
{
public:
	Raumschach();
	~Raumschach();

	void Initialize(Player * white, Player * black);

	void Start();

	void IdleDrawBoard();
	void MouseClick(SysConfig::MouseButton button, int x, int y);

private:
	Render * render;
	GraphicBoard * graphicBoard;
	Board * board;
	BitBoardMovePool * movePool;
	BoardTileState * tileState;

	Piece selectedPiece;

	// not owned, so no destruction
	Player * whitePlayer;
	Player * blackPlayer;
};

#endif // __RAUMSCHACH_H__