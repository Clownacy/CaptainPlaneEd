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

#include <string.h>

#include "SelRect.h"

Graphics* SelRect::GfxStuff = nullptr;
LevMap* SelRect::LevelMap = nullptr;

/* constructor */
SelRect::SelRect(Graphics *GfxStuff, LevMap *LevelMap)
{
	this->GfxStuff = GfxStuff;
	this->LevelMap = LevelMap;
	this->xStart = 0;
	this->yStart = 0;
	this->xSize = 0;
	this->ySize = 0;
	MapData = nullptr;
}

/* copy constructor */
SelRect::SelRect(const SelRect *sr)
{
	xStart = sr->xStart;
	yStart = sr->yStart;
	xSize = sr->xSize;
	ySize = sr->ySize;
	if (sr->MapData != nullptr)
	{
		MapData = new Tile**[ySize];
		for (int y = 0; y < ySize; ++y) MapData[y] = new Tile*[xSize];
		for (int y = 0; y < ySize; ++y)
		{
			for (int x = 0; x < xSize; ++x)
			{
				MapData[y][x] = new Tile(sr->MapData[y][x]);
			}
		}
	}
	else
	{
		MapData = nullptr;
	}
}

/* destructor */
SelRect::~SelRect(void)
{
	Kill();
}

/* returns true if the selected rectangle contains some data, false otherwise */
bool SelRect::isActive(void)
{
	if (MapData == nullptr)
		return false;
	else
		return true;
}

/* write selected content to map */
void SelRect::AssignSection(void)
{
	if (isActive())
		for (int y = 0; y < ySize; ++y)
			for (int x = 0; x < xSize; ++x)
				if ((x+xStart < LevelMap->xSize) && (y + yStart < LevelMap->ySize))
					LevelMap->MapData[y + yStart][x + xStart] = *MapData[y][x];
}

/* write selected content to current position map */
void SelRect::PasteSection(void)
{
	xStart = LevelMap->CurX;
	yStart = LevelMap->CurY;
	AssignSection();
}

/* reads content within rectangle from map */
void SelRect::TakeSection(void)
{
	Kill();
	MapData = new Tile**[ySize];
	for (int y = 0; y < ySize; ++y) MapData[y] = new Tile*[xSize];
	for (int y = 0; y < ySize; ++y)
	{
		for (int x = 0; x < xSize; ++x)
		{
			if ((x+xStart < LevelMap->xSize) && (y+yStart < LevelMap->ySize))
				MapData[y][x] = new Tile(LevelMap->MapData[y+yStart][x+xStart]);
		}
	}
}

/* Sets the start position of the rectangle
   parameters: the cursor position */
void SelRect::SelInit(int x, int y)
{
	if (MapData != NULL)
		SelClearRect();
	GfxStuff->PosScreenToTileRound(&x, &y);
	xStart = x;
	yStart = y;
	AdaptStartBounds();
}

/* Sets the ending position of the rectangle and grabs data from the map
   parameters: the cursor position */
void SelRect::SelFinalize(int x, int y)
{
	GfxStuff->PosScreenToTileRound(&x, &y);

	if (x < xStart)
	{
		int temp = x;
		x = xStart;
		xStart = temp;
	}

	if (y < yStart)
	{
		int temp = y;
		y = yStart;
		yStart = temp;
	}

	xSize = x - xStart;
	ySize = y - yStart;
	AdaptBounds();

	if (xSize > 0 && ySize > 0)
		TakeSection();
}

/* draws the rectangle on screen */
void SelRect::SelDrawRect(void)
{
	if (isActive())
		GfxStuff->DrawFreeRect(xStart, yStart, xSize, ySize);
}

/* re-draws the section on the map that was previously covered by the selection
   rectangle to remove its graphics */
void SelRect::SelClearRect(void)
{
	// TODO - I guess get rid of this
}

/* unselects the currently selected area */
void SelRect::Unselect(void)
{
	if (isActive()) SelClearRect();
		Kill();
}

