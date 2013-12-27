#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

typedef char coord; // The minimum type that can be used for indexing all 3D cubes in the board by single value

namespace Config
{
	// the type of player colours
	enum PlayerColour
	{
		PCOLOUR_WHITE,
		PCOLOUR_BLACK,
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

	static const unsigned short PIECE_POSITION_MASK = 0x007f;
	static const char PIECE_POSITION_LSHIFT = 0;
	static const unsigned short PIECE_COLOUR_MASK = 0x0080;
	static const char PIECE_COLOUR_LSHIFT = 7;
	static const unsigned short PIECE_TYPE_MASK = 0x0700;
	static const char PIECE_TYPE_LSHIFT = 8;

	static const int BOARD_SIZE = 5; // the size of the board's side
	static const char BITBOARD_SIZE = 2; // the size of the bitboard array of bitholding structs, so it could contain all the necessary coords

	static const unsigned long long BITBOARD_FULL_BOARD[] = { 0xffffffffffffffff, 0xfffffffffffffff8};
	static const unsigned long long BITBOARD_BIT = 0x8000000000000000; // just one bit mask for BitBoard::GetBit(...)
	static const unsigned long long BITBOARD_ROW = 0xf800000000000000; // one row from the bit mask for BitBoard::GetBit(...)
	static const unsigned long long BITBOARD_LEVEL = 0xffffff8000000000; // one level from the bit mask for BitBoard::GetBit(...)

	static const int PLAYER_PIECES_COUNT = 20;
};

namespace GraphicConfig
{
	static const int SCREEN_WIDTH = 1350; // screen width
	static const int SCREEN_HEIGHT = 400; // screen height
	
	static const unsigned long WHITE_BOARD_COLOUR = 0xffffffff; // the normal colour of white square from the board
	static const unsigned long BLACK_BOARD_COLUOR = 0x000000ff; // the normal colour of black square from the board
	static const unsigned long SELECTED_BOARD_COLOUR = 0xffff00ff; // the colour of selecetd piece square colour
	static const unsigned long MOVEABLE_BOARD_COLOUR = 0x00ff00ff; // the colour of moveable tiles by the currently selected piece
	static const unsigned long CAPTURE_BOARD_COLOUR = 0xff0000ff; // the colour of threatened enemy tiles by the currently selected piece

	static const unsigned long BACKGROUND_COLOUR = 0x00ccccff; // the background colour of the graphic window

	static const int BOARD_SQUARE_SIDE_SIZE = 50; // the size of the square on the window ( pixels )
	static const int BOARD_PADDING = 10; // the size of the board padding ( pixels )
};

#endif // __CONFIGURAION_H__