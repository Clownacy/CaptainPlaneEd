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
    void AssignInfo(const infoType type, char* content);
};
