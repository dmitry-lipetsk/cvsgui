//=================================================================
// ¥ CCharModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handling the character model. Just a few modifications
// to CTextModel to get this to work right. This routine mainly 
// dissallows many action since char cannot have submodels.
//
// Revision:
//
//=================================================================
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#include <UMemoryMgr.h>

#include "WCharModel.h"
#include "WSelectionText.h"
#include "WTextView.h"

#include <PP_DebugMacros.h>

static SInt32	mCharPosition = 0;


//don't use this.
WCharModel::WCharModel()
{}

WCharModel::WCharModel(
				const WCharModel & inOriginal)
: WTextModel(inOriginal)
{
	InitCharModel(inOriginal.GetSuperModel());
}

WCharModel::WCharModel(
				LModelObject	*inObject,
				SInt32 			inPosition)
	: WTextModel(inObject, inPosition)
{
	InitCharModel(inObject);
}

WCharModel::~WCharModel()
{
}

void
WCharModel::InitCharModel(
					LModelObject *	inObject)
{
	ValidateObject_(inObject);
	
	SetModelKind(cChar);
  
  	DescType	superKind = inObject->GetModelKind();
	SInt32		start, end;
	if(superKind == cChar)//char cannot have submodels that are chars
	{
		ThrowOSErr_(errAEEventNotHandled);
	}
	else
	{
		SInt32	position = mModelPosition;
		if(position < 0)
		{
			position = CountChars();
		}
		SInt8	charSize = FindNthChar(nil, position);
		start = position;
		end = start + charSize;
		SetSearchPointers(start, end);
	}
}

WCharModel&
WCharModel::operator=(
				WCharModel & inRhs)
{
	if(this == &inRhs)
		return *this;
		
	InitCharModel(inRhs.GetSuperModel());
	return *this;
}

SInt32
WCharModel::CountSubModels(
				DescType	/*inModelID*/) const
{
	//characters cannot have submodels
	ThrowOSErr_(errAEEventNotHandled);
	return 0;
}

void
WCharModel::GetSubModelByPosition(DescType		/*inModelID*/,
								SInt32			/*inPosition*/,
								AEDesc			&/*outToken*/) const
{
	//characters cannot have submodels
	ThrowOSErr_(errAEEventNotHandled);
}