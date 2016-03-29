#pragma once

#include <SDL2/SDL.h>

extern void CreateMenuBar(SDL_Window* window);
extern void HandleWindowsEvent(SDL_Event* event);
extern bool OpenProjectFilePrompt(char* filepath);
