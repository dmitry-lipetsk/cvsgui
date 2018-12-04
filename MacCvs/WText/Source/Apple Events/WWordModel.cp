//=================================================================
// ¥ CWordModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle word objects in apple events.
//
// Revision:
//
//=================================================================
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#include <UAppleEventsMgr.h>

#include "WWordModel.h"
#include "WTextView.h"
#include "WSelectionText.h"

#include <UMemoryMgr.h>

 
#include <PP_DebugMacros.h>

//don't call this
WWordModel::WWordModel()
{
}

WWordModel::WWordModel(
			const WWordModel & inOriginal)
:WTextModel(inOriginal)
{
	InitWordModel(inOriginal.GetSuperModel());
}

WWordModel::WWordModel(
				LModelObject	*inObject,
				SInt32 			inPosition)
	: WTextModel(inObject, inPosition)
{
  	InitWordModel(inObject);
}

WWordModel::~WWordModel()
{
}

void
WWordModel::InitWordModel(
						LModelObject	* inObject)
{
	SetModelKind(cWord);
	//words can have submodels
	SetUseSubModelList(true);

	DescType	superKind = inObject->GetModelKind();
	SInt32		start, end;
	
	if((superKind == cChar) || (superKind == cWord))
		ThrowOSErr_(errAEEventNotHandled);

	if(mModelPosition < 0)
	{
		mModelPosition = CountWords();
	}
	FindNthWord(nil, mModelPosition, start, end);
	SetSearchPointers(start, end);
}

SInt32
WWordModel::CountSubModels(
				DescType	inModelID) const
{
	SInt32	count = 0;
	if(inModelID != cChar)
	{
		//the only submodels words can have is characters.
		ThrowOSErr_(errAEEventNotHandled);
	}
	else
	{
		count = WTextModel::CountSubModels(inModelID);
	}
	
	return count;
}

void
WWordModel::GetSubModelByPosition(
								DescType		inModelID,
								SInt32			inPosition,
								AEDesc			&outToken) const
{
	if(inModelID == cChar)
	{
		WTextModel::GetSubModelByPosition(inModelID, inPosition, outToken);
	}
	else
	{
		//the only submodels words can have is characters.
		ThrowOSErr_(errAEEventNotHandled);
	}
}