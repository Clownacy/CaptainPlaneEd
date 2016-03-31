#pragma once

#include <SDL2/SDL.h>

namespace WinAPI
{

extern void SaveHWND(SDL_Window* const window);
extern void CreateMenuBar(void);
extern void HandleWindowsEvent(const SDL_Event* const event);
extern bool OpenProjectFilePrompt(char* const filepath);

}
