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
#ifdef _WIN32
#include "WinAPI.h"
#endif

#ifdef _WIN32
#define WINDOW_HEIGHT SCREEN_HEIGHT+20	// Windows menu bar adds 20 pixels
#else
#define WINDOW_HEIGHT SCREEN_HEIGHT
#endif

Screen::Screen(void)
{
	if (SDL_Init(SDL_INIT_VIDEO)<0)
		this->ShowInternalError("Unable to init SDL video");

	atexit(SDL_Quit);

	this->window = SDL_CreateWindow("Captain PlaneEd v1.0.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (this->window == NULL)
		this->ShowInternalError("Unable to init SDL Window");

	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (this->renderer == NULL)
		this->ShowInternalError("Unable to init SDL Renderer");

	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	this->surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);	// Implicitly ARGB8888, compatible with the below texture
	if (this->surface==NULL)
		this->ShowInternalError("Unable to init screen SDL Surface");
	
	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture==NULL)
		this->ShowInternalError("Unable to init screen SDL Texture");

	this->background_colour = {.r = 0, .g = 0, .b = 0};

#ifdef _WIN32
	// Windows-only crap to generate a menu bar
	WinAPI::SaveHWND(this->window);
	WinAPI::CreateMenuBar();
#endif
}

void Screen::ProcessDisplay(void)
{
	void* pixels;
	int pitch;
	SDL_LockTexture(this->texture, NULL, &pixels, &pitch);
	memcpy(pixels, this->surface->pixels, pitch*this->surface->h);
	SDL_UnlockTexture(this->texture);

	SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
	SDL_RenderPresent(this->renderer);
}

void Screen::Clear(void)
{
	SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, this->background_colour.r, this->background_colour.g, this->background_colour.b));
}

void Screen::ShowInformation(const char* const message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", message, this->window);
}

void Screen::ShowInformation(const char* const message_part1, const char* const message_part2)
{
	char* const whole_message = new char[strlen(message_part1)+strlen(message_part2)+1];
	sprintf(whole_message, "%s%s", message_part1, message_part2);
	this->ShowInformation(whole_message);
	delete[] whole_message;
}

void Screen::ShowWarning(const char* const message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", message, this->window);
}

void Screen::ShowError(const char* const message)
{
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, this->window);
	exit(1);
}

void Screen::ShowInternalError(const char* const message)
{
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Internal Error", message, this->window);
	exit(1);
}

