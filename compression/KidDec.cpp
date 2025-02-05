/*
 * The compression format used in Kid Chameleon, which this code decompresses,
 * was originally figured out and documented by Damian Grove (Saxman727);
 * also the decompression routine was initially written by him.
 *
 * The code in this file is based on the original code, with slight adaptions
 * for the use with PlaneEd.
 */

#include "KidDec.h"

#include <fstream>
#include <vector>

int Decompress(const int address_data, const unsigned short size, std::istream &rom);
int ReadData(void);
void WriteData(const unsigned char var);
int SeekData(const int pos, const signed char seek);

int ReadShort(std::istream &stream)
{
	const auto upper = stream.get();
	const auto lower = stream.get();

	return upper << 8 | lower;
}

static std::vector<unsigned char> output_data;
static unsigned short output_pos;

long KidDec(const std::filesystem::path &srcfile, std::ostream &dstfile, const long Pointer) {
    std::ifstream rom(srcfile, rom.binary);
    if (!rom.is_open())
	return -2;

    int offset = Pointer;

    output_data.clear();
    output_pos = 0;

    int size = Decompress(offset, 0x8000, rom);

    /* write data to file */
    dstfile.write(reinterpret_cast<const char*>(output_data.data()), size);
    output_data.clear();
    
    return size;
}

