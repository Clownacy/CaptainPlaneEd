#include <cstdio>
#include <cstring>

#include "TxtRead.h"

struct stringToEnum
{
	const char string[32];
	fileCompression compression;
};

struct stringToEnum comprTypes[COMP_TYPE_AMOUNT] = {
	{ "None", NONE },
	{ "Enigma", ENIGMA },
	{ "Kosinski", KOSINSKI },
	{ "Nemesis", NEMESIS },
	{ "Kid Chameleon", KIDCHAMELEON },
	{ "Comper", COMPER },
	{ "Saxman", SAXMAN }
};

fileCompression readComprType(char* string)
{
	for (int i=0; i < COMP_TYPE_AMOUNT; ++i)
		if (!strncmp(string, comprTypes[i].string, strlen(comprTypes[i].string)))
			return comprTypes[i].compression;

	return INVALID;
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
