#include <cstdio>
#include <cstring>
#include <fstream>

#include <SDL2/SDL.h>

#include "PrjHndl.h"
#include "TxtRead.h"
#include "Resource.h"

ProjectData::ProjectData(const char* const prjtxt) {
    tileOffset = 0;
    letterOffset = numberOffset = 0;

    FILE* prjfile = fopen(prjtxt, "r");
    if (prjfile == NULL) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Please drag and drop your project file onto this program to start it.", NULL);
        exit(1);
    }

    while (!feof(prjfile)) {
        char line[256];
        fgets(line, 256, prjfile);
	infoType info_type = readInfoType(line);
	if (info_type != infoType::INVALID)
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
    }
}
