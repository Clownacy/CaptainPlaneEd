#include <cstdio>
#include <cstring>
#include <fstream>

#include <SDL2/SDL.h>

#include "PrjHndl.h"
#include "TxtRead.h"
#include "Resource.h"

#include "compression/KidDec.h"
#include "compression/ReadPlain.h"
#include "FW_KENSC/comper.h"
#include "FW_KENSC/enigma.h"
#include "FW_KENSC/kosinski.h"
#include "FW_KENSC/nemesis.h"
#include "FW_KENSC/saxman.h"

using namespace std;

#define FILE_MAP_DEFAULT "MapDefault.bin"

#define TYPE_AMOUNT sizeof(infoTypes)/32

ProjectData::ProjectData(const char* const prjtxt) {
    palOffset = 0;
    palLength = 0;
    tileOffset = 0;
    letterOffset = 0; numberOffset = 0;

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
		strcpy(palName, trimString(content));
		break;
        case infoType::MAPPING_FILE:
		strcpy(map.name, trimString(content));
		break;
        case infoType::ART_FILE:
		strcpy(art.name, trimString(content));
		break;
        case infoType::PALETTE_OFFSET:
		palOffset = strtol(content, NULL, 0);
		break;
        case infoType::MAPPING_OFFSET:
		map.offset = strtol(content, NULL, 0);
		break;
        case infoType::ART_OFFSET:
		art.offset = strtol(content, NULL, 0);
		break;
        case infoType::PALETTE_LENGTH:
		palLength = strtol(content, NULL, 0);
		break;
        case infoType::MAPPING_LENGTH:
		map.length = strtol(content, NULL, 0);
		break;
        case infoType::ART_LENGTH:
		art.length = strtol(content, NULL, 0);
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

void ProjectData::LoadPal(const char* const filename) {
    palLength = ReadPlain(palName, filename, palOffset, palLength);
    if (palLength < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Palette file not found. Are you sure the path is correct?", NULL);
        exit(1);
    }
}

long ProjectData::DecompressFile(const comprType compr_type, const char* const srcfile, const char* const dstfile, const long Pointer, const int length)
{
	int decompressed_length;
	switch (compr_type)
	{
		case comprType::NONE:
			decompressed_length = ReadPlain(srcfile, dstfile, Pointer, length);
			break;
		case comprType::ENIGMA:
			decompressed_length = enigma::decode(srcfile, dstfile, Pointer, false);
			break;
		case comprType::KOSINSKI:
			decompressed_length = kosinski::decode(srcfile, dstfile, Pointer, false, 16u);
			break;
		case comprType::MODULED_KOSINSKI:
			decompressed_length = kosinski::decode(srcfile, dstfile, Pointer, true, 16u);
			break;
		case comprType::NEMESIS:
			decompressed_length = nemesis::decode(srcfile, dstfile, Pointer, 0);
			break;
		case comprType::KID_CHAMELEON:
			decompressed_length = KidDec(srcfile, dstfile, Pointer);
			break;
		case comprType::COMPER:
			decompressed_length = comper::decode(srcfile, dstfile, Pointer);
			break;
		case comprType::SAXMAN:
			decompressed_length = saxman::decode(srcfile, dstfile, Pointer, 0);
			break;
	}

	return decompressed_length;
}

void ProjectData::CompressFile(const comprType compr_type, const char* const srcfile, const char* const dstfile)
{
	switch (compr_type)
	{
		case comprType::NONE:
			remove(dstfile);
			rename(srcfile, dstfile);
			break;
		case comprType::ENIGMA:
			enigma::encode(srcfile, dstfile, false);
			break;
		case comprType::KOSINSKI:
			kosinski::encode(srcfile, dstfile, false, 0x1000, 16u);
			break;
		case comprType::MODULED_KOSINSKI:
			kosinski::encode(srcfile, dstfile, true, 0x1000, 16u);
			break;
		case comprType::NEMESIS:
			nemesis::encode(srcfile, dstfile);
			break;
		case comprType::COMPER:
			comper::encode(srcfile, dstfile);
			break;
		case comprType::SAXMAN:
			saxman::encode(srcfile, dstfile, false);
			break;
	}
}
