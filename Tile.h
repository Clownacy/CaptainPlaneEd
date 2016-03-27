#pragma once

#include <cstdint>
#include <cstdio>

class Tile
{
public: 
    uint8_t priority : 1;
    uint8_t paletteLine : 2;
    uint8_t xFlip : 1;
    uint8_t yFlip : 1;
    uint16_t tileID;

    Tile(void);
    Tile(const Tile& tile);
    void ReadTile(FILE* file);
    void WriteTile(FILE* file);
    void ClearTile(void);
    void SetPal(uint8_t paletteLine);
    void FlipX(void);
    void FlipY(void);
    void SwapPriority(void);
    void SetID(int ID);

protected:
    void LoadFromRawTile(uint16_t);
    uint16_t GetRawTile(void);
};
