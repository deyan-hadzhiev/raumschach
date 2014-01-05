#ifndef __RENDER_H__
#define __RENDER_H__

#include <SDL.h>
#include "configuration.h"
#include "raumschach.h"
#include "utils.h"

class Colour
{
public:
	Colour(Uint32 colour = 0);
	Colour(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xff);
	Colour(const Colour& copy);

	Colour& operator=(const Colour& assign);

	void SetR(Uint8 value);
	void SetG(Uint8 value);
	void SetB(Uint8 value);
	void SetA(Uint8 value);
	void SetColour(Uint32 value);

	Uint8 GetR() const;
	Uint8 GetG() const;
	Uint8 GetB() const;
	Uint8 GetA() const;
	Uint32 GetColour() const;

private:
	union
	{
		Uint8 colArr[GraphicConfig::COLOURS_COUNT];
		Uint32 col;
	};
};

class Render;
class Texture
{
public:
	Texture();
	Texture(const char* filename);

	~Texture();

	bool LoadTexture(const char* filename);
	bool SetPixelFormat(SDL_PixelFormat* source, Uint32 flags = 0);
	bool RenderTexture(Render* render);
	void InitTransparent(Colour col, Colour mask = 0xffffff00);
	SDL_Texture* GetTexture() const;

	int GetWidth() const;
	int GetHeight() const;

private:
	// disable copy and assign of textures, because no pretty sdl routine seems to exist
	Texture(const Texture& copy);
	Texture& operator=(const Texture& assign);

	SDL_Texture* tex;
	SDL_Surface* img;

	int width;
	int height;
};

class Render
{
public:
	friend class Texture;

	Render(int width, int height);
	~Render();

	//some accessors
	int GetWidth() const;
	int GetHeight() const;
	SDL_PixelFormat* GetPixelFormat() const;

	void BeginDraw();
	void DrawRectangle(Colour col, int x, int y, int width, int height);
	void DrawRectangle(Colour col, Rect r);
	void DrawArray(const Colour arr[], int x, int y, int width, int height);
	void DrawArray(const Colour arr[], Rect r);
	void DrawTexture(const Texture* tex, int x, int y);
	void EndDraw();

	void StartEventLoop(Raumschach* eventHandler);
private:
	// disable copy and assignment of redner objects
	Render(const Render& copy);
	Render& operator=(const Render& assign);

	void FreeResources();
	void ErrorExit();

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* mainTexture;
	SDL_PixelFormat* pixelFormat;

	int frameWidth;
	int frameHeight;
};

#endif // __RENDER_H__