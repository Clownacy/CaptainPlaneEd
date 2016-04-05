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
	AppendMenu(hSubMenu_File, MF_STRING, MENUBAR_FILE_OPENPROJECT, "&Open");
	AppendMenu(hSubMenu_File, MF_STRING | MF_GRAYED, MENUBAR_FILE_SAVE, "&Save");
	AppendMenu(hSubMenu_File, MF_STRING | MF_GRAYED, MENUBAR_FILE_CLOSE, "&Close");
	AppendMenu(hSubMenu_File, MF_SEPARATOR, 0, NULL);
	AppendMenu(hSubMenu_File, MF_STRING, MENUBAR_FILE_EXIT, "&Exit");
	hSubMenu_View = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu_View, "&View");
	AppendMenu(hSubMenu_View, MF_STRING, MENUBAR_VIEW_BACKGROUNDCOLOUR, "&Background Colour...");

	SetMenu(hWnd, hMenu);

	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
}

void HandleWindowsEvent(const SDL_Event* const event)
{
	if (event->syswm.msg->msg.win.msg == WM_COMMAND)
	{
		switch (LOWORD(event->syswm.msg->msg.win.wParam))
		{
			case MENUBAR_FILE_OPENPROJECT:
			{
				char filename[500] = "";
				if (WinAPI::OpenProjectFilePrompt(filename) == true)
				{
					if (CurProject != NULL)
						delete CurProject;
					CurProject = new Project(filename, MainScreen);

					EnableMenuItem(hSubMenu_File, MENUBAR_FILE_SAVE, MF_ENABLED);
					EnableMenuItem(hSubMenu_File, MENUBAR_FILE_CLOSE, MF_ENABLED);

					//Process initial display
					MainScreen->Fill(MainScreen->BackgroundColour.red, MainScreen->BackgroundColour.green, MainScreen->BackgroundColour.blue);
					CurProject->Redraw();
				}
				break;
			}
			case MENUBAR_FILE_SAVE:
			{
				CurProject->Save();
				break;
			}
			case MENUBAR_FILE_CLOSE:
			{
				delete CurProject;
				CurProject = NULL;	// Deleting an object does not NULL this pointer, so we have to do it ourselves
				EnableMenuItem(hSubMenu_File, MENUBAR_FILE_SAVE, MF_GRAYED);
				EnableMenuItem(hSubMenu_File, MENUBAR_FILE_CLOSE, MF_GRAYED);
				MainScreen->Fill(MainScreen->BackgroundColour.red, MainScreen->BackgroundColour.green, MainScreen->BackgroundColour.blue);
				break;
			}
			case MENUBAR_FILE_EXIT:
			{
				exit(1);
			}
			case MENUBAR_VIEW_BACKGROUNDCOLOUR:
			{
				CHOOSECOLOR user_colour;
				memset(&user_colour, 0, sizeof(user_colour));
				user_colour.lStructSize = sizeof(user_colour);
				user_colour.hwndOwner = hWnd;
				user_colour.rgbResult = RGB(MainScreen->BackgroundColour.red,MainScreen->BackgroundColour.green,MainScreen->BackgroundColour.blue);
				user_colour.lpCustColors = custom_colours;
				user_colour.Flags = CC_RGBINIT;
				if (ChooseColor(&user_colour) == true)
				{
					MainScreen->BackgroundColour.red = GetRValue(user_colour.rgbResult);
					MainScreen->BackgroundColour.green = GetGValue(user_colour.rgbResult);
					MainScreen->BackgroundColour.blue = GetBValue(user_colour.rgbResult);
					MainScreen->Fill(MainScreen->BackgroundColour.red, MainScreen->BackgroundColour.green, MainScreen->BackgroundColour.blue);
					if (CurProject != NULL)
						CurProject->Redraw();
				}
				break;
			}
		}
	}
}

bool OpenProjectFilePrompt(char* const filepath)
{
	OPENFILENAME ofn;
        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWnd;
        ofn.hInstance = NULL;
        ofn.lpstrFilter = TEXT("PlaneEd project file (*.txt)\0*.txt\0\0");
        ofn.nFilterIndex = 1;
        ofn.lpstrFile = filepath;
        ofn.nMaxFile = 500;
        ofn.Flags = OFN_FILEMUSTEXIST;
        bool bRes = GetOpenFileName(&ofn);
	return bRes;
}

}
