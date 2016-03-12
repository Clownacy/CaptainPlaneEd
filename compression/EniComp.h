/*-----------------------------------------------------------------------------*\
|																				|
|	Enigma.dll: Compression / Decompression of data in Enigma format			|
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

/*-----------------------------Enigma compressor---------------------------------------*\
| Function:
|	This program compresses a file into the "Enigma" compression format used in Sonic 1,
|	Sonic Crackes, and Knuckles Chaotix. It also supports stripping the padding off S1
|	special stage level maps.
|
| Operation:
|	S1SSCMP [infile] [outfile] <options>
|	
|	infile:		The file that contains the uncompressed level.
|	outfile:	The name of the file to create containing the compressed level.
|
|	<options>
|	-p:			Strip padding from file. Use if compressing a special stage that's in
|				the extended 16kb form.
|
|
|											Programmed by Roger Sanders (AKA Nemesis)
|											Created 6/8/2003  Last modified 14/6/2003
\*-------------------------------------------------------------------------------------*/

//#include "iostream.h"
//#include <malloc.h>
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char writebit(unsigned char bit);
short checkval(int offsetloc);
int writeinc(int vincp);
int writecommon(int vcommonp);
int writecomp(int vcomp);
int writecompinc(int vcompinc);
int writecompdec(int vcompdec);
int writecomprep(int vcomprep);
bool calc_headerval(void);


/*********************************Variabale block***************************************\
|	This contains all the global variables that are used throughout the program.
\*********************************Variabale block***************************************/
namespace
{

FILE* dst;
bool padding = false;
unsigned char bytebuffer = 0;
unsigned char *comp_pointer = 0;
//unsigned char *outpointer = 0;
short checkbuffer = 0;
//int loopcount;
//int infile;
//int outfile;
int infilelength;
int comp_offset = 0;
int waitingbits = 0;
//int result;

int incp = 0;
int commonp = 0;
int comp = 0;
int compinc = 0;
int compdec = 0;
int comprep = 0;

unsigned char packet_length_comp = 0x0B;
unsigned char bitmask_comp = 0x00;
unsigned short common_value_comp = 0x0000;
unsigned short tempval_comp = 0x0000;
short incrementing_value_comp = 0x0000;

}

