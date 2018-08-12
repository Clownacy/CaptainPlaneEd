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

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <SDL2/SDL.h>

#include "Screen.h"
#ifdef _WIN32
#include "WinAPI.h"
#include <windows.h>
#endif

#define INTERNAL_UPSCALE_FACTOR 3

Screen::Screen(void)
{
	if (SDL_Init(SDL_INIT_VIDEO)<0)
		this->ShowInternalError("Unable to init SDL video\n\n", SDL_GetError());

	atexit(SDL_Quit);

	this->window = SDL_CreateWindow("Captain PlaneEd v1.0.1+", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT + GetSystemMetrics(SM_CYMENU), SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (this->window == NULL)
		this->ShowInternalError("Unable to init SDL Window\n\n", SDL_GetError());

	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	if (this->renderer == NULL)
		this->ShowInternalError("Unable to init SDL Renderer\n\n", SDL_GetError());

	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	this->surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);	// Implicitly ARGB8888, compatible with the below texture
	if (this->surface==NULL)
		this->ShowInternalError("Unable to init screen SDL Surface\n\n", SDL_GetError());

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture==NULL)
		this->ShowInternalError("Unable to init screen SDL Texture\n\n", SDL_GetError());

	SDL_LockTexture(this->texture, NULL, &this->surface->pixels, &this->surface->pitch);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	this->upscaled_texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH*INTERNAL_UPSCALE_FACTOR, SCREEN_HEIGHT*INTERNAL_UPSCALE_FACTOR);
	if (this->upscaled_texture==NULL)
		this->ShowInternalError("Unable to init screen SDL Texture\n\n", SDL_GetError());

	this->background_colour = {.r = 0, .g = 0, .b = 0, .a = 0};

#ifdef _WIN32
	// Windows-only crap to generate a menu bar
	WinAPI::SaveHWND(this->window);
	WinAPI::CreateMenuBar();
#endif
}

void Screen::ProcessDisplay(void)
{
	// SDL2's 'linear' filter is ugly with pixel art, and the
	// 'nearest' one gets pretty bad with non-integer scaling.
	// Instead, we're going to emulate a type of upscale that
	// preserves the quality of the image, while smoothing pixels
	// that 'bleed'.
	SDL_UnlockTexture(this->texture);

	SDL_SetRenderTarget(this->renderer, this->upscaled_texture);		// Render to texture...
	SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);		// ...and upscale using 'nearest'

	SDL_LockTexture(this->texture, NULL, &this->surface->pixels, &this->surface->pitch);

	SDL_SetRenderTarget(this->renderer, NULL);				// Render to screen...
	SDL_RenderCopy(this->renderer, this->upscaled_texture, NULL, NULL);	// ...and upscale/downscale using 'linear'

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

void Screen::ShowInternalError(const char* const message_part1, const char* const message_part2)
{
	char* const whole_message = new char[strlen(message_part1)+strlen(message_part2)+1];
	sprintf(whole_message, "%s%s", message_part1, message_part2);
	this->ShowInternalError(whole_message);
	delete[] whole_message;
}
