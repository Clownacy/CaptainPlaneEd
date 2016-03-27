#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

long ReadPlain(const char* const srcfile, const char* const dstfile, const long Pointer, int length)
{
    FILE* src = fopen(srcfile, "rb");
    if (src == NULL)
        return -2;

    FILE* dst = fopen(dstfile, "wb");

    if (length == 0) {
        if (Pointer == 0) {
            fseek(src, 0, SEEK_END);
            length = ftell(src);
            rewind(src);
        } else {
            fclose(src);
            fclose(dst);
            return -1;
        }
    }

    fseek(src, Pointer, SEEK_SET);
    rewind(dst);

    char* buffer = (char*) malloc(length);
    fread(buffer, length, 1, src);
    fwrite(buffer, length, 1, dst);
    free(buffer);

    fclose(src);
    fclose(dst);
    return length;
}

bool CheckCreateBlankFile(const char* const srcfile, const char* const dstfile, const long Pointer, const int length)
{
    FILE* src = fopen(srcfile, "rb");
    if (src != NULL)
    {
	//file does exist, don't overwrite
	fclose(src);
	return false;
    }

    FILE* dst = fopen(dstfile, "wb");

    //create blank file
    for (int i=0; i< Pointer + length; ++i)
        fputc(0, dst);
    fclose(dst);
    return true;
}

#ifdef __cplusplus
}
#endif
