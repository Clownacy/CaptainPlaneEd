#pragma once

#include "Graphics.h"

class LevMap
{
private:
    Graphics* GfxStuff;
    uint8_t xSize;
    uint8_t ySize;
    Tile** MapData;
    short int CurX, CurY;
    Tile SelectedTile;   //data that will overwrite a tile upon click
    
    friend class SelRect;
public:
    LevMap(uint8_t xSize, uint8_t ySize, Graphics* GfxStuff);
    void LoadMap(const char* filename);
    void SaveMap(const char* filename);
    void DrawMap();
    void DrawMapSection(int xStart, int yStart, int xSize, int ySize);
    void DrawCurrentTile();
    void RefreshTile(uint8_t x, uint8_t y, bool curFlag);
    void RefreshTileScreen(int x, int y, bool curFlag);
    void DrawSelectedTile(int x, int y);
    void SelectTile(int ID);
    void SelectedTileIncrID();
    void SelectedTileDecrID();
    void SetCurrentTile(int ID);
    void SelectTile(uint8_t x, uint8_t y);
    void SelectTileCur();
    void SetTile(uint8_t x, uint8_t y);
    void SetTileSelected(); //Selected tile to current position
    void ClearCurrentTile();
    void SetPalCurrent(uint8_t palette);
    void SetPalSelectedTile(uint8_t palette);
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
    uint8_t GetXSize() {return xSize;}
    uint8_t GetYSize() {return ySize;}
};
