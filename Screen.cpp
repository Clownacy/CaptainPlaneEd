#include <cstdint>
#include <SDL2/SDL.h>

#include "Screen.h"
#include "WinAPI.h"

#define WINDOW_HEIGHT SCREEN_HEIGHT+20	//Windows menu bar adds 20 pixels

Screen::Screen(void)
{
	if (SDL_Init(SDL_INIT_VIDEO)<0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL video", SDL_GetError(), NULL);
		exit(1);
	}
	atexit(SDL_Quit);

	window = SDL_CreateWindow("Captain PlaneEd", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL Window", SDL_GetError(), NULL);
		exit(1);
	}

	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (render == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL Renderer", SDL_GetError(), NULL);
		exit(1);
	}

	SDL_RenderSetLogicalSize(render, SCREEN_WIDTH, SCREEN_HEIGHT);

	surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);	// Implicitly ARGB8888, compatible with the below texture
	if (surface==NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init surface SDL Surface", SDL_GetError(), NULL);
		exit(1);
	}
	
	texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (texture==NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL Texture", SDL_GetError(), NULL);
		exit(1);
	}

	this->BackgroundColour = {.red = 0, .green = 0, .blue = 0};

	// Windows-only crap to generate a menu bar
	WinAPI::SaveHWND(window);
	WinAPI::CreateMenuBar();
}

void Screen::ProcessDisplay(void)
{
	void* pixels;
	int pitch;
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	memcpy(pixels, surface->pixels, pitch*surface->h);
	SDL_UnlockTexture(texture);

	SDL_RenderCopy(render, texture, NULL, NULL);
	SDL_RenderPresent(render);
}

void Screen::Fill(uint8_t red, uint8_t green, uint8_t blue)
{
	SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, red, green, blue));
}
