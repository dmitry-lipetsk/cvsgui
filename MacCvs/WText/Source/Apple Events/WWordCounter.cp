//=================================================================
// ¥ WTWordCounter
// © Timothy Paustian All Rights Reserved 1999
//	 WT TextEngine
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Count the words in a given strech of runs. PerformAction
// takes each run and tests to see if it contains text. if it does 
// the words are counted. We do this run by run to avoid non-text 
// objects
//
// Revision:
//
//=================================================================

#include "WWordCounter.h"
#include "WTextView.h"

#include <UMemoryMgr.h>

WWordCounter::WWordCounter(
						WTextView	* 	inEngine)
:  WRunLooper(inEngine)
{
}
											
WWordCounter::~WWordCounter()
{
}

bool
WWordCounter::PerformAction(							
						WERunInfo			inRun,
						SInt32			inStart,
						SInt32			inEnd)
{
	SInt32	wordCount = 0;
	SInt32	segStart, segLength;
	//set up the segment start and end
	(inRun.runStart > inStart) ? segStart = inRun.runStart : segStart = inStart;
	
	Assert_(segStart >= 0);
	
	if(inRun.runEnd < inEnd)
	{
		 segLength = inRun.runEnd - segStart;
	}
	else
	{
		segLength = inEnd - segStart;
	}
	
	Assert_(segStart + segLength <= mTE->GetTextLength());
	
	SInt32	offset = segStart;
	SInt32 	segEnd = offset + segLength;
	WEEdge	theEdge = kLeadingEdge;
	SInt32	wordEnd;
	SInt32	wordStart;
	Handle	theText = mTE->GetText();
	
	StHandleLocker	lock(theText);
	
	Ptr		textPtr = *theText;
	
	while(offset < segEnd)
	{
		mTE->FindWord(offset, theEdge, wordStart, wordEnd);
		//debugging code.
		offset = wordEnd + 1;
		//if the first character is white space this is not
		//a word and we don't want to count it.
		if( !IsSpace(*(textPtr + wordStart)))
			wordCount++;
		mWordStart = wordStart;
		mWordEnd = wordEnd;
		if((mWordCount + wordCount) == mFindIndex)
		{
			mWordCount += wordCount;
			return true;
		}
	}
	
	mWordCount += wordCount;
	return false;
}

void
WWordCounter::GetModelOffsets(
					SInt32	&	outStart,
					SInt32	&	outEnd)
{
	outStart = mWordStart;
	outEnd = mWordEnd;
}

void
WWordCounter::SetModelOffsets(
					SInt32		inStart,
					SInt32		inEnd)
{
	mWordStart = inStart;
	mWordEnd = inEnd;
}