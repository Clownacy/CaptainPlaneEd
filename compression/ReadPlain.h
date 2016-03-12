#pragma once

long ReadPlain(const char *srcfile, FILE* dst, long Pointer, int length);
bool CheckCreateBlankFile(char *srcfile, FILE* dst, long Pointer, int length);