/* clears the memory occupied by the mapdata within the object */
void SelRect::Kill(void)
{
	if (MapData != nullptr)
	{
		for (int y = 0; y < ySize; ++y)
			for (int x = 0; x < xSize; ++x)
				delete MapData[y][x];

		for (int y = 0; y < ySize; ++y)
			delete[] MapData[y];

		delete[] MapData;
		MapData = nullptr;
	}
}

/* fills the selected rectangle with empty tiles */
void SelRect::clear(void)
{
	for (int y = 0; y < ySize; ++y)
	{
		for (int x = 0; x < xSize; ++x)
		{
			delete MapData[y][x];
			MapData[y][x] = new Tile;
		}
	}
}

void SelRect::AdaptBounds(void)
{
	if (xStart + xSize > LevelMap->xSize)
		xSize = LevelMap->xSize - xStart;

	if (yStart + ySize > LevelMap->ySize)
		ySize = LevelMap->ySize - yStart;
}

void SelRect::AdaptStartBounds(void)
{
	if (xStart > LevelMap->xSize)
		xStart = LevelMap->xSize - 1;

	if (yStart > LevelMap->ySize)
		yStart = LevelMap->ySize - 1;

	if (xStart < 0)
		xStart = 0;

	if (yStart < 0)
		yStart = 0;
}

/* xFlip selected content */
void SelRect::FlipX(void)
{
	if (isActive())
	{
		for (int y=0; y < ySize; ++y)
		{
			for (int x=0; x < xSize; ++x)
			{
				if ((x + xStart < LevelMap->xSize) && (y + yStart < LevelMap->ySize))
				{
					LevelMap->MapData[y + yStart][x + xStart] = *MapData[y][xSize - x - 1];
					LevelMap->MapData[y + yStart][x + xStart].FlipX();
				}
			}
		}

		TakeSection();
	}
}

/* yFlip selected content */
void SelRect::FlipY(void)
{
	if (isActive())
	{
		for (int y = 0; y < ySize; ++y)
		{
			for (int x = 0; x < xSize; ++x)
			{
				if ((x + xStart < LevelMap->xSize) && (y + yStart < LevelMap->ySize))
				{
					LevelMap->MapData[y + yStart][x + xStart] = *MapData[ySize - y - 1][x];
					LevelMap->MapData[y + yStart][x + xStart].FlipY();
				}
			}
		}

		TakeSection();
	}
}

void SelRect::SwapPriority(void)
{
	if (isActive())
	{
		for (int y = 0; y < ySize; ++y)
			for (int x = 0; x < xSize; ++x)
				if ((x + xStart < LevelMap->xSize) && (y + yStart < LevelMap->ySize))
					MapData[y][x]->SwapPriority();

		AssignSection();
	}
}

void SelRect::IncrID(void)
{
	if (isActive())
	{
		for (int y = 0; y < ySize; ++y)
		{
			for (int x = 0; x < xSize; ++x)
			{
				if ((x + xStart < LevelMap->xSize) && (y + yStart < LevelMap->ySize))
				{
					if (MapData[y][x]->tileID == 0 && GfxStuff->GetTileOffset() != 0)
						MapData[y][x]->tileID = GfxStuff->GetTileOffset();
					else if (MapData[y][x]->tileID + 1 < GfxStuff->GetTileAmount() + GfxStuff->GetTileOffset())
						++MapData[y][x]->tileID;
				}
			}
		}

		AssignSection();
	}
}

void SelRect::DecrID(void)
{
	if (isActive())
	{
		for (int y=0; y < ySize; ++y)
		{
			for (int x=0; x < xSize; ++x)
			{
				if ((x+xStart < LevelMap->xSize) && (y+yStart < LevelMap->ySize)) 
				{
					if (MapData[y][x]->tileID > GfxStuff->GetTileOffset())
						--MapData[y][x]->tileID;
					else
						MapData[y][x]->tileID = 0;
				}
			}
		}

		AssignSection();
	}
}
