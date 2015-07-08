#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL/SDL.h>

class Graphics
{
private:
    SDL_Surface* screen;
    Uint16 (*palette)[16];
    SDL_Surface**** tiles;    //surface data: [tile][pal][flip]
    Uint16**** tileData;   //pixel data: [tile][pal][flip][64]
    Uint8 paletteLines;
    Uint8 currentPal;
    Uint8 selectorWidth;
    Uint16 tileAmount;
    Uint16 selXMin;        //x position of left boundary of selector map
    Uint16 xDisplaySize;        //x position of left boundary of selector map
    Uint16 tileOffset;    //amount the non-empty tiles' ID will be offset
    Uint16 screenTileYOffset;
    Uint16 selTileYOffset;
    Uint16 screenTileXOffset;
    bool highPriorityDisplay;
    bool lowPriorityDisplay;
public:
    Graphics(Uint16 xSize, Uint16 tileOffset, Uint16 tileAmount);
    void ReadPalette(char* filename);
    void ReadTiles(char* filename);
    void CreateTiles();
    SDL_Surface* InitSurface(Uint16 *pixelsT, int width, int height, int bbp);
    void DrawSurface(SDL_Surface *img, SDL_Surface *screen, int x, int y);
    void ClearMap();
    void ClearSelector();
    void DrawSelector();
    bool CheckSelValidPos(int x, int y);
    void DrawTileSingle(int x, int y, Tile tile);
    void DrawTileBlank(int x, int y, Tile tile);
    void DrawTileFullColor(int x, int y, Uint32 color);
    void DrawTileNone(int x, int y);
    void DrawTileInvalid(int x, int y);
    void ProcessDisplay() {SDL_Flip(screen);}
    void SetCurrentPal(Uint8 currentPal) {this->currentPal = currentPal;}
    Uint8 GetCurrentPal() {return currentPal;}
    Uint8 GetPaletteLines() {return paletteLines;}
    Uint8 GetSelectorWidth() {return selectorWidth;}
    Uint16 GetTileAmount() {return tileAmount;}
    Uint16 GetTileOffset() {return tileOffset;}
    void ToggleHighPriority() {highPriorityDisplay = !highPriorityDisplay;}
    void ToggleLowPriority() {lowPriorityDisplay = !lowPriorityDisplay;}
    void DrawPixel(int x, int y);
    void DrawRect(int x, int y); //x, y = tile coordinates (not pixel)
    void DrawFreeRect(int x, int y, int xSize, int ySize);
    void IncScreenOffset() {screenTileYOffset++;}
    void DecScreenOffset() {if(screenTileYOffset>0) screenTileYOffset--;}
    int GetScreenOffset() {return screenTileYOffset;}
    void IncScreenXOffset() {screenTileXOffset++;}
    void DecScreenXOffset() {if(screenTileXOffset>0) screenTileXOffset--;}
    int GetScreenXOffset() {return screenTileXOffset;}
    void IncSelOffset() {selTileYOffset++;}
    void DecSelOffset() {if(selTileYOffset>0) selTileYOffset--;}
    int GetSelOffset() {return selTileYOffset;}
    void PosScreenToTile(int& x, int& y);
    void PosScreenToTileRound(int& x, int& y);
    void PosTileToScreen(int& x, int& y);
};

#endif
