/*
    Copyright (C) 2009-2011 qiuu
    Copyright (C) 2016 Clownacy

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

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
