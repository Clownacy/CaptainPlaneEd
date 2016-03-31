#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "Common.h"
#include "LevMap.h"
#include "PrjHndl.h"

void CreateMenuBar(SDL_Window* const window);
void HandleWindowsEvent(const SDL_Event* const event);
bool OpenProjectFilePrompt(char* const filepath);

enum
{
	ID_FILE_OPENPROJECT,
	ID_FILE_SAVE,
	ID_FILE_EXIT
};

void CreateMenuBar(SDL_Window* const window)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version)
	SDL_GetWindowWMInfo(window,&info);
	HWND hWnd = info.info.win.window;

	HMENU hMenu = CreateMenu();
	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPENPROJECT, "&Open project file");
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_SAVE, "&Save");
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "&Exit");

	SetMenu(hWnd, hMenu);
	
	SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
}

void HandleWindowsEvent(const SDL_Event* const event)
{
	if (event->syswm.msg->msg.win.msg == WM_COMMAND)
	{
		switch (LOWORD(event->syswm.msg->msg.win.wParam))
		{
			case ID_FILE_OPENPROJECT:
			{
				char filename[500] = "";
				if (OpenProjectFilePrompt(filename) == true)
				{
					MessageBox(0, filename,"Is this your card?", MB_OK);
				}
				break;
			}
			case ID_FILE_SAVE:
			{
				Current_LevelMap->SaveMap(FILE_MAP_TEMP);
				Current_ProjectData->map.Save(FILE_MAP_TEMP, Current_ProjectData->map.saveName);
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", "Save complete.", NULL);
				break;
			}
			case ID_FILE_EXIT:
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
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = filepath;
        ofn.nMaxFile = 500;
        ofn.hInstance = NULL;
        ofn.lpstrFilter = TEXT("PlaneEd project file (*.txt)\0*.txt\0\0");
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_FILEMUSTEXIST;
        bool bRes = GetOpenFileName(&ofn);
	return bRes;
}
