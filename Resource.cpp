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

#define FILE_MAP_DEFAULT "MapDefault.bin"

Resource::Resource(void)
{
	offset = length = 0;
	compression = comprType::INVALID;
}

void Resource::Save(const char* const filename, const char* const dstfilename) {
    CompressFile(compression, filename, dstfilename);
}

long Resource::DecompressFile(const comprType compr_type, const char* const srcfile, const char* const dstfile, const long Pointer, const int length)
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

void Resource::CompressFile(const comprType compr_type, const char* const srcfile, const char* const dstfile)
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

int ResourceArt::Load(const char* const filename)
{
	if (compression == comprType::INVALID)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid art compression format. Should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Kid Chameleon'\n'Comper'\n'Saxman'", NULL);
		exit(1);
	}

	length = DecompressFile(compression, name, filename, offset, length);

	if (length < 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not decompress art file. Are you sure the compression is correct?", NULL);
		exit(1);
	}
	return length/0x20;
}

int ResourceMap::Load(const char* const filename)
{
	if (compression == comprType::INVALID || compression == comprType::KID_CHAMELEON)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Invalid map compression format. Should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Comper'\n'Saxman'", NULL);
		exit(1);
	}

	length = DecompressFile(compression, name, filename, offset, length);

	FILE* mapfile = fopen(filename, "r+b");

	if (length < 0) {
		//file could not be decompressed or found
		length = 2*xSize*ySize;
		if (!CheckCreateBlankFile(name, mapfile, offset, length))
		{
			//file is existant but could not be decompressed
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not decompress map file. Are you sure the compression is correct?", NULL);
			exit(1);
		}
		else
		{
			//file non-existant, blank template created
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", "No map file found, created blank template.", NULL);
		}
	}

	fclose(mapfile);

	if (length < 2*xSize*ySize)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", "Specified size exceeds map size.\nField has been trimmed vertically.", NULL);
		ySize = (length/xSize) / 2;
		if (ySize == 0)
			exit(1);
	}
    
	if (strlen(saveName) == 0)
	{
		if (offset == 0)
			strcpy(saveName, name); //overwrite existing map
		else
			strcpy(saveName, FILE_MAP_DEFAULT); //write to default file
	}
}

int ResourcePal::Load(const char* const filename)
{
	length = ReadPlain(name, filename, offset, length);
	if (length < 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Palette file not found. Are you sure the path is correct?", NULL);
		exit(1);
	}
}
