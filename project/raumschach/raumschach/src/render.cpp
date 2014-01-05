#include <SDL.h>
#include "render.h"
#include "error.h"
#include "configuration.h"
#include "utils.h"

Colour::Colour(Uint32 colour) : col(colour) {}
Colour::Colour(Uint8 r, Uint8 g, Uint8 b, Uint8 a) : col(0)
{
	colArr[GraphicConfig::RED] = r;
	colArr[GraphicConfig::GREEN] = g;
	colArr[GraphicConfig::BLUE] = b;
	colArr[GraphicConfig::ALPHA] = a;
}

Colour::Colour(const Colour& copy) : col(copy.col) {}

Colour& Colour::operator=(const Colour& assign)
{
	col = assign.col;
	return *this;
}

void Colour::SetR(Uint8 value)
{
	colArr[GraphicConfig::RED] = value;
}

void Colour::SetG(Uint8 value)
{
	colArr[GraphicConfig::GREEN] = value;
}

void Colour::SetB(Uint8 value)
{
	colArr[GraphicConfig::BLUE] = value;
}

void Colour::SetA(Uint8 value)
{
	colArr[GraphicConfig::ALPHA] = value;
}

void Colour::SetColour(Uint32 value)
{
	col = value;
}

Uint8 Colour::GetR() const
{
	return colArr[GraphicConfig::RED];
}

Uint8 Colour::GetG() const
{
	return colArr[GraphicConfig::GREEN];
}

Uint8 Colour::GetB() const
{
	return colArr[GraphicConfig::BLUE];
}

Uint8 Colour::GetA() const
{
	return colArr[GraphicConfig::ALPHA];
}

Uint32 Colour::GetColour() const
{
	return col;
}

Texture::Texture()
	: tex(nullptr), img(nullptr), width(0), height(0)
{}

Texture::Texture(const char* filename)
	: tex(nullptr), img(nullptr), width(0), height(0)
{
	if(filename)
	{
		img = SDL_LoadBMP(filename);
		width = img->w;
		height = img->h;
	}
}

Texture::~Texture()
{
	if(tex)
	{
		SDL_DestroyTexture(tex);
		tex = nullptr;
	}
	if(img)
	{
		SDL_FreeSurface(img);
		img = nullptr;
	}
}

bool Texture::LoadTexture(const char* filename)
{
	bool retval = false;
	if(filename)
	{
		if(tex)
		{
			SDL_DestroyTexture(tex);
			tex = nullptr;
		}
		if(img)
		{
			SDL_FreeSurface(img);
			img = nullptr;
		}
		img = SDL_LoadBMP(filename);
		retval = img != NULL;
		if(retval)
		{
			width = img->w;
			height = img->h;
		}
	}
	return retval;
}

bool Texture::SetPixelFormat(SDL_PixelFormat* source, Uint32 flags)
{
	bool retval = false;
	if(source)
	{
		SDL_Surface* newImg = SDL_ConvertSurface(img, source, flags);
		if(newImg)
		{
			retval = true;
			SDL_FreeSurface(img);
			img = newImg;
		}
	}
	return retval;
}

bool Texture::RenderTexture(Render* render)
{
	bool retval = false;
	if(render && img)
	{
		if(tex)
		{
			SDL_DestroyTexture(tex);
			tex = nullptr;
		}
		tex = SDL_CreateTextureFromSurface(render->renderer, img);
		if(tex)
		{
			retval = true;
			SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
		}
	}
	return retval;
}

void Texture::InitTransparent(Colour col, Colour mask)
{
	if(img)
	{
		if(tex)
		{
			SDL_DestroyTexture(tex);
			tex = nullptr;
		}

		int w = img->w;
		int h = img->h;
		Uint32* pixels = (Uint32*) img->pixels;
		int colourChannels = BitBoard::BitCount(mask.GetColour()) / 8;
		Uint32 maskRaw = mask.GetColour();
		Uint32 colourRaw = col.GetColour();
		for(int i = 0; i < w*h; ++i)
		{
			Uint32 difference = 0;
			difference += ((pixels[i] & (0xff000000 & maskRaw)) ^ (colourRaw & (0xff000000 & maskRaw))) >> 24;
			difference += ((pixels[i] & (0x00ff0000 & maskRaw)) ^ (colourRaw & (0x00ff0000 & maskRaw))) >> 16;
			difference += ((pixels[i] & (0x0000ff00 & maskRaw)) ^ (colourRaw & (0x0000ff00 & maskRaw))) >> 8;
			difference /= colourChannels;
			pixels[i] = (pixels[i] & 0xffffff00) | difference;
		}
	}
}

SDL_Texture* Texture::GetTexture() const
{
	return tex;
}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
}

