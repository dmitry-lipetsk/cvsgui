//=================================================================
// ¥ WParaCounter
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WText/overview.html>
//=================================================================
//
// Purpose:	 Count the number of paragraphs in a specified strech
// of text.
//
// Revision:
//
//=================================================================
#include <UMemoryMgr.h>
#include <PP_DebugMacros.h>
#include "WParaCounter.h"
#include "WTextView.h"


#include "waste.h"

WParaCounter::WParaCounter(
						WTextView	* 	inEngine)
: WRunLooper(inEngine)
{
}
											
WParaCounter::~WParaCounter()
{
}

	
bool
WParaCounter::PerformAction(							
						WERunInfo			inRun,
						SInt32				inStart,
						SInt32				inEnd)
{
	SInt32	paraCount = 0;
	SInt32	segLength;
	SInt32	segStart;
	SInt32	charCount = 1;
	
	//set up the segment start and end
	(inRun.runStart > inStart) ? segStart = inRun.runStart : segStart = inStart;
	
	if(inRun.runEnd < inEnd)
	{
		 segLength = inRun.runEnd - segStart;
	}
	else
	{
		segLength = inEnd - segStart;
	}
	
	//Get a reference to the the text
	Handle	theTextH = mTE->GetText();
	//lock the handle
	StHandleLocker	lock(theTextH);
	//get a pointer to where we are
	Ptr	charPtr = *(theTextH) + segStart;
	
	while(charCount <= segLength){
		if(*charPtr == '\r'){
			paraCount++;
			if(mParaCount + paraCount == mFindIndex)
			{
				//we found the paragraph we want
				//subtract one so as not to count the return character
				mParaEnd = segStart + charCount - 1;
				//This only happens when you have consecutive returns
				//in this case you want to back up one.
				if(mParaEnd == mParaStart)
					mParaStart--;
				mParaCount += paraCount;
				//break the loop
				return true;
			}
			mParaStart = segStart + charCount;
		}
		*charPtr++;
		charCount++;
	}
	
	
	if(inRun.runEnd == mTE->GetTextLength())
	{
		char lastChar = mTE->GetChar(segStart + segLength);
		if(lastChar != '\r')
		{
			//if the last character is not a return then
			//we need to add one to the paragraph count
			//you only want to do this at the end of the text
			paraCount++;
		}
	}	
	if(mParaCount + paraCount == mFindIndex)
	{
		//we found the paragraph we want
		mParaEnd = segStart + charCount - 1;
		mParaCount += paraCount;
		//break the loop
		return true;
	}

	//now add the total for this run to the 
	//total paragraph count
	mParaCount += paraCount;
	//keep looping
	return false;
}

void
WParaCounter::GetModelOffsets(
						SInt32	& 	outStart,
						SInt32	& 	outEnd)
{
	outStart = mParaStart;
	outEnd = mParaEnd;
}
void
WParaCounter::SetModelOffsets(
						SInt32		inStart,
						SInt32		inEnd)
{
	mParaStart = inStart;
	mParaEnd = inEnd;
}