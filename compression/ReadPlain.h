#pragma once

#ifdef __cplusplus
extern "C" {
#endif

long ReadPlain(const char *srcfile, FILE* dst, long Pointer, int length);
bool CheckCreateBlankFile(char *srcfile, FILE* dst, long Pointer, int length);

#ifdef __cplusplus
}
#endif
