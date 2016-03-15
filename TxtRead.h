#pragma once

// compression types
enum class fileCompression{
	NONE,
	ENIGMA,
	KOSINSKI,
	MODULED_KOSINSKI,
	NEMESIS,
	KID_CHAMELEON,
	COMPER,
	SAXMAN,
	COMP_TYPE_AMOUNT,
	INVALID
};

struct stringToEnum
{
	const char* const string;
	fileCompression compression;
};

fileCompression readComprType(char* string, struct stringToEnum*, int type_amount);
char* trimString(char* string);
