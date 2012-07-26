

/*
RealGameCode.com - Tutorials For the Real World
http://www.RealGameCode.com

LogFile.H - Sets up a static singleton to maintain a log for us.
*/

// Inclusion guards... (note: '#pragma once' will do the same job, but is Microsoft specific)
#ifndef RealGameCode_LogFile
#define RealGameCode_LogFile

class LogFile
{
private:

	// Hold the name of the file we're going to log to.
	static char strFileName[30];

public:

	// Writes the string out to the log file.
	static void Log(char *strLog);

	// Clear the contents of the log file.
	static void Clear(void);
};
#endif
