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

#include <cstdarg>

#include "SDL.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480	//minimum size to allow for 64x64 maps (TODO - what about 128x64?)

#include "libraries/file-utilities/file-utilities.h"
#include "libraries/imgui/imgui.h"

class Screen
{
	friend class Graphics;
public:
	SDL_Color background_colour;

	Screen(void);
	~Screen(void);
	void MarkDisplayChanged(void);
	void ProcessDisplay(void);
	void Clear(void);
	void WindowResized(int width, int height);
	void ShowInformation(const char *message);
	void ShowInformation(const char *message_part1, const char *message_part2);
	void ShowWarning(const char *message);
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
	[[noreturn]] void ShowError(const char *message);
	[[noreturn]] void ShowInternalError(const char *format, std::va_list args);
#else
	void ShowError(const char *message);
	void ShowInternalError(const char *format, std::va_list args);
#endif
	void ShowInternalError(const char *format, ...);
	void ProcessEvent(const SDL_Event &event);
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *upscaled_texture;
	SDL_Texture *texture;
	bool display_changed;
	float screen_scale = 1.0f;
	float screen_x, screen_y;
	bool left_mouse_button_down, right_mouse_button_down;
	Sint32 previous_mouse_x, previous_mouse_y;
	float dpi_scale;
	ImGuiStyle style_backup;
	FileUtilities file_utilities;

	float GetDPIScale(void) const;
	void ReloadFonts(void);
};
