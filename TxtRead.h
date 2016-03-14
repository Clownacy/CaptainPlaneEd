#pragma once

// compression types
typedef enum {
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
} fileCompression;

struct stringToEnum
{
	const char* const string;
	fileCompression compression;
};

int readComprType(char* string, struct stringToEnum*, int type_amount);
char* trimString(char* string);
