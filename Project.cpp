#include "Common.h"
#include "LevMap.h"
#include "Project.h"
#include "PrjHndl.h"
#include "Screen.h"
#include "SelRect.h"

const char* const FILE_MAP_TEMP = "tempmap.bin";
const char* const FILE_ART_TEMP = "tempart.bin";
const char* const FILE_PAL_TEMP = "temppal.bin";

Project::Project(char* const parameter_filepath, Screen* const MainScreen)
{
	this->PrjData = new ProjectData(parameter_filepath);
	this->PrjData->art.Load(FILE_ART_TEMP);
	this->PrjData->map.Load(FILE_MAP_TEMP);
	this->PrjData->pal.Load(FILE_PAL_TEMP);

	this->GfxStuff = new Graphics(this->PrjData->map.xSize, this->PrjData->tileOffset, this->PrjData->art.tileAmount, MainScreen);

	this->LevelMap = new LevMap(this->PrjData->map.xSize, this->PrjData->map.ySize, this->GfxStuff);

	this->GfxStuff->ReadPalette(FILE_PAL_TEMP);
	this->GfxStuff->ReadTiles(FILE_ART_TEMP);
	this->GfxStuff->CreateTiles();

	this->LevelMap->LoadMap(FILE_MAP_TEMP);

	this->SelectionRect = new SelRect(this->GfxStuff, this->LevelMap);
	this->CopyRect = new SelRect(this->GfxStuff, this->LevelMap);
}

void Project::Save(void)
{
	CurProject->LevelMap->SaveMap(FILE_MAP_TEMP);
	CurProject->PrjData->map.Save(FILE_MAP_TEMP, CurProject->PrjData->map.saveName);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", "Save complete.", NULL);
}

void Project::Redraw(void)
{
	CurProject->LevelMap->DrawMap();
	CurProject->GfxStuff->DrawSelector();
	CurProject->SelectionRect->SelDrawRect();
}
