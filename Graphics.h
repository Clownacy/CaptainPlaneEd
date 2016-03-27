#pragma once

#include <SDL2/SDL.h>

#include "Tile.h"

class Graphics
{
private:
    SDL_Window* window;
    SDL_Renderer* render;
    SDL_Texture* texture;
    SDL_Surface* screen;
    uint16_t (*palette)[16];
    SDL_Surface**** tiles;    //surface data: [tile][pal][flip]
    uint16_t**** tileData;   //pixel data: [tile][pal][flip][64]
    uint8_t paletteLines;
    uint8_t currentPal;
    uint8_t selectorWidth;
    uint16_t tileAmount;
    uint16_t selXMin;        //x position of left boundary of selector map
    uint16_t xDisplaySize;        //x position of left boundary of selector map
    uint16_t tileOffset;    //amount the non-empty tiles' ID will be offset
    uint16_t screenTileYOffset;
    uint16_t selTileYOffset;
    uint16_t screenTileXOffset;
    bool highPriorityDisplay;
    bool lowPriorityDisplay;
public:
    Graphics(uint16_t xSize, uint16_t tileOffset, uint16_t tileAmount);
    void ReadPalette(const char* const filename);
    void ReadTiles(const char* const filename);
    void CreateTiles(void);
    SDL_Surface* InitSurface(uint16_t* pixelsT, int width, int height, int bbp);
    void DrawSurface(SDL_Surface* img, SDL_Surface* screen, int x, int y);
    void ClearMap(void);
    void ClearSelector(void);
    void DrawSelector(void);
    bool CheckSelValidPos(int x, int y);
    void DrawTileSingle(int x, int y, Tile tile);
    void DrawTileBlank(int x, int y, Tile tile);
    void DrawTileFullColor(int x, int y, uint32_t color);
    void DrawTileNone(int x, int y);
    void DrawTileInvalid(int x, int y);
    void ProcessDisplay(void);
    void SetCurrentPal(uint8_t currentPal) {this->currentPal = currentPal;}
    uint8_t GetCurrentPal(void) {return currentPal;}
    uint8_t GetPaletteLines(void) {return paletteLines;}
    uint8_t GetSelectorWidth(void) {return selectorWidth;}
    uint16_t GetTileAmount(void) {return tileAmount;}
    uint16_t GetTileOffset(void) {return tileOffset;}
    void ToggleHighPriority(void) {highPriorityDisplay = !highPriorityDisplay;}
    void ToggleLowPriority(void) {lowPriorityDisplay = !lowPriorityDisplay;}
    void DrawPixel(int x, int y);
    void DrawRect(int x, int y); //x, y = tile coordinates (not pixel)
    void DrawFreeRect(int x, int y, int xSize, int ySize);
    void IncScreenOffset(void) {++screenTileYOffset;}
    void DecScreenOffset(void) {if (screenTileYOffset>0) --screenTileYOffset;}
    int GetScreenOffset(void) {return screenTileYOffset;}
    void IncScreenXOffset(void) {++screenTileXOffset;}
    void DecScreenXOffset(void) {if (screenTileXOffset>0) --screenTileXOffset;}
    int GetScreenXOffset(void) {return screenTileXOffset;}
    void IncSelOffset(void) {++selTileYOffset;}
    void DecSelOffset(void) {if (selTileYOffset>0) --selTileYOffset;}
    int GetSelOffset(void) {return selTileYOffset;}
    void PosScreenToTile(int* x, int* y);
    void PosScreenToTileRound(int* x, int* y);
    void PosTileToScreen(int* x, int* y);
};
