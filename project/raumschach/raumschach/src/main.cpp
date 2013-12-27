#include <iostream>
#include <SDL.h>
#include <cmath>
#include "utils.h"
#include "configuration.h"
#include "constants.h"
#include "board.h"

static const unsigned screenWidth = 640;
static const unsigned screenHeight = 480;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

void PrintBitBoard( const BitBoard& bb)
{
	for( char i = 0; i < Config::BITBOARD_SIZE; ++i)
	{
		unsigned long long bit = bb.bits[i];
		for( int j = 0; j < sizeof(bit) * 8; ++j)
		{
			std::cout << (((bit << j) & 0x8000000000000000) ? "1" : "0");
		}
		std::cout << std::endl;
	}
}

void PrintFormatedBB( const BitBoard& bb)
{
	using namespace std;
	for( char level = 0; level < Config::BOARD_SIZE; ++level)
	{
		unsigned long long levelBits = bb.GetBits( level * 25, Config::BITBOARD_LEVEL);
		for( int row = 0; row < Config::BOARD_SIZE; ++row)
		{
			for(int column = 0; column < Config::BOARD_SIZE; ++column)
			{
				std::cout << (((levelBits << (row * Config::BOARD_SIZE + column)) & 0x8000000000000000) ? "1" : "0");
			}
			cout << endl;
		}
		cout << endl;
	}
}

void PrintCharBits( unsigned char cb)
{
	for(int i = 0; i < sizeof(char) * 8; ++i)
	{
		std::cout << (((cb << i) & 0x80) ? "1" : "0");
	}
	std::cout << std::endl;
}


inline int colToInt( float col)
{
	return (int) std::floor( col * 255.0f + 0.5f);
}

inline Uint32 toRGBA( float r, float g, float b)
{
	return (colToInt(r) << 16) | (colToInt(g) << 8) | colToInt(b);
}

SDL_Texture* LoadImage(std::string file){
	SDL_Surface *loadedImage = nullptr;
	SDL_Texture *texture = nullptr;
	loadedImage = SDL_LoadBMP(file.c_str());
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
		SDL_FreeSurface(loadedImage);
	}
	else
		std::cout << SDL_GetError() << std::endl;
	return texture;
}

SDL_Surface* LoadSurface( std::string file)
{
	SDL_Surface *loadedImage = nullptr;
	loadedImage = SDL_LoadBMP(file.c_str());
	return loadedImage;
}

void ApplySurface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend){
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(rend, tex, NULL, &pos);
}

