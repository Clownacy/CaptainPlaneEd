#pragma once

// compression types
typedef enum {
	NONE,
	ENIGMA,
	KOSINSKI,
	NEMESIS,
	KIDCHAMELEON,
	COMP_TYPE_AMOUNT,
	INVALID
} fileCompression;

char* strsrch(char* string, const char* const substring);
fileCompression readComprType(char* string);
char* trimString(char* string);
