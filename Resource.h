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
#include <istream>

#include "TxtRead.h"

class Resource
{
public:
	std::filesystem::path name;
	int offset = 0;
	int length = 0;
	comprType compression = comprType::INVALID;
	int kosinski_module_size = 0x1000;

	virtual void Load(const std::filesystem::path &filename) = 0;
	void Save(std::istream &stream);

protected:
	long DecompressToFile(const std::filesystem::path &dstfile);
	void Compress(std::istream &stream);
};

class ResourceArt : public Resource
{
public:
	int tileAmount = 0;

	void Load(const std::filesystem::path &filename);
};

class ResourceMap : public Resource
{
public:
	int xSize = 0;
	int ySize = 0;
	std::filesystem::path saveName;

	void Load(const std::filesystem::path &filename);
};

class ResourcePal : public Resource
{
public:
	int destination_offset = 0;

	ResourcePal(void);
	void Load(const std::filesystem::path &filename);
};
