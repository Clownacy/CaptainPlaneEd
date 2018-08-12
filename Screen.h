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

#pragma once

#include <cstdint>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480	//minimum size to allow for 64x64 maps

class Screen
{
	friend class Graphics;
public:
	SDL_Color background_colour;

	Screen(void);
	void ProcessDisplay(void);
	void Clear(void);
	void ShowInformation(const char* const message);
	void ShowInformation(const char* const message_part1, const char* const message_part2);
	void ShowWarning(const char* const message);
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
	[[noreturn]] void ShowError(const char* const message);
	[[noreturn]] void ShowInternalError(const char* const message);
#else
	void ShowError(const char* const message);
	void ShowInternalError(const char* const message);
#endif
	void ShowInternalError(const char* const message_part1, const char* const message_part2);
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* upscaled_texture;
	SDL_Texture* texture;
	SDL_Surface* surface;
};
