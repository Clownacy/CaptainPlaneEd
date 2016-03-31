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
HMENU hSubMenu;

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
	hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
	AppendMenu(hSubMenu, MF_STRING, MENUBAR_FILE_OPENPROJECT, "&Open project file");
	AppendMenu(hSubMenu, MF_STRING | MF_GRAYED, MENUBAR_FILE_SAVE, "&Save");
	AppendMenu(hSubMenu, MF_STRING | MF_GRAYED, MENUBAR_FILE_CLOSE, "&Close");
	AppendMenu(hSubMenu, MF_STRING, MENUBAR_FILE_EXIT, "&Exit");

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

					EnableMenuItem(hSubMenu, MENUBAR_FILE_SAVE, MF_ENABLED);
					EnableMenuItem(hSubMenu, MENUBAR_FILE_CLOSE, MF_ENABLED);

					//Process initial display
					MainScreen->Fill(0, 0, 0);
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
				EnableMenuItem(hSubMenu, MENUBAR_FILE_SAVE, MF_GRAYED);
				EnableMenuItem(hSubMenu, MENUBAR_FILE_CLOSE, MF_GRAYED);
				MainScreen->Fill(0, 0, 0);
				break;
			}
			case MENUBAR_FILE_EXIT:
			{
				exit(1);
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