int Decompress(const int address_data, const unsigned short size, std::istream &rom)
{
	//int compressed_data_start;
	//int compressed_data_size;
	int address_input_data;
	int address_stop;
	int address_key_data;
	//unsigned char *input_data;		// compressed level data
	//unsigned char *key_data;		// store all the individual 1's and 0's of key data
	unsigned short in1;	// input byte
	unsigned short in2;	// extra input byte for long references
	unsigned short in3;
	unsigned char bitpos;
	unsigned short unit;	// keep track of how many blocks we have when decompressing
	unsigned short key;		// key data
	unsigned short count;	// counter
	bool terminate;
	
	rom.seekg(address_data);
	//compressed_data_start = ftell(rom);
	address_input_data = ReadShort(rom) + address_data + 2;
	address_stop = address_input_data;
	address_key_data = rom.tellg();	// only needed if debugging
	
	unit=0;
	bitpos=0;
	terminate = false;
	for (int n=0; !terminate && unit < size && address_key_data < address_stop; n++){
		key = ReadShort(rom);
		rom.seekg(-2, rom.cur);
		unsigned short keybit = (((key << bitpos) & 0x8000) >> 15);	// get bit
		bitpos++;
		switch(keybit){
			case 1:		// direct copy
				#ifdef DEBUG
				printf("%05X %05X %X  direct          u-%04X-%04X\n", address_key_data, address_input_data, bitpos, unit, (unsigned)ftell(dump));// system("PAUSE");
				#endif
				unit++;	// increase block count
				address_key_data = rom.tellg();
				rom.seekg(address_input_data);
				//output_size++;
				//output_data = realloc(output_data, output_size);
				in1 = rom.get();
				WriteData(in1);
				//fwrite(&in1, 1, 1, dump);
				address_input_data = rom.tellg();
				rom.seekg(address_key_data);
				break;
			case 0:		// reference copy
				keybit = (((key << bitpos) & 0x8000) >> 15);
				bitpos++;
				switch(keybit){
					case 0:		// short-range reference
						n += 2;
						keybit = (((key << bitpos) & 0x8000) >> 15);
						bitpos++;
						address_key_data = rom.tellg();
						rom.seekg(address_input_data);
						in1 = rom.get();	// get source
						#ifdef DEBUG
						printf("%05X %05X %X  ref-short       u-%04X-%04X  s-%04X  c-%04X\n", address_key_data, address_input_data, bitpos, unit, (unsigned)ftell(dump), in1, keybit+2);// system("PAUSE");
						#endif
						if (in1 != 0){
							for (count=keybit+2; count > 0; count--){
								SeekData(-in1, SEEK_CUR);
								in3 = ReadData();
								SeekData(in1-1, SEEK_CUR);
								WriteData(in3);
								//fseek(dump, -in1, SEEK_CUR);
								//fread(&in3, 1, 1, dump);
								//fseek(dump, in1-1, SEEK_CUR);
								//fwrite(&in3, 1, 1, dump);
							}
						}
						else{
							unit--;
							//in3=0;
							for (count=0; count < keybit+1; count++)
								WriteData(0);
							//fwrite(&in3, 1, keybit+1, dump);
						}
						//else
						//	fwrite(&in3, 1, count, dump);
						output_data.resize(output_data.size() + (keybit+2));
						unit += (keybit+2);		// increase block count
						address_input_data++;
						rom.seekg(address_key_data);
						break;
					case 1:		// long-range reference
						n += 6;
						keybit = (((key << bitpos) & 0xE000) >> 13);
						bitpos += 3;
						count = (((key << bitpos) & 0xC000) >> 14);
						bitpos += 2;
						switch(count){
							case 3:		// large copy
								address_key_data = rom.tellg();
								rom.seekg(address_input_data);
								in1 = rom.get();		// source
								in2 = rom.get();		// number of bytes to copy
								count=in2;
								in3=0;
								#ifdef DEBUG
								printf("%05X %05X %X  ref-long-large  u-%04X-%04X  s-%04X  c-%04X\n", address_key_data, address_input_data, bitpos, unit, (unsigned)ftell(dump), in1+(keybit<<8), in2); //system("PAUSE");
								#endif
								if (count < 6) {
                                    if (count == 0) terminate = true;
								    address_input_data += 2;
								    rom.seekg(address_key_data);
                                    break;
                                }
								if (in1+(keybit<<8) != 0){
									for (; count > 0; count--){
										SeekData(-in1 - (keybit<<8), SEEK_CUR);
										in3 = ReadData();
										SeekData(in1-1 + (keybit<<8), SEEK_CUR);
										WriteData(in3);
										//fseek(dump, -in1 - (keybit<<8), SEEK_CUR);
										//fread(&in3, 1, 1, dump);
										//fseek(dump, in1-1 + (keybit<<8), SEEK_CUR);
										//fwrite(&in3, 1, 1, dump);
									}
								}
								else if (count > 0)
								{
									unit--;
									//fwrite(&in3, 1, count-1, dump);
									for (; count > 1; count--)
										WriteData(in3);
								}
								output_data.resize(output_data.size() + (in2));
								unit += (in2);		// increase block count
								address_input_data += 2;
								rom.seekg(address_key_data);
								break;
							default:	// small copy
								address_key_data = rom.tellg();
								rom.seekg(address_input_data);
								in1 = rom.get();	// source
								count += 3;			// setting up counter for writing blocks
								in2 = count;		// keeping a backup for later use
								in3=0;
								#ifdef DEBUG
								printf("%05X %05X %X  ref-long-small  u-%04X-%04X  s-%04X  c-%04X\n", address_key_data, address_input_data, bitpos, unit, (unsigned)ftell(dump), in1+(keybit<<8), in2);// system("PAUSE");
								#endif
								if (in1+(keybit<<8) != 0){
									for (; count > 0; count--){
										SeekData(-in1 - (keybit<<8), SEEK_CUR);
										in3 = ReadData();
										SeekData(in1-1 + (keybit<<8), SEEK_CUR);
										WriteData(in3);
										//fseek(dump, -in1 - (keybit<<8), SEEK_CUR);
										//fread(&in3, 1, 1, dump);
										//fseek(dump, in1-1 + (keybit<<8), SEEK_CUR);
										//fwrite(&in3, 1, 1, dump);
									}
								}
								else{
									unit--;
									//fwrite(&in3, 1, count-1, dump);
									for (; count > 1; count--)
										WriteData(in3);
								}
								output_data.resize(output_data.size() + in2);
								unit += in2;		// increase block count
								address_input_data++;
								rom.seekg(address_key_data);
								break;
						};
				};
		};
		if (bitpos > 7){
			bitpos &= 7;
			rom.seekg(1, rom.cur);
		}
		in1 = 0;
		in2 = 0;
		in3 = 0;
		keybit = 0;
		count = 0;
		//printf("\n");
	}
	
	// Fill in the rest with zeros
	/*if (unit < size){
		//in3 = 0;
		//fwrite(&in3, 1, size-unit, dump);
		for (int i=unit; i < size; i++)
			WriteData(0);
	}*/
	
	//output_data = realloc(output_data, size);
	//output_size = size;
	
	//compressed_data_size = address_input_data - compressed_data_start;
	/*printf("  COMPRESSED DATA SIZE:   %i bytes\n", compressed_data_size);
	printf("  DECOMPRESSED DATA SIZE: %i bytes\n", unit);
	printf("  RATIO:                  %3.01f %c\n", (1 - (float)compressed_data_size/(float)unit) * 100, '%');
	printf("\n");*/
	//system("PAUSE");
	//free(key_data);
	
	return unit;
}

int ReadData(void){
	if (output_pos >= output_data.size())
		return 0;
	
	output_pos++;
	return output_data[output_pos-1];
}

void WriteData(const unsigned char var){
	if (output_pos >= output_data.size()){
		output_data.resize(output_data.size() + 1);
	}
	output_data[output_pos] = var;
	output_pos++;
}

int SeekData(const int pos, const signed char seek){
	switch(seek){
		case SEEK_CUR:
			output_pos += pos;
			break;
		case SEEK_SET:
			output_pos = pos;
			break;
		case SEEK_END:
			output_pos = output_data.size() - pos;
			break;
		default:
			return 1;
	};
	
	return 0;
}
