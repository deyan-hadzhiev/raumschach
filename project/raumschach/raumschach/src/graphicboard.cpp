#include "render.h"
#include "graphicboard.h"
#include "error.h"
#include "constants.h"
#include "board.h"

GraphicBoard::GraphicBoard(Render * render, Rect position)
	:
	render(render),
	pos(position),
	fullRefresh(true)
{
	for(int i = 0; i < COUNT_OF(pieceTextures); ++i)
	{
		for(int j = 0; j < COUNT_OF(pieceTextures[i]); ++j)
		{
			pieceTextures[i][j] = NULL;
		}
	}
}

GraphicBoard::~GraphicBoard()
{
	for(int i = 0; i < COUNT_OF(pieceTextures); ++i)
	{
		for(int j = 0; j < COUNT_OF(pieceTextures[i]); ++j)
		{
			delete pieceTextures[i][j];
		}
	}
}

void GraphicBoard::Initialize()
{
	// initialize piece textures
	for(int i = 0; i < Config::PIECE_TYPE_COUNT; ++i)
	{
		if(i != Config::NO_TYPE)
		{
			for(int col = 0; col < Config::PCOLOUR_COUNT; ++col)
			{
				Texture* currTex = LoadTexture(GraphicConfig::PIECE_TEXTURE_PATHS[col][i].name);
				if(currTex && currTex->IsLoaded())
				{
					bool error = false;
					error |= ! currTex->SetPixelFormat(render->GetPixelFormat());
					currTex->InitTransparent( GraphicConfig::TEXTURE_TRANSPARENT_COLOURS[col][0], GraphicConfig::TEXTURE_TRANSPARENT_COLOURS[col][1]);
					error |= ! currTex->RenderTexture(render);
					if(error) Error( SDL_GetError()).Post().Exit(SysConfig::EXIT_GRAPHIC_BOARD_INIT_ERROR);
					else 
					{
						delete pieceTextures[col][i];
						pieceTextures[col][i] = currTex;
					}
				}
				else
				{
					CharString errmsg = CharString("ERROR: Failed allocation of texture ") + CharString(GraphicConfig::PIECE_TEXTURE_PATHS[col][i].name);
					Error(errmsg).Post().Exit(SysConfig::EXIT_GRAPHIC_BOARD_INIT_ERROR);
				}
			}
		}
		else
		{
			pieceTextures[Config::WHITE][i] = NULL;
			pieceTextures[Config::BLACK][i] = NULL;
		}
	}

	backgroundColour = Colour(GraphicConfig::BOARD_BACKGROUND_COLOUR);
	
	for(int colour = 0; colour < Config::PCOLOUR_COUNT; ++colour)
	{
		for(int tileType = 0; tileType < Config::TILE_TYPE_COUNT; ++tileType)
		{
			tileColours[colour][tileType] = GraphicConfig::BOARD_COLOURS[colour][tileType];
		}
	}
}

Texture* GraphicBoard::LoadTexture(const char* filename)
{
	Texture* retval = NULL;
	if(filename)
	{
		retval = new Texture(filename);
	}
	return retval;
}

bool GraphicBoard::OnBoard(int x, int y) const
{
	if(x < pos.x || x >= pos.width + pos.x || y < pos.y || y >= pos.height + pos.y)
		return false;

	int rx = x - pos.x;
	int ry = y - pos.y;
	const int levelWidth = GraphicConfig::BOARD_PADDING + GraphicConfig::BOARD_SQUARE_SIDE_SIZE * Config::BOARD_SIDE; 
	bool onBoard = rx / levelWidth < Config::BOARD_SIDE;
	onBoard &= rx % levelWidth >= GraphicConfig::BOARD_PADDING;
	onBoard &= ry >= GraphicConfig::BOARD_PADDING && ry < levelWidth;
	return onBoard;
}

ChessVector GraphicBoard::ScreenToBoard(int x, int y) const
{
	const int levelWidth = GraphicConfig::BOARD_PADDING + GraphicConfig::BOARD_SQUARE_SIDE_SIZE * Config::BOARD_SIDE;
	ChessVector tile;
	if(OnBoard(x, y))
	{
		tile.z = Config::BOARD_SIDE - 1 - (x - pos.x) / levelWidth;
		int rx = (x - pos.x) % levelWidth - GraphicConfig::BOARD_PADDING;
		int ry = (y - pos.y) - GraphicConfig::BOARD_PADDING;
		tile.x = rx / GraphicConfig::BOARD_SQUARE_SIDE_SIZE;
		tile.y = Config::BOARD_SIDE - 1 - ry / GraphicConfig::BOARD_SQUARE_SIDE_SIZE;
	}
	else
	{
		tile = Const::INVALID_CHESS_VECTOR;
	}
	return tile;
}

ChessVector GraphicBoard::ScreenToBoard(Rect pos) const
{
	return ScreenToBoard(pos.x, pos.y);
}

Rect GraphicBoard::BoardToScreen(ChessVector tile) const
{
	Rect retval;
	if(Board::ValidVector(tile))
	{
		const int levelWidth = GraphicConfig::BOARD_PADDING + GraphicConfig::BOARD_SQUARE_SIDE_SIZE * Config::BOARD_SIDE;
		const int x = (int) tile.x;
		const int y = Config::BOARD_SIDE - 1 - (int) tile.y;
		const int z = Config::BOARD_SIDE - 1 - (int) tile.z;
		retval.x = levelWidth * z + GraphicConfig::BOARD_PADDING + x * GraphicConfig::BOARD_SQUARE_SIDE_SIZE;
		retval.y = GraphicConfig::BOARD_PADDING + y * GraphicConfig::BOARD_SQUARE_SIDE_SIZE;
		retval.width = GraphicConfig::BOARD_SQUARE_SIDE_SIZE;
		retval.height = GraphicConfig::BOARD_SQUARE_SIDE_SIZE;
		
		// add the relative displacement
		retval.x += pos.x;
		retval.y += pos.y;
	}
	else
	{
		retval.x = -1;
		retval.y = -1;
	}
	return retval;
}

void GraphicBoard::DrawBoard(const Board * board, const BoardTileState * tileState)
{
	using namespace Config;

	//first draw the background
	if(fullRefresh)
	{
		render->DrawRectangle( backgroundColour, pos.x, pos.y, pos.width, pos.height);
	}

	//draw all the tiles and draw a piece if there is one
	for(coord pos = 0; pos < BOARD_SIZE; ++pos)
	{
		ChessVector posVec(pos);
		if(tileState->GetChangedTile(posVec) || fullRefresh)
		{
			TileType tileType = tileState->GetBoardTileState(posVec);
			char colour = (posVec.x + posVec.y + posVec.z) & 1;

			render->DrawRectangle(tileColours[colour][tileType], BoardToScreen(posVec));

			Piece tilePiece = board->GetPiece(posVec);
			if(tilePiece.GetType() != NO_TYPE)
			{
				const PlayerColour colour = tilePiece.GetColour();
				const PieceType type = tilePiece.GetType();
				const Texture * texture = pieceTextures[colour][type];
				Rect screenRect = BoardToScreen(posVec);
				int x = screenRect.x + (screenRect.width - texture->GetWidth()) / 2;
				int y = screenRect.y + (screenRect.height - texture->GetHeight()) / 2;
				render->DrawTexture(texture, x, y);
			}
		}
	}

	// we have refreshed everything, now we optimize by stoping further full refreshments
	if(fullRefresh) fullRefresh = false;

	render->EndDraw();
}

void GraphicBoard::Refresh(bool flag)
{
	fullRefresh = flag;
}