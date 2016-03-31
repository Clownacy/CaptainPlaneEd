#pragma once

#include "Project.h"
#include "PrjHndl.h"
#include "LevMap.h"

const char* const FILE_MAP_TEMP = "tempmap.bin";
const char* const FILE_ART_TEMP = "tempart.bin";
const char* const FILE_PAL_TEMP = "temppal.bin";

extern Screen* MainScreen;
extern Project* CurProject;

extern ProjectData* Current_ProjectData;
extern LevMap* Current_LevelMap;
