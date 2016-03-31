#pragma once

#include <cstdint>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480	//minimum size to allow for 64x64 maps

class Screen
{
public:
	struct
	{
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	} background_colour;

	SDL_Window* window;
	SDL_Renderer* render;
	SDL_Surface* surface;
	SDL_Texture* texture;

	Screen(void);
	void ProcessDisplay(void);
	void Fill(uint8_t r, uint8_t g, uint8_t b);
};
