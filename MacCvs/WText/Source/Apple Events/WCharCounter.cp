//=================================================================
// ¥ WCharCounter 
// © Timothy Paustian All Rights Reserved 1999
//	 WT TextEngine
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WText/overview.html>
//=================================================================
//
// Purpose:	Count the chars in a given strech of runs. PerformAction
// takes each run and tests to see if it contains text. if it does 
// the chars in the run are counted. We split it into runs so that we
// only do the character by character processing on double byte fonts
//
//
// Revision:
//
//=================================================================

#include "WCharCounter.h"
#include "WTextView.h"

#include <UMemoryMgr.h>

WCharCounter::WCharCounter(
						WTextView	* 	inEngine)
:  WRunLooper(inEngine)
{
}
											
WCharCounter::~WCharCounter()
{
}

bool
WCharCounter::PerformAction(							
						WERunInfo			inRun,
						SInt32			inStart,
						SInt32			inEnd)
{
	SignalIf_(inStart < 0);
	SignalIf_(inEnd < 0);
	SignalIf_(inEnd < inStart);
	
	SInt16	fontNum = inRun.runStyle.tsFont;
	
	SInt16	scriptCode = ::FontToScript(fontNum);
	
	SInt32	segStart, segLength;
	//set up the segment start and end
	(inRun.runStart > inStart) ? segStart = inRun.runStart : segStart = inStart;
	
	if((inRun.runEnd) < inEnd)
	{
		 segLength = inRun.runEnd - inRun.runStart;
	}
	else
	{
		segLength = inEnd - segStart;
	}
	
	SInt32		charCount = 0;
	//test to see if we have only single byte scripts or if this 
	//particular script is single byte, then the number of characters is easy.
	if( (::GetScriptVariable(scriptCode, smScriptFlags) & (1UL << smsfSingByte)))
	{
		if(mCharCount + segLength >= mFindIndex)
		{
			mCharCount = mFindIndex;
			//the offset is before the character so subtract one
			mCharOffset = mFindIndex - 1 + inStart;
			mIsTwoByte = false;
			return true;
		}
		else
		{
			charCount += segLength;
			mCharOffset += segLength;
		}
	}
	else
	{
		//we need to walk through each character testing if it is double or single byte
		Handle	theTextH = mTE->GetText();
		StHandleLocker	lock(theTextH);
		
		Ptr			charPtr = (*theTextH) + segStart;				
		Ptr			paraEnd = charPtr + segLength;
		bool		isTwoByte;
		
		while (charPtr < paraEnd)
		{
		 	//we have to check each character and make sure we count double bytes as one
		 	//character.
		 	isTwoByte = (::CharacterByteType(charPtr, charCount, scriptCode) == smFirstByte);
		 	if(isTwoByte)
		 	{
		 		*charPtr += 2;
		 	}
		 	else
		 	{
		 		*charPtr++;
		 	}
		 	charCount++;
		 	
		 	if(charCount == mFindIndex)
		 	{
		 		mCharCount += charCount;
		 		Ptr	startPtr = (*theTextH) + segStart;
		 		mCharOffset += (charPtr - startPtr) - (isTwoByte ? 2 : 1);
		 		mIsTwoByte = isTwoByte;
		 		return true;
		 	}	
		 }
	}
	mCharCount += charCount;		
	//keep running
	return false;
}

void
WCharCounter::GetModelOffsets(
							SInt32	& inStart,
							SInt32	& inEnd)
{
	inStart = mCharOffset;
	inEnd = (mIsTwoByte ? mCharOffset + 2 : mCharOffset + 1);
}

void
WCharCounter::SetModelOffsets(
							SInt32	inStart,
							SInt32	inEnd)
{
	mCharOffset = inStart;
	mIsTwoByte = ( (inStart + 2) == inEnd);
} 