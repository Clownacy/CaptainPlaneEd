/*
    Copyright (C) 2016-2018 Clownacy

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

#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "Common.h"
#include "PrjHndl.h"
#include "WinAPI.h"

namespace WinAPI
{

HWND hWnd;
HMENU hMenu;
HMENU hSubMenu_File;
HMENU hSubMenu_View;
COLORREF custom_colours[16];

void SaveHWND(SDL_Window* const window);
void CreateMenuBar(void);
void HandleWindowsEvent(const SDL_Event* const event);
bool OpenProjectFilePrompt(char** const filepath);
void SetMenuBarOptionGreyed(bool enable, int menu_option);

void SaveHWND(SDL_Window* const window)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version)
	SDL_GetWindowWMInfo(window,&info);
	hWnd = info.info.win.window;
}

void CreateMenuBar(void)
{
	hMenu = CreateMenu();
	hSubMenu_File = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu_File, "&File");
	AppendMenu(hSubMenu_File, MF_STRING, MENUBAR_OPENPROJECT, "&Open");
	AppendMenu(hSubMenu_File, MF_STRING | MF_GRAYED, MENUBAR_SAVE, "&Save");
	AppendMenu(hSubMenu_File, MF_STRING | MF_GRAYED, MENUBAR_CLOSE, "&Close");
	AppendMenu(hSubMenu_File, MF_SEPARATOR, 0, NULL);
	AppendMenu(hSubMenu_File, MF_STRING, MENUBAR_EXIT, "&Exit");
	hSubMenu_View = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu_View, "&View");
	AppendMenu(hSubMenu_View, MF_STRING, MENUBAR_BACKGROUNDCOLOUR, "&Background colour...");
	AppendMenu(hSubMenu_View, MF_SEPARATOR, 0, NULL);
	AppendMenu(hSubMenu_View, MF_STRING | MF_GRAYED | MF_UNCHECKED, MENUBAR_PALETTELINE1, "Palette line &1");
	AppendMenu(hSubMenu_View, MF_STRING | MF_GRAYED | MF_UNCHECKED, MENUBAR_PALETTELINE2, "Palette line &2");
	AppendMenu(hSubMenu_View, MF_STRING | MF_GRAYED | MF_UNCHECKED, MENUBAR_PALETTELINE3, "Palette line &3");
	AppendMenu(hSubMenu_View, MF_STRING | MF_GRAYED | MF_UNCHECKED, MENUBAR_PALETTELINE4, "Palette line &4");

	SetMenu(hWnd, hMenu);

	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
}

void HandleWindowsEvent(const SDL_Event* const event)
{
	if (event->syswm.msg->msg.win.msg == WM_COMMAND)
	{
		switch (LOWORD(event->syswm.msg->msg.win.wParam))
		{
			case MENUBAR_OPENPROJECT:
			{
				char* filename;
				if (OpenProjectFilePrompt(&filename) == true)
				{
					delete CurProject;
					CurProject = new Project(filename);

					//Process initial display
					MainScreen->Clear();
					CurProject->Redraw();
				}
				delete[] filename;
				break;
			}
			case MENUBAR_SAVE:
			{
				CurProject->Save();
				break;
			}
			case MENUBAR_CLOSE:
			{
				delete CurProject;
				CurProject = NULL;	// Deleting an object does not NULL this pointer, so we have to do it ourselves
				MainScreen->Clear();
				break;
			}
			case MENUBAR_EXIT:
			{
				exit(1);
			}
			case MENUBAR_BACKGROUNDCOLOUR:
			{
				CHOOSECOLOR user_colour;
				memset(&user_colour, 0, sizeof(user_colour));
				user_colour.lStructSize = sizeof(user_colour);
				user_colour.hwndOwner = hWnd;
				user_colour.rgbResult = RGB(MainScreen->background_colour.r,MainScreen->background_colour.g,MainScreen->background_colour.b);
				user_colour.lpCustColors = custom_colours;
				user_colour.Flags = CC_RGBINIT;
				if (ChooseColor(&user_colour) == true)
				{
					MainScreen->background_colour.r = GetRValue(user_colour.rgbResult);
					MainScreen->background_colour.g = GetGValue(user_colour.rgbResult);
					MainScreen->background_colour.b = GetBValue(user_colour.rgbResult);
					MainScreen->Clear();
					if (CurProject != NULL)
						CurProject->Redraw();
				}
				break;
			}
			case MENUBAR_PALETTELINE1:
			{
				CurProject->LevelMap->SetPalCurrent(0);
				CurProject->GfxStuff->DrawSelector();
				break;
			}
			case MENUBAR_PALETTELINE2:
			{
				CurProject->LevelMap->SetPalCurrent(1);
				CurProject->GfxStuff->DrawSelector();
				break;
			}
			case MENUBAR_PALETTELINE3:
			{
				CurProject->LevelMap->SetPalCurrent(2);
				CurProject->GfxStuff->DrawSelector();
				break;
			}
			case MENUBAR_PALETTELINE4:
			{
				CurProject->LevelMap->SetPalCurrent(3);
				CurProject->GfxStuff->DrawSelector();
				break;
			}
		}
	}
}

bool OpenProjectFilePrompt(char** const filepath)
{
	const int filepath_size = 500;
	*filepath = new char[filepath_size];
	(*filepath)[0] = '\0';
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = TEXT("PlaneEd project file (*.txt)\0*.txt\0All files (*.*)\0*.*\0\0");
	ofn.lpstrFile = *filepath;
	ofn.nMaxFile = filepath_size;
	ofn.lpstrTitle = TEXT("Open project file");
	ofn.Flags = OFN_FILEMUSTEXIST;
	return GetOpenFileName(&ofn);
}

void SetMenuBarOptionGreyed(bool enable, int menu_option)
{
	HMENU submenu;
	if (menu_option > MENUBAR_FILE_START && menu_option < MENUBAR_FILE_END)
		submenu = hSubMenu_File;
	else //if (menu_option > MENUBAR_VIEW_START && menu_option < MENUBAR_VIEW_END)
		submenu = hSubMenu_View;

	EnableMenuItem(submenu, menu_option, enable ? MF_ENABLED : MF_DISABLED);
}

void SetMenuBarOptionChecked(bool enable, int menu_option)
{
	HMENU submenu;
	if (menu_option > MENUBAR_FILE_START && menu_option < MENUBAR_FILE_END)
		submenu = hSubMenu_File;
	else //if (menu_option > MENUBAR_VIEW_START && menu_option < MENUBAR_VIEW_END)
		submenu = hSubMenu_View;

	CheckMenuItem(submenu, menu_option, enable ? MF_CHECKED : MF_UNCHECKED);
}

}
