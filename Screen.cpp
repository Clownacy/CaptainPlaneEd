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

#include "Screen.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>

#include "SDL.h"

#ifdef _WIN32
#include "WinAPI.h"
#include <windows.h>
#endif

#ifdef _WIN32
#define ADJUSTED_SCREEN_HEIGHT (SCREEN_HEIGHT + GetSystemMetrics(SM_CYMENU))
#else
#define ADJUSTED_SCREEN_HEIGHT SCREEN_HEIGHT
#endif

Screen::Screen(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		this->ShowInternalError("Unable to init SDL video\n\n", SDL_GetError());

	atexit(SDL_Quit);

	this->window = SDL_CreateWindow("Captain PlaneEd v1.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, ADJUSTED_SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (this->window == nullptr)
		this->ShowInternalError("Unable to init SDL Window\n\n", SDL_GetError());

	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	if (this->renderer == nullptr)
		this->ShowInternalError("Unable to init SDL Renderer\n\n", SDL_GetError());

	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture == nullptr)
		this->ShowInternalError("Unable to init screen SDL Texture\n\n", SDL_GetError());

	this->upscaled_texture = nullptr;

	this->background_colour.r = 0;
	this->background_colour.g = 0;
	this->background_colour.b = 0;
	this->background_colour.a = 0xFF;

#ifdef _WIN32
	// Windows-only crap to generate a menu bar
	WinAPI::SaveHWND(this->window);
	WinAPI::CreateMenuBar();
#endif
}

void Screen::ProcessDisplay(void)
{
	if (this->upscaled_texture != nullptr)
	{
		SDL_SetRenderTarget(this->renderer, this->upscaled_texture);
		SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
	}

	SDL_SetRenderTarget(this->renderer, nullptr);
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(this->renderer);
	SDL_RenderCopy(this->renderer, this->upscaled_texture != nullptr ? this->upscaled_texture : this->texture, nullptr, nullptr);

	SDL_RenderPresent(this->renderer);
}

void Screen::Clear(void)
{
	SDL_SetRenderTarget(this->renderer, this->texture);

	SDL_SetRenderDrawColor(this->renderer, this->background_colour.r, this->background_colour.g, this->background_colour.b, this->background_colour.a);
	SDL_RenderFillRect(this->renderer, nullptr);
}

void Screen::WindowResized(int width, int height)
{
	static unsigned int upscale_factor;

#ifdef _WIN32
	height -= GetSystemMetrics(SM_CYMENU);
#endif

	unsigned int new_upscale_factor = std::max(1, std::min((width + SCREEN_WIDTH / 2) / SCREEN_WIDTH, (height + SCREEN_HEIGHT / 2) / SCREEN_HEIGHT));

	if (new_upscale_factor != upscale_factor)
	{
		upscale_factor = new_upscale_factor;

		if (this->upscaled_texture != nullptr)
		{
			SDL_DestroyTexture(this->upscaled_texture);
			this->upscaled_texture = nullptr;
		}

		if (upscale_factor != 1)
		{
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
			this->upscaled_texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH * upscale_factor, SCREEN_HEIGHT * upscale_factor);
		}
	}
}

void Screen::ShowInformation(const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", message, this->window);
}

void Screen::ShowInformation(const char *message_part1, const char *message_part2)
{
	char *whole_message = new char[strlen(message_part1)+strlen(message_part2)+1];
	sprintf(whole_message, "%s%s", message_part1, message_part2);
	this->ShowInformation(whole_message);
	delete[] whole_message;
}

void Screen::ShowWarning(const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", message, this->window);
}

void Screen::ShowError(const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, this->window);
	exit(1);
}

void Screen::ShowInternalError(const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Internal Error", message, this->window);
	exit(1);
}

void Screen::ShowInternalError(const char *message_part1, const char *message_part2)
{
	char *whole_message = new char[strlen(message_part1)+strlen(message_part2)+1];
	sprintf(whole_message, "%s%s", message_part1, message_part2);
	this->ShowInternalError(whole_message);
	delete[] whole_message;
}
