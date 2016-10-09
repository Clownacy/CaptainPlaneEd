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

#include <cstring>
#include <fstream>
#include <unistd.h>

#include "Common.h"
#include "PrjHndl.h"
#include "TxtRead.h"
#include "Resource.h"
#ifdef _WIN32
#include "WinAPI.h"
#endif

ProjectData::ProjectData(const char* const prjtxt) {
    tileOffset = 0;
    letterOffset = numberOffset = 0;

    std::ifstream prjfile(prjtxt, std::ios::in);

    // Make working directory from project path
    char prjdir[strlen(prjtxt)+1];
    strcpy(prjdir, prjtxt);
    // Find last path separator
    char* posix_seperator = strrchr(prjdir, '/');	// First, POSIX
    char* windows_seperator = strrchr(prjdir, '\\');	// Then whatever Windows uses
    if (posix_seperator != NULL || windows_seperator != NULL)
    {
        if (posix_seperator != NULL)
            (*posix_seperator) = '\0';
        else if (windows_seperator != NULL)
            (*windows_seperator) = '\0';

        chdir(prjdir);
    }

    while (!prjfile.eof()) {
        char line[256];
        prjfile.getline(line, 256);
	infoType info_type = readInfoType(line);
	AssignInfo(info_type, line+strcspn(line, ":")+1);
    }
}

void ProjectData::AssignInfo(const infoType type, char* content) {
    switch(type) {
        case infoType::PALETTE_FILE:
		strcpy(pal.name, trimString(content));
		break;
        case infoType::MAPPING_FILE:
		strcpy(map.name, trimString(content));
		break;
        case infoType::ART_FILE:
		strcpy(art.name, trimString(content));
		break;
        case infoType::PALETTE_OFFSET:
		pal.offset = strtol(content, NULL, 0);
		break;
        case infoType::MAPPING_OFFSET:
		map.offset = strtol(content, NULL, 0);
		break;
        case infoType::ART_OFFSET:
		art.offset = strtol(content, NULL, 0);
		break;
        case infoType::PALETTE_LENGTH:
		pal.length = strtol(content, NULL, 0);
		break;
        case infoType::MAPPING_LENGTH:
		map.length = strtol(content, NULL, 0);
		break;
        case infoType::ART_LENGTH:
		art.length = strtol(content, NULL, 0);
		break;
        case infoType::PALETTE_COMPRESSION:
		pal.compression = readComprType(trimString(content));
		break;
        case infoType::MAPPING_COMPRESSION:
		map.compression = readComprType(trimString(content));
		break;
        case infoType::ART_COMPRESSION:
		art.compression = readComprType(trimString(content));
		break;
        case infoType::PALETTE_DESTINATION_OFFSET:
		pal.destination_offset = strtol(content, NULL, 0);
		if (pal.destination_offset >= 0x80)
			MainScreen->ShowError("Palette Destination Offset cannot be higher than 0x7F (16th entry of 4th palette line; the last palette entry)");
		break;
        case infoType::X_SIZE:
		map.xSize = strtol(content, NULL, 0);
		break;
        case infoType::Y_SIZE:
		map.ySize = strtol(content, NULL, 0);
		break;
        case infoType::TILE_OFFSET:
		tileOffset = strtol(content, NULL, 0);
		break;
        case infoType::LETTER_OFFSET:
		letterOffset = strtol(content, NULL, 0);
		break;
        case infoType::NUMBER_OFFSET:
		numberOffset = strtol(content, NULL, 0);
		break;
        case infoType::SAVE_FILE:
		strcpy(map.saveName, trimString(content));
		break;
        case infoType::KOSINSKI_MODULE_SIZE:
		art.kosinski_module_size = strtol(content, NULL, 0);
		break;
    }
}
