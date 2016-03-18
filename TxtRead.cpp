#include <cstdio>
#include <cstring>

#include "TxtRead.h"

struct
{
	const char* const string;
	const infoType info_type;
} infoTypes[(int)infoType::INFO_TYPE_AMOUNT] = {
	{ "Palette File:",        infoType::PALETTE_FILE },
	{ "Mapping File:",        infoType::MAPPING_FILE },
	{ "Art File:",            infoType::ART_FILE },
	{ "Palette Offset:",      infoType::PALETTE_OFFSET },
	{ "Mapping Offset:",      infoType::MAPPING_OFFSET },
	{ "Art Offset:",          infoType::ART_OFFSET },
	{ "Palette Length:",      infoType::PALETTE_LENGTH },
	{ "Mapping Length:",      infoType::MAPPING_LENGTH },
	{ "Art Length:",          infoType::ART_LENGTH },
	{ "Palette Compression:", infoType::PALETTE_COMPRESSION },
	{ "Mapping Compression:", infoType::MAPPING_COMPRESSION },
	{ "Art Compression:",     infoType::ART_COMPRESSION },
	{ "x-Size:",              infoType::X_SIZE },
	{ "y-Size:",              infoType::Y_SIZE },
	{ "Tile Offset:",         infoType::TILE_OFFSET },
	{ "Letter Offset:",       infoType::LETTER_OFFSET },
	{ "Number Offset:",       infoType::NUMBER_OFFSET },
	{ "Save File:",           infoType::SAVE_FILE }
};

struct
{
	const char* const string;
	const comprType compr_type;
} comprTypes[(int)comprType::COMPR_TYPE_AMOUNT] = {
	{ "None",             comprType::NONE },
	{ "Enigma",           comprType::ENIGMA },
	{ "Kosinski",         comprType::KOSINSKI },
	{ "Moduled Kosinski", comprType::MODULED_KOSINSKI },
	{ "Nemesis",          comprType::NEMESIS },
	{ "Kid Chameleon",    comprType::KID_CHAMELEON },
	{ "Comper",           comprType::COMPER },
	{ "Saxman",           comprType::SAXMAN }
};

infoType readInfoType(const char* const string)
{
	for (int i=0; i < (int)infoType::INFO_TYPE_AMOUNT; ++i)
		if (!strncmp(string, infoTypes[i].string, strlen(infoTypes[i].string)))
			return infoTypes[i].info_type;

	return infoType::INVALID;
}

comprType readComprType(const char* const string)
{
	for (int i=0; i < (int)comprType::COMPR_TYPE_AMOUNT; ++i)
		if (!strncmp(string, comprTypes[i].string, strlen(comprTypes[i].string)))
			return comprTypes[i].compr_type;

	return comprType::INVALID;
}

char* trimString(char* string)
{
	string[strcspn(string, "\n")] = '\0';	// Replace newline with terminator character
	// Move pointer to start of text
	while (string[0] == ' ' || string[0] == '\n' || string[0] == '\t')
		++string;
	// Fill 'empty' characters after text with terminator characters
	int i = strlen(string)-1;
	while (string[i] == ' ' || string[i] == '\n' || string[i] == '\t')
	{
		string[i] = '\0';
		--i;
	}

	return string;
}
