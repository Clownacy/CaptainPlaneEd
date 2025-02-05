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

#pragma once

#include <filesystem>

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
    ProjectData(const std::filesystem::path &prjtxt);

private:
    void AssignInfo(infoType type, const char *content, const std::filesystem::path &directory);
};
