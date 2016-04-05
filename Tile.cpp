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

Tile::Tile(const Tile* const tile)
{
	this->tileID = tile->tileID;
	this->paletteLine = tile->paletteLine;
	this->xFlip = tile->xFlip;
	this->yFlip = tile->yFlip;
	this->priority = tile->priority;
}

void Tile::ReadTile(FILE* file)
{
	uint16_t tile = (fgetc(file)<<8)|fgetc(file);
	LoadFromRawTile(tile);
}

void Tile::WriteTile(FILE* file)
{
	uint16_t tile = GetRawTile();
	fputc(tile>>8, file);
	fputc(tile, file);
}

void Tile::LoadFromRawTile(const uint16_t tile)
{
	this->priority = (tile & 0x8000) >> 15;
	this->paletteLine = (tile & 0x6000) >> 13;
	this->yFlip = (tile & 0x1000) >> 12;
	this->xFlip = (tile & 0x0800) >> 11;
	this->tileID = tile & 0x07FF;
}

uint16_t Tile::GetRawTile(void)
{
	return ((this->priority << 15)
	      | (this->paletteLine << 13)
	      | (this->yFlip << 12)
	      | (this->xFlip << 11)
	      |  this->tileID);
}

void Tile::ClearTile(void)
{
	this->tileID = 0;
	this->paletteLine = 0;
	this->xFlip = 0;
	this->yFlip = 0;
	this->priority = 0;
}

void Tile::SetPal(const uint8_t paletteLine)
{
	this->paletteLine = paletteLine;
}

void Tile::SetID(const int ID)
{
	this->tileID = ID;
	this->xFlip = 0;
	this->yFlip = 0;
	this->priority = 0;
}

void Tile::FlipX(void)
{
	this->xFlip ^= 1;
}

void Tile::FlipY(void)
{
	this->yFlip ^= 1;
}

void Tile::SwapPriority(void)
{
	this->priority ^= 1;
}
