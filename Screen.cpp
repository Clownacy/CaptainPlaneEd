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

#ifdef _WIN32
#define ADJUSTED_SCREEN_HEIGHT (SCREEN_HEIGHT + GetSystemMetrics(SM_CYMENU))
#else
#define ADJUSTED_SCREEN_HEIGHT SCREEN_HEIGHT
#endif

Screen::Screen(void)
{
	if (SDL_Init(SDL_INIT_VIDEO)<0)
		this->ShowInternalError("Unable to init SDL video\n\n", SDL_GetError());

	atexit(SDL_Quit);

	this->window = SDL_CreateWindow("Captain PlaneEd v1.0.1+", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, ADJUSTED_SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (this->window == NULL)
		this->ShowInternalError("Unable to init SDL Window\n\n", SDL_GetError());

	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	if (this->renderer == NULL)
		this->ShowInternalError("Unable to init SDL Renderer\n\n", SDL_GetError());

	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (this->texture==NULL)
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
	SDL_SetRenderTarget(this->renderer, NULL);
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(this->renderer);
	SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);

	SDL_RenderPresent(this->renderer);
}

void Screen::Clear(void)
{
	SDL_SetRenderDrawColor(this->renderer, this->background_colour.r, this->background_colour.g, this->background_colour.b, 0xFF);
	SDL_RenderFillRect(this->renderer, NULL);
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
