#ifndef TILE_H
#define TILE_H

class Tile
{
public: 
    Uint8 priority;
    Uint8 paletteLine;
    Uint8 xFlip;
    Uint8 yFlip;
    Uint16 tileID;
public:
    Tile();
    Tile(const Tile& tile);
    void ReadTile(FILE* file);
    void WriteTile(FILE* file);
    void ClearTile();
    void SetPal(Uint8 paletteLine);
    void FlipX() {this->xFlip ^= 0x08;}
    void FlipY() {this->yFlip ^= 0x10;}
    void SwapPriority() {this->priority ^= 0x80;}
    void SetID(int ID);
};

#endif
