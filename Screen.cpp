/*
    Copyright (C) 2009-2011 qiuu
    Copyright (C) 2016 Clownacy

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

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

	this->window = SDL_CreateWindow("Captain PlaneEd v1.0.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (this->window == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL Window", SDL_GetError(), NULL);
		exit(1);
	}

	this->render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (this->render == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL Renderer", SDL_GetError(), NULL);
		exit(1);
	}

	SDL_RenderSetLogicalSize(render, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	this->surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);	// Implicitly ARGB8888, compatible with the below texture
	if (this->surface==NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init screen SDL Surface", SDL_GetError(), NULL);
		exit(1);
	}
	
	this->texture = SDL_CreateTexture(this->render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture==NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init screen SDL Texture", SDL_GetError(), NULL);
		exit(1);
	}

	this->BackgroundColour = {.red = 0, .green = 0, .blue = 0};

	// Windows-only crap to generate a menu bar
	WinAPI::SaveHWND(this->window);
	WinAPI::CreateMenuBar();
}

void Screen::ProcessDisplay(void)
{
	void* pixels;
	int pitch;
	SDL_LockTexture(this->texture, NULL, &pixels, &pitch);
	memcpy(pixels, this->surface->pixels, pitch*this->surface->h);
	SDL_UnlockTexture(this->texture);

	SDL_RenderCopy(this->render, this->texture, NULL, NULL);
	SDL_RenderPresent(this->render);
}

void Screen::Fill(uint8_t red, uint8_t green, uint8_t blue)
{
	SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, red, green, blue));
}