/***************************************************************************************\
|
\***************************************************************************************/
long EniComp(const char *SrcFile, char *DstFile/*, bool padding*/)
{
// Magus: The original code by Nemesis was command line based, so, in order to make as few
// changes as possible I simply copied the arguments passed to the function into the variable
// that is used to receive the command line arguments
	/*char argv[3][256];
	strcpy(argv[1], SrcFile);
	strcpy(argv[2], DstFile);*/

// I prefered to initialize the following variables before starting the algorithm.
	bool done = false;
	bytebuffer = 0;
	checkbuffer = 0;
	comp_offset = 0;
	waitingbits = 0;
	incp = 0;
	commonp = 0;
	comp = 0;
	compinc = 0;
	compdec = 0;
	comprep = 0;
	packet_length_comp = 0x0B;
	bitmask_comp = 0x00;
	common_value_comp = 0x0000;
	tempval_comp = 0x0000;
	incrementing_value_comp = 0x0000;

// There starts the original code by Nemesis (all Console I/O opperations were removed)
/////////////////////////////////////////////////////////////////////////////////////////
//File handling
	/*infile = _open(argv[1], _S_IWRITE, _O_BINARY);
	if (infile < 0)
	{
//		cout << "\n\nError opening source file!";
		return -1;
	}
	outfile = _creat(argv[2], _S_IWRITE);
	result = _setmode(infile, _O_BINARY);
	result = _setmode(outfile, _O_BINARY);
	infilelength = _filelength(infile);*/
	FILE* src = fopen(SrcFile, "rb");
	fseek(src, 0, SEEK_END);
	infilelength = ftell(src);
	rewind(src);
	comp_pointer = (unsigned char *)calloc(infilelength, 0x01);
	
	dst = fopen(DstFile, "wb");
	if(dst == NULL) return -1;

/////////////////////////////////////////////////////////////////////////////////////////
//Format conversion		- This section of code removes the surrounding areas if the stage
//						- is stored in the full 80x80 block version
	/*if(padding)
	{
		_lseek(infile, 0x1000, 0);
		for(loopcount = 0; loopcount < 0x2000; loopcount += 0x80)
		{
			_lseek(infile, _tell(infile) + 0x20, 0);
			_read(infile, (pointer + (loopcount / 2)), 0x40);
			_lseek(infile, _tell(infile) + 0x20, 0);
		}
		infilelength = 0x1000;
	}
	else
	{
		_read(infile, pointer, infilelength);
	}*/
	fread(comp_pointer, infilelength, 1, src);
	fclose(src);

	tempval_comp = *(comp_pointer);
	unsigned short tempval_comp2 = 0;
	for(int loopcount = 0; loopcount < infilelength; loopcount += 2)
	{
		tempval_comp2 |= (*(comp_pointer + loopcount) << 8);
		tempval_comp2 |= *(comp_pointer + loopcount + 1);
	}

	bitmask_comp = (tempval_comp2 >> 0x0B);
	for(int loopcount = 0; loopcount < 0x0A; loopcount++)
	{
		if(((tempval_comp2 >> (0x0A - loopcount)) & 0x0001) == 0)
		{
			packet_length_comp--;
		}
		else
		{
			loopcount = 0x0A;
		}
	}

//	printf("Calculating common and increment values......");
	calc_headerval();
//	printf("\tDone!\n");
	unsigned char crapway1 = (incrementing_value_comp >> 8);
	unsigned char crapway2 = (incrementing_value_comp & 0x00FF);
	unsigned char crapway3 = (common_value_comp >> 8);
	unsigned char crapway4 = (common_value_comp & 0x00FF);
	fputc(packet_length_comp, dst);
	fputc(bitmask_comp, dst);
	fputc(crapway1, dst);
	fputc(crapway2, dst);
	fputc(crapway3, dst);
	fputc(crapway4, dst);
	/*_write(outfile, &packet_length_comp, 0x1);
	_write(outfile, &bitmask_comp, 0x1);
	_write(outfile, &crapway1, 0x1);
	_write(outfile, &crapway2, 0x1);
	_write(outfile, &crapway3, 0x1);
	_write(outfile, &crapway4, 0x1);*/
	bitmask_comp <<= 3;

/////////////////////////////////////////////////////////////////////////////////////////
//Main compression loop
//	printf("Compressing file......");
	for(;!done;)
	{
		if(comp_offset >= infilelength)
		{
			writebit(1);
			writebit(1);
			writebit(1);
			writebit(1);
			writebit(1);
			writebit(1);
			writebit(1);
			bytebuffer <<= (8 - waitingbits);
			//_write(outfile, &bytebuffer, 0x1);
			fputc(bytebuffer, dst);
			done = true;
			continue;
		}
		
		for(commonp = 0; (checkval(commonp * 2) == common_value_comp) && (commonp < 0x10); commonp++){}
		for(incp = 0; (checkval(incp * 2) == (incrementing_value_comp + incp)) && (incp < 0x10); incp++){}
		for(compinc = 0; (checkval(compinc * 2) == (checkval(0) + compinc)) && (compinc < 0x10); compinc++){}
		for(compdec = 0; (checkval(compdec * 2) == (checkval(0) - compdec)) && (compdec < 0x10); compdec++){}
		for(comp = 0; (checkval(comp * 2) == checkval(0)) && (comp < 0x10); comp++){}
		for(comprep = 0; (comprep < 0x0F) && (checkval(comprep * 2) != common_value_comp) && (checkval(comprep * 2) != incrementing_value_comp) && (checkval(comprep * 2) != checkval(comprep * 2 + 2)) && (checkval(comprep * 2) != (checkval(comprep * 2 + 2) + 1)) && (checkval(comprep * 2) != (checkval(comprep * 2 + 2) - 1)); comprep++){}

		if(incp > 0)
		{
			writeinc(incp);
			incrementing_value_comp += incp;
			continue;
		}
		if(commonp > 0)
		{
			writecommon(commonp);
			continue;
		}
		if((comp > compinc) && (comp > compdec))
		{
			writecomp(comp);
			continue;
		}
		if(compinc > compdec)
		{
			writecompinc(compinc);
			continue;
		}
		if(compdec > 1)
		{
			writecompdec(compdec);
			continue;
		}
		writecomprep(comprep);
	}

//	printf("\t\t\t\tDone!\n\n%s successfully compressed into file %s\nOriginal filesize:\t%i\nCompressed filesize:\t%i\nPercentage of original:\t%i", argv[1], argv[2], infilelength, _tell(outfile), (int)(((float)_tell(outfile) / (float)infilelength) * 100));

	//if (_tell(outfile) & 1) _lseek(outfile, 1, SEEK_CUR);
	if(ftell(dst) & 0x01) fputc(0x00, dst);

	free(comp_pointer);
	//_close(infile);
	//_close(outfile);
	fclose(dst);
	return 0;
}

