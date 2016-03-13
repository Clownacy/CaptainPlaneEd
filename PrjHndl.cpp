#include <cstdio>
#include <fstream>

#include <SDL2/SDL.h>

#include "PrjHndl.h"
#include "TxtRead.h"

#include "compression/EniDec.h"
#include "compression/KosDec.h"
#include "compression/NemDec.h"
#include "compression/KidDec.h"
#include "compression/ReadPlain.h"
#include "compression/EniComp.h"
#include "FW_KENSC/comper.h"
#include "FW_KENSC/enigma.h"
#include "FW_KENSC/kosinski.h"
#include "FW_KENSC/nemesis.h"
#include "FW_KENSC/saxman.h"

using namespace std;

#define FILE_MAP_DEFAULT "MapDefault.bin"

#define TYPE_AMOUNT 17

char infoTypes[TYPE_AMOUNT][32] = {
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

long ComprFunc(const fileCompression compression, const char* srcfile, FILE* dst, long Pointer, int length)
{
	switch (compression)
	{
		case NONE:
			length = ReadPlain(srcfile, dst, Pointer, length);
			break;
		case ENIGMA:
			length = EniDec(srcfile, dst, Pointer, length);
			break;
		case KOSINSKI:
			length = KosDec(srcfile, dst, Pointer, length);
			break;
		case NEMESIS:
			length = NemDec(srcfile, dst, Pointer, length);
			break;
		case KIDCHAMELEON:
			length = KidDec(srcfile, dst, Pointer, length);
			break;
	}

	return length;
}

long ComprFunc_FW(const fileCompression compression, istream &fin, iostream &fout, long Pointer, int length)
{
	switch (compression)
	{
		case ENIGMA:
			length = enigma::decode(fin, fout, 0, false);
			break;
		case KOSINSKI:
			length = kosinski::decode(fin, fout, 0, false, 16u);
			break;
		case NEMESIS:
			length = nemesis::decode(fin, fout, 0, 0);
			break;
		case COMPER:
			length = comper::decode(fin, fout, 0);
			break;
		case SAXMAN:
			length = saxman::decode(fin, fout, 0, 0);
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
            char* content = strsrch(line, infoTypes[type]);
            if (content != NULL) AssignInfo(type, content);
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
        case  9: mapCompr = readComprType(content); break;
        case 10: artCompr = readComprType(content); break;
        case 11: xSize = strtol(content, NULL, 0); break;
        case 12: ySize = strtol(content, NULL, 0); break;
        case 13: tileOffset = strtol(content, NULL, 0); break;
        case 14: letterOffset = strtol(content, NULL, 0); break;
        case 15: numberOffset = strtol(content, NULL, 0); break;
        case 16: strcpy(saveName, trimString(content)); break;
    }
}

void ProjectData::LoadArt(const char* const filename) {
	if (artCompr != COMPER && artCompr != SAXMAN)
	{
	    FILE* artfile = fopen(filename, "w+b");
	    
	    if (artCompr == INVALID) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid art compression format. Should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Nemesis'\n'Kid Chameleon'", NULL);
		exit(1);
	    }

	    artLength = ComprFunc(artCompr, artName, artfile, artOffset, artLength);

	    if (artLength < 0) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not decompress art file. Are you sure the compression is correct?", NULL);
		exit(1);
	    }
	    tileAmount = artLength/0x20;
	    fclose(artfile);
	}
	else
	{
		if (artCompr == INVALID)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid art compression format. Should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Nemesis'\n'Kid Chameleon'", NULL);
			exit(1);
		}

		ifstream fin(artName, ios::in|ios::binary);
		fstream fout(filename, ios::in|ios::out|ios::binary|ios::trunc);

		artLength = ComprFunc_FW(artCompr, fin, fout, artOffset, artLength);

		if (artLength < 0)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not decompress art file. Are you sure the compression is correct?", NULL);
			exit(1);
		}
		tileAmount = artLength/0x20;
		fin.close();
		fout.close();
	}
}

void ProjectData::LoadMap(const char* const filename) {
    FILE* mapfile = fopen(filename, "wb");

    if (mapCompr != NONE && mapCompr != ENIGMA) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid map compression format. Should be one of the following:\n\n'None'\n'Enigma'", NULL);
        exit(1);
    }

    mapLength = ComprFunc(mapCompr, mapName, mapfile, mapOffset, mapLength);
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
    FILE* palfile = fopen(filename, "wb");

    palLength = ReadPlain(palName, palfile, palOffset, palLength);
    if (palLength < 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Palette file not found. Are you sure the path is correct?", NULL);
        exit(1);
    }

    fclose(palfile);
}

void ProjectData::SaveMap(const char* filename) {
    if (mapCompr == NONE) {
        remove(saveName);
        rename(filename, saveName);
    } else if (mapCompr == ENIGMA) {
        if (EniComp(filename, saveName) < 0) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not save map file.", NULL);
            exit(1);
        }
        remove(filename);
    } else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid map compression format. Should be one of the following:\n\n'None'\n'Enigma'", NULL);
        exit(1);
    }
}
