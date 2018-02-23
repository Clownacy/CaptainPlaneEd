/*
    Copyright (C) 2009-2011 qiuu
    Copyright (C) 2016-2018 Clownacy

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
	{ "Palette Destination Offset:", infoType::PALETTE_DESTINATION_OFFSET },
	{ "x-Size:",              infoType::X_SIZE },
	{ "y-Size:",              infoType::Y_SIZE },
	{ "Tile Offset:",         infoType::TILE_OFFSET },
	{ "Letter Offset:",       infoType::LETTER_OFFSET },
	{ "Number Offset:",       infoType::NUMBER_OFFSET },
	{ "Save File:",           infoType::SAVE_FILE },
	{ "Kosinski Module Size:",infoType::KOSINSKI_MODULE_SIZE }
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
	{ "Saxman",           comprType::SAXMAN },
	{ "Rocket",           comprType::ROCKET }
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
	string[strcspn(string, "\n\r")] = '\0';	// Replace newline with terminator character
	// Move pointer to start of text
	while (string[0] == ' ' || string[0] == '\t')
		++string;
	// Fill 'empty' characters after text with terminator characters
	int i = strlen(string)-1;
	while (string[i] == ' ' || string[i] == '\t')
	{
		string[i] = '\0';
		--i;
	}

	return string;
}
