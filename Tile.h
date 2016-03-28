#pragma once

#include <cstdint>
#include <cstdio>

class Tile
{
public: 
    bool priority : 1;
    uint8_t paletteLine : 2;
    bool xFlip : 1;
    bool yFlip : 1;
    uint16_t tileID;

    Tile(void);
    Tile(const Tile* const tile);
    void ReadTile(FILE* file);
    void WriteTile(FILE* file);
    void ClearTile(void);
    void SetPal(const uint8_t paletteLine);
    void SetID(const int ID);
    void FlipX(void);
    void FlipY(void);
    void SwapPriority(void);

protected:
    void LoadFromRawTile(const uint16_t);
    uint16_t GetRawTile(void);
};
