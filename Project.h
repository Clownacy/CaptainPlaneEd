#pragma once

#include "LevMap.h"
#include "PrjHndl.h"
#include "Screen.h"
#include "SelRect.h"

class Project
{
public:
	ProjectData* PrjData;
	Graphics* GfxStuff;
	LevMap* LevelMap;
	SelRect* SelectionRect;
	SelRect* CopyRect;

	Project(char* const parameter_filepath, Screen* MainScreen);
};
