#ifndef __GRAPHICBOARD_H__
#define __GRAPHICBOARD_H__

#include "render.h"
#include "configuration.h"
#include "utils.h"

class GraphicBoard
{
public:
	GraphicBoard(Render * render, Rect position = Rect(GraphicConfig::BOARD_POSITION_X, GraphicConfig::BOARD_POSITION_Y, GraphicConfig::BOARD_SIZE_WIDTH, GraphicConfig::BOARD_SIZE_HEIGHT));
	~GraphicBoard();

	void Initialize();

	bool OnBoard(int x, int y) const;
	ChessVector ScreenToBoard(int x, int y) const;
	ChessVector ScreenToBoard(Rect pos) const;
	Rect BoardToScreen(ChessVector pos) const;

	void DrawBoard(const Board * board, const BoardTileState * tileState);

	// this causes full refresh of the board the next time it is drawn
	// NOTE: this is slow and should be done as rarely as possible
	void Refresh(bool flag = true);
private:
	Rect pos;

	Texture* LoadTexture(const char* filename);

	Texture* pieceTextures[Config::PCOLOUR_COUNT][Config::PIECE_TYPE_COUNT];
	Colour backgroundColour;
	Colour tileColours[Config::PCOLOUR_COUNT][Config::TILE_TYPE_COUNT];

	bool fullRefresh;

	// not owned object - no destruction
	Render * render;
};

#endif // __GRAPHICBOARD_H__