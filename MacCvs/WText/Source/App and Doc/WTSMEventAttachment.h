// ===========================================================================
//	WTSMEventAttachment.h		©1995-97 Timothy Paustian All rights reserved.
// ===========================================================================
//	An attachment to let TSM handle events

#ifndef _H_WTSMEventAttachment
#define _H_WTSMEventAttachment
#pragma once

#include "LAttachment.h"

class WTSMEventAttachment : public LAttachment
{
	public:
	
		virtual	Boolean		Execute(
								MessageT			inMessage,
								void *				ioParam);
};

#endif
