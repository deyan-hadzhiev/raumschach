#include <iostream>
#include <SDL.h>
#include <cmath>
#include "utils.h"
#include "configuration.h"
#include "constants.h"
#include "board.h"
#include "render.h"
#include "raumschach.h"
#include "player.h"

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
	for( char level = 0; level < Config::BOARD_SIDE; ++level)
	{
		unsigned long long levelBits = bb.GetBits( level * 25, Config::BITBOARD_LEVEL);
		for( int row = 0; row < Config::BOARD_SIDE; ++row)
		{
			for(int column = 0; column < Config::BOARD_SIDE; ++column)
			{
				std::cout << (((levelBits << (row * Config::BOARD_SIDE + column)) & 0x8000000000000000) ? "1" : "0");
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

	Player * white = new Player;
	Player * black = new Player;

	Raumschach rchess;

	rchess.Initialize(white, black);
	rchess.Start();

	delete white;
	delete black;

	return 0;
	Render render( SysConfig::SCREEN_WIDTH, SysConfig::SCREEN_HEIGHT);

	Texture* tex = new Texture();
	if(tex->LoadTexture("data/pieces/black/small_knight.bmp"))
	{
		tex->SetPixelFormat(render.GetPixelFormat());
		tex->InitTransparent(Colour(0xffffff00), Colour(0xffffff00));
		tex->RenderTexture(&render);
	}

	Texture* tex2 = new Texture();
	if(tex2->LoadTexture("data/pieces/white/small_knight.bmp"))
	{
		tex2->SetPixelFormat(render.GetPixelFormat());
		tex2->InitTransparent(Colour(0x00000000), Colour(0xffffff00));
		tex2->RenderTexture(&render);
	}

	SDL_Surface* pawnSurface = LoadSurface("data/pieces/black/small_knight.bmp");

	int w = pawnSurface->w;
	int h = pawnSurface->h;

	//SDL_Surface* formattedPawnSurface = SDL_CreateRGBSurface( 0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	//SDL_PixelFormat* fmt = formattedPawnSurface->format; //373694468

	render.BeginDraw();

	render.DrawRectangle(Colour(0x808080ff), 0, 0, render.GetWidth(), render.GetHeight());
	render.DrawTexture(tex, 10, 10);
	render.DrawTexture(tex2, 40, 40);
	//SDL_RenderCopy(render.renderer, pawn, NULL, &pos);

	render.EndDraw();

	//SDL_RenderCopy(render.renderer, pawn, NULL, &pos);
	//SDL_RenderPresent(render.renderer);

	SDL_Delay( 10000);

	return 0;

	using namespace std;
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}


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
		SDL_RenderPresent( renderer);
		std::cout << ++counter << std::endl;
	}

	SDL_DestroyTexture( texture);

	SDL_DestroyRenderer( renderer);
	SDL_DestroyWindow( window);

	return 0;
}
