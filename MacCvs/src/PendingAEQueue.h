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
 * PendingAEQueue.h
 * Author : Miro Jurisic <meeroh@MIT.EDU> --- January 1998
 */

#pragma once

#include <AppleEvents.h>
#include <utility>

/*	Problem: CVS library is not reentrant. However, it calls CWGUSI and SIOUX, 
		which both have their own event handlers. Those event handlers attempt
		to handle AppleEvents. Unfortunatelly, if MacCVS receives a DoScript
		AppleEvent while it is already performing a CVS operation, this will cause
		the CVS library to be reentered, resulting in corruption of its globals
		and a subsequent crash in the garbage collector.
		
		Beacuse of this, while the CVS library is loaded, MacCVS has to receive
		AppleEvents and then internally queue them for handling after the CVS library
		is unloaded. To do this, MacCVS suspends all DoScript AppleEvents received
		while the CVS library is loaded and puts them on a global queue, and the
		main event loop then handles them by dequeueing the events and resuming them,
		in the order opposite from the order in which they were received. (well, that's
		actually a stack of events then, isn't it?)
		
		Also, since enqueueing of the events happens in the AppleEvent handler, the
		event data is on the stack at that time. This data is copied to the queue of
		pending events; it is not sufficient to store only a pointer to the event
		because the stack frame that holds it is destroyed before the event is resumed.
		
		This could maybe also be fixed by making MacCVS multithreaded and handling each
		event in its own thread, if each thread instantiates its own copy of CVS library
		globals. This, however, is too much trouble. */

class PendingAEQueue {
	public:
		
		PendingAEQueue ();
		~PendingAEQueue ();
		
		void				EnqueueEvent (
								const AppleEvent&		inEvent,
								const AppleEvent&    inReply);
		void			DequeueEvent (AppleEvent& outEvent, AppleEvent& outReply);
		
		bool				IsEmpty () const;
	
	protected:
	  struct PendingEvent
	  {
	    AppleEvent    event;
	    AppleEvent    reply;
	  };
		PendingEvent*			mEvents;
		UInt32				    mEventCount;
};