/*********************************Read/Write functions**********************************\
|	These functions give a layer of abstraction over the reading and writing to and from
|	the files.
\*********************************Read/Write functions**********************************/
unsigned char writebit(unsigned char bit)
{
	waitingbits++;
	bytebuffer <<= 1;
	bytebuffer |= bit;

	if(waitingbits >= 8)
	{
		//_write(outfile, &bytebuffer, 0x1);
		fputc(bytebuffer, dst);
		waitingbits = 0;
	}
	return bit;
}

short checkval(int offsetloc)
{
	checkbuffer = (*(comp_pointer + comp_offset + offsetloc) << 8);
	checkbuffer |= *(comp_pointer + comp_offset + offsetloc + 1);
	return checkbuffer;
}

/**********************************Packet write functions*******************************\
|	These functions write the compressed packets to the output file.
\**********************************Packet write functions*******************************/
int writeinc(int vincp)
{
	vincp--;
	writebit(0);
	writebit(0);
	writebit((vincp & 0x0008) >> 3);
	writebit((vincp & 0x0004) >> 2);
	writebit((vincp & 0x0002) >> 1);
	writebit(vincp & 0x0001);
	comp_offset += ((vincp * 2) + 2);

	return 0;
}

int writecommon(int vcommonp)
{
	vcommonp--;
	writebit(0);
	writebit(1);
	writebit((vcommonp & 0x0008) >> 3);
	writebit((vcommonp & 0x0004) >> 2);
	writebit((vcommonp & 0x0002) >> 1);
	writebit(vcommonp & 0x0001);
	comp_offset += ((vcommonp * 2) + 2);

	return 0;
}

int writecomp(int vcomp)
{
	vcomp--;
	writebit(1);
	writebit(0);
	writebit(0);
	writebit((vcomp & 0x0008) >> 3);
	writebit((vcomp & 0x0004) >> 2);
	writebit((vcomp & 0x0002) >> 1);
	writebit(vcomp & 0x0001);

	for(int loopcount = 0; loopcount < 5; loopcount++)
	{
		if(((bitmask_comp << loopcount) & 0x80) != 0)
		{
			writebit((checkval(0) >> (0x0F - loopcount)) & 0x0001);
		}
	}
	for(int loopcount = 0; loopcount < packet_length_comp; loopcount++)
	{
		writebit((checkval(0) >> ((packet_length_comp - 1) - loopcount)) & 0x0001);
	}

	comp_offset += ((vcomp * 2) + 2);
	return 0;
}

