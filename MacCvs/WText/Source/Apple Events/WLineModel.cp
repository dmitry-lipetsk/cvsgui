//=================================================================
// ¥ CLineModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle lines objects in apple events. These you have to
// be careful of because they change depending upon the word wrap
// of the text. For non-word wrap text paragraphs would work just
// as well
//
// Revision:
//
//=================================================================
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#include <UAppleEventsMgr.h>
#include <UMemoryMgr.h>

#include "WLineModel.h"
#include "WTextView.h"
#include "WSelectionText.h"

#include <PP_DebugMacros.h>

						
WLineModel::WLineModel(
				LModelObject	*inObject,
				SInt32 			inPosition)
	: WTextModel(inObject, inPosition)
{
	InitLineModel(inObject);
}

WLineModel::~WLineModel()
{
}

void
WLineModel::InitLineModel(
						LModelObject	*inObject)
{
	ValidateObject_(inObject);
	SetModelKind(cLine);
	//lines can have submodels
	SetUseSubModelList(true);
   	
    DescType	superKind = inObject->GetModelKind();
	SInt32		start, end;
	if((superKind == cParagraph) || (superKind == cText))
	{
		if(mModelPosition < 0)
		{
			mModelPosition = CountLines();
		}
		FindNthLine(nil, mModelPosition, start, end);
		SetSearchPointers(start, end);
	}
	else //lines cannot have words or chars as supermodels
	{
		ThrowOSErr_(errAEEventNotHandled);
	}
}

SInt32
WLineModel::CountSubModels(
				DescType	inModelID) const
{
	SInt32	count;
	switch( inModelID){
		case cChar:
		case cWord:
			count = WTextModel::CountSubModels(inModelID);
			break;
		default:
			//the only submodels lines can have is characters and words.
			ThrowOSErr_(errAEEventNotHandled);
			break;
	}
	return count;
}

void
WLineModel::GetSubModelByPosition(
								DescType		inModelID,
								SInt32			inPosition,
								AEDesc			&outToken) const
{
	switch( inModelID){
		case cChar:
		case cWord:
			WTextModel::GetSubModelByPosition(inModelID, inPosition, outToken);
			break;
		default:
			//the only submodels lines can have is characters and words.
			ThrowOSErr_(errAEEventNotHandled);
			break;
	}
}