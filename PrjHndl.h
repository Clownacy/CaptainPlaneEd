#pragma once

#include "TxtRead.h"
#include "Resource.h"

class ProjectData
{
public:
    int tileOffset;
    int letterOffset;
    int numberOffset;

    int tileAmount;

    ResourceArt art;
    ResourceMap map;
    ResourcePal pal;
    ProjectData(const char* const prjtxt);
    void LoadPal(const char* const filename);
private:
    char palName[128];
    int palOffset;
    int palLength;

    void AssignInfo(const infoType type, char* content);
    long DecompressFile(const comprType compression, const char* const srcfile, const char* const dstfile, const long Pointer, const int length);
    void CompressFile(const comprType compression, const char* const srcfile, const char* const dstfile);
};
