//=================================================================
// ¥ WRunLooper 
// © Timothy Paustian All Rights Reserved 1999
//	 WText wrapper
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WText/overview.html>
//=================================================================
//
// Purpose:	An abstract class to encapsulate processing a number of runs and
// performing some action on each run. subclasses 
//	of this class must override PerformAction to take the required action
// on each segment.
//
// when you create an instance of this class you need to know what runs
// you want work with. 
//
// Revision:
//
//=================================================================

#include "WRunLooper.h"
#include "WTextView.h"

#include <PP_DebugMacros.h>

WRunLooper::WRunLooper(
						WTextView	*	inEngine)
:  mTE(inEngine)
{
	ValidateObject_(inEngine);
}
											
WRunLooper::~WRunLooper()
{
}

void
WRunLooper::RunLoopByOffset(
					SInt32				inStart,
					SInt32				inEnd,
					SInt32				inFindIndex)
{
	SignalIf_(inStart < 0);
	SignalIf_(inEnd < 0);
	SignalIf_(inStart > inEnd);
	ValidateObj_(mTE);
	
	SInt32	startIndex = mTE->OffsetToRun(inStart);
	SInt32	endIndex = mTE->OffsetToRun(inEnd);
	
	SignalIf_(startIndex > endIndex);
 	RunLoop(startIndex, inStart, endIndex, inEnd, inFindIndex);
}

// I had to hack this a bit because I do not have access to _WEGetIndStyle yet
// each time through the loop we have to find the run by using offsets which is
// slower											
void
WRunLooper::RunLoop(
					SInt32		inFirstRun,
					SInt32		inStartOffset,
					SInt32		inLastRun,
					SInt32		inEndOffset,
					SInt32		inFindIndex)
{
	SignalIf_(inFirstRun < 0);
	SignalIf_(inStartOffset < 0);
	SignalIf_(inLastRun < 0);
	SignalIf_(inEndOffset < 0);
	SignalIf_(inFindIndex < 0);
	SignalIf_(inFirstRun > inLastRun);
	SignalIf_(inStartOffset > inEndOffset);
	
	mFindIndex = inFindIndex;
	SInt32		textLength = mTE->GetTextLength();
	//sanity checks
	if(textLength == 0)
	{
		if(mFindIndex == LONG_MAX)
		{
			//we are not trying to find anything, therefore this
			//is not an error
			return;
		} 
		else
		{
			//we are trying to find something that does not exist
			//this is an error
			ThrowIfOSErr_(errAEEventNotHandled);
		}
	}
	
	WERunInfo	currRun;
	SInt32		currIndex = inFirstRun;
	
	while(currIndex <= inLastRun)
	{
		#if WASTE_VERSION > 0x02000000
		mTE->GetRunInfoByIndex(currIndex, currRun);
		currIndex++;
		//skip objects that are not text
		//Is this the best idea?, I think so.
		if(currRun.runObject != nil)
			continue;
		#else
		currIndex = inLastRun + 1;
		currRun.runStart = inStartOffset;
		currRun.runEnd = inEndOffset;
		#endif
		if(PerformAction(currRun, inStartOffset, inEndOffset))
			break;
		
	}	
	
	if(mFindIndex != LONG_MAX)
	{
		SInt32	modelCount = GetModelCount();
	
		//we are trying to find something
		if( modelCount == 0) 
		{
			modelCount = 1;
			//there has to be at least one of each model.
			//we know from above that there is text
			SetModelCount(1);
			//since there is only one model it must be the 
			//whole text. 
			SetModelOffsets(0, mTE->GetTextLength());
		}
		if (mFindIndex !=  modelCount)
		{
				ThrowIfOSErr_(errAEEventNotHandled);
		}
	}
}
	
bool
WRunLooper::PerformAction(							
						WERunInfo			/*inRun*/,
						SInt32			/*inStart*/,
						SInt32			/*inEnd*/)
{
	return false;
}