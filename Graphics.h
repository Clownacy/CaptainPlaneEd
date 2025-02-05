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

#include <array>
#include <istream>
#include <vector>
#include <stddef.h>
#include <stdint.h>

#include "Screen.h"
#include "Tile.h"

class Graphics
{
private:
    static constexpr unsigned int PALETTE_ENTRIES_PER_LINE = 16;

    std::vector<std::array<uint16_t, PALETTE_ENTRIES_PER_LINE>> palette;
    SDL::Texture tile_atlas;
    size_t atlas_quadrant_dimension;
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
    void ReadPalette(std::istream &stream, long buffer_size);
    void ReadTiles(std::istream &stream);
    void DrawTileFromAtlas(int tile_index, int x, int y, int palette_line, bool x_flip, bool y_flip);
    void ClearMap(void);
    void ClearSelector(void);
    void DrawSelector(void);
    bool CheckSelValidPos(int x, int y);
    void DrawTileSingle(int x, int y, const Tile *tile);
    void DrawTileBlank(int x, int y, const Tile *tile);
    void DrawTileFullColor(int x, int y, unsigned char red, unsigned char green, unsigned char blue);
    void DrawTileNone(int x, int y);
    void DrawTileInvalid(int x, int y);
    void SetCurrentPal(uint8_t currentPal);
    uint8_t GetCurrentPal(void) {return currentPal;}
    uint8_t GetPaletteLines(void) {return paletteLines;}
    uint8_t GetSelectorWidth(void) {return selectorWidth;}
    uint16_t GetTileAmount(void) {return tileAmount;}
    uint16_t GetTileOffset(void) {return tileOffset;}
    void ToggleHighPriority(void) {highPriorityDisplay = !highPriorityDisplay;}
    void ToggleLowPriority(void) {lowPriorityDisplay = !lowPriorityDisplay;}
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
    void PosScreenToTile(int *x, int *y);
    void PosScreenToTileRound(int *x, int *y);
    void PosTileToScreen(int *x, int *y);
};
