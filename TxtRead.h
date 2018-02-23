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

#pragma once

enum class infoType {
	PALETTE_FILE,
	MAPPING_FILE,
	ART_FILE,
	PALETTE_OFFSET,
	MAPPING_OFFSET,
	ART_OFFSET,
	PALETTE_LENGTH,
	MAPPING_LENGTH,
	ART_LENGTH,
	PALETTE_COMPRESSION,
	MAPPING_COMPRESSION,
	ART_COMPRESSION,
	PALETTE_DESTINATION_OFFSET,
	X_SIZE,
	Y_SIZE,
	TILE_OFFSET,
	LETTER_OFFSET,
	NUMBER_OFFSET,
	SAVE_FILE,
	KOSINSKI_MODULE_SIZE,
	INFO_TYPE_AMOUNT,
	INVALID
};

// compression types
enum class comprType {
	NONE,
	ENIGMA,
	KOSINSKI,
	MODULED_KOSINSKI,
	NEMESIS,
	KID_CHAMELEON,
	COMPER,
	SAXMAN,
	ROCKET,
	COMPR_TYPE_AMOUNT,
	INVALID
};

infoType readInfoType(const char* const string);
comprType readComprType(const char* const string);
char* trimString(char* string);
