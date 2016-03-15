#include <cstdio>
#include <cstring>

#include "TxtRead.h"

struct
{
	const char* const string;
	const infoType info_type;
} infoTypes[(int)infoType::INFO_TYPES_AMOUNT] = {
	{ "Palette File:", infoType::PALETTE_FILE },
	{ "Mapping File:", infoType::MAPPING_FILE },
	{ "Art File:", infoType::ART_FILE },
	{ "Palette Offset:", infoType::PALETTE_OFFSET },
	{ "Mapping Offset:", infoType::MAPPING_OFFSET },
	{ "Art Offset:", infoType::ART_OFFSET },
	{ "Palette Length:", infoType::PALETTE_LENGTH },
	{ "Mapping Length:", infoType::MAPPING_LENGTH },
	{ "Art Length:", infoType::ART_LENGTH },
	{ "Mapping Compression:", infoType::MAPPING_COMPRESSION },
	{ "Art Compression:", infoType::ART_COMPRESSION },
	{ "x-Size:", infoType::X_SIZE },
	{ "y-Size:", infoType::Y_SIZE },
	{ "Tile Offset:", infoType::TILE_OFFSET },
	{ "Letter Offset:", infoType::LETTER_OFFSET },
	{ "Number Offset:", infoType::NUMBER_OFFSET },
	{ "Save File:", infoType::SAVE_FILE }
};

struct
{
	const char* const string;
	const fileCompression compression;
} comprTypes[(int)fileCompression::COMP_TYPE_AMOUNT] = {
	{ "None", fileCompression::NONE },
	{ "Enigma", fileCompression::ENIGMA },
	{ "Kosinski", fileCompression::KOSINSKI },
	{ "Moduled Kosinski", fileCompression::MODULED_KOSINSKI },
	{ "Nemesis", fileCompression::NEMESIS },
	{ "Kid Chameleon", fileCompression::KID_CHAMELEON },
	{ "Comper", fileCompression::COMPER },
	{ "Saxman", fileCompression::SAXMAN }
};

infoType readInfoType(char* string)
{
	for (int i=0; i < (int)infoType::INFO_TYPES_AMOUNT; ++i)
		if (!strncmp(string, infoTypes[i].string, strlen(infoTypes[i].string)))
			return infoTypes[i].info_type;
	
	return infoType::INVALID;
}

fileCompression readComprType(char* string)
{
	for (int i=0; i < (int)fileCompression::COMP_TYPE_AMOUNT; ++i)
		if (!strncmp(string, comprTypes[i].string, strlen(comprTypes[i].string)))
			return comprTypes[i].compression;

	return fileCompression::INVALID;
}

char* getProjectInfo(char* string)
{
	string[strcspn(string, "\n")] = '\0';
	// Move pointer to beginning of text
	while (string[0] != ':')
		++string;
	++string;	// Advance one more time to skip the ':'
	return string;
}

char* trimString(char* string)
{
	string[strcspn(string, "\n")] = '\0';
	while (string[0] == ' ' || string[0] == '\n' || string[0] == '\t')
		++string;
	int i = strlen(string)-1;
	while (string[i] == ' ' || string[i] == '\n' || string[i] == '\t')
	{
		string[i] = '\0';
		--i;
	}
       
	return string;
}
