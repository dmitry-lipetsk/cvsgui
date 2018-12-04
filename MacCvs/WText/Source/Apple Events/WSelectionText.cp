//=================================================================
// ¥ WSelectionText
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle the selection object in Apple script. Unlike the
// other text objects. This one sticks around. There is only one
// selection per document and we can afford to keep it around.
//
// Revision:
//
//=================================================================
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif

#ifndef __AEPACKOBJECT__
#include <AEPackObject.h>
#endif

#include <LModelObject.h>
#include <UAppleEventsMgr.h>
#include <UExtractFromAEDesc.h>
#include <UMemoryMgr.h>

#include "WSelectionText.h"
#include "WTextView.h"
#include "WRangeText.h"

WSelectionText::WSelectionText(
			LModelObject	*inObject)
: WTextModel(inObject, 1)
{
	SetModelKind(cSelection);
	//the selection object is this object
	mSelection = this;
	SetLaziness(false);
}

WSelectionText::~WSelectionText()
{
}


	
SInt32
WSelectionText::CountSubModels(
					DescType	inModelID) const
{
	
	SInt32	count = 0;
	
	count = WTextModel::CountSubModels(inModelID);
	
	return count;
}

//-------------------------------------------------------------------------
//	SetAEProperty
//	Purpose: Handles selections and other properties that can be set. Most of
// 	the code deals with handling specifiying a seletion. Everything else
// 	is passed up to the parent.
//
//	Revision: 10/12/96 added code to handle AppleScript like...
//	set selection to paragraph 10. There were also changes to
//	and CAEText. See comments there. I may need to change set/target stuff.
//------------------------------------------------------------------------- 
void
WSelectionText::SetAEProperty(
	DescType		inProperty,
	const AEDesc	&inValue,
	AEDesc&			outAEReply)
{
	SInt32	 	size, position;
	OSErr		anErr;
	DescType	typeCode, modelKind, relPosition;
	StAEDescriptor	insertSpec, insertObj, insertData;
	
	if(inValue.descriptorType == cInsertionLoc)
		inProperty = pInsertionLoc;
		
	if( (inProperty == pInsertionLoc) || (inProperty == pSelection)){

		if(inProperty == pInsertionLoc){
			anErr = AECoerceDesc(&inValue, typeAERecord, &insertSpec.mDesc);
			ThrowIfOSErr_(anErr);
			anErr = AEGetKeyDesc (&insertSpec.mDesc, keyAEObject, typeObjectSpecifier, &insertObj.mDesc);
			ThrowIfOSErr_(anErr);
			anErr = AECoerceDesc(&insertObj.mDesc, typeAERecord, &insertData.mDesc);
			ThrowIfOSErr_(anErr);
		} else {
			anErr = AECoerceDesc(&inValue, typeAERecord, &insertData.mDesc);
			ThrowIfOSErr_(anErr);
		}
		
		
		
		//Get the position of the starting model
		anErr = AEGetKeyPtr(&insertData.mDesc, keyAEKeyData, typeLongInteger, &typeCode, 
						&position, sizeof(long), &size);
		ThrowIfOSErr_(anErr);
		
		anErr = AEGetKeyPtr(&insertData.mDesc, keyAEDesiredClass, typeType, &typeCode, 
						&modelKind, sizeof(DescType), &size);
		ThrowIfOSErr_(anErr);
		
		anErr = AEGetKeyPtr(&insertSpec.mDesc, keyAEPosition, typeEnumerated, &typeCode, 
						&relPosition, sizeof(DescType), &size);
		
		if( (anErr == errAENotAEDesc) && (inProperty == pSelection)){
			relPosition = kAENotRelative;
		}else {
			ThrowIfOSErr_(anErr);
		}
		SInt32		selStart, selEnd;
		SetSearchPointers(0, mTE->GetTextLength());	
		switch(modelKind){
			case cChar:
				SInt8	charSize = FindNthChar(nil, position);
				selStart = position;
				selEnd =  position + charSize;
				break;
			
			case cWord:
				FindNthWord(nil, position, selStart, selEnd);
				break;
			case cLine:
				FindNthLine(nil, position, selStart, selEnd);
				break;
			case cParagraph:
				FindNthParagraph(nil, position, selStart, selEnd);
				break;
			case cProperty: //used when setting the selction after of before the selection
				mTE->GetTextSelection(selStart, selEnd);
				break; 
		}
		if(relPosition == kAEBefore){
			selEnd = selStart;
		} else if (relPosition == kAEAfter) {
			 selStart = selEnd;
		}
		mTE->SetTextSelection(selStart, selEnd);
	} else {
		mTE->GetTextSelection(mStart, mEnd);
		WTextModel::SetAEProperty(inProperty, inValue, outAEReply);
	}
}

void
WSelectionText::MakeSelfSpecifier(
						AEDesc	&inSuperSpecifier,
						AEDesc	&outSelfSpecifier) const
{
	DescType		theType;
	OSErr			err;
	//the property type is font
	theType = pSelection;
	//create the property descriptor
	StAEDescriptor	propSel(theType);
	//create the object specifier for the property. We define the selection
	//as a property
	err = ::CreateObjSpecifier(cProperty, &inSuperSpecifier, formPropertyID,
								&propSel.mDesc, false, &outSelfSpecifier);
	ThrowIfOSErr_(err);	
}