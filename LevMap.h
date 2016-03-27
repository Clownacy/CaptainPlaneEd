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
    LevMap(const uint8_t xSize, const uint8_t ySize, Graphics* const GfxStuff);
    void LoadMap(const char* const filename);
    void SaveMap(const char* const filename);
    void DrawMap();
    void DrawMapSection(const int xStart, const int yStart, const int xSize, const int ySize);
    void DrawCurrentTile();
    void RefreshTile(const uint8_t x, const uint8_t y, const bool curFlag);
    void RefreshTileScreen(const int x, const int y, const bool curFlag);
    void DrawSelectedTile(const int x, const int y);
    void SelectTile(const int ID);
    void SelectedTileIncrID();
    void SelectedTileDecrID();
    void SetCurrentTile(const int ID);
    void SelectTile(const uint8_t x, const uint8_t y);
    void SelectTileCur();
    void SetTile(const uint8_t x, const uint8_t y);
    void SetTileSelected(); //Selected tile to current position
    void ClearCurrentTile();
    void SetPalCurrent(const uint8_t palette);
    void SetPalSelectedTile(const uint8_t palette);
    void CheckSetTile(const int x, const int y);
    void CheckSelectTile(const int x, const int y);
    void CheckClickTile(const int x, const int y);
    bool CheckValidPos(const int x, const int y);
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
