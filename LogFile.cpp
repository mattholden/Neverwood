
/*
RealGameCode.com - Tutorials For the Real World
http://www.RealGameCode.com

LogFile.CPP - Singleton wrapper for a log file
*/

/////////////////////////////////////////////
// Include files
	#include <stdio.h>							// For file writing functions
	#include <string.h>							// For strlen()
	#include "LogFile.h"							// Always include our own header last


//////////////////////////////////////////////
// Static members
	char LogFile::strFileName[30] = "error.log";


///////////////////////////////////////////////
// Write a string to the log.
	void LogFile::Log(char *strLog)
	{

		// Open the file and append to it. Open in ASCII mode.
		FILE *pFile = fopen(strFileName, "a+");

		// Write to the log. Insert a newline after the write to make the log look nicer.
		fwrite(strLog, int(strlen(strLog)), 1, pFile);
		char cNewLine = '\n';
		fwrite(&cNewLine, 1, 1, pFile);

		// Close the log.
		fclose(pFile);

	}

////////////////////////////////////////////////
// Clear the contents of the log file.
	void LogFile::Clear(void) 	
	{
		// Open the file without appending
		FILE *pFile = fopen(strFileName, "w");

		// Close it and save changes
		fclose(pFile);
	}
