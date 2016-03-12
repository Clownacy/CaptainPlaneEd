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
    SelRect(const SelRect& sr);
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
