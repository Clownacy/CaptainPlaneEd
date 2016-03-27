#pragma once

#include <cstdint>
#include <cstdio>

class Tile
{
public: 
    uint8_t priority;
    uint8_t paletteLine;
    uint8_t xFlip;
    uint8_t yFlip;
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
    uint16_t GetRawTile(void);
    void LoadFromRawTile(uint16_t);
};
