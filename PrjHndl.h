#pragma once

#include "TxtRead.h"
#include "Resource.h"

class ProjectData
{
public:
    int tileOffset;
    int letterOffset;
    int numberOffset;

    ResourceArt art;
    ResourceMap map;
    ResourcePal pal;
    ProjectData(const char* const prjtxt);

private:
    void AssignInfo(const infoType type, char* content);
};
