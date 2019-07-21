#ifndef __TRACE_H__
#define __TRACE_H__

#include <stdio.h>
#ifndef _WIN32
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#endif
#define MAX_PATH 260

extern FILE* TRACE_OpenLogFile(const char *filenameprefix);
extern void TRACE_CloseLogFile(FILE* fLog);
extern void TRACE(char* szFormat, ...);
extern void TRACE_LOG(FILE* fLog, const char* szFormat, ...);

#endif
