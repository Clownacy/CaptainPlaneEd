#pragma once

#include "TxtRead.h"

class Resource
{
public:
	char name[128];
	int offset;
	int length;
	comprType compression;

	Resource(void);
	virtual void Load(const char* const filename) = 0;
	void Save(const char* const filename, const char* const dstfilename);

protected:
	long DecompressFile(const comprType compr_type, const char* const srcfile, const char* const dstfile, const long Pointer, const int length);
	void CompressFile(const comprType compr_type, const char* const srcfile, const char* const dstfile);
};

class ResourceArt : public Resource
{
public:
	int tileAmount;

	void Load(const char* const filename);
};

class ResourceMap : public Resource
{
public:
	int xSize;
	int ySize;
	char saveName[128] = "";

	void Load(const char* const filename);
};

class ResourcePal : public Resource
{
public:
	void Load(const char* const filename);
};
