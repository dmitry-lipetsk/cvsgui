#if 0
#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>
#define fprintf myfprintf

static int myfprintf(FILE *f, const char *format, ...)
{
	va_list args;
	char mess[1024] = {'\0'};

	va_start (args, format);
	vsprintf (mess, format, args);
	va_end (args);

	OutputDebugString(mess);
	return 0;
}
#endif

#pragma warning(disable:4786)
#include "lexer.cc"
#pragma warning(default:4786)
