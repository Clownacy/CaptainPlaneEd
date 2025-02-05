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

#include "Resource.h"

#include <cassert>
#include <string.h>
#include <fstream>

#include "Common.h"
#include "PrjHndl.h"
#include "TxtRead.h"

#include "compression/KidDec.h"
#include "compression/ReadPlain.h"
#include "compression/FW_KENSC/comper.h"
#include "compression/FW_KENSC/enigma.h"
#include "compression/FW_KENSC/kosinski.h"
#include "compression/FW_KENSC/nemesis.h"
#include "compression/FW_KENSC/saxman.h"
#include "compression/FW_KENSC/rocket.h"

#define FILE_MAP_DEFAULT "MapDefault.bin"

void Resource::Save(std::istream &stream)
{
	Compress(stream);
}

long Resource::DecompressToFile()
{
	int decompressed_length = 0;
	switch (this->compression)
	{
		case comprType::COMPR_TYPE_AMOUNT:
		case comprType::INVALID:
			assert(false);
			break;
		case comprType::NONE:
			decompressed_length = ReadPlain(this->name, buffer, this->offset, this->length);
			break;
		case comprType::KID_CHAMELEON:
			// TODO: This.
			//decompressed_length = KidDec(this->name, dstfile, this->offset);
			break;
		case comprType::ENIGMA:
		case comprType::KOSINSKI:
		case comprType::MODULED_KOSINSKI:
		case comprType::NEMESIS:
		case comprType::COMPER:
		case comprType::SAXMAN:
		case comprType::ROCKET:
			std::ifstream srcfile_stream(this->name, srcfile_stream.binary);
			if (!srcfile_stream.is_open())
			{
				decompressed_length = -2;
				break;
			}
			srcfile_stream.seekg(this->offset);

			switch (this->compression)
			{
				case comprType::NONE:
				case comprType::KID_CHAMELEON:
				case comprType::COMPR_TYPE_AMOUNT:
				case comprType::INVALID:
					assert(false);
					break;
				case comprType::ENIGMA:
					enigma::decode(srcfile_stream, buffer);
					break;
				case comprType::KOSINSKI:
					kosinski::decode(srcfile_stream, buffer);
					break;
				case comprType::MODULED_KOSINSKI:
					kosinski::moduled_decode(srcfile_stream, buffer);
					break;
				case comprType::NEMESIS:
					nemesis::decode(srcfile_stream, buffer);
					break;
				case comprType::COMPER:
					comper::decode(srcfile_stream, buffer);
					break;
				case comprType::SAXMAN:
					saxman::decode(srcfile_stream, buffer);
					break;
				case comprType::ROCKET:
					rocket::decode(srcfile_stream, buffer);
					break;
			}

			decompressed_length = buffer.tellp();
			break;
	}

	return decompressed_length;
}

void Resource::Compress(std::istream &srcfile_stream)
{
	std::ofstream dstfile_stream(this->name, dstfile_stream.binary);

	switch (this->compression)
	{
		case comprType::COMPR_TYPE_AMOUNT:
		case comprType::INVALID:
			assert(false);
			break;
		case comprType::NONE:
			dstfile_stream << srcfile_stream.rdbuf();
			break;
		case comprType::KID_CHAMELEON:
			MainScreen.ShowWarning("Cannot save Kid Chameleon-compressed files.");
			break;
		case comprType::ENIGMA:
		case comprType::KOSINSKI:
		case comprType::MODULED_KOSINSKI:
		case comprType::NEMESIS:
		case comprType::COMPER:
		case comprType::SAXMAN:
		case comprType::ROCKET:

			switch (this->compression)
			{
				case comprType::NONE:
				case comprType::KID_CHAMELEON:
				case comprType::COMPR_TYPE_AMOUNT:
				case comprType::INVALID:
					assert(false);
					break;
				case comprType::ENIGMA:
					enigma::encode(srcfile_stream, dstfile_stream);
					break;
				case comprType::KOSINSKI:
					kosinski::encode(srcfile_stream, dstfile_stream);
					break;
				case comprType::MODULED_KOSINSKI:
					kosinski::moduled_encode(srcfile_stream, dstfile_stream, this->kosinski_module_size);
					break;
				case comprType::NEMESIS:
					nemesis::encode(srcfile_stream, dstfile_stream);
					break;
				case comprType::COMPER:
					comper::encode(srcfile_stream, dstfile_stream);
					break;
				case comprType::SAXMAN:
					saxman::encode(srcfile_stream, dstfile_stream, false);
					break;
				case comprType::ROCKET:
					rocket::encode(srcfile_stream, dstfile_stream);
					break;
			}

			break;
	}
}

