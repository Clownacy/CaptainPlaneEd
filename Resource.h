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

#include <filesystem>

#include "TxtRead.h"

class Resource
{
public:
	std::filesystem::path name;
	int offset;
	int length;
	comprType compression;
	int kosinski_module_size;

	Resource(void);
	virtual void Load(const std::filesystem::path &filename) = 0;
	void Save(const std::filesystem::path &filename);

protected:
	long DecompressToFile(const std::filesystem::path &dstfile);
	void CompressFromFile(const std::filesystem::path &srcfile);
};

class ResourceArt : public Resource
{
public:
	int tileAmount;

	ResourceArt(void);
	void Load(const std::filesystem::path &filename);
};

class ResourceMap : public Resource
{
public:
	int xSize;
	int ySize;
	std::filesystem::path saveName;

	ResourceMap(void);
	void Load(const std::filesystem::path &filename);
};

class ResourcePal : public Resource
{
public:
	int destination_offset;

	ResourcePal(void);
	void Load(const std::filesystem::path &filename);
};
