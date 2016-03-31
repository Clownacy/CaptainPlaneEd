#include <SDL2/SDL.h>

#include "Screen.h"
#include "WinAPI.h"

Screen::Screen(void)
{
    if (SDL_Init(SDL_INIT_VIDEO)<0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL video", SDL_GetError(), NULL);
        exit(1);
    }
    atexit(SDL_Quit);
    
    window = SDL_CreateWindow("Captain PlaneEd", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL Window", SDL_GetError(), NULL);
        exit(1);
    }
    
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (render == NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init SDL Renderer", SDL_GetError(), NULL);
        exit(1);
    }
    
    SDL_RenderSetLogicalSize(render, SCREEN_WIDTH, SCREEN_HEIGHT);

    screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    if (screen==NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init screen SDL Surface", SDL_GetError(), NULL);
        exit(1);
    }
    texture = SDL_CreateTextureFromSurface(render, screen);
    if (texture==NULL)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to init screen SDL Texture", SDL_GetError(), NULL);
        exit(1);
    }

    // Windows-only crap to generate a menu bar
    WinAPI::SaveHWND(window);
    WinAPI::CreateMenuBar();
}

void Screen::ProcessDisplay(void)
{
    SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
    SDL_RenderClear(render);
    SDL_RenderCopy(render, texture, NULL, NULL);
    SDL_RenderPresent(render);
}
