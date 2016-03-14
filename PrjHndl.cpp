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

char infoTypes[][32] = {
    "Palette File:",
    "Mapping File:",
    "Art File:",
    "Palette Offset:",
    "Mapping Offset:",
    "Art Offset:",
    "Palette Length:",
    "Mapping Length:",
    "Art Length:",
    "Mapping Compression:",
    "Art Compression:",
    "x-Size:",
    "y-Size:",
    "Tile Offset:",
    "Letter Offset:",
    "Number Offset:",
    "Save File:"
};

static long ComprFunc(const fileCompression compression, const char* const srcfile, const char* const dstfile, long Pointer, int length)
{
	switch (compression)
	{
		case NONE:
			length = ReadPlain(srcfile, dstfile, Pointer, length);
			break;
		case ENIGMA:
			length = enigma::decode(srcfile, dstfile, Pointer, false);
			break;
		case KOSINSKI:
			length = kosinski::decode(srcfile, dstfile, Pointer, false, 16u);
			break;
		case MODULED_KOSINSKI:
			length = kosinski::decode(srcfile, dstfile, Pointer, true, 16u);
			break;
		case NEMESIS:
			length = nemesis::decode(srcfile, dstfile, Pointer, 0);
			break;
		case KID_CHAMELEON:
			length = KidDec(srcfile, dstfile, Pointer);
			break;
		case COMPER:
			length = comper::decode(srcfile, dstfile, Pointer);
			break;
		case SAXMAN:
			length = saxman::decode(srcfile, dstfile, Pointer, 0);
			break;
	}

	return length;
}

ProjectData::ProjectData(const char* const prjtxt) {
    palOffset = mapOffset = artOffset = 0;
    palLength = mapLength = artLength = 0;
                mapCompr =  artCompr = INVALID;
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
        for (int type=0; type < TYPE_AMOUNT; ++type) {
            if (!strncmp(line, infoTypes[type], strlen(infoTypes[type])))
		    AssignInfo(type, line+strlen(infoTypes[type]));
        }
    }
}

void ProjectData::AssignInfo(int type, char* content) {
    switch(type) {
        case  0: strcpy(palName, trimString(content)); break;
        case  1: strcpy(mapName, trimString(content)); break;
        case  2: strcpy(artName, trimString(content)); break;
        case  3: palOffset = strtol(content, NULL, 0); break;
        case  4: mapOffset = strtol(content, NULL, 0); break;
        case  5: artOffset = strtol(content, NULL, 0); break;
        case  6: palLength = strtol(content, NULL, 0); break;
        case  7: mapLength = strtol(content, NULL, 0); break;
        case  8: artLength = strtol(content, NULL, 0); break;
        case  9: mapCompr = readComprType(trimString(content)); break;
        case 10: artCompr = readComprType(trimString(content)); break;
        case 11: xSize = strtol(content, NULL, 0); break;
        case 12: ySize = strtol(content, NULL, 0); break;
        case 13: tileOffset = strtol(content, NULL, 0); break;
        case 14: letterOffset = strtol(content, NULL, 0); break;
        case 15: numberOffset = strtol(content, NULL, 0); break;
        case 16: strcpy(saveName, trimString(content)); break;
    }
}

void ProjectData::LoadArt(const char* const filename)
{
	if (artCompr == INVALID)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid art compression format. Should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Kid Chameleon'\n'Comper'\n'Saxman'", NULL);
		exit(1);
	}

	artLength = ComprFunc(artCompr, artName, filename, artOffset, artLength);

	if (artLength < 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not decompress art file. Are you sure the compression is correct?", NULL);
		exit(1);
	}
	tileAmount = artLength/0x20;
}

void ProjectData::LoadMap(const char* const filename) {
    if (mapCompr != NONE && mapCompr != ENIGMA) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid map compression format. Should be one of the following:\n\n'None'\n'Enigma'", NULL);
        exit(1);
    }

    FILE* mapfile;

    mapLength = ComprFunc(mapCompr, mapName, filename, mapOffset, mapLength);

    mapfile = fopen(filename, "r+b");

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

void ProjectData::SaveMap(const char* filename) {
    if (mapCompr == NONE) {
        remove(saveName);
        rename(filename, saveName);
    } else if (mapCompr == ENIGMA) {
        ifstream fin(filename, ios::in|ios::binary);
        fstream fout(saveName, ios::in|ios::out|ios::binary|ios::trunc);
        enigma::encode(fin, fout, false);
        fin.close();
        fout.close();
        remove(filename);
    } else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid map compression format. Should be one of the following:\n\n'None'\n'Enigma'", NULL);
        exit(1);
    }
}
