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
	ResourcePal(void);
	void Load(const char* const filename);
};
