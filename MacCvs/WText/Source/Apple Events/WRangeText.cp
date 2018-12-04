//=================================================================
// ¥ CRangeText
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Routines to handle a range of text. This is a little 
// more complex since you can have two different text object types
// (although that is bad apple script).
//
// Revision:
//
//=================================================================
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#include <LModelObject.h>
#include <UAppleEventsMgr.h>
#include <UExtractFromAEDesc.h>
#include <UMemoryMgr.h>

#include "WRangeText.h"
#include "WTextView.h"

#include <PP_DebugMacros.h>

WRangeText::WRangeText(
				LModelObject	*inObject, 
				DescType startType, 
				SInt32 inStart, 
				DescType stopType, 
				SInt32 inStop)
	: WTextModel(inObject, inStart)
		
{
	//where does the model start
  	mRangeStart = inStart;
	mRangeStop = inStop;
	mModelKindStart = startType;
	mModelKindStop = stopType;
	
	InitRangeText();
	
	
}

WRangeText::~WRangeText()
{
}

void
WRangeText::InitRangeText()
{
	//Ranges cannot have submodels
	SetUseSubModelList(false);
	
	//note that I throw an error for weird text ranges like
	//Word 7 to character 58, but you're an idiot if you use such ranges.
	if(mModelKindStart != mModelKindStop) 
  		ThrowOSErr_(errAEEventNotHandled);
  		
  	SInt32	start, end, oldStart, oldEnd, notUsed;
  	GetSearchPointers(oldStart, oldEnd);
  	GetModelRange(mModelKindStart, mRangeStart, start, notUsed);
  	SetSearchPointers(oldStart, oldEnd);
  	GetModelRange(mModelKindStop, mRangeStop, notUsed, end);
  	SetSearchPointers(start, end);
  	SetPosition(mRangeStart);
}


void
WRangeText::GetModelRange(
					DescType	inModelKind,
					SInt32	&	ioPosition,
					SInt32	&	outStartOffset,
					SInt32	&	outEndOffset)
{
	//set the search pointer based on this model and remember the
	//character position
	switch(inModelKind){
		case cChar:
			if(ioPosition < 0)
			{
				ioPosition = CountChars();
			}
			SInt8	charSize = FindNthChar(nil, ioPosition);
			outStartOffset = ioPosition;
			outEndOffset = outStartOffset + charSize;
			break;
		case cWord:
			if(ioPosition < 0)
			{
				ioPosition = CountWords();
			}
			FindNthWord(nil, ioPosition, outStartOffset, outEndOffset);;
			break;
		case cLine:
			if(ioPosition < 0)
			{
				ioPosition = CountLines();
			}
			FindNthLine(nil, ioPosition, outStartOffset, outEndOffset);	
			break;
		case cParagraph:
			if(ioPosition < 0)
			{
				ioPosition = CountParagraphs();
			}
			FindNthParagraph(nil, ioPosition, outStartOffset, outEndOffset);
			break;
	}

}