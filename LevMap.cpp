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

#include "LevMap.h"

#include <fstream>
#include <stdint.h>

#include "Common.h"
#include "Graphics.h"

LevMap::LevMap(uint8_t xSize, uint8_t ySize, Graphics &GfxStuff)
	: GfxStuff(GfxStuff)
{
	if (xSize == 0 || ySize == 0)
		MainScreen.ShowError("Invalid map size: must be at least 1x1");
	
	this->xSize=xSize;
	this->ySize=ySize;
	
	MapData.resize(ySize);
	for (int i=0; i < ySize; ++i)
		MapData[i].resize(xSize);

	CurY = 0; 
	CurX = 0;
	this->SelectedTile = MapData[CurY][CurX];
}

void LevMap::LoadMap(std::istream &stream)
{
	for (int y = 0; y < ySize; ++y)
		for (int x = 0; x < xSize; ++x)
			this->MapData[y][x].ReadTile(stream);
}

void LevMap::SaveMap(std::ostream &stream)
{
	for (int y = 0; y < ySize; ++y)
		for (int x = 0; x < xSize; ++x)
			MapData[y][x].WriteTile(stream);
}

void LevMap::DrawMap(void)
{
	GfxStuff.ClearMap();
	for (int x = 0; x < xSize; ++x)
		for (int y = 0; y < ySize; ++y)
			GfxStuff.DrawTileSingle(x, y, &MapData[y][x]);
}

void LevMap::DrawMapSection(int xStart, int yStart, int xSize, int ySize)
{
	for (int x = 0; x < xSize; ++x)
		for (int y = 0; y < ySize; ++y)
			GfxStuff.DrawTileSingle(xStart + x, yStart + y, &MapData[yStart + y][xStart + x]);
}

void LevMap::DrawCurrentTile(void)
{
	GfxStuff.DrawTileSingle(CurX, CurY, &MapData[CurY][CurX]);
	GfxStuff.DrawRect(CurX, CurY);
}

void LevMap::SelectTile(uint8_t x, uint8_t y)
{
	if (CheckValidPos(x, y))
	{
		SelectedTile = MapData[y][x];
		RefreshTile(CurX, CurY, false);
		CurX = x; CurY = y;
	}
}

void LevMap::SelectTile(int ID)
{
	SelectedTile.ClearTile();
	SelectedTile.SetID(ID);
	SelectedTile.SetPal(GfxStuff.GetCurrentPal());
}

void LevMap::SelectedTileIncrID(void)
{
	if (SelectedTile.tileID == 0 && GfxStuff.GetTileOffset() != 0)
		SelectedTile.tileID = GfxStuff.GetTileOffset();
	else if (SelectedTile.tileID + 1 < GfxStuff.GetTileAmount() + GfxStuff.GetTileOffset())
		++SelectedTile.tileID;
}

void LevMap::SelectedTileDecrID(void)
{
	if (SelectedTile.tileID > GfxStuff.GetTileOffset())
		--SelectedTile.tileID;
	else
		SelectedTile.tileID = 0;
}

/* map coords */
void LevMap::SetTile(uint8_t x, uint8_t y)
{
	if (CheckValidPos(x, y))
	{
		MapData[y][x] = SelectedTile;
		RefreshTile(CurX, CurY, false);
	}

	DrawCurrentTile();
}

void LevMap::SetTileSelected(void)
{
	MapData[CurY][CurX] = SelectedTile;
	RefreshTile(CurX, CurY, false);
    DrawCurrentTile();
}

void LevMap::SelectTileCur(void)
{
	SelectedTile = MapData[CurY][CurX];
}

void LevMap::ClearCurrentTile(void)
{
	MapData[CurY][CurX].ClearTile();
	DrawCurrentTile();
}

void LevMap::SetPalCurrent(const uint8_t palette)
{
	if (palette < GfxStuff.GetPaletteLines())
	{
		SelectedTile.SetPal(palette);
		MapData[CurY][CurX].SetPal(palette);
		GfxStuff.SetCurrentPal(palette);
		DrawCurrentTile();
	}
}

/* mouse coords */
void LevMap::CheckSetTile(int x, int y)
{
	GfxStuff.PosScreenToTile(&x, &y);
	SetTile(x, y);
}

/* mouse coords */
void LevMap::CheckSelectTile(int x, int y)
{
	GfxStuff.PosScreenToTile(&x, &y);
	SelectTile(x, y);
}

/* if curFlag is set and the tile is CurrentTile, it will be re-drawn with boundary
 * Map position as parameter */
void LevMap::RefreshTile(uint8_t x, uint8_t y, bool curFlag)
{
	if (CheckValidPos(x, y))
	{
		if (x == CurX && y == CurY && curFlag)
			DrawCurrentTile();
		else
			GfxStuff.DrawTileSingle(x, y, &MapData[y][x]);
	}
}

/* if curFlag is set and the tile is CurrentTile, it will be re-drawn with boundary
 * Mouse position as parameter */
void LevMap::RefreshTileScreen(int x, int y, bool curFlag)
{
	GfxStuff.PosScreenToTile(&x, &y);

	if (CheckValidPos(x, y))
	{
		if (x == CurX && y == CurY && curFlag)
			DrawCurrentTile();
		else
			GfxStuff.DrawTileSingle(x, y, &MapData[y][x]);
	}
}

/* mouse coords */
void LevMap::DrawSelectedTile(int x, int y)
{
	GfxStuff.PosScreenToTile(&x, &y);
	if (CheckValidPos(x, y))
		GfxStuff.DrawTileSingle(x, y, &SelectedTile);
}

void LevMap::CheckClickTile(int x, int y)
{
	//within bounds of tile selector?
	if (GfxStuff.CheckSelValidPos(x, y))
	{
		x /= 8;
		y /= 8;
		x -= xSize+1;
		SelectTile(x+GfxStuff.GetSelectorWidth()*(y + GfxStuff.GetSelOffset()) + GfxStuff.GetTileOffset()+ 1);
	}
}

/* map coords */
bool LevMap::CheckValidPos(int x, int y)
{
	if (y<ySize && x<xSize && x>=0 && y>=0)
		return true;
	else
		return false;
}

void LevMap::SetCurrentTile(int ID)
{
	if (ID < GfxStuff.GetTileAmount() && ID >= 0)
	{
		MapData[CurY][CurX].ClearTile();
		MapData[CurY][CurX].SetID(ID);
		MapData[CurY][CurX].SetPal(GfxStuff.GetCurrentPal());
		DrawCurrentTile();
	}
}

void LevMap::CurShiftRight(void)
{
	RefreshTile(CurX, CurY, false);

	if (CurX < xSize - 1 || CurY < ySize - 1)
	{
		CurY += ++CurX / xSize;
		CurX = CurX % xSize;
	}

	DrawCurrentTile();
}

void LevMap::CurShiftLeft(void)
{
	RefreshTile(CurX, CurY, false);

	if (CurX > 0 || CurY > 0)
	{
		if (--CurX < 0)
		{
			--CurY;
			CurX += xSize;
		}
	}

	DrawCurrentTile();
}

void LevMap::CurShiftDown(void)
{
	RefreshTile(CurX, CurY, false);

	if (CurY < ySize-1)
		++CurY;

	DrawCurrentTile();
}

void LevMap::CurShiftUp(void)
{
	RefreshTile(CurX, CurY, false);

	if (CurY > 0)
		--CurY;

	DrawCurrentTile();
}
