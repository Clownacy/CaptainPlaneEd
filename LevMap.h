#ifndef LEVMAP_H
#define LEVMAP_H

#include "Graphics.h"

class LevMap
{
private:
    Graphics* GfxStuff;
    Uint8 xSize;
    Uint8 ySize;
    Tile** MapData;
    short int CurX, CurY;
    Tile SelectedTile;   //data that will overwrite a tile upon click
    
    friend class SelRect;
public:
    LevMap(Uint8 xSize, Uint8 ySize, Graphics* GfxStuff);
    void LoadMap(char* filename);
    void SaveMap(char* filename);
    void DrawMap();
    void DrawMapSection(int xStart, int yStart, int xSize, int ySize);
    void DrawCurrentTile();
    void RefreshTile(Uint8 x, Uint8 y, bool curFlag);
    void RefreshTileScreen(int x, int y, bool curFlag);
    void DrawSelectedTile(int x, int y);
    void SelectTile(int ID);
    void SelectedTileIncrID();
    void SelectedTileDecrID();
    void SetCurrentTile(int ID);
    void SelectTile(Uint8 x, Uint8 y);
    void SelectTileCur();
    void SetTile(Uint8 x, Uint8 y);
    void SetTileSelected(); //Selected tile to current position
    void ClearCurrentTile();
    void SetPalCurrent(Uint8 palette);
    void SetPalSelectedTile(Uint8 palette);
    void CheckSetTile(int x, int y);
    void CheckSelectTile(int x, int y);
    void CheckClickTile(int x, int y);
    bool CheckValidPos(int x, int y);
    void CurShiftRight();
    void CurShiftLeft();
    void CurShiftDown();
    void CurShiftUp();
    void CurFlipX() {MapData[CurY][CurX].FlipX(); DrawCurrentTile();}
    void CurFlipY() {MapData[CurY][CurX].FlipY(); DrawCurrentTile();}
    void CurSwapPriority() {MapData[CurY][CurX].SwapPriority(); DrawCurrentTile();}
    Uint8 GetXSize() {return xSize;}
    Uint8 GetYSize() {return ySize;}
};

#endif
