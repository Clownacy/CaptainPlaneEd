#pragma once

#include <SDL2/SDL.h>

extern void CreateMenuBar(SDL_Window* const window);
extern void HandleWindowsEvent(const SDL_Event* const event);
extern bool OpenProjectFilePrompt(char* const filepath);
