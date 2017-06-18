#include "trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

FILE* TRACE_OpenLogFile(const char * filenameprefix)
{
    FILE* fLog = NULL;
    char szTime[64] = {0,};
    time_t tt = time(NULL);
    struct tm *_timetmp = NULL;
    _timetmp = localtime(&tt);
    if (NULL != _timetmp)   strftime(szTime, 32, "%Y%m%d_%H%M%S",_timetmp);

    if (NULL == fLog)
    {
        char szFile[MAX_PATH] = {0,};
        sprintf(szFile, "%s.%s.log", filenameprefix,szTime);
        fLog = fopen(szFile, "wb");
    }

	return fLog;
}

void TRACE_CloseLogFile(FILE* fLog)
{
    if (NULL != fLog)
    {
        fclose(fLog);
        fLog = NULL;
    }
}

void TRACE(char* szFormat, ...)
{
	char buff[1024] = {0,};
	va_list args;
	va_start(args,szFormat);
	#ifdef _WIN32
	_vsnprintf(buff, 1023, szFormat,args);
	#else
	vsnprintf(buff, 1023, szFormat,args);
	#endif
	va_end(args);

	printf(buff);
}

void TRACE_LOG(FILE* fLog, const char *szFormat, ...)
{
	char buff[1024] = {0,};

	va_list args;

	va_start(args,szFormat);
	#ifdef _WIN32
	_vsnprintf(buff, 1023, szFormat,args);
	#else
	vsnprintf(buff, 1023, szFormat,args);
	#endif
	va_end(args);

	if (NULL != fLog)
	{
		char szTime[64] = {0,};
		time_t tt = time(NULL);
		struct tm *_timetmp = NULL;
		_timetmp = localtime(&tt);
		if (NULL != _timetmp)	strftime(szTime, 32, "%Y-%m-%d %H:%M:%S ", _timetmp);

		fwrite(szTime, 1, (int)strlen(szTime), fLog);

		fwrite(buff, 1, (int)strlen(buff), fLog);
		fflush(fLog);
	}
}
