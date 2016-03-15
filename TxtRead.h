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

fileCompression readComprType(char* string);
char* trimString(char* string);
