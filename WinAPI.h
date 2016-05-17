/*
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

#include <windows.h>
#include <SDL2/SDL.h>

enum
{
	MENUBAR_FILE_START,
	MENUBAR_FILE_OPENPROJECT,
	MENUBAR_FILE_SAVE,
	MENUBAR_FILE_CLOSE,
	MENUBAR_FILE_EXIT,
	MENUBAR_FILE_END,

	MENUBAR_VIEW_START,
	MENUBAR_VIEW_BACKGROUNDCOLOUR,
	MENUBAR_VIEW_PALETTELINE1,
	MENUBAR_VIEW_PALETTELINE2,
	MENUBAR_VIEW_PALETTELINE3,
	MENUBAR_VIEW_PALETTELINE4,
	MENUBAR_VIEW_END
};

namespace WinAPI
{

extern HMENU hSubMenu_File;
extern HMENU hSubMenu_View;

extern void SaveHWND(SDL_Window* const window);
extern void CreateMenuBar(void);
extern void HandleWindowsEvent(const SDL_Event* const event);
extern bool OpenProjectFilePrompt(char** const filepath);
extern void SetMenuBarOptionGreyed(bool enable, int menu_option);
extern void SetMenuBarOptionChecked(bool enable, int menu_option);

}