Render::Render(int width, int height)
	: 
	window(nullptr), 
	renderer(nullptr),
	mainTexture(nullptr),
	frameWidth(width),
	frameHeight(height)
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0) ErrorExit();

	atexit(SDL_Quit);

	window = SDL_CreateWindow( 
		SysConfig::WINDOW_NAME,
		SysConfig::SCREEN_SPAWN_X,
		SysConfig::SCREEN_SPAWN_Y,
		SysConfig::SCREEN_WIDTH,
		SysConfig::SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	if(window == NULL) ErrorExit();

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if(renderer == NULL) ErrorExit();

	mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SysConfig::SCREEN_WIDTH, SysConfig::SCREEN_HEIGHT);

	if(mainTexture == NULL) ErrorExit();

	// init pixel format
	pixelFormat = new SDL_PixelFormat;
	pixelFormat->format = SDL_PIXELFORMAT_RGBA8888;
	pixelFormat->Rshift = 24;
	pixelFormat->Rmask = 0xff000000;
	pixelFormat->Rloss = 0;
	pixelFormat->Gshift = 16;
	pixelFormat->Gmask = 0x00ff0000;
	pixelFormat->Gloss = 0;
	pixelFormat->Bshift = 8;
	pixelFormat->Bmask = 0x0000ff00;
	pixelFormat->Bloss = 0;
	pixelFormat->Ashift = 0;
	pixelFormat->Amask = 0x000000ff;
	pixelFormat->Aloss = 0;
	pixelFormat->palette = NULL;
	pixelFormat->padding[0] = '\0';
	pixelFormat->padding[1] = '\0';
	pixelFormat->BitsPerPixel = 32;
	pixelFormat->BytesPerPixel = 4;
}

Render::~Render()
{
	FreeResources();
}

int Render::GetWidth() const
{
	return frameWidth;
}

int Render::GetHeight() const
{
	return frameHeight;
}

SDL_PixelFormat* Render::GetPixelFormat() const
{
	return pixelFormat;
}

void Render::BeginDraw()
{
	SDL_RenderClear(renderer);
}

void Render::DrawRectangle(Colour col, int destX, int destY, int width, int height)
{
	void * pixels;
	int pitch;
	Uint32 destColour = col.GetColour();
	SDL_LockTexture(mainTexture, NULL, &pixels, &pitch);
	for(int y = 0; y < height; ++y)
	{
		Uint32* dest = (Uint32*)((Uint8*)pixels + (destY + y) * pitch) + destX;
		for(int x = 0; x < width; ++x)
		{
			*dest++ = destColour;
		}
	}
	SDL_UnlockTexture(mainTexture);
	
	// update the renderer
	SDL_Rect updateRect;
	updateRect.x = destX;
	updateRect.y = destY;
	updateRect.w = width;
	updateRect.h = height;
	SDL_RenderCopy(renderer, mainTexture, &updateRect, &updateRect);
}

void Render::DrawRectangle(Colour col, Rect r)
{
	DrawRectangle(col, r.x, r.y, r.width, r.height);
}

void Render::DrawArray(const Colour arr[], int destX, int destY, int width, int height)
{
	void * pixels;
	int pitch;
	SDL_LockTexture(mainTexture, NULL, &pixels, &pitch);
	for(int y = 0; y < height; ++y)
	{
		Uint32* dest = (Uint32*)((Uint8*)pixels + (destY + y) * pitch) + destX;
		for(int x = 0; x < width; ++x)
		{
			*dest++ = arr[y * width + x].GetColour();
		}
	}
	SDL_UnlockTexture(mainTexture);
	
	// update the renderer
	SDL_Rect updateRect;
	updateRect.x = destX;
	updateRect.y = destY;
	updateRect.w = width;
	updateRect.h = height;
	SDL_RenderCopy(renderer, mainTexture, &updateRect, &updateRect);
}

void Render::DrawArray(const Colour arr[], Rect r)
{
	DrawArray(arr, r.x, r.y, r.width, r.height);
}

void Render::DrawTexture(const Texture* tex, int x, int y)
{
	if(tex)
	{
		SDL_Rect dest;
		dest.x = x;
		dest.y = y;
		SDL_QueryTexture(tex->GetTexture(), NULL, NULL, &dest.w, &dest.h);
		SDL_RenderCopy(renderer, tex->GetTexture(), NULL, &dest);
	}
}

void Render::EndDraw()
{
	SDL_RenderPresent(renderer);
}

void Render::FreeResources()
{
	if(mainTexture)
	{
		SDL_DestroyTexture(mainTexture);
		mainTexture = nullptr;
	}
	if(renderer)
	{
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}
	if(window)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
	}
	if(pixelFormat)
	{
		delete pixelFormat;
		pixelFormat = nullptr;
	}
}

void Render::ErrorExit()
{
	FreeResources();
	Error( SDL_GetError()).Post().Exit(SysConfig::EXIT_SDL_ERROR);
}

void Render::StartEventLoop(Raumschach* handler)
{
	using namespace std;
	bool quit = false;
	SDL_Event evnt;
	int counter = 0;
	bool refresh = true;
	while( !quit )
	{
		while( SDL_PollEvent( &evnt))
		{
			switch( evnt.type)
			{
			case SDL_QUIT:
			case SDL_KEYDOWN:
				quit = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					SysConfig::MouseButton button = (SysConfig::MouseButton) evnt.button.button;
					handler->MouseClick(button, evnt.button.x, evnt.button.y);
					refresh = true;
					break;
				}
			}
			
		}

		if(refresh)
		{
			handler->IdleDrawBoard();
			refresh = false;
		}
	}
}