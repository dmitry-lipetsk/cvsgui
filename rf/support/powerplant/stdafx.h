#ifndef __stdafx__
#define __stdafx__

#define qMacPP 1

#if qDebug
#	include <PP_DebugHeaders.cp>
#else
#	include <PP_ClassHeaders.cp>
#endif

/* the MacAPP assert */
#define UASSERT Assert_
#define UTHROW(err) Throw_Err(mFulErr)

extern "C" char *strdup(const char *str);

#endif // __stdafx__
