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
    Graphics(const uint16_t xSize, const uint16_t tileOffset, const uint16_t tileAmount);
    void ReadPalette(const char* const filename);
    void ReadTiles(const char* const filename);
    void CreateTiles(void);
    SDL_Surface* InitSurface(uint16_t* const pixelsT, int const width, const int height, const int bbp);
    void DrawSurface(SDL_Surface* const img, SDL_Surface* const screen, const int x, const int y);
    void ClearMap(void);
    void ClearSelector(void);
    void DrawSelector(void);
    bool CheckSelValidPos(const int x, const int y);
    void DrawTileSingle(const int x, const int y, const Tile tile);
    void DrawTileBlank(const int x, const int y, const Tile tile);
    void DrawTileFullColor(const int x, const int y, const uint32_t color);
    void DrawTileNone(const int x, const int y);
    void DrawTileInvalid(const int x, const int y);
    void ProcessDisplay(void);
    void SetCurrentPal(const uint8_t currentPal) {this->currentPal = currentPal;}
    uint8_t GetCurrentPal(void) {return currentPal;}
    uint8_t GetPaletteLines(void) {return paletteLines;}
    uint8_t GetSelectorWidth(void) {return selectorWidth;}
    uint16_t GetTileAmount(void) {return tileAmount;}
    uint16_t GetTileOffset(void) {return tileOffset;}
    void ToggleHighPriority(void) {highPriorityDisplay = !highPriorityDisplay;}
    void ToggleLowPriority(void) {lowPriorityDisplay = !lowPriorityDisplay;}
    void DrawPixel(const int x, const int y);
    void DrawRect(int x, int y); //x, y = tile coordinates (not pixel)
    void DrawFreeRect(int x, int y, const int xSize, const int ySize);
    void IncScreenOffset(void) {++screenTileYOffset;}
    void DecScreenOffset(void) {if (screenTileYOffset>0) --screenTileYOffset;}
    int GetScreenOffset(void) {return screenTileYOffset;}
    void IncScreenXOffset(void) {++screenTileXOffset;}
    void DecScreenXOffset(void) {if (screenTileXOffset>0) --screenTileXOffset;}
    int GetScreenXOffset(void) {return screenTileXOffset;}
    void IncSelOffset(void) {++selTileYOffset;}
    void DecSelOffset(void) {if (selTileYOffset>0) --selTileYOffset;}
    int GetSelOffset(void) {return selTileYOffset;}
    void PosScreenToTile(int* const x, int* const y);
    void PosScreenToTileRound(int* const x, int* const y);
    void PosTileToScreen(int* const x, int* const y);
};
