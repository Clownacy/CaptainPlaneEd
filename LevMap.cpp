/*
    Copyright (C) 2009-2011 qiuu
    Copyright (C) 2016 Clownacy

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

#include "Common.h"
#include "Graphics.h"
#include "LevMap.h"

LevMap::LevMap(const uint8_t xSize, const uint8_t ySize, Graphics* const GfxStuff) {
    if (xSize == 0 || ySize == 0)
        MainScreen->ShowError("Invalid map size: must be at least 1x1");
	
    this->xSize=xSize;
    this->ySize=ySize;
	
    MapData = new Tile*[ySize];
    for (int i=0; i < ySize; ++i)
        MapData[i] = new Tile[xSize];

    CurY = 0; 
    CurX = 0;
    this->SelectedTile = MapData[CurY][CurX];
    this->GfxStuff = GfxStuff;
}

void LevMap::LoadMap(const char* const filename) {
    FILE* mapfile = fopen(filename, "rb");
    if (mapfile == NULL)
        MainScreen->ShowInternalError("Decompressed map file not found");

    for (int y=0; y < ySize; ++y)
        for (int x=0; x < xSize; ++x)
            this->MapData[y][x].ReadTile(mapfile);
    fclose(mapfile);
    remove(filename);
}

void LevMap::SaveMap(const char* const filename) {
    FILE* mapfile = fopen(filename, "wb");
    if (mapfile == NULL)
        MainScreen->ShowInternalError("Unable to create map file for saving");

    for (int y=0; y < ySize; ++y)
        for (int x=0; x < xSize; ++x)
            MapData[y][x].WriteTile(mapfile);
    fclose(mapfile);
}

void LevMap::DrawMap(void) {
    GfxStuff->ClearMap();
    for (int x=0; x < xSize; ++x)
        for (int y=0; y < ySize; ++y)
            GfxStuff->DrawTileSingle(x, y, &MapData[y][x]);
}

void LevMap::DrawMapSection(const int xStart, const int yStart, const int xSize, const int ySize) {
    for (int x=0; x < xSize; ++x)
        for (int y=0; y < ySize; ++y)
            GfxStuff->DrawTileSingle(xStart+x, yStart+y, &MapData[yStart+y][xStart+x]);
}

void LevMap::DrawCurrentTile(void) {
    GfxStuff->DrawTileSingle(CurX, CurY, &MapData[CurY][CurX]);
    GfxStuff->DrawRect(CurX, CurY);
}

void LevMap::SelectTile(const uint8_t x, const uint8_t y) {
    if (CheckValidPos(x, y)) {
        SelectedTile = MapData[y][x];
        RefreshTile(CurX, CurY, false);
        CurX = x; CurY = y;
    }
}

void LevMap::SelectTile(const int ID) {
    SelectedTile.ClearTile();
    SelectedTile.SetID(ID);
    SelectedTile.SetPal(GfxStuff->GetCurrentPal());
}

void LevMap::SelectedTileIncrID(void) {
    if (SelectedTile.tileID == 0 && GfxStuff->GetTileOffset() != 0) SelectedTile.tileID = GfxStuff->GetTileOffset();
    else if (SelectedTile.tileID + 1 < GfxStuff->GetTileAmount() + GfxStuff->GetTileOffset())
        ++SelectedTile.tileID;
}

void LevMap::SelectedTileDecrID(void) {
    if (SelectedTile.tileID > GfxStuff->GetTileOffset()) --SelectedTile.tileID;
    else SelectedTile.tileID = 0;
}

/* map coords */
void LevMap::SetTile(const uint8_t x, const uint8_t y) {
    if (CheckValidPos(x, y)) {
        MapData[y][x] = SelectedTile;
        RefreshTile(CurX, CurY, false);
        CurX = x; CurY = y;
    }
    DrawCurrentTile();
}

void LevMap::SetTileSelected(void) {
    MapData[CurY][CurX] = SelectedTile;
    RefreshTile(CurX, CurY, false);
    DrawCurrentTile();
}

