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

#include <sstream>

#include "Common.h"
#include "LevMap.h"
#include "PrjHndl.h"
#include "Screen.h"
#include "SelRect.h"

Project::Project(const std::filesystem::path &parameter_filepath)
	: PrjData(parameter_filepath)
	, GfxStuff(this->PrjData.map.xSize, this->PrjData.tileOffset, this->PrjData.art.tileAmount)
	, LevelMap(this->PrjData.map.xSize, this->PrjData.map.ySize, this->GfxStuff)
	, SelectionRect(this->GfxStuff, this->LevelMap)
	, CopyRect(SelRect(this->GfxStuff, this->LevelMap))
{
	this->GfxStuff.ReadPalette(PrjData.pal.buffer, PrjData.pal.buffer_size);
	this->GfxStuff.ReadTiles(PrjData.art.buffer);

	this->LevelMap.LoadMap(PrjData.map.buffer);

	// We no longer need the file buffers, so free them using this trick.
	std::stringstream().swap(PrjData.pal.buffer);
	std::stringstream().swap(PrjData.art.buffer);
	std::stringstream().swap(PrjData.map.buffer);
}

void Project::Save(void)
{
	std::stringstream mapfile;
	this->LevelMap.SaveMap(mapfile);
	this->PrjData.map.Save(mapfile);
	MainScreen.ShowInformation("Save complete");
}

void Project::Redraw(void)
{
	this->LevelMap.DrawMap();
	this->GfxStuff.DrawSelector();
	this->SelectionRect.SelDrawRect();
	this->LevelMap.DrawCurrentTile();
}
