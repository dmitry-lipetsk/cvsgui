#ifndef _W_WT_DEBUG
#define _W_WT_DEBUG
#pragma once


#define DebugFindPaneByID_( View, PaneID, ClassType )			\
				(static_cast<ClassType*>(View->FindPaneByID(PaneID)))
	
#define	DebugCast_(ptr, BaseType, ResultType)					\
				(static_cast<ResultType*>(ptr))

#define	SuppressSignal_()
#define ChangeSignal_(debugAction)
#define DisposeOf_(obj)													\
	do {																\
		delete (obj);													\
		(obj) = nil;													\
	} while ( false )
	
#define DisposeOfSimple_(obj)	DisposeOf_(obj)

#define AssertHandleLocked_(h)
#define AssertHandleUnlocked_(h)
#define Assert_(h)
#define ValidateHandle_(h)
#define ValidateObject_(h)

#endif