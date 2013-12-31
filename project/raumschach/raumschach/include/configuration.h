#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

typedef char coord; // The minimum type that can be used for indexing all 3D cubes in the board by single value

namespace SysConfig
{
	enum ExitCodes
	{
		EXIT_NORMAL,
		EXIT_CHESS_INIT_ERROR,
		EXIT_SDL_ERROR,
		EXIT_GRAPHIC_BOARD_INIT_ERROR,
	};

	enum MouseButton
	{
		NO_TYPE,
		LEFT,
		MIDDLE,
		RIGHT,
	};

	static const int SCREEN_WIDTH = 1400; // screen width
	static const int SCREEN_HEIGHT = 400; // screen height

	static const int SCREEN_SPAWN_X = 100;
	static const int SCREEN_SPAWN_Y = 100;

	static const char WINDOW_NAME[] = "Raumschach 1.0";
	
	static const int MAX_PATH = 260;

	struct Path
	{
		Path(const char* src)
		{
			int i = 0;
			while(i < sizeof(name) && src[i] != '\0')
			{
				name[i] = src[i];
				++i;
			}
			if(i < sizeof(name)) name[i] = '\0';
			else name[i - 1] = '\0';
		}

		char name[MAX_PATH];
	};
};

namespace Config
{
	// the type of player colours
	enum PlayerColour
	{
		WHITE,
		BLACK,
		PCOLOUR_COUNT,
	};

	enum PieceType
	{
		NO_TYPE,
		KING,
		QUEEN,
		ROOK,
		BISHOP,
		KNIGHT,
		UNICORN,
		PAWN,
		PIECE_TYPE_COUNT,
	};

	// possible states of tiles
	enum TileType
	{
		TILE_NORMAL,
		TILE_SELECTED,
		TILE_MOVEABLE,
		TILE_CAPTUREABLE,
		TILE_TYPE_COUNT,
	};

	static const unsigned short PIECE_POSITION_MASK = 0x007f;
	static const char PIECE_POSITION_LSHIFT = 0;
	static const unsigned short PIECE_COLOUR_MASK = 0x0080;
	static const char PIECE_COLOUR_LSHIFT = 7;
	static const unsigned short PIECE_TYPE_MASK = 0x0700;
	static const char PIECE_TYPE_LSHIFT = 8;

	static const int BOARD_SIDE = 5;
	static const int BOARD_SIZE = BOARD_SIDE * BOARD_SIDE * BOARD_SIDE; // the size of the board
	static const char BITBOARD_SIZE = 2; // the size of the bitboard array of bitholding structs, so it could contain all the necessary coords

	static const unsigned long long BITBOARD_FULL_BOARD[] = { 0xffffffffffffffff, 0xfffffffffffffff8};
	static const unsigned long long BITBOARD_BIT = 0x8000000000000000; // just one bit mask for BitBoard::GetBit(...)
	static const unsigned long long BITBOARD_ROW = 0xf800000000000000; // one row from the bit mask for BitBoard::GetBit(...)
	static const unsigned long long BITBOARD_LEVEL = 0xffffff8000000000; // one level from the bit mask for BitBoard::GetBit(...)

	static const int PLAYER_PIECES_COUNT = 20;

};

namespace GraphicConfig
{
	enum Colours
	{
		ALPHA,
		BLUE,
		GREEN,
		RED,
		COLOURS_COUNT,
	};

	static const unsigned long BOARD_COLOURS[Config::PCOLOUR_COUNT][Config::TILE_TYPE_COUNT] =
	{
		// { normal, selected, moveable, capturable}
		{ 0xccccccff, 0x999900ff, 0x00aa00ff, 0xaa0000ff}, // white
		{ 0x333333ff, 0x777700ff, 0x006600ff, 0x660000ff}, // black
	};

	static const unsigned long BACKGROUND_COLOUR = 0x888888ff; // the background colour of the graphic window

	static const int BOARD_SQUARE_SIDE_SIZE = 50; // the size of the square on the window ( pixels )
	static const int BOARD_PADDING = 20; // the size of the board padding ( pixels )
	
	static const SysConfig::Path PIECE_TEXTURE_PATHS [Config::PCOLOUR_COUNT][Config::PIECE_TYPE_COUNT] =
	{
		// white pieces
		{
			"",
			"data/pieces/white/small_king.bmp",
			"data/pieces/white/small_queen.bmp",
			"data/pieces/white/small_rook.bmp",
			"data/pieces/white/small_bishop.bmp",
			"data/pieces/white/small_knight.bmp",
			"data/pieces/white/small_unicorn.bmp",
			"data/pieces/white/small_pawn.bmp",
		},

		// black pieces
		{
			"",
			"data/pieces/black/small_king.bmp",
			"data/pieces/black/small_queen.bmp",
			"data/pieces/black/small_rook.bmp",
			"data/pieces/black/small_bishop.bmp",
			"data/pieces/black/small_knight.bmp",
			"data/pieces/black/small_unicorn.bmp",
			"data/pieces/black/small_pawn.bmp",
		},
	};

	static const unsigned long TEXTURE_TRANSPARENT_COLOURS [Config::PCOLOUR_COUNT][2] =
	{
		{0x00000000, 0xffffff00},
		{0xffffff00, 0xffffff00},
	};
};

#endif // __CONFIGURAION_H__