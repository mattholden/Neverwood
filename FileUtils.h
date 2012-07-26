
#ifndef StormForge_Shared_FileUtils
#define StormForge_Shared_FileUtils

#include <stdio.h>

// Reads a bool from a file, returning it in this manner so that it can be stored into a bool
// that we are only storing in a bit.
static bool ReadBit(FILE *pFile) { bool bTemp; fread(&bTemp, 1, 1, pFile); return bTemp; }

// Writes a bool from a file, doing so in this manner so that it can be written into a full byte
// even though we are only storing in a bit. The iData is an int to combat warnings about bit fields
// being passed in; the compiler doesn't know we're treating it as a bool.
static void WriteBit(FILE *pFile, int iData) { bool bBit = (iData != 0); fwrite(&bBit, 1, 1, pFile); }

// Reads a RAW file, which is like a texture. For height mapping, mostly.
bool ReadRawFile(char* strFileName, unsigned char **pHeightMap);



#endif