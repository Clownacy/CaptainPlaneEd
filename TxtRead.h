#ifndef TXTREAD_H
#define TXTREAD_H

#include <cstdio>
#include <cstring>

#define COMP_TYPE_AMOUNT 5

/* compression types */
#define NONE 0
#define ENIGMA 1
#define KOSINSKI 2
#define NEMESIS 3
#define KIDCHAMELEON 4

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

int readComprType(char* string) {
    for(int i=0; i<COMP_TYPE_AMOUNT; i++) {
        if(strsrch(string, comprTypes[i]) != NULL)
            return i;
    }
    return -1;
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
