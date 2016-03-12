#include <cstdint>
#include <cstdio>

#include "Tile.h"

Tile::Tile()
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
	fputc((char) (this->priority
		    | this->xFlip
		    | this->yFlip
		   | (this->paletteLine << 5)
		   | (this->tileID >> 8)), file);
	fputc(this->tileID & 0xFF, file);
}

void Tile::ClearTile()
{
	this->tileID = 0;
	this->paletteLine = 0;
	this->xFlip = 0;
	this->yFlip = 0;
	this->priority = 0;
}

void Tile::SetPal(uint8_t paletteLine)
{
	this->paletteLine = paletteLine;
}

void Tile::SetID(int ID)
{
	this->tileID = ID;
	this->xFlip = 0;
	this->yFlip = 0;
	this->priority = 0;
}
