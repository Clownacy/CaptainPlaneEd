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

#include "Project.h"

#include "Common.h"
#include "LevMap.h"
#include "PrjHndl.h"
#include "Screen.h"
#include "SelRect.h"

static const std::filesystem::path FILE_MAP_TEMP = "tempmap.bin";
static const std::filesystem::path FILE_ART_TEMP = "tempart.bin";
static const std::filesystem::path FILE_PAL_TEMP = "temppal.bin";

Project::Project(const std::filesystem::path &parameter_filepath)
	: PrjData(parameter_filepath, FILE_ART_TEMP, FILE_MAP_TEMP, FILE_PAL_TEMP)
	, GfxStuff(this->PrjData.map.xSize, this->PrjData.tileOffset, this->PrjData.art.tileAmount)
	, LevelMap(this->PrjData.map.xSize, this->PrjData.map.ySize, this->GfxStuff)
	, SelectionRect(this->GfxStuff, this->LevelMap)
	, CopyRect(SelRect(this->GfxStuff, this->LevelMap))
{
	this->GfxStuff.ReadPalette(FILE_PAL_TEMP);
	this->GfxStuff.ReadTiles(FILE_ART_TEMP);

	this->LevelMap.LoadMap(FILE_MAP_TEMP);
}

void Project::Save(void)
{
	this->LevelMap.SaveMap(FILE_MAP_TEMP);
	this->PrjData.map.Save(FILE_MAP_TEMP, this->PrjData.map.saveName);
	MainScreen.ShowInformation("Save complete");
}

void Project::Redraw(void)
{
	this->LevelMap.DrawMap();
	this->GfxStuff.DrawSelector();
	this->SelectionRect.SelDrawRect();
	this->LevelMap.DrawCurrentTile();
}
