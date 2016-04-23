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

#pragma once

#include "TxtRead.h"

class Resource
{
public:
	char name[128];
	int offset;
	int length;
	comprType compression;
	int kosinski_module_size;

	Resource(void);
	virtual void Load(const char* const filename) = 0;
	void Save(const char* const filename, const char* const dstfilename);

protected:
	long DecompressToFile(const char* const dstfile);
	void CompressFile(const char* const srcfile, const char* const dstfile);
};

class ResourceArt : public Resource
{
public:
	int tileAmount;

	ResourceArt(void);
	void Load(const char* const filename);
};

class ResourceMap : public Resource
{
public:
	int xSize;
	int ySize;
	char saveName[128];

	ResourceMap(void);
	void Load(const char* const filename);
};

class ResourcePal : public Resource
{
public:
	int destination_offset;

	ResourcePal(void);
	void Load(const char* const filename);
};
