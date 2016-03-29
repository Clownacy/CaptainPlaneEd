#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

enum
{
	ID_FILE_OPENPROJECT,
	ID_FILE_SAVE,
	ID_FILE_EXIT
};

void CreateMenuBar(SDL_Window* window)
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

void HandleWindowsEvent(SDL_Event* event)
{
	if (event->syswm.msg->msg.win.msg == WM_COMMAND)
	{
		switch (LOWORD(event->syswm.msg->msg.win.wParam))
		{
			case ID_FILE_OPENPROJECT:
				break;
			case ID_FILE_SAVE:
				break;
			case ID_FILE_EXIT:
				exit(1);
				break;
		}
	}
}
