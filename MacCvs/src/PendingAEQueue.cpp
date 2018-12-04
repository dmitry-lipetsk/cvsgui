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
 * PendingAEQueue.cpp --- see header
 * Author : Miro Jurisic <meeroh@MIT.EDU> --- January 1998
 */

#include "PendingAEQueue.h"

#include <stdlib.h>
#include <string.h>

#include "AppConsole.h"

PendingAEQueue::PendingAEQueue ():
	mEvents (NULL),
	mEventCount (0)
{
}

PendingAEQueue::~PendingAEQueue ()
{
	if (mEvents != NULL)
		free (mEvents);
}

void
PendingAEQueue::EnqueueEvent (
	const AppleEvent&		inEvent,
	const AppleEvent&   inReply)
{
	PendingEvent*		newAlloc;
	
	if (mEvents == NULL)
		newAlloc = (PendingEvent*) malloc (sizeof (PendingEvent));
	else
		newAlloc = (PendingEvent*) realloc (mEvents, (mEventCount + 1) * sizeof (PendingEvent));
	
	if (newAlloc == NULL) {
		cvs_err("Impossible to allocate %d bytes !\n", (mEventCount + 1) * sizeof (PendingEvent));
		exit(1);
	}

	mEvents = newAlloc;
	mEvents [mEventCount].event = inEvent;
	mEvents [mEventCount].reply = inReply;
	mEventCount++;
}

void
PendingAEQueue::DequeueEvent (AppleEvent& outEvent, AppleEvent& outReply)
{
	if (mEvents != NULL) {
		mEventCount--;
		outEvent = mEvents [mEventCount].event;
		outReply = mEvents [mEventCount].reply;
		/* Always can realloc down */
		mEvents = (PendingEvent*) realloc (mEvents, mEventCount * sizeof (PendingEvent));
		if (mEventCount == 0)
			mEvents = NULL;
	}
}

bool
PendingAEQueue::IsEmpty () const
{
	return (mEventCount == 0);
}
