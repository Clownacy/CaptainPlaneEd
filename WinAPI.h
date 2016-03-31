#pragma once

#include <windows.h>
#include <SDL2/SDL.h>

enum
{
	MENUBAR_FILE_OPENPROJECT,
	MENUBAR_FILE_SAVE,
	MENUBAR_FILE_CLOSE,
	MENUBAR_FILE_EXIT,
	MENUBAR_VIEW_BACKGROUNDCOLOUR
};

namespace WinAPI
{

extern HMENU hSubMenu_File;
extern HMENU hSubMenu_View;

extern void SaveHWND(SDL_Window* const window);
extern void CreateMenuBar(void);
extern void HandleWindowsEvent(const SDL_Event* const event);
extern bool OpenProjectFilePrompt(char* const filepath);

}
