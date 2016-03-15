#include <cstdio>
#include <cstring>

#include "TxtRead.h"

// Returned value should be cast to enum
fileCompression readComprType(char* string, struct stringToEnum* string_to_enum, int type_amount)
{
	for (int i=0; i < type_amount; ++i)
		if (!strncmp(string, string_to_enum[i].string, strlen(string_to_enum[i].string)))
			return string_to_enum[i].compression;

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
