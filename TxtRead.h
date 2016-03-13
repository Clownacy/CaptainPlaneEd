#pragma once

// compression types
typedef enum {
	NONE,
	ENIGMA,
	KOSINSKI,
	NEMESIS,
	KIDCHAMELEON,
	COMPER,
	SAXMAN,
	COMP_TYPE_AMOUNT,
	INVALID
} fileCompression;

fileCompression readComprType(char* string);
char* trimString(char* string);
