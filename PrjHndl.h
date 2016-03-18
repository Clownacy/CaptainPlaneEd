#pragma once

#include "TxtRead.h"
#include "Resource.h"

class ProjectData
{
public:
    int xSize;
    int ySize;
    int tileOffset;
    int letterOffset;
    int numberOffset;

    int tileAmount;

    char saveName[128];

    ResourceArt art;
    ProjectData(const char* const prjtxt);
    void LoadArt(const char* const filename);
    void LoadMap(const char* const filename);
    void LoadPal(const char* const filename);
    void SaveMap(const char* const filename);
private:
    char palName[128];
    char mapName[128];
    char artName[128];
    int palOffset;
    int mapOffset;
    int artOffset;
    int palLength;
    int mapLength;
    int artLength;
    comprType mapCompr;
    comprType artCompr;

    void AssignInfo(const infoType type, char* content);
    long DecompressFile(const comprType compression, const char* const srcfile, const char* const dstfile, const long Pointer, const int length);
    void CompressFile(const comprType compression, const char* const srcfile, const char* const dstfile);
};
