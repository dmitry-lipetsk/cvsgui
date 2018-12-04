// ===========================================================================
//	WTSMEventAttachment.cp		©1995-97 Timothy Paustian All rights reserved.
// ===========================================================================

#include "WTSMEventAttachment.h"

#ifndef __TEXTSERVICES__
#include <TextServices.h>
#endif


// ---------------------------------------------------------------------------
//		¥ Execute
// ---------------------------------------------------------------------------

Boolean
WTSMEventAttachment::Execute(
	MessageT	inMessage,
	void *		ioParam)
{
	if (inMessage == msg_Event) {
		// TSM returns true if it handled the event, in that case we want to
		// return false so that we don't dispatch the event
		return !TSMEvent((EventRecord *)ioParam);
	}

	return true;
}
