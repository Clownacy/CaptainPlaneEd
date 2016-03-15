#include <cstdio>
#include <cstring>

#include "TxtRead.h"

struct
{
	const char* const string;
	const fileCompression compression;
} comprTypes[(int)fileCompression::COMP_TYPE_AMOUNT] = {
	{ "None", fileCompression::NONE },
	{ "Enigma", fileCompression::ENIGMA },
	{ "Kosinski", fileCompression::KOSINSKI },
	{ "Moduled Kosinski", fileCompression::MODULED_KOSINSKI },
	{ "Nemesis", fileCompression::NEMESIS },
	{ "Kid Chameleon", fileCompression::KID_CHAMELEON },
	{ "Comper", fileCompression::COMPER },
	{ "Saxman", fileCompression::SAXMAN }
};

fileCompression readComprType(char* string)
{
	for (int i=0; i < (int)fileCompression::COMP_TYPE_AMOUNT; ++i)
		if (!strncmp(string, comprTypes[i].string, strlen(comprTypes[i].string)))
			return comprTypes[i].compression;

	return fileCompression::INVALID;
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
