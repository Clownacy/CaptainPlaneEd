#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

long ReadPlain(const char* const srcfile, const char* const dstfile, const long Pointer, int length);
bool CheckCreateBlankFile(const char* const srcfile, const char* const dstfile, const long Pointer, const int length);

#ifdef __cplusplus
}
#endif