int writecompinc(int vcompinc)
{
	vcompinc--;
	writebit(1);
	writebit(0);
	writebit(1);
	writebit((vcompinc & 0x0008) >> 3);
	writebit((vcompinc & 0x0004) >> 2);
	writebit((vcompinc & 0x0002) >> 1);
	writebit(vcompinc & 0x0001);
	
	for(int loopcount = 0; loopcount < 5; loopcount++)
	{
		if(((bitmask_comp << loopcount) & 0x80) != 0)
		{
			writebit((checkval(0) >> (0x0F - loopcount)) & 0x0001);
		}
	}
	for(int loopcount = 0; loopcount < packet_length_comp; loopcount++)
	{
		writebit((checkval(0) >> ((packet_length_comp - 1) - loopcount)) & 0x0001);
	}

	comp_offset += ((vcompinc * 2) + 2);
	return 0;
}

int writecompdec(int vcompdec)
{
	vcompdec--;
	writebit(1);
	writebit(1);
	writebit(0);
	writebit((vcompdec & 0x0008) >> 3);
	writebit((vcompdec & 0x0004) >> 2);
	writebit((vcompdec & 0x0002) >> 1);
	writebit(vcompdec & 0x0001);
	
	for(int loopcount = 0; loopcount < 5; loopcount++)
	{
		if(((bitmask_comp << loopcount) & 0x80) != 0)
		{
			writebit((checkval(0) >> (0x0F - loopcount)) & 0x0001);
		}
	}
	for(int loopcount = 0; loopcount < packet_length_comp; loopcount++)
	{
		writebit((checkval(0) >> ((packet_length_comp - 1) - loopcount)) & 0x0001);
	}

	comp_offset += ((vcompdec * 2) + 2);
	return 0;
}

int writecomprep(int vcomprep)
{
	vcomprep--;
	writebit(1);
	writebit(1);
	writebit(1);
	writebit((vcomprep & 0x0008) >> 3);
	writebit((vcomprep & 0x0004) >> 2);
	writebit((vcomprep & 0x0002) >> 1);
	writebit(vcomprep & 0x0001);

	for(; vcomprep >= 0; vcomprep--)
	{
		for(int loopcount = 0; loopcount < 5; loopcount++)
		{
			if(((bitmask_comp << loopcount) & 0x80) != 0)
			{
				writebit((checkval(0) >> (0x0F - loopcount)) & 0x0001);
			}
		}
		for(int loopcount = 0; loopcount < packet_length_comp; loopcount++)
		{
			writebit((checkval(0) >> ((packet_length_comp - 1) - loopcount)) & 0x0001);
		}
		comp_offset += 2;
	}
	return 0;
}

/****************************calc_headerval function************************************\
|	This function calculates the most common value occuring in the file, and the best
|	increment value to use.
\****************************calc_headerval function************************************/
bool calc_headerval(void)
{
	int comhitcount = 0;
	int inchitcount = 0;
	int comhighestcount = 0;
	int inchighestcount = 0;
	int commonoffset = 0;
	unsigned short commontemp = 0;
	unsigned short tempincval = 0;
	unsigned short lowestval = 0xFFFF;
	for(int loopcount = 0; loopcount < infilelength; loopcount += 2)
	{
		commontemp = (*(comp_pointer + commonoffset++) << 8);
		commontemp |= *(comp_pointer + commonoffset++);
		if(commontemp < lowestval)
		{
			lowestval = commontemp;
		}
	}

	for(int loopcount = lowestval; loopcount < 0x10000; loopcount++)
	{
		commonoffset = 0;
		inchitcount = 0;
		comhitcount = 0;
		tempincval = loopcount;
		for(;commonoffset < infilelength;)
		{
			commontemp = (*(comp_pointer + commonoffset++) << 8);
			commontemp |= *(comp_pointer + commonoffset++);
			if(commontemp == (unsigned short)loopcount)
			{
				comhitcount++;
			}
			if(commontemp == tempincval)
			{
				inchitcount++;
				tempincval++;
			}
			if(comhitcount > comhighestcount)
			{
				comhighestcount++;
				common_value_comp = (unsigned short)loopcount;
			}
			if(inchitcount > inchighestcount)
			{
				inchighestcount++;
				incrementing_value_comp = (tempincval - inchitcount);
			}
		}
	}
	return true;
}
