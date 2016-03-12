/*-----------------------------------------------------------------------------*\
|																				|
|	Kosinski.dll: Compression / Decompression of data in Kosinski format		|
|	Copyright © 2002-2004 The KENS Project Development Team						|
|																				|
|	This library is free software; you can redistribute it and/or				|
|	modify it under the terms of the GNU Lesser General Public					|
|	License as published by the Free Software Foundation; either				|
|	version 2.1 of the License, or (at your option) any later version.			|
|																				|
|	This library is distributed in the hope that it will be useful,				|
|	but WITHOUT ANY WARRANTY; without even the implied warranty of				|
|	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU			|
|	Lesser General Public License for more details.								|
|																				|
|	You should have received a copy of the GNU Lesser General Public			|
|	License along with this library; if not, write to the Free Software			|
|	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA	|
|																				|
\*-----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define SUCCESS										0x00
#define ERROR_UNKNOWN								  -2
#define ERROR_SOURCE_FILE_DOES_NOT_EXIST			  -1

//-----------------------------------------------------------------------------------------------
// Name: KDecomp(char *SrcFile, char *DstFile, long Location, bool Moduled)
// Desc: Decompresses the data using the Kosinski compression format
//-----------------------------------------------------------------------------------------------
long KosDec(const char *SrcFile, FILE *Dst, long Location, int length)
{
// Files
	FILE *Src;
	//FILE *Dst;

// Bitfield Infos
	unsigned short BITFIELD;
	unsigned char BFP;
	unsigned char Bit;

// R/W infos
	unsigned char Byte;
	unsigned char Low, High;

// Localization Infos
	int Pointer;
	
// Count and Offest
	long Count = 0;
	long Offset = 0;

// Moduled infos
	long FullSize = 0;
	long DecBytes = 0;

//------------------------------------------------------------------------------------------------

	Src=fopen(SrcFile,"rb");	
	if (Src==NULL) return ERROR_SOURCE_FILE_DOES_NOT_EXIST;
	//Dst=fopen(DstFile,"w+b");
	rewind(Dst);

	fseek(Src, Location, SEEK_SET);

	/*if (Moduled)
	{
		fread(&High, 1, 1, Src);
		fread(&Low, 1, 1, Src);
		FullSize = ((long)High << 8) + (long)Low;
	}*/

start:
	fread(&BITFIELD, 2, 1, Src);
	BFP=0;

//------------------------------------------------------------------------------------------------
	while(1)
	{
		if(BITFIELD & (1<<BFP++)) Bit=1; else Bit=0;
		if (BFP>=16) { fread(&BITFIELD, 2, 1, Src); BFP=0; }
//-- Direct Copy ---------------------------------------------------------------------------------
		if (Bit)
		{
			fread(&Byte, 1, 1, Src);
			fwrite(&Byte, 1, 1, Dst);
			DecBytes+=1;
		}
		else
		{
			if(BITFIELD & (1<<BFP++)) Bit=1; else Bit=0;
			if (BFP>=16) { fread(&BITFIELD, 2, 1, Src); BFP=0; }
//-- Embedded / Separate -------------------------------------------------------------------------
			if (Bit)
			{
				fread(&Low, 1, 1, Src);
				fread(&High, 1, 1, Src);

				Count=(long)(High & 0x07);

				if (Count==0)
				{
					fread(&Count, 1, 1, Src);
					if (Count==0) goto end;
					if (Count==1) continue;
				}
				else
				{
					Count+=1;
			    }

				Offset = 0xFFFFE000 | ((long)(0xF8 & High) << 5) | (long)Low;
			}
//-- Inline --------------------------------------------------------------------------------------
			else
			{
				if(BITFIELD & (1<<BFP++)) Low=1; else Low=0;
				if (BFP>=16) { fread(&BITFIELD, 2, 1, Src); BFP=0; }
				if(BITFIELD & (1<<BFP++)) High=1; else High=0;
				if (BFP>=16) { fread(&BITFIELD, 2, 1, Src); BFP=0; }

				Count = ((long)Low)*2 + ((long)High) + 1;

				fread(&Offset, 1, 1, Src);
				Offset|=0xFFFFFF00;
			}
//-- Write to file for indirect copy -------------------------------------------------------------
			for (int i=0; i<=Count; i++)
			{
				Pointer=ftell(Dst);
				fseek(Dst, Offset, SEEK_CUR);
				fread(&Byte, 1, 1, Dst);
				fseek(Dst, Pointer, SEEK_SET);
				fwrite(&Byte, 1, 1, Dst);
			}
			DecBytes+=Count+1;
//------------------------------------------------------------------------------------------------
		}
	}
//------------------------------------------------------------------------------------------------

end:
	/*if (Moduled)
	{
		if (DecBytes < FullSize)
		{
			do { fread(&Byte, 1, 1, Src); } while (Byte==0);
			fseek(Src, -1, SEEK_CUR);
			goto start;
		}
	}*/
	//fclose(Dst);
	fclose(Src);
	fseek(Dst, 0, SEEK_END);
	return ftell(Dst);
}

#ifdef __cplusplus
}
#endif