int main(int argc, char **argv){
	using namespace std;
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	unsigned long long bits[] = {0xf0f0f0f0, 0xfedcba9876543210};
	unsigned long long bits2[] = {0xcccccccccccccccc, 0xcccccccccccccccc};
	unsigned long long bits3[] = {0xfedcba9876543210, 0x0123456789abcdef};
	unsigned long long fullBits[] = { 0xffffffffffffffff, 0xffffffffffffffff};

	BitBoard test(bits);
	PrintBitBoard( test);
	BitBoard test2(bits2);
	PrintBitBoard( test2);

	BitBoard andRes = test & test2;
	
	cout << "and" << endl;
	PrintBitBoard( andRes);
	cout << "test after operation" << endl;
	cout << "or" << endl;
	PrintBitBoard( test | test2);
	cout << "xor" << endl;
	PrintBitBoard( test ^ test2);

	cout << "test" << endl;
	PrintBitBoard( test);
	
	BitBoard test3( bits3);
	cout << "test3" << endl;
	PrintBitBoard( test3);
	test3.SetBits( 0xcc00000000000000, 60, 0xff00000000000000);
	PrintBitBoard( test3);

	cout << (test3 == test2 ? "true" : "false" ) << endl;
	cout << (test3 == test3 ? "true" : "false" ) << endl;

	test.SetBits( 0UL, 0, 0xffffffffffffffff);
	test.SetBits( 0UL, 64, 0xffffffffffffffff);

	int count = COUNT_OF(bits);
	int otherCount = COUNT_OF(Const::PIECE_WORTH);

	cout << sizeof( BitBoard) << endl;
	cout << sizeof( BitBoardMovePool) << endl;

	BitBoardMovePool* pool = new BitBoardMovePool;
	pool->Initalize();

	cout << "BitBoard Debug" << endl;

	PrintFormatedBB( pool->GetPieceMoves(Piece(Config::QUEEN, Config::PCOLOUR_BLACK, ChessVector(0, 0, 0))));

	coord bitcount = BitBoard( Config::BITBOARD_FULL_BOARD).GetBitCount();
	coord vectors[][3] = {{3,3,4}, {4,4,3}};
	size_t vecSize = sizeof( vectors);
	ChessVector some(123);
	
	DynamicArray<int> dynArr(10);

	auto PrintDynArray = [] (DynamicArray<int> arr) {
		for( int i = 0; i < arr.Count(); ++i)
		{
			printf("%d ", arr[i]);
		}
		printf("\n");
	};

	cout << "empty array" << endl;
	PrintDynArray( dynArr);

	cout << "add 5 elements" << endl;
	for(int i = 0; i < 5; ++i)
	{
		dynArr += i;
	}
	cout << "count " << dynArr.Count() << endl;
	PrintDynArray( dynArr);

	cout << "add 10 elements" << endl;
	for( int i = 0; i < 10; ++i)
	{
		dynArr += i*2;
	}
	cout << "count " << dynArr.Count() << endl;
	PrintDynArray( dynArr);

	cout << "removing elements" << endl;
	dynArr.RemoveItem( 4);
	dynArr.RemoveItem( 9);

	PrintDynArray( dynArr);

	(dynArr += 5) += 8;

	PrintDynArray( dynArr);


	//return 0;
	atexit(SDL_Quit);

	window = SDL_CreateWindow( "SDL_Template", 100, 100, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

	if( window == nullptr){
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if( renderer == nullptr){
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
	if( texture == nullptr) {
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Surface* frameSurface = SDL_CreateRGBSurface( 0, screenWidth, screenHeight, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

	Uint32* frameBuffer = (Uint32*) frameSurface->pixels;
	Uint32 frameBufferPitch = frameSurface->pitch / sizeof( Uint32);

	for(int y = 0; y < screenHeight; ++y)
	{
		for( int x = 0; x < screenWidth; ++x)
		{
			frameBuffer[y * frameBufferPitch + x] = toRGBA( x / (float) screenWidth, y / (float) screenHeight, 0.5f);
		}
	}

	SDL_UpdateTexture( texture, NULL, frameBuffer, frameBufferPitch * sizeof( Uint32));

	SDL_RenderClear( renderer);
	SDL_RenderCopy( renderer, texture, NULL, NULL);
	SDL_RenderPresent( renderer);

	SDL_Surface* pawnSurface = LoadSurface("data/pieces/black/small_knight.bmp");

	int w = pawnSurface->w;
	int h = pawnSurface->h;

	//SDL_Surface* formattedPawnSurface = SDL_CreateRGBSurface( 0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	//SDL_PixelFormat* fmt = formattedPawnSurface->format; //373694468

	SDL_PixelFormat fmt;
	fmt.format = SDL_PIXELFORMAT_RGBA8888;
	fmt.Ashift = 0;
	fmt.Amask = 0x000000ff;
	fmt.Aloss = 0;
	fmt.Rshift = 24;
	fmt.Rmask = 0xff000000;
	fmt.Rloss = 0;
	fmt.Gshift = 16;
	fmt.Gmask = 0x00ff0000;
	fmt.Gloss = 0;
	fmt.Bshift = 8;
	fmt.Bmask = 0x0000ff00;
	fmt.Bloss = 0;
	fmt.palette = NULL;
	fmt.padding[0] = '\0';
	fmt.padding[1] = '\0';
	fmt.BitsPerPixel = 32;
	fmt.BytesPerPixel = 4;

	SDL_Surface* formattedPawn = SDL_ConvertSurface( pawnSurface, &fmt, 0);

	Uint32* formattedPixels = (Uint32*) formattedPawn->pixels;

	for(int i = 0; i < w*h; ++i)
	{
		if( formattedPixels[i] != 0x000000ff)
		{
			formattedPixels[i] = 0x000000ff - ((formattedPixels[i] & 0xff00) >> 8);
		}
	}

	SDL_Texture* pawn = SDL_CreateTextureFromSurface(renderer, formattedPawn);
	SDL_SetTextureBlendMode( pawn, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(pawnSurface);

	bool quit = false;
	SDL_Event evnt;
	int counter = 0;
	while( !quit )
	{
		while( SDL_PollEvent( &evnt))
		{
			switch( evnt.type)
			{
			case SDL_QUIT:
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
				quit = true;
			}
		}

		
		for(int y = 0; y < screenHeight; ++y)
		{
			for( int x = 0; x < screenWidth; ++x)
			{
				frameBuffer[y * frameBufferPitch + x] = toRGBA( (x / (float) screenWidth) * ( 1 - counter / 500.0f), (y / (float) screenHeight) * (counter/ 500.0f), 0.5f);
			}
		}

		SDL_UpdateTexture( texture, NULL, frameBuffer, frameBufferPitch * sizeof( Uint32));

		SDL_RenderClear( renderer);
		SDL_RenderCopy( renderer, texture, NULL, NULL);
		ApplySurface( 10, 10, pawn, renderer);
		SDL_RenderPresent( renderer);
		std::cout << ++counter << std::endl;
	}

	SDL_DestroyTexture( texture);
	SDL_DestroyTexture( pawn);

	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);

	return 0;
}