void ResourceArt::Load(void)
{
	if (this->compression == comprType::INVALID)
		MainScreen.ShowError("Invalid art compression format: should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Kid Chameleon'\n'Comper'\n'Saxman'");

	long decompressed_length = DecompressToFile();

	if (decompressed_length == -2)
		MainScreen.ShowError("Could not find art file. Are you sure the path is correct?");
	else if (decompressed_length < 0)
		MainScreen.ShowError("Could not decompress art file. Are you sure the compression is correct?");

	this->tileAmount = decompressed_length/0x20;
}

void ResourceMap::Load(void)
{
	if (this->compression == comprType::INVALID || this->compression == comprType::KID_CHAMELEON)
		MainScreen.ShowError("Invalid map compression format: should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Comper'\n'Saxman'");

	long decompressed_length = DecompressToFile();

	if (decompressed_length == -2)
	{
		//file non-existant, blank template created
		decompressed_length =  2 * this->xSize * this->ySize;
		CheckCreateBlankFile(this->name, buffer, this->offset, decompressed_length);
		MainScreen.ShowInformation("No map file found; created blank template");
	}
	else if (decompressed_length < 0)
	{
		//file is existant but could not be decompressed
		MainScreen.ShowError("Could not decompress map file. Are you sure the compression is correct?");
	}

	if (decompressed_length < 2 * this->xSize * this->ySize)
	{
		MainScreen.ShowWarning("Specified size exceeds map size.\nField has been trimmed vertically.");
		this->ySize = (decompressed_length / this->xSize) / 2;
		if (this->ySize == 0)
			exit(1);
	}

	if (this->saveName.empty())
	{
		if (this->offset == 0)
		{
			this->saveName = this->name; //overwrite existing map
		}
		else
		{
			MainScreen.ShowInformation("This tool cannot overwrite a ROM; plane map will be saved to " FILE_MAP_DEFAULT);
			this->saveName = FILE_MAP_DEFAULT; //write to default file
		}
	}
}

ResourcePal::ResourcePal(void)
{
	// For backwards compatibility, palette is assumed to be uncompressed by default
	this->compression = comprType::NONE;
}

void ResourcePal::Load(void)
{
	if (this->compression == comprType::INVALID)
		MainScreen.ShowError("Invalid palette compression format; should be one of the following:\n\n'None'\n'Enigma'\n'Kosinski'\n'Moduled Kosinski'\n'Nemesis'\n'Kid Chameleon'\n'Comper'\n'Saxman'");

	long decompressed_length = DecompressToFile();

	if (decompressed_length == -2)
		MainScreen.ShowError("Could not find palette file. Are you sure the path is correct?");
	else if (decompressed_length < 0)
		MainScreen.ShowError("Could not decompress palette file. Are you sure the compression is correct?");

	// Create blank palette file
	std::stringstream palfilenew;
	for (int i=0; i < 0x80; ++i)
		palfilenew.put(0x0E);
	palfilenew.seekp(this->destination_offset);

	// Insert the loaded palette
	palfilenew << buffer.rdbuf();

	palfilenew.swap(buffer);
	this->buffer_size = buffer.tellp();
}
