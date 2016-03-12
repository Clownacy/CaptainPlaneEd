#ifndef TXTREAD_H
#define TXTREAD_H

#include <cstdio>
#include <cstring>

/* compression types */
typedef enum {
	NONE,
	ENIGMA,
	KOSINSKI,
	NEMESIS,
	KIDCHAMELEON,
	COMP_TYPE_AMOUNT,
	INVALID
} fileCompression;

char comprTypes[COMP_TYPE_AMOUNT][32] = {
    "None",
    "Enigma",
    "Kosinski",
    "Nemesis",
    "Kid Chameleon"
};

/* returns pointer to position after the found string */
char* strsrch(char* string, char* substring) {
    int length = strlen(string);
    int sublength = strlen(substring);
    int position = -1;
    for(int i=0; i<=length-sublength; i++) {
        if(!strncmp(&string[i], substring, sublength)) {
            position = i;
            break;
        }
    }
    if(position < 0) return NULL;
    else return &string[position+sublength];
}

fileCompression readComprType(char* string) {
    for(int i=0; i<COMP_TYPE_AMOUNT; i++) {
        if(strsrch(string, comprTypes[i]) != NULL)
        {
            fileCompression ret;
            switch (i)
            {
                case 0:
                    ret = NONE;
                    break;
                case 1:
                    ret =  ENIGMA;
                    break;
                case 2:
                    ret =  KOSINSKI;
                    break;
                case 3:
                    ret =  NEMESIS;
                    break;
                case 4:
                    ret =  KIDCHAMELEON;
                    break;
            }
            return ret;
        }
    }
    return INVALID;
}

char* trimString(char* string) {
    string[strcspn(string, "\n")] = '\0';
    while(string[0] == ' ' || string[0] == '\n' || string[0] == '\t')
        string++;
    int i = strlen(string)-1;
    while(string[i] == ' ' || string[i] == '\n' || string[i] == '\t') {
        string[i] = '\0';
        i--;
    }
        
    return string;
}

#endif
