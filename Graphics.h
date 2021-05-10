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

#pragma once

#include <cstddef>
#include <cstdint>
#include <SDL2/SDL.h>

#include "Screen.h"
#include "Tile.h"

class Graphics
{
private:
    uint16_t (*palette)[16];
    SDL_Texture* tile_atlas;
    std::size_t atlas_quadrant_dimension;
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
    ~Graphics(void);
    void ReadPalette(const char* const filename);
    void ReadTiles(const char* const filename);
    void DrawTileFromAtlas(const int tile_index, SDL_Texture* const screen, const int x, const int y, const int palette_line, bool x_flip, bool y_flip);
    void ClearMap(void);
    void ClearSelector(void);
    void DrawSelector(void);
    bool CheckSelValidPos(const int x, const int y);
    void DrawTileSingle(const int x, const int y, const Tile* const tile);
    void DrawTileBlank(const int x, const int y, const Tile* const tile);
    void DrawTileFullColor(const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue);
    void DrawTileNone(const int x, const int y);
    void DrawTileInvalid(const int x, const int y);
    void SetCurrentPal(const uint8_t currentPal);
    uint8_t GetCurrentPal(void) {return currentPal;}
    uint8_t GetPaletteLines(void) {return paletteLines;}
    uint8_t GetSelectorWidth(void) {return selectorWidth;}
    uint16_t GetTileAmount(void) {return tileAmount;}
    uint16_t GetTileOffset(void) {return tileOffset;}
    void ToggleHighPriority(void) {highPriorityDisplay = !highPriorityDisplay;}
    void ToggleLowPriority(void) {lowPriorityDisplay = !lowPriorityDisplay;}
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
