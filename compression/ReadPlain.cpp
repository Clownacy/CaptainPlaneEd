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

#include "ReadPlain.h"

#include <filesystem>
#include <fstream>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

long ReadPlain(const std::filesystem::path &srcfile, std::ostream &dstfile, const long Pointer, int length)
{
    std::ifstream src(srcfile, src.binary);
    if (!src.is_open())
        return -2;

    if (length == 0) {
        if (Pointer == 0) {
            length = std::filesystem::file_size(srcfile);
        } else {
            return -1;
        }
    }

    src.seekg(Pointer);
    dstfile << src.rdbuf();

    return length;
}

bool CheckCreateBlankFile(const std::filesystem::path &srcfile, std::ostream &dstfile, const long Pointer, const int length)
{
    if (std::filesystem::exists(srcfile))
    {
	//file does exist, don't overwrite
	return false;
    }

    //create blank file
    for (int i=0; i< Pointer + length; ++i)
        dstfile.put(0);
    return true;
}
