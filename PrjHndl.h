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
    ProjectData(const char* const prjtxt);
    void AssignInfo(int type, char* content);
    void LoadArt(const char* const filename);
    void LoadMap(const char* const filename);
    void LoadPal(const char* const filename);
    void SaveMap(const char* const filename);
    long DecompressFile(const fileCompression compression, const char* const srcfile, const char* const dstfile, long Pointer, int length);
};
