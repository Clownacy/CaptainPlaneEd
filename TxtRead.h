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
	MAPPING_COMPRESSION,
	ART_COMPRESSION,
	X_SIZE,
	Y_SIZE,
	TILE_OFFSET,
	LETTER_OFFSET,
	NUMBER_OFFSET,
	SAVE_FILE,
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
	COMPR_TYPE_AMOUNT,
	INVALID
};

infoType readInfoType(const char* const string);
comprType readComprType(const char* const string);
char* trimString(char* string);
