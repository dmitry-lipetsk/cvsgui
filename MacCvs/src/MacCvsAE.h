/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

#ifndef CVS_APP_AE_H
#define CVS_APP_AE_H

#include "PendingAEQueue.h"
#include <LModelDirector.h>

char *ae_getenv(const char *name);
	// getenv capture of cvs

extern PendingAEQueue	gPendingAEQueue;
	// global queue of pending AppleEvents
	// necessary because of non-reentrancy of cvs lib

// our own AE director
class LMacCVSModelDirector : public LModelDirector
{
public:
	LMacCVSModelDirector(LModelObject *inDefaultModel);

	static const long ae_MacCvsDoCommandLine;
	static const long ae_ShowPreferences;
	
		// our DoScript
	
	virtual void		HandleAppleEvent(
								const AppleEvent	&inAppleEvent,
								AppleEvent			&outReply,
#if __PowerPlant__ >= 0x02114002
								AEHandlerRefConT	inRefCon
#elif __PowerPlant__ >= 0x02100000
								UInt32				inRefCon
#else
								SInt32				inRefCon
#endif
								);
};

#endif /* CVS_APP_AE_H */
