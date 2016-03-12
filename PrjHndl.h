#pragma once

#include "TxtRead.h"

class ProjectData
{
public:
    char palName[128];
    char mapName[128];
    char artName[128];
    char saveName[128];
    int palOffset;
    int mapOffset;
    int artOffset;
    int palLength;
    int mapLength;
    int artLength;
    fileCompression mapCompr;
    fileCompression artCompr;
    int xSize;
    int ySize;
    int tileOffset;
    int letterOffset;
    int numberOffset;
    
    int tileAmount;
public:
    ProjectData(char* prjtxt);
    void AssignInfo(int type, char* content);
    void LoadArt(const char* filename);
    void LoadMap(const char* filename);
    void LoadPal(const char* filename);
    void SaveMap(const char* filename);
};
