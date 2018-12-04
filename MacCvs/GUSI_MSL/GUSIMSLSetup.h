#ifndef GUSIMSLSETUP_H
#define GUSIMSLSETUP_H

#include <Events.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GUSI_EXPORT __declspec(dllexport)

typedef long (*GusiMSLWriteConsole)(char *buffer, long n);
typedef int (*GusiMSLHandleEvent)(EventRecord *ev);

#ifdef LSH_VERSION_H_INCLUDED
#	define GusiMSLSetWriteConsole GusiLSHSetWriteConsole
#	define GusiMSLSetErrConsole GusiLSHSetErrConsole
#	define GusiMSLSetInConsole GusiLSHSetInConsole
#	define GusiMSLSetHandleEvent GusiLSHSetHandleEvent
#endif

extern GUSI_EXPORT GusiMSLWriteConsole GusiMSLSetWriteConsole(GusiMSLWriteConsole hook);
extern GUSI_EXPORT GusiMSLWriteConsole GusiMSLSetErrConsole(GusiMSLWriteConsole hook);
extern GUSI_EXPORT GusiMSLWriteConsole GusiMSLSetInConsole(GusiMSLWriteConsole hook);
extern GUSI_EXPORT GusiMSLHandleEvent GusiMSLSetHandleEvent(GusiMSLHandleEvent hook);

#ifdef __cplusplus
}
#endif

#endif
