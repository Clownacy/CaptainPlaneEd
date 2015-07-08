#ifndef PRJHNDL_H
#define PRJHNDL_H

#include <cstdio>

#include "TxtRead.h"

#include "compression/EniDec.h"
#include "compression/KosDec.h"
#include "compression/NemDec.h"
#include "compression/KidDec.h"
#include "compression/ReadPlain.h"
#include "compression/EniComp.h"

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

long (*ComprFunc[COMP_TYPE_AMOUNT])(char*, FILE*, long, int) = {
    ReadPlain,
    EniDec,
    KosDec,
    NemDec,
    KidDec
};

class ProjectData
{
public:
    char palName[128];
    char mapName[128];
    char artName[128];
    char saveName[128];
    int palOffset;
    int mapOffset;
    int artOffset;
    int palLength;
    int mapLength;
    int artLength;
    int mapCompr;
    int artCompr;
    int xSize;
    int ySize;
    int tileOffset;
    int letterOffset;
    int numberOffset;
    
    int tileAmount;
public:
    ProjectData(char* prjtxt);
    void AssignInfo(int type, char* content);
    void LoadArt(char* filename);
    void LoadMap(char* filename);
    void LoadPal(char* filename);
    void SaveMap(char* filename);
};

ProjectData::ProjectData(char* prjtxt) {
    palOffset = 0; mapOffset = 0; artOffset = 0;
    palLength = 0; mapLength = 0; artLength = 0;
                   mapCompr = -1; artCompr = -1;
    xSize = 0; ySize = 0;
    tileOffset = 0;
    letterOffset = 0; numberOffset = 0;
    strcpy(saveName, "");

    FILE* prjfile = fopen(prjtxt, "r");
    if(prjfile == NULL) {
        fprintf(stderr, "Usage: PlaneEd projectfile.ext\n");
        exit(1);
    }

    while(!feof(prjfile)) {
        char line[256];
        fgets(line, 256, prjfile);
        for(int type=0; type<TYPE_AMOUNT; type++) {
            char* content = strsrch(line, infoTypes[type]);
            if(content != NULL) AssignInfo(type, content);
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

void ProjectData::LoadArt(char* filename) {
    FILE* artfile = fopen(filename, "w+b");
    
    if(artCompr < 0 || artCompr > COMP_TYPE_AMOUNT) {
        fprintf(stderr, "Invalid art compression format.\n");
        exit(1);
    }

    artLength = ComprFunc[artCompr](artName, artfile, artOffset, artLength);

    if(artLength < 0) {
        fprintf(stderr, "Could not read art file.\n");
        exit(1);
    }
    tileAmount = artLength/0x20;
    fclose(artfile);
}

void ProjectData::LoadMap(char* filename) {
    FILE* mapfile = fopen(filename, "wb");

    if(mapCompr < 0 || mapCompr > 1) {
        fprintf(stderr, "Invalid map compression format.\n");
        exit(1);
    }

    mapLength = ComprFunc[mapCompr](mapName, mapfile, mapOffset, mapLength);
    if(mapLength < 0) {
        //file could not be decompressed or found
        mapLength = 2*xSize*ySize;
        if(!CheckCreateBlankFile(mapName, mapfile, mapOffset, mapLength)) {
            //file is existant but could not be decompressed
            fprintf(stderr, "Could not read map file.\n");
            exit(1);
        } else {
            //file non-existant, blank template created
            fprintf(stderr, "No map file found, created blank template.\n");
        }
    }

    fclose(mapfile);

    if(mapLength < 2*xSize*ySize) {
        fprintf(stderr, "Warning: Specified size exceeds map size.\nField has been trimmed vertically.\n");
        ySize = (mapLength/xSize) / 2;
        if(ySize == 0) exit(1);
    }
    
    if(strlen(saveName) == 0) {
        if(mapOffset == 0) strcpy(saveName, mapName); //overwrite existing map
        else strcpy(saveName, FILE_MAP_DEFAULT); //write to default file
    }
}

void ProjectData::LoadPal(char* filename) {
    FILE* palfile = fopen(filename, "wb");

    palLength = ReadPlain(palName, palfile, palOffset, palLength);
    if(palLength < 0) {
        fprintf(stderr, "Could not read palette file.\n");
        exit(1);
    }

    fclose(palfile);
}

void ProjectData::SaveMap(char* filename) {
    if(mapCompr == NONE) {
        remove(saveName);
        rename(filename, saveName);
    } else if(mapCompr == ENIGMA) {
        if(EniComp(filename, saveName) < 0) {
            fprintf(stderr, "Could not write map file.\n");
            exit(1);
        }
        remove(filename);
    } else {
        fprintf(stderr, "Invalid map compression format.\n");
        exit(1);
    }
}

#endif
