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
		EXIT_PANEL_INIT_ERROR,
	};

	enum MouseButton
	{
		NO_TYPE,
		LEFT,
		MIDDLE,
		RIGHT,
	};

	static const int SCREEN_WIDTH = 1370; // screen width
	static const int SCREEN_HEIGHT = 380; // screen height

	static const int SCREEN_SPAWN_X = 100;
	static const int SCREEN_SPAWN_Y = 100;

	static const char WINDOW_NAME[] = "Raumschach 1.0";
	
	static const int MAX_PATH = 260;

	struct Name
	{
		Name(const char* src)
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
		BOTH_COLOURS,
	};

	enum PlayerType
	{
		PLAYER_HUMAN,
		PLAYER_AI,
		PLAYERS_TYPE_COUNT,
	};

	static const int AI_PLAYER_SEARCH_DEPTH = 4;
	static const int MAX_AI_PLAYER_SEARCH_DEPTH = 16;
	static const int INT_NEGATIVE_INFINITY = 1 << (sizeof(int) * 8 - 1);
	static const int INT_POSITIVE_INFINITY = ~ INT_NEGATIVE_INFINITY;

	inline PlayerColour GetOppositePlayer(PlayerColour col)
	{
		return (col == WHITE ? BLACK : WHITE);
	}

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

	enum KingState
	{
		NORMAL,
		CHECK,
		CHECKMATE,
		STALEMATE,
		NO_KING, // this must be error
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

	static const char BOARD_SAVE_FILENAME[] = "SavedBoard.dat";
	static const int BOARD_SAVE_HEADER_SIZE = 2; // bytes in the save file that will be used for header flags
	static const int BOARD_STATE_TURN_COLOUR_LSHIFT = 0;
	static const int BOARD_STATE_GAME_ENDED_LSHIFT = 2;
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

	static const unsigned long BOARD_BACKGROUND_COLOUR = 0x888888ff; // the background colour of the board
	static const unsigned long PANEL_BACKGROUND_COLOUR = 0x777777ff; // the background colour of the panel

	static const int BOARD_SQUARE_SIDE_SIZE = 50; // the size of the square on the window ( pixels )
	static const int BOARD_PADDING = 20; // the size of the board padding ( pixels )
	
	static const int PANEL_POSITION_X = 0;
	static const int PANEL_POSITION_Y = 290;
	static const int PANEL_SIZE_WIDTH = SysConfig::SCREEN_WIDTH;
	static const int PANEL_SIZE_HEIGHT = SysConfig::SCREEN_HEIGHT - PANEL_POSITION_Y;

	static const int PANEL_PADDING = 10;

	static const int BOARD_POSITION_X = 0;
	static const int BOARD_POSITION_Y = 0;
	static const int BOARD_SIZE_WIDTH = SysConfig::SCREEN_WIDTH;
	static const int BOARD_SIZE_HEIGHT = SysConfig::SCREEN_HEIGHT - PANEL_SIZE_HEIGHT;

	static const SysConfig::Name PIECE_TEXTURE_PATHS [Config::PCOLOUR_COUNT][Config::PIECE_TYPE_COUNT] =
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

	static const int ASCII_CHART_COUNT = 256;
	static const int ASCII_CHART_COUNT_ROW = 16;
	static const int ASCII_CHART_COUNT_COLUMN = 16;
	static const char ASCII_CHART_FILENAME[] = "data/text/arial.bmp";
	static const unsigned long ASCII_CHART_TRANSPARENT_COLOUR[2] = {0xff000000, 0xff000000};
	static const int ASCII_CHART_CHAR_WIDTH = 16;
	static const int ASCII_CHART_CHAR_HEIGHT = 16;
	static const int ASCII_CHART_CHAR_RENDER_WIDTH[ASCII_CHART_COUNT] =
	{
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	/*    ,  !,  ",  #,  $,  %,  &,  ',  (,  ),  *,  +,  ,,  -,  .,  /,*/
		10,  6,  9, 10,  9, 12, 12,  6,  7,  7,  7, 10,  6,  7,  6,  7,
	/*   0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  :,  ;,  <,  =,  >,  ?,*/
		10,  8, 10, 10, 10, 10, 10, 10, 10, 10,  6,  6, 10, 10, 10, 10,
	/*   @,  A,  B,  C,  D,  E,  F,  G,  H,  I,  J,  K,  L,  M,  N,  O,*/
		16, 11, 11, 11, 11, 10,  9, 11, 11,  6, 10, 11, 10, 13, 11, 12,
	/*   P,  Q,  R,  S,  T,  U,  V,  W,  X,  Y,  Z,  [,  \,  ],  ^,  _,*/
		11, 12, 12, 11, 10, 11, 13, 16, 11, 12, 10,  7,  7,  7, 10, 11,
	/*   `,  a,  b,  c,  d,  e,  f,  g,  h,  i,  j,  k,  l,  m,  n,  o,*/
		 7, 10, 10, 10, 10, 10,  9, 10, 10,  6,  7, 10,  6, 14, 10, 10,
	/*   p,  q,  r,  s,  t,  u,  v,  w,  x,  y,  z,  {,  |,  },  ~,   ,*/
		10, 10,  8, 11,  8, 10, 11, 13, 10, 11,  9,  9,  7,  9, 11, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	};

	static const unsigned long BUTTON_COLOUR = 0x555555ff;
	static const int BUTTON_SIZE_WIDTH = 120;
	static const int BUTTON_SIZE_HEIGHT = 40;
	static const int BUTTON_POSITION_HORIZONTAL = PANEL_SIZE_HEIGHT - BUTTON_SIZE_HEIGHT - PANEL_PADDING; // the 'y' coordinate of the buttons

	static const int POSITION_BUTTON_SAVE[] =
	{
		PANEL_SIZE_WIDTH - BUTTON_SIZE_WIDTH * 2 - PANEL_PADDING * 2, //x
		BUTTON_POSITION_HORIZONTAL, //y
		BUTTON_SIZE_WIDTH, //width
		BUTTON_SIZE_HEIGHT, //height
	};

	static const int POSITION_BUTTON_LOAD[] =
	{
		PANEL_SIZE_WIDTH - BUTTON_SIZE_WIDTH - PANEL_PADDING, //x
		BUTTON_POSITION_HORIZONTAL, //y
		BUTTON_SIZE_WIDTH, //width
		BUTTON_SIZE_HEIGHT, //height
	};

	static const int POSITION_BUTTON_RESET[] =
	{
		PANEL_PADDING, //x
		BUTTON_POSITION_HORIZONTAL, //y
		BUTTON_SIZE_WIDTH, //width
		BUTTON_SIZE_HEIGHT, //height
	};

	static const int POSITION_BUTTON_NEW_AI_BLACK[] =
	{
		PANEL_SIZE_WIDTH - PANEL_PADDING * 4 - BUTTON_SIZE_WIDTH * 3, // x
		BUTTON_POSITION_HORIZONTAL, // y
		BUTTON_SIZE_WIDTH, // width
		BUTTON_SIZE_HEIGHT, // height
	};

	static const int POSITION_BUTTON_NEW_AI_WHITE[] =
	{
		PANEL_SIZE_WIDTH - PANEL_PADDING * 5 - BUTTON_SIZE_WIDTH * 4, // x
		BUTTON_POSITION_HORIZONTAL, // y
		BUTTON_SIZE_WIDTH, // width
		BUTTON_SIZE_HEIGHT, // height
	};

	static const int POSITION_BUTTON_NEW_HUMAN_BLACK[] =
	{
		PANEL_SIZE_WIDTH - PANEL_PADDING * 6 - BUTTON_SIZE_WIDTH * 5, // x
		BUTTON_POSITION_HORIZONTAL, // y
		BUTTON_SIZE_WIDTH, // width
		BUTTON_SIZE_HEIGHT, // height
	};

	static const int POSITION_BUTTON_NEW_HUMAN_WHITE[] =
	{
		PANEL_SIZE_WIDTH - PANEL_PADDING * 7 - BUTTON_SIZE_WIDTH * 6, // x
		BUTTON_POSITION_HORIZONTAL, // y
		BUTTON_SIZE_WIDTH, // width
		BUTTON_SIZE_HEIGHT, // height
	};
};

#endif // __CONFIGURAION_H__