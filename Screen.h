#pragma once

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480	//minimum size to allow for 64x64 maps
#define WINDOW_HEIGHT SCREEN_HEIGHT+20	//Windows menu bar adds 20 pixels

class Screen
{
public:
	SDL_Window* window;
	SDL_Renderer* render;
	SDL_Surface* surface;
	SDL_Texture* texture;

	Screen(void);
	void ProcessDisplay(void);
};
