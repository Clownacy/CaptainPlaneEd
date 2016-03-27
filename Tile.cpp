#include <cstdint>
#include <cstdio>

#include "Tile.h"

Tile::Tile(void)
{
	this->tileID = 0;
	this->paletteLine = 0;
	this->xFlip = 0;
	this->yFlip = 0;
	this->priority = 0;
}

Tile::Tile(const Tile& tile)
{
	this->tileID = tile.tileID;
	this->paletteLine = tile.paletteLine;
	this->xFlip = tile.xFlip;
	this->yFlip = tile.yFlip;
	this->priority = tile.priority;
}

void Tile::ReadTile(FILE* file)
{
	uint8_t byte = fgetc(file);
	this->tileID = fgetc(file);
	this->paletteLine = (byte & 0x60) >> 5;
	this->xFlip = byte & 0x08;
	this->yFlip = byte & 0x10;
	this->priority = byte & 0x80;
	this->tileID += (byte & 0x07)<<8;
}

void Tile::WriteTile(FILE* file)
{
	uint16_t tile = GetRawTile();
	fputc(tile>>8, file);
	fputc(tile&0xFF, file);
}

void Tile::ClearTile(void)
{
	this->tileID = 0;
	this->paletteLine = 0;
	this->xFlip = 0;
	this->yFlip = 0;
	this->priority = 0;
}

uint16_t Tile::GetRawTile(void)
{
	return ((this->priority << 8)
	      | (this->xFlip << 8)
	      | (this->yFlip << 8)
	      | (this->paletteLine << 13)
	      |  this->tileID);
}

void Tile::SetPal(uint8_t paletteLine)
{
	this->paletteLine = paletteLine;
}

void Tile::FlipX(void)
{
	this->xFlip ^= 0x08;
}

void Tile::FlipY(void)
{
	this->yFlip ^= 0x10;
}

void Tile::SwapPriority(void)
{
	this->priority ^= 0x80;
}

void Tile::SetID(int ID)
{
	this->tileID = ID;
	this->xFlip = 0;
	this->yFlip = 0;
	this->priority = 0;
}
