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

#include "PrjHndl.h"

#include <cassert>
#include <string.h>
#include <fstream>

#include "Common.h"
#include "TxtRead.h"
#include "Resource.h"

ProjectData::ProjectData(const std::filesystem::path &prjtxt)
{
	tileOffset = 0;
	letterOffset = numberOffset = 0;

	std::ifstream prjfile(prjtxt, std::ios::in);

	while (!prjfile.eof())
	{
		char line[256];
		prjfile.getline(line, sizeof(line));
		const infoType info_type = readInfoType(line);
		if (info_type != infoType::INVALID)
			AssignInfo(info_type, trimString(line + strcspn(line, ":") + 1), prjtxt.parent_path());
	}

	art.Load();
	map.Load();
	pal.Load();
}

void ProjectData::AssignInfo(infoType type, const char *content, const std::filesystem::path &directory) {
	switch (type)
	{
		case infoType::INFO_TYPE_AMOUNT:
		case infoType::INVALID:
			assert(false);
			break;
		case infoType::PALETTE_FILE:
			pal.name = directory / content;
			break;
		case infoType::MAPPING_FILE:
			map.name = directory / content;
			break;
		case infoType::ART_FILE:
			art.name = directory / content;
			break;
		case infoType::PALETTE_OFFSET:
			pal.offset = strtol(content, nullptr, 0);
			break;
		case infoType::MAPPING_OFFSET:
			map.offset = strtol(content, nullptr, 0);
			break;
		case infoType::ART_OFFSET:
			art.offset = strtol(content, nullptr, 0);
			break;
		case infoType::PALETTE_LENGTH:
			pal.length = strtol(content, nullptr, 0);
			break;
		case infoType::MAPPING_LENGTH:
			map.length = strtol(content, nullptr, 0);
			break;
		case infoType::ART_LENGTH:
			art.length = strtol(content, nullptr, 0);
			break;
		case infoType::PALETTE_COMPRESSION:
			pal.compression = readComprType(content);
			break;
		case infoType::MAPPING_COMPRESSION:
			map.compression = readComprType(content);
			break;
		case infoType::ART_COMPRESSION:
			art.compression = readComprType(content);
			break;
		case infoType::PALETTE_DESTINATION_OFFSET:
			pal.destination_offset = strtol(content, nullptr, 0);
			if (pal.destination_offset >= 0x80)
				MainScreen.ShowError("Palette Destination Offset cannot be higher than 0x7F (16th entry of 4th palette line; the last palette entry)");
			break;
		case infoType::X_SIZE:
			map.xSize = strtol(content, nullptr, 0);
			break;
		case infoType::Y_SIZE:
			map.ySize = strtol(content, nullptr, 0);
			break;
		case infoType::TILE_OFFSET:
			tileOffset = strtol(content, nullptr, 0);
			break;
		case infoType::LETTER_OFFSET:
			letterOffset = strtol(content, nullptr, 0);
			break;
		case infoType::NUMBER_OFFSET:
			numberOffset = strtol(content, nullptr, 0);
			break;
		case infoType::SAVE_FILE:
			map.saveName = directory / content;
			break;
		case infoType::KOSINSKI_MODULE_SIZE:
			art.kosinski_module_size = strtol(content, nullptr, 0);
			break;
	}
}
