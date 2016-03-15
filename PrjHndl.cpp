#include <cstdio>
#include <cstring>
#include <fstream>

#include <SDL2/SDL.h>

#include "PrjHndl.h"
#include "TxtRead.h"

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
    palOffset = mapOffset = artOffset = 0;
    palLength = mapLength = artLength = 0;
                mapCompr =  artCompr = comprType::INVALID;
    xSize = ySize = 0;
    tileOffset = 0;
    letterOffset = 0; numberOffset = 0;
    strcpy(saveName, "");

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
		strcpy(mapName, trimString(content));
		break;
        case infoType::ART_FILE:
		strcpy(artName, trimString(content));
		break;
        case infoType::PALETTE_OFFSET:
		palOffset = strtol(content, NULL, 0);
		break;
        case infoType::MAPPING_OFFSET:
		mapOffset = strtol(content, NULL, 0);
		break;
        case infoType::ART_OFFSET:
		artOffset = strtol(content, NULL, 0);
		break;
        case infoType::PALETTE_LENGTH:
		palLength = strtol(content, NULL, 0);
		break;
        case infoType::MAPPING_LENGTH:
		mapLength = strtol(content, NULL, 0);
		break;
        case infoType::ART_LENGTH:
		artLength = strtol(content, NULL, 0);
		break;
        case infoType::MAPPING_COMPRESSION:
		mapCompr = readComprType(trimString(content));
		break;
        case infoType::ART_COMPRESSION:
		artCompr = readComprType(trimString(content));
		break;
        case infoType::X_SIZE:
		xSize = strtol(content, NULL, 0);
		break;
        case infoType::Y_SIZE:
		ySize = strtol(content, NULL, 0);
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
		strcpy(saveName, trimString(content));
		break;
    }
}

void ProjectData::LoadArt(const char* const filename)
{
	if (artCompr == comprType::INVALID)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid art compression format. Should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Kid Chameleon'\n'Comper'\n'Saxman'", NULL);
		exit(1);
	}

	artLength = DecompressFile(artCompr, artName, filename, artOffset, artLength);

	if (artLength < 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not decompress art file. Are you sure the compression is correct?", NULL);
		exit(1);
	}
	tileAmount = artLength/0x20;
}

void ProjectData::LoadMap(const char* const filename) {
    if (mapCompr == comprType::INVALID || mapCompr == comprType::KID_CHAMELEON) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid map compression format. Should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Comper'\n'Saxman'", NULL);
        exit(1);
    }

    mapLength = DecompressFile(mapCompr, mapName, filename, mapOffset, mapLength);

    FILE* mapfile = fopen(filename, "r+b");

    if (mapLength < 0) {
        //file could not be decompressed or found
        mapLength = 2*xSize*ySize;
        if (!CheckCreateBlankFile(mapName, mapfile, mapOffset, mapLength)) {
            //file is existant but could not be decompressed
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not decompress map file. Are you sure the compression is correct?", NULL);
            exit(1);
        } else {
            //file non-existant, blank template created
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", "No map file found, created blank template.", NULL);
        }
    }

    fclose(mapfile);

    if (mapLength < 2*xSize*ySize) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", "Specified size exceeds map size.\nField has been trimmed vertically.", NULL);
        ySize = (mapLength/xSize) / 2;
        if (ySize == 0) exit(1);
    }
    
    if (strlen(saveName) == 0) {
        if (mapOffset == 0) strcpy(saveName, mapName); //overwrite existing map
        else strcpy(saveName, FILE_MAP_DEFAULT); //write to default file
    }
}

void ProjectData::LoadPal(const char* const filename) {
    palLength = ReadPlain(palName, filename, palOffset, palLength);
    if (palLength < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Palette file not found. Are you sure the path is correct?", NULL);
        exit(1);
    }
}

void ProjectData::SaveMap(const char* const filename) {
    CompressFile(mapCompr, filename, saveName);
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
