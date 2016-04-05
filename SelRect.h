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

#include "Graphics.h"
#include "LevMap.h"

class SelRect
{
private:
    static Graphics* GfxStuff;
    static LevMap* LevelMap;
    int xStart;
    int yStart;
    int xSize;
    int ySize;
    Tile*** MapData; //NULL if selection still being created
    
    friend class LevMap;
public:
    SelRect(Graphics* GfxStuff, LevMap* LevelMap);
    SelRect(SelRect* const sr);
    ~SelRect();
    bool isActive();
/*    uint8_t SetXStart(int xs) {xStart = xs;}
    uint8_t SetYStart(int ys) {yStart = ys}
    uint8_t GetXSize() {return xSize;}
    uint8_t GetYSize() {return ySize;}*/
    void AssignSection();
    void PasteSection();
    void TakeSection();
    void SelInit(int x, int y);
    void SelFinalize(int x, int y);
    void Kill();
    void SelDrawRect();
    void SelClearRect();
    void Unselect();
    void CheckRedraw(int x, int y); //map coords
    void clear();
    void FlipX();
    void FlipY();
    void SwapPriority();
    void IncrID();
    void DecrID();
private:
    void AdaptBounds();
    void AdaptStartBounds();
};