void LevMap::SelectTileCur(void) {
    SelectedTile = MapData[CurY][CurX];
}

void LevMap::ClearCurrentTile(void) {
    MapData[CurY][CurX].ClearTile();
    DrawCurrentTile();
}

void LevMap::SetPalCurrent(const uint8_t palette) {
    if (palette < GfxStuff->GetPaletteLines()) {
        SelectedTile.SetPal(palette);
        MapData[CurY][CurX].SetPal(palette);
        GfxStuff->SetCurrentPal(palette);
        DrawCurrentTile();
    }
}

/* mouse coords */
void LevMap::CheckSetTile(int x, int y) {
    GfxStuff->PosScreenToTile(&x, &y);
    SetTile(x, y);
}

/* mouse coords */
void LevMap::CheckSelectTile(int x, int y) {
    GfxStuff->PosScreenToTile(&x, &y);
    SelectTile(x, y);
}

/* if curFlag is set and the tile is CurrentTile, it will be re-drawn with boundary
 * Map position as parameter */
void LevMap::RefreshTile(const uint8_t x, const uint8_t y, const bool curFlag) {
    if (CheckValidPos(x, y)) {
        if (x == CurX && y == CurY && curFlag) DrawCurrentTile();
        else GfxStuff->DrawTileSingle(x, y, &MapData[y][x]);
    }
}

/* if curFlag is set and the tile is CurrentTile, it will be re-drawn with boundary
 * Mouse position as parameter */
void LevMap::RefreshTileScreen(int x, int y, const bool curFlag) {
    GfxStuff->PosScreenToTile(&x, &y);
    if (CheckValidPos(x, y)) {
        if (x == CurX && y == CurY && curFlag) DrawCurrentTile();
        else GfxStuff->DrawTileSingle(x, y, &MapData[y][x]);
    }
}

/* mouse coords */
void LevMap::DrawSelectedTile(int x, int y) {
    GfxStuff->PosScreenToTile(&x, &y);
    if (CheckValidPos(x, y))
        GfxStuff->DrawTileSingle(x, y, &SelectedTile);
}

void LevMap::CheckClickTile(int x, int y) {
    //within bounds of tile selector?
    if (GfxStuff->CheckSelValidPos(x, y)) {
        x /= 8; y /= 8;
        x -= xSize+1;
        SelectTile(x+GfxStuff->GetSelectorWidth()*(y + GfxStuff->GetSelOffset()) + GfxStuff->GetTileOffset()+ 1);
    }
}

/* map coords */
bool LevMap::CheckValidPos(const int x, const int y) {
    if (y<ySize && x<xSize && x>=0 && y>=0) return true;
    else return false;
}

void LevMap::SetCurrentTile(const int ID) {
    if (ID < GfxStuff->GetTileAmount() && ID >= 0) {
        MapData[CurY][CurX].ClearTile();
        MapData[CurY][CurX].SetID(ID);
        MapData[CurY][CurX].SetPal(GfxStuff->GetCurrentPal());
        DrawCurrentTile();
    }
}

void LevMap::CurShiftRight(void) {
    RefreshTile(CurX, CurY, false);
    if (CurX < xSize - 1 || CurY < ySize - 1) {
        CurY += ++CurX / xSize;
        CurX = CurX % xSize;
    }
    DrawCurrentTile();
}

void LevMap::CurShiftLeft(void) {
    RefreshTile(CurX, CurY, false);    
    if (CurX > 0 || CurY > 0) {
        if (--CurX < 0) { 
            --CurY;
            CurX += xSize;
        }
    }
    DrawCurrentTile();
}

void LevMap::CurShiftDown(void) {
    RefreshTile(CurX, CurY, false);    
    if (CurY < ySize-1) ++CurY;
    DrawCurrentTile();
}

void LevMap::CurShiftUp(void) {
    RefreshTile(CurX, CurY, false);    
    if (CurY > 0) --CurY;
    DrawCurrentTile();
}
