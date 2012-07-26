
#include <stdio.h>
#include <Windows.h>
#include "FileUtils.h"
#include "MemoryMgr.h"

// Reads a RAW file, which is like a texture. For height mapping, mostly.
// Don't forget to free this pointer! If you pass it a NULL pointer, the 
// function will declare it with the memory manager for you, to help you
// remember :)
bool ReadRawFile(char* strFileName, unsigned char **pData)
{

	// Make sure file opened OK
	FILE *pFile = 	fopen(strFileName, "rb");
	if (!pFile) return false;

	// Get the size of the file and put it back to the beginning
	fseek(pFile, 0, SEEK_END);
	int iSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// If you gave me a null pointer, declare it for you
	if (!*pData)
	{
		*pData = new unsigned char[iSize];

		#if _DEBUG
			MemoryManager::New(*pData, iSize, 1, BASICTYPE_CHAR, __FILE__, __LINE__);
		#endif
	}

	// Read the data and close the file
	fread(*pData, 1, iSize, pFile);
	fclose(pFile);

	return true;
}
