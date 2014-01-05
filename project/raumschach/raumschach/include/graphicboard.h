#ifndef __GRAPHICBOARD_H__
#define __GRAPHICBOARD_H__

#include "render.h"
#include "configuration.h"
#include "utils.h"

class GraphicBoard
{
public:
	GraphicBoard(Render * render);
	~GraphicBoard();

	void Initialize();

	static bool OnBoard(int x, int y);
	static ChessVector ScreenToBoard(int x, int y);
	static ChessVector ScreenToBoard(Rect pos);
	static Rect BoardToScreen(ChessVector pos);

	void DrawBoard(const Board * board, const BoardTileState * tileState);

	// this causes full refresh of the board the next time it is drawn
	// NOTE: this is slow and should be done as rarely as possible
	void Refresh(bool flag = true);
private:
	Texture* LoadTexture(const char* filename);

	Texture* pieceTextures[Config::PCOLOUR_COUNT][Config::PIECE_TYPE_COUNT];
	Colour backgroundColour;
	Colour tileColours[Config::PCOLOUR_COUNT][Config::TILE_TYPE_COUNT];

	bool fullRefresh;

	// not owned object - no destruction
	Render * render;
};

#endif // __GRAPHICBOARD_H__