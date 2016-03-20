#pragma once

#include "TxtRead.h"

class Resource
{
public:
	char name[128] = "";
	int offset = 0;
	int length = 0;
	comprType compression = comprType::INVALID;
	int kosinski_module_size = 0x1000;

	virtual void Load(const char* const filename) = 0;
	void Save(const char* const filename, const char* const dstfilename);

protected:
	long DecompressToFile(const char* const dstfile);
	void CompressFile(const char* const srcfile, const char* const dstfile);
};

class ResourceArt : public Resource
{
public:
	int tileAmount = 0;

	void Load(const char* const filename);
};

class ResourceMap : public Resource
{
public:
	int xSize = 0;
	int ySize = 0;
	char saveName[128] = "";

	void Load(const char* const filename);
};

class ResourcePal : public Resource
{
public:
	ResourcePal(void);
	void Load(const char* const filename);
};
