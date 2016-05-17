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

#include "Common.h"
#include "LevMap.h"
#include "Project.h"
#include "PrjHndl.h"
#include "Screen.h"
#include "SelRect.h"
#ifdef _WIN32
#include "WinAPI.h"
#endif

const char* const FILE_MAP_TEMP = "tempmap.bin";
const char* const FILE_ART_TEMP = "tempart.bin";
const char* const FILE_PAL_TEMP = "temppal.bin";

Project::Project(char* const parameter_filepath)
{
	this->PrjData = new ProjectData(parameter_filepath);
	this->PrjData->art.Load(FILE_ART_TEMP);
	this->PrjData->map.Load(FILE_MAP_TEMP);
	this->PrjData->pal.Load(FILE_PAL_TEMP);

	this->GfxStuff = new Graphics(this->PrjData->map.xSize, this->PrjData->tileOffset, this->PrjData->art.tileAmount);

	this->LevelMap = new LevMap(this->PrjData->map.xSize, this->PrjData->map.ySize, this->GfxStuff);

	this->GfxStuff->ReadPalette(FILE_PAL_TEMP);
	this->GfxStuff->ReadTiles(FILE_ART_TEMP);
	this->GfxStuff->CreateTiles();

	this->LevelMap->LoadMap(FILE_MAP_TEMP);

	this->SelectionRect = new SelRect(this->GfxStuff, this->LevelMap);
	this->CopyRect = new SelRect(this->GfxStuff, this->LevelMap);

#ifdef _WIN32
	WinAPI::SetMenuBarOptionGreyed(true, MENUBAR_SAVE);
	WinAPI::SetMenuBarOptionGreyed(true, MENUBAR_CLOSE);
	WinAPI::SetMenuBarOptionGreyed(true, MENUBAR_PALETTELINE1);
	WinAPI::SetMenuBarOptionGreyed(true, MENUBAR_PALETTELINE2);
	WinAPI::SetMenuBarOptionGreyed(true, MENUBAR_PALETTELINE3);
	WinAPI::SetMenuBarOptionGreyed(true, MENUBAR_PALETTELINE4);
#endif
}

Project::~Project(void)
{
	delete this->PrjData;
	delete this->GfxStuff;
	delete this->LevelMap;
	delete this->SelectionRect;
	delete this->CopyRect;

#ifdef _WIN32
	WinAPI::SetMenuBarOptionGreyed(false, MENUBAR_SAVE);
	WinAPI::SetMenuBarOptionGreyed(false, MENUBAR_CLOSE);
	WinAPI::SetMenuBarOptionGreyed(false, MENUBAR_PALETTELINE1);
	WinAPI::SetMenuBarOptionGreyed(false, MENUBAR_PALETTELINE2);
	WinAPI::SetMenuBarOptionGreyed(false, MENUBAR_PALETTELINE3);
	WinAPI::SetMenuBarOptionGreyed(false, MENUBAR_PALETTELINE4);
#endif
}

void Project::Save(void)
{
	this->LevelMap->SaveMap(FILE_MAP_TEMP);
	this->PrjData->map.Save(FILE_MAP_TEMP, this->PrjData->map.saveName);
	MainScreen->ShowInformation("Save complete");
}

void Project::Redraw(void)
{
	this->LevelMap->DrawMap();
	this->GfxStuff->DrawSelector();
	this->SelectionRect->SelDrawRect();
}
