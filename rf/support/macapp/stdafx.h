#ifndef __stdafx__
#define __stdafx__

#define qMacAPP 1

#if qDebug
#	define qRangeCheck 1
#	define qReleaseBuild 0
#else
#	define qRangeCheck 0
#	define qReleaseBuild 1
#endif

#undef qRelease
#define qRelease qReleaseBuild

#define wantDebugging qDebug

#include "MacAppTypes.h"

/* the MacAPP assert */
#if qDebug
#	define UASSERT(cond) DebugAssert_AC((cond), #cond, __FILE__, __LINE__)
#else
#	define UASSERT(cond)
#endif
#define UTHROW(err) throw CException_AC(mFulErr)

extern "C" char *strdup(const char *str);

#endif // __stdafx__
