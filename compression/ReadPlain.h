#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

long ReadPlain(const char* const srcfile, const char* const dstfile, long Pointer, int length);
bool CheckCreateBlankFile(char *srcfile, FILE* dst, long Pointer, int length);

#ifdef __cplusplus
}
#endif
