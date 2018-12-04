//=================================================================
// ¥ CParaModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle paragraph objects in apple events.
//
// Revision:
//
//=================================================================
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#include <UAppleEventsMgr.h>
#include <UMemoryMgr.h>

#include "WParaModel.h"
#include "WSelectionText.h"
#include "WTextView.h"


#include <PP_DebugMacros.h>

WParaModel::WParaModel(
				LModelObject	*inObject,
				SInt32 			inPosition)
	: WTextModel(inObject, inPosition)
{
	InitParaModel(inObject);
}

WParaModel::~WParaModel()
{
}

void
WParaModel::InitParaModel(
						LModelObject	*inObject)
{
	ValidateObject_(inObject);
	SetModelKind(cParagraph);
  //paragraphs can have submodels
  SetUseSubModelList(true);
  
	DescType	superKind = inObject->GetModelKind();
	SInt32		start, end;

	if(superKind == cText)
	{
		if(mModelPosition < 0)
		{
			mModelPosition = CountParagraphs();
		}
		FindNthParagraph(nil, mModelPosition, start, end);
		SetSearchPointers(start, end);
	}
	else
	{
		//paragraphs can have lines, words and characters
		ThrowOSErr_(errAEEventNotHandled);
	}
}

SInt32
WParaModel::CountSubModels(
				DescType	inModelID) const
{
	SInt32	count = 0;
	
	switch( inModelID){
		case cChar:
		case cWord:
		case cLine:
			count = WTextModel::CountSubModels(inModelID);
			break;
		default:
			//the only submodels paras can have is lines, characters and words.
			ThrowOSErr_(errAEEventNotHandled);
			break;
	}
	return count;
}

void
WParaModel::GetSubModelByPosition(
								DescType		inModelID,
								SInt32			inPosition,
								AEDesc			&outToken) const
{
	switch( inModelID){
		case cChar:
		case cWord:
		case cLine:
			WTextModel::GetSubModelByPosition(inModelID, inPosition, outToken);
			break;
		default:
			//the only submodels lines can have is characters and words.
			ThrowOSErr_(errAEEventNotHandled);
			break;
	}
}