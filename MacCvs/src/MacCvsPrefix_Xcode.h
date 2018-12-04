/*
 *  MacCvsPrefix_Dbg.h
 *  MacCvs
 *
 *  Created by Jens Miltner on Mon Nov 10 2003.
 *
 */

#define qMachO 1
#define qMacPP 1 // for pseudo
#define qMacCvsPP 1
#define qCarbon 1

#define _STD std
#define _CSTD std
#define __dest_os __mac_os_x
#define __GNU_LIBRARY__ 1

#define PP_Target_Carbon    1

#define PP_Target_Classic   0

#define TARGET_API_MAC_CARBON   PP_Target_Carbon
#define TARGET_API_MAC_OS8      PP_Target_Classic
#define TARGET_API_MAC_OSX      PP_Target_Carbon


// ------------------------------------------------------------------
//  Options
        
#define PP_Uses_PowerPlant_Namespace    0
#define PP_Supports_Pascal_Strings      1
#define PP_StdDialogs_Option            PP_StdDialogs_NavServicesOnly

#define PP_Uses_Old_Integer_Types       0
#define PP_Obsolete_AllowTargetSwitch   0
#define PP_Obsolete_ThrowExceptionCode  0
#define PP_Warn_Obsolete_Classes        1

#define PP_Suppress_Notes_221            1

#define PP_Uses_Aqua_MenuBar            TARGET_API_MAC_OSX

//	Thread Classes

#define THREAD_DEBUG					0	// Off
#define THREAD_PROFILE					0	// Off
#define THREAD_INACTIVITY				0	// Off
#define THREAD_PROTECT_TIMETASK			0	// Off

// ---------------------------------------------------------------------------
//	Networking classes

#define USE_MACTCP						TARGET_API_MAC_OS8
#define USE_OPENTPT						1

#if qCvsDebug
#       define DEBUG    1 
#       define DEBUG_INTERNAL   1
#	define PP_Debug				1
#	if 0 //!qCarbon
#		define DEBUG_NEW						DEBUG_NEW_LEAKS
#	else
#		define DEBUG_NEW						DEBUG_NEW_OFF
#	endif
#	if 0 // !qCarbon
#		define PP_DebugNew_Support				1
#	else
#		define PP_DebugNew_Support				0
#	endif
        // Define all debugging symbols
#       define Debug_Throw
#       define Debug_Signal
#else
#	define PP_Debug				0
	#define DEBUG_NEW						DEBUG_NEW_OFF
		// Not debugging, so make sure everything is off
		// (only used for final builds)
	#define PP_DebugNew_Support				0
#endif

//
// Carbon headers
#include <Carbon/Carbon.h>

#if defined(__cplusplus)
//
// PowerPlantheaders
    // Action Classes
#include <LAction.h>
#include <LUndoer.h>
#include <UTETextAction.h>
#include <UTEViewTextAction.h>
    // AppleEvent Classes
#include <LModelDirector.h>
#include <LModelObject.h>
#include <LModelProperty.h>
#include <UAppleEventsMgr.h>
#include <UExtractFromAEDesc.h>
    // Array Classes
#include <LArray.h>
#include <LArrayIterator.h>
#include <LComparator.h>
#include <LRunArray.h>
#include <LVariableArray.h>
#include <TArray.h>
#include <TArrayIterator.h>
    // Commander Classes
#include <LApplication.h>
#include <LCommander.h>
#include <LDocApplication.h>
#include <LDocument.h>
#include <LSingleDoc.h>
    // Feature Classes
#include <LAttachable.h>
#include <LAttachment.h>
#include <LBroadcaster.h>
#include <LDragAndDrop.h>
#include <LDragTask.h>
#include <LEventDispatcher.h>
#include <LListener.h>
#include <LPeriodical.h>
#include <LSharable.h>
    // File & Stream Classes
#include <LDataStream.h>
#include <LFile.h>
#include <LFileStream.h>
#include <LHandleStream.h>
#include <LStream.h>
    // Pane Classes
#include <LButton.h>
#include <LCaption.h>
#include <LCicnButton.h>
#include <LControl.h>
#include <LDialogBox.h>
#include <LEditField.h>
#include <LFocusBox.h>
#include <LGrafPortView.h>
#include <LListBox.h>
#include <LOffscreenView.h>
#include <LPane.h>
#include <LPicture.h>
#include <LPlaceHolder.h>
#include <LPrintout.h>
#include <LRadioGroupView.h>
#include <LScroller.h>
#include <LStdControl.h>
#include <LTabGroupView.h>
#include <LTableView.h>
#include <LTextEditView.h>
#include <LView.h>
#include <LWindow.h>
#include <UGWorld.h>
#include <UQuickTime.h>
    // PowerPlant Headers
#include <PP_Constants.h>
#include <PP_KeyCodes.h>
#include <PP_Macros.h>
#include <PP_Messages.h>
#include <PP_Prefix.h>
#include <PP_Resources.h>
#include <PP_Types.h>
                                    // Support Classes
#include <LClipboard.h>
#include <LFileTypeList.h>
#include <LGrowZone.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LRadioGroup.h>
#include <LString.h>
#include <LTabGroup.h>
#include <UDesktop.h>
                                    // Utility Classes
#include <UAttachments.h>
#include <UCursor.h>
#include <UDebugging.h>
#include <UDrawingState.h>
#include <UDrawingUtils.h>
#include <UEnvironment.h>
#include <UException.h>
#include <UKeyFilters.h>
#include <UMemoryMgr.h>
#include <UModalDialogs.h>
#include <UPrinting.h>
#include <UReanimator.h>
#include <URegions.h>
#include <URegistrar.h>
#include <UScrap.h>
#include <UScreenPort.h>
#include <UTextEdit.h>
#include <UTextTraits.h>
#include <UWindows.h>

//#   include "DebugNew.h"
	#define NEW new
	//#define NEW_NOTHROW(x) new(x)  // hh 981221
#endif

#define HAVE_UNISTD_H 1
#define HAVE_ERRNO_H 1
#define TIME_WITH_SYS_TIME 1
#define HAVE_SYS_TIME_H 1
#define HAVE_STRING_H 1
#define HAVE_PWD_H 1
#define HAVE_TIMEZONE 1

// waste
#define WASTE_IC_SUPPORT	0
#define	WASTE_OBJECTS		0
#define WASTE_DEBUG			qCvsDebug

#include "stdafx.h"

// adjusting for the toolbar
extern short CvsGetMBarHeight(void);
#define GetMBarHeight() CvsGetMBarHeight()

