//=================================================================
// ¥ CTextModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	 An implementation of the text model for apple events
// this is the base class that represents the document. Paragraphs
// lines, words and characters are subclassses of this class. Each of 
// these objects is created on the fly during the apple event request.
// It does not make sense to keep model objects around for them during
// non use.
//
// All of the utility routines for finding words, chars, lines etc.
// are kept here since many model objects in a text hierchy might want
// to use them.
//
// subclasses of CTextModel just add modifying routines to filter the
// call and adapt them to paragraphs, lines, words and chars.
//
// Revision: 3/9/98 initial creation
//
//=================================================================


#include <UAppleEventsMgr.h>
#include <PP_Messages.h>
#include <UExtractFromAEDesc.h>
#include <LException.h>
#include <LString.h>
#include <UMemoryMgr.h>

#include "WTextModel.h"
#include "WSelectionText.h"
#include "WCharCounter.h"
#include "WWordCounter.h"
#include "WParaCounter.h"
#include "WRangeText.h"
#include "WCharModel.h"
#include "WWordModel.h"
#include "WLineModel.h"
#include "WParaModel.h"
#include "WTextView.h"


#include <LCommander.h>
#include <LModelObject.h>

#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif

#ifndef __AEPACKOBJECT__
#include <AEPackObject.h>
#endif

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#include <PP_DebugMacros.h>


//=================================================================
// ¥ WTextModel
//=================================================================
//
// Purpose: This constructor is called by subclasses when they are
// created. It sets up all the necessary variable without creating 
// the loopers and the selection object.
//
// Revision: 3/10/98 initial creation
//
//=================================================================

WTextModel::WTextModel(
					LModelObject	*inSuperModel,
					SInt32			inPosition)
{
	//the supermodel is the caller, typically a WTextModel or a subclass of WTextModel
	SetSuperModel(inSuperModel);
	//we want this to be the default submodel so we can process AE here
	inSuperModel->SetDefaultSubModel(this);
	//all objects created this way should be destroyed after the AE
	SetLaziness(true);
  	//this will fail if a non WTextModel object tries to create an object this
  	//way. That is what I want.
  	WTextModel	*theTextModel = DebugCast_(inSuperModel, LModelObject, WTextModel);
	ValidateObject_(theTextModel);
	mTE = theTextModel->GetTextView();
	//grab the pointers to the models counters
	//we only want one copy of the loopers per document
	//submodels just use a pointer to the loopers that
	//were created by the parent WTextModel class
	mCharLooper = theTextModel->GetCharLoop();
	mWordLooper = theTextModel->GetWordLoop();
	mParaLooper = theTextModel->GetParaLoop();
	mSelection = theTextModel->GetSelectionObj();
	ValidateObject_(mCharLooper);
	ValidateObject_(mWordLooper);
	ValidateObject_(mParaLooper);
	
	SetPosition(inPosition);
	
	//grab the search pointers from the supermodel
	//to begin with, assume the search pointers are at least what the super
	//model has. More than likely this will change when the 
	//subclass is initialized.
	theTextModel->GetSearchPointers(mStart, mEnd);
}

//=================================================================
// ¥ WTextModel
//=================================================================
//
// Purpose: This constructor is called by the supermodel and results
// in the creation of the classes for handling the text model. 
//
// Revision: 3/10/98 initial creation
//
//=================================================================

WTextModel::WTextModel(
				LModelObject	* 	inSuperModel,
				WTextView	*	inClient)
			: mTE(inClient)
{
	ValidateObject_(inSuperModel);
	ValidateObject_(inClient);
	//the supermodel is the caller, typically the LDocument subclass
	SetSuperModel(inSuperModel);
	inSuperModel->SetDefaultSubModel(this);
	//we will be creating submodels when para, lines, words etc. are asked
	//for set this to true.
  	SetUseSubModelList(true);
	//this model kind is text
	SetModelKind(cText);

	//WASTE takes care of lines.
	mCharLooper = new WCharCounter(inClient);
	ValidateObject_(mCharLooper);
	mWordLooper = new WWordCounter(inClient);
	ValidateObject_(mWordLooper);
	mParaLooper = new WParaCounter(inClient);
	ValidateObject_(mParaLooper);
	//Create a selection model object to keep track of the selection, since
	//there is only one selection in a WASTE instance, we only need one.
	mSelection = new WSelectionText(this);
	ValidateObject_(mSelection);
	//we want cText to be the default submodel not the selection.
	SetDefaultSubModel(nil);
}

WTextModel::~WTextModel()
{
	//Test if this is the cText object. If cText is getting
	//deleted, then we need to get rid of out char, word and paragraph loopers
	//since the document is closing
	if(GetModelKind() == cText)
	{
		DisposeOf_(mCharLooper);
		DisposeOf_(mWordLooper);
		DisposeOf_(mParaLooper);
		DisposeOf_(mSelection);
	}
	else
	{
		//when the window closes. the supermodel will
		//be gone, check for it.
		LModelObject *	theSuper = GetSuperModel();
		if(theSuper != nil)
		{
			DescType theType = theSuper->GetModelKind();
			
			//When the supermodel is a selection, after we are done messing
			//around we want to restore the original selection for future calls
			if(theType == cSelection)
				mTE->SetTextSelection(mStartPosition, mEndPosition);
		}
	}
}

//==============================================================================
//	CountSubModels
//==============================================================================
//
// This is used by LModelObject for two purposes. First if a count event is received
// This routine will count all the submodel of inModelID type. Second it is used when
// a request for the last subModel is asked for (get last character). LModelObject
// counts them up, and then gets the last one.
//
// mEnd and mStart are pointers that point to the area of text that we are intersted 
// in. If you write AppleScript like, "get paragraph 2 of document 1" the pointers will
// point to all the text (mStart = 0, mEnd = TextLength). However, it is also possible 
// to refer to things like, "get word 1 of paragraph 2 of document 1". In this case mStart 
// would point to the begining of paragraph 2 and mEnd would point to the end of paragraph 2.
//==============================================================================

SInt32
WTextModel::CountSubModels(DescType	inModelID) const
{
	SInt32	count = 0;
	
	switch (inModelID)
	{
		case cChar:
			//a routine below. It is not just the bytes, since some scripts
			//have 2 byte characters.
			count = CountChars();
			break;
		
		case cWord:
			//a routine below
			count = CountWords();
			break;
		
		case cLine:
			//a routine below
			count = CountLines();
			break;
		
		case cParagraph: 
			//a routine below
			count = CountParagraphs();
			break;
		
		case cSelection:
			//there is only one selection object
			count = 1;
			break;
		default:
			count = mSuperModel->CountSubModels(inModelID);
			break;
	}

	return count;
}

//=================================================================
// ¥ CountChars
//=================================================================
//
// Purpose: Count the characters in the text. This was added to take
// account of twobyte scripts.
// 
// Revision: 7/27/97 initial creation
//
//=================================================================
SInt32
WTextModel::CountChars()const
{
	//set the word count to zero
	mCharLooper->SetModelCount(0);
	
	//now fire off the word count
	mCharLooper->RunLoopByOffset(mStart, mEnd);
	
	return mCharLooper->GetModelCount();
}


//=================================================================
// 	CountWords
// A word is defined as a series of characters separated by a space, a return
// or a tab character. This funtion scans for these and increments a counter when
// one is observe. Extra white space between words is ignored
//=================================================================

SInt32
WTextModel::CountWords() const
{
	//we have to count each run separately and ignore 
	//objects that are not text. We can use the function
	//defined in WTStyle::IsText to test this. Then get the text
	//We use a run looper to do this.
	//set the word count to zero
	mWordLooper->SetModelCount(0);
	
	//now fire off the word count
	mWordLooper->RunLoopByOffset(mStart, mEnd);
	
	return mWordLooper->GetModelCount();
}

SInt32
WTextModel::CountLines() const
{
	SInt32	startLine;
	SInt32 	endLine;
	
	startLine = mTE->OffsetToLine(mStart);
	endLine = mTE->OffsetToLine(mEnd);
	
	return endLine - startLine + 1;
}
//=================================================================
// 	CountParagraphs
//=================================================================
// We want to define a paragraph when ever a return character is seen. This function 
// scans for returns and increments a counter when it sees one. Note that it does not
// skip extra paragraphs.
//=================================================================

SInt32
WTextModel::CountParagraphs() const
{	
	mParaLooper->SetModelCount(0);
	mParaLooper->RunLoopByOffset(mStart, mEnd);
	return mParaLooper->GetModelCount();	
}

void
WTextModel::GetModelToken(
	DescType		inModelID,
	DescType		inKeyForm,
	const AEDesc	&inKeyData,
	AEDesc			&outToken) const
{
	bool callLModel = true;
	if(inModelID == cProperty)
	{
		if(inKeyData.descriptorType == cType)
		{
			LStr255	theString;
			SInt32	dataSize = 255;
			#if TARGET_API_MAC_CARBON == 1
			AEGetDescData(&inKeyData, theString.TextPtr(), dataSize);
			#else
			{
			StHandleLocker	lock(inKeyData.dataHandle);
			theString.Assign(*(inKeyData.dataHandle), ::GetHandleSize(inKeyData.dataHandle));
			}
			#endif
			if(theString == "\pcsel")
			{
				PutInToken(mSelection, outToken);
				callLModel = false;
			}
		}	
	}
	if(callLModel)
		LModelObject::GetModelToken(inModelID, inKeyForm, inKeyData, outToken);
}
//=====================================================================
// 	GetModelProperty
//=====================================================================
// This is called when a pSelection property is wanted. mSelection
// is returned and that is used as the token to resolve the apple
// event
//=====================================================================

LModelObject*
WTextModel::GetModelProperty(DescType inProperty) const
{
	if(inProperty == pSelection)
		return mSelection;
	else
		return LModelObject::GetModelProperty(inProperty);
}
		
		
//=====================================================================
//	GetPositionOfSubModel
//=====================================================================
// The thinking here is to remember the position of the last submodel
// that was created. Then return it when it was called. We can't find
// the model because it was destroyed after the event (a lazy object)
//=====================================================================

SInt32
WTextModel::GetPositionOfSubModel(	DescType			inModelID,
									const LModelObject	*inSubModel) const
{
	#pragma unused(inModelID)
	#pragma unused(inSubModel)
	return mModelPosition;
}

//=====================================================================
//	GetSubModelByPosition
//=====================================================================
//	It would be inefficient to have all sorts of model objects for characters, words,
//	line etc. hanging around, so we create a LModelObject subclass on the fly here, 
//	saving the position that we actually want. Later in the call, this ModelObject 
//	will get the apple event and we handle it from there. The ModelObject is destroyed 
//	at the end of the apple event (It's Lazy)
//
//	This brings up a point about how PPlant handles AppleEvents. It first resolves the 
//	ModelObject that should handle the event, then it sends that object the apple
//	event. Some important information about the apple event may come to you in the
//	first phase (during resolution) and this is often the time to save that info.
//	
//	CModelText is a ModelObject that handles characters, words, lines and paragraphs.
//	I implemented it this way since the routines to handle them were very similar.
//
//	GetSubModelByPosition just figures out what type of submodel is being asked for
//	and passes that information onto subclasses, which do all the work. 
//=====================================================================
void
WTextModel::GetSubModelByPosition(
								DescType		inModelID,
								SInt32			inPosition,
								AEDesc			&outToken) const
{
	switch (inModelID)
	{
		case cChar:
		{
			WCharModel * charModel = new WCharModel((LModelObject*)this, inPosition);
			ValidateObject_(charModel);
			PutInToken(charModel, outToken);
			break;
		}
		case cWord:
		{
			WWordModel * wordModel = new WWordModel((LModelObject*)this, inPosition);
			ValidateObject_(wordModel);
			PutInToken(wordModel, outToken);
			break;
		}
		case cLine:
		{
			WLineModel * lineModel = new WLineModel((LModelObject*)this, inPosition);
			ValidateObject_(lineModel);
			PutInToken(lineModel, outToken);
			break;
		}
		case cParagraph:
		{
			WParaModel * paraModel = new WParaModel((LModelObject*)this, inPosition);
			ValidateObject_(paraModel);
			PutInToken(paraModel, outToken);
			break;
		}
		case cSelection:
			//their is only one selection object for the text, mSelection, so
			//pass it back.
			ValidateObject_(mSelection);
			PutInToken(mSelection, outToken);
			break;
		
		case cText:
			PutInToken((LModelObject*)this, outToken);
			break;
			
		default:
			LModelObject::GetSubModelByPosition(inModelID, inPosition, outToken);
			break;
	}
}
//=====================================================================
//	GetSubModelByComplexKey
//=====================================================================
// Right now this routine handles only ranges. I hope to someday add formWhose. The type
// and position of the start and end descriptors is coerced out of the inKeydata, and
// then this is passed to CRangeText, which handles ranges.
//===================================================================== 
void
WTextModel::GetSubModelByComplexKey(
					DescType		 inModelID,
					DescType		 inKeyForm,
					const AEDesc&	 inKeyData,
					AEDesc&			 outToken) const
{
	DescType		 stopType;
	SInt32			start, stop;
	//we only handle formRange complex keys
	if(inKeyForm != formRange)
		return; 
	//extract the Range Information that is needed
	GetRangeInfo(inKeyData, &start, &stopType, &stop);
	//create the RangeText ModelObject that will handle the Range.
	WRangeText	*rangeModel = new WRangeText((LModelObject*)this, inModelID, start, stopType, stop);
	PutInToken(rangeModel, outToken);
}

//=====================================================================
//	GetRangeInfo or fun with object specifiers.
//=====================================================================
// This routine extracts the the start position, stop position, and stop type for 
// the range of interest. The formRange descripter is basically to object specifiers
// that contain the model kind and position of the model.
// Point of interest, the ObjSpecifier has to be coerced to an AERecord before being
// extracted
////=====================================================================

void
WTextModel::GetRangeInfo(
			const AEDesc inKeyData, 
			SInt32 *outStart, 
			DescType *outStopType, 
			SInt32 *outStop) const
{
	StAEDescriptor	startObj, startRange, stopObj, stopRange, rangeList;
	DescType		typeCode, stopType;
	SInt32			start, stop, size;
	
	//coerce the range descriptor in inKeyData to an AERecord so we can pass it to AEGetKeyDesc
	OSErr anErr = AECoerceDesc(&inKeyData, typeAERecord, &rangeList.mDesc);
	ThrowIfOSErr_(anErr);
	//Pull out the first object specifier that lists the start object and position
	anErr = AEGetKeyDesc (&rangeList.mDesc, keyAERangeStart, typeObjectSpecifier, &startObj.mDesc);
	ThrowIfOSErr_(anErr);
	//coerce the start specifier to an AERecord
	anErr = AECoerceDesc(&startObj.mDesc, typeAERecord, &startRange.mDesc);
	ThrowIfOSErr_(anErr);
	//Get the position of the starting model
	anErr = AEGetKeyPtr(&startRange.mDesc, keyAEKeyData, typeLongInteger, &typeCode, 
						&start, sizeof(long), &size);
	ThrowIfOSErr_(anErr);
	*outStart = start;
	
	//Get the stop object specifier
	anErr = AEGetKeyDesc (&rangeList.mDesc, keyAERangeStop, typeObjectSpecifier, &stopObj.mDesc);
	ThrowIfOSErr_(anErr);
	//Coerce the object specifier to an AERecord
	anErr = AECoerceDesc(&stopObj.mDesc, typeAERecord, &stopRange.mDesc);
	ThrowIfOSErr_(anErr);
	//Get the stop position out of the object specifier
	anErr = AEGetKeyPtr(&stopRange.mDesc, keyAEKeyData, typeLongInteger, &typeCode, 
						&stop, sizeof(long), &size);
	ThrowIfOSErr_(anErr);
	*outStop = stop;
	
	//Get the desired class for the stop
	anErr = AEGetKeyPtr(&stopRange.mDesc, keyAEDesiredClass, typeType, &typeCode, 
						&stopType, sizeof(DescType), &size);
	ThrowIfOSErr_(anErr);
	*outStopType = stopType;
} 

//-------------------------------------------------------------------------
//	GetAllSubModels
//	Purpose: The user asked for all the text. This is the only time
//	that this is called. In reality, there are no subModels, we will
//	just pass back a reference to the this object, which will know what
//	to do in GetAEProperty.
//
//	Revision: 6/20/96 - initial code
//------------------------------------------------------------------------- 
void
WTextModel::GetAllSubModels(
	DescType		inModelID,
	AEDesc			&outToken) const
{
	#pragma unused(inModelID)
	
	PutInToken((LModelObject*)this, outToken);
}

//=====================================================================**
//	FindNthChar
//=====================================================================
// Set a pointer to the text and move it to the appropriate character as
// defined by inPosition. This routine assumes the handle is valid if not
// nil. If the handle is nil, nothing is inserted into it. 
//
// On exit inPosition contains the byte offset of the character. The routine
// returns the size of the character also. 
//
// this should work with two byte scripts.
//=====================================================================

SInt8
WTextModel::FindNthChar(
		Handle outCharH, 
		SInt32 & ioPosition) const
{
	//set the word count to zero
	mCharLooper->SetModelCount(0);
	mCharLooper->SetModelOffsets(0, 1);
	
	//now fire off the word count
	mCharLooper->RunLoopByOffset(mStart, mEnd, ioPosition);
	
	SInt8		charSize = 1;
	SInt32	start, end;
	mCharLooper->GetModelOffsets(start, end);
	
	if(mCharLooper->GetIsTwoByte())
	{
		charSize = 2;
	}
	if(ioPosition - charSize > start + mStart)
		ThrowIfOSErr_(errAEEventNotHandled);
	
	ioPosition = start;
	
	if(outCharH != nil)
	{
		mTE->StreamRange(ioPosition, ioPosition + charSize, kTypeText, 0, outCharH);
	}
	return charSize;
}

//=====================================================================
//	FindNthWord
//=====================================================================
//	A word is defined as any set of characters separated by a space, excluding
//	multiple spaces of course. 
//=====================================================================

void
WTextModel::FindNthWord(
			Handle outWordH, 
			SInt32 inPosition, 
			SInt32 &outStart, 
			SInt32 &outEnd) const
{
	//set the word count to zero
	mWordLooper->SetModelCount(0);
	//The model offsets must be at least the begining
	//of the place we are looking
	mWordLooper->SetModelOffsets(mStart, mStart);
	
	//now fire off the word count
	mWordLooper->RunLoopByOffset(mStart, mEnd, inPosition);
	
	//record the offsets
	mWordLooper->GetModelOffsets(outStart, outEnd);
	//return the position relative to the whole text
	
	//fill the handle if not nil
	if(outWordH != nil)
	{
		ValidateHandle_(outWordH);
		mTE->StreamRange(outStart, outEnd, kTypeText, 0, outWordH);
	}		
}
//=====================================================================
//	FindNthLine
//=====================================================================
// Finds the line by using WTTextEngine fucntions. Then copy that offset if necessary
//=====================================================================

void
WTextModel::FindNthLine(
			Handle outLineH, 
			SInt32 inNumber, 
			SInt32& outStart, 
			SInt32& outEnd) const
{
	SInt32	lineStart;
	
	//find the line that is the begining of the text, if we
	//are talking about a submodel, mStart will be somewhere in the 
	//text
	lineStart = mTE->OffsetToLine(mStart);
	//add this to the number that we want. Subtract one
	//since mStart is line number 1 not line 0
	lineStart += inNumber - 1;
	mTE->GetLineRange(lineStart, outStart, outEnd);
	
	if(outLineH != nil)
	{
		ValidateHandle_(outLineH);
		mTE->StreamRange(outStart, outEnd, kTypeText, 0, outLineH);
	}
}

//==============================================================================
//	FindNthParagraph.
//==============================================================================
// Find the paragraph that we want. Similar strategy to FindNthWord,using two pointers
// to the start and end of the word and recording the offsets.
//==============================================================================

void
WTextModel::FindNthParagraph(
			Handle outParaH, 
			SInt32 inNumber, 
			SInt32 & outStart, 
			SInt32 & outEnd) const
{
	//set the para count to zero
	mParaLooper->SetModelCount(0);
	//the paragraph must start at mStart 
	//we are restricted to this range
	mParaLooper->SetModelOffsets(mStart, mStart);
	//now fire off the word count
	mParaLooper->RunLoopByOffset(mStart, mEnd, inNumber);
	
	//record the offsets
	mParaLooper->GetModelOffsets(outStart, outEnd);
	//outStart += mStart;
	//outEnd += mStart;
	//fill the handle if not nil
	if(outParaH != nil)
	{
		ValidateHandle_(outParaH);
		mTE->StreamRange(outStart, outEnd, kTypeText, 0, outParaH);
	}
}

//=====================================================================
// SetSearchPointers
//==============================================================================
//
// Set and get the search pointers. Used by submodels to restrict the text
// used in Count and Find routines. This is needed for AppleScript such as
// get word 1 of line 5 of paragraph 7 of document "Trials" 
//==============================================================================
// note: I may get into trouble because I made the search pointers static.
// what happens when you switch from one text instance to another? test this.
void
WTextModel::SetSearchPointers(
					SInt32	start, 
					SInt32 	end)
{
	SInt32	textLength = mTE->GetTextLength();
	
	if(end > textLength)
		end = textLength;
	
	Assert_(end >= start);
	mStart = start;
	mEnd = end;
	
	
}

void
WTextModel::GetSearchPointers(
						SInt32	& 	start, 
						SInt32	&	end) const
{
	start = mStart;
	end = mEnd;
}

//==============================================================================*
//	HandleAppleEvent
//==============================================================================
// The text model object can handle the count, GetData, GetDataSize, SetData
// Select, Cut, Copy, Paste, and Delete events. Most of the functionality is in
// other routines. If the event is not handled, It is passes to the superModel, in
// this case, The LDocument subclass, CWasteTextDoc
//==============================================================================

void
WTextModel::HandleAppleEvent(
	const AppleEvent	&inAppleEvent,
	AppleEvent			&outAEReply,
	AEDesc				&outResult,
	long				inAENumber)
{
	
	switch(inAENumber)
	{
		case ae_CountElements:
		{
			StAEDescriptor	classDesc(inAppleEvent, keyAEObjectClass, typeType);
			DescType		classType;
			UExtractFromAEDesc::TheType(classDesc, classType);
			HandleCount(classType, outResult);
			break;
		}
		case ae_GetData:
		case ae_GetDataSize:
			HandleGetData(inAppleEvent, outResult, inAENumber);
			break;

		case ae_SetData:
			HandleSetData(inAppleEvent, outResult);
			break;

		case ae_Select:
			//This is the cText model object. If it receives a select
			//message, all the text should be selected 
			HandleSelect();
			break;
		case ae_Cut:
			mTE->FocusDraw();
			mTE->Cut();
			SetSearchPointers(0, LONG_MAX);
			break;
		case ae_Copy:
			mTE->FocusDraw();
			mTE->Copy();
			break;
		case ae_Paste:
			mTE->FocusDraw();
			mTE->Paste();
			SetSearchPointers(0, LONG_MAX);
			break;
		case ae_Delete:
			HandleDelete(outAEReply, outResult);
			break;
		case ae_Print:
			mTE->ProcessCommand(cmd_PrintOne);
			break;
		default:
			mSuperModel->HandleAppleEvent(inAppleEvent, outAEReply, outResult, inAENumber);
			break;
	}//end switch inAENumber
}

void
WTextModel::HandleSelect()
{
	mTE->FocusDraw();
	mTE->SetTextSelection(mStart, mEnd);
}

void
WTextModel::HandleDelete(	
					AppleEvent&	/* outAEReply */,
					AEDesc&		/* outResult */)
{
	mTE->FocusDraw();
	mTE->SetTextSelection(mStart, mEnd); 
	mTE->Delete();
	
	mEndPosition -= (mEnd - mStart);
	mEnd = mStart;
}

//=====================================================================*
// HandleGetData
//==============================================================================
//
// Get in the text model mean returning the text or some property of it.
// This routine returns the contents of the text. If the size is requested. Then 
// the size of the Handle is returned instead
//==============================================================================

void
WTextModel::HandleGetData(
	const AppleEvent	&inAppleEvent,
	AEDesc				&outResult,
	long				inAENumber)
{
									// Find requested type for the data
									// This parameter is optional, so it's OK
									//   if it's not found
									
	DescType		theProperty = pContents;
	StAEDescriptor	requestedType;
	requestedType.GetOptionalParamDesc(inAppleEvent, keyAERequestedType, typeAEList);
	
	
	GetAEProperty(theProperty, requestedType, outResult);
		
	if (inAENumber == ae_GetDataSize) {
									// For GetDataSize, size of Property
									//   is the result
		SInt32	theSize;
		#if		TARGET_API_MAC_CARBON == 1
		theSize = ::AEGetDescDataSize(&outResult);
		#else
		theSize = ::GetHandleSize(outResult.dataHandle);
		#endif
		
		::AEDisposeDesc(&outResult);
		outResult.dataHandle = nil;
		OSErr err = ::AECreateDesc(typeLongInteger, &theSize, sizeof(SInt32),
								&outResult);
		ThrowIfOSErr_(err);
	}
}

//============================================================================
// HandleSetData
//============================================================================
// Set is only called if we are setting the contents of the text to something
// else. Setting all other properties is handled by LModelProperty, which then 
// calls SetAEProperty below.
//============================================================================

void
WTextModel::HandleSetData(const AppleEvent	&inAppleEvent,
						AppleEvent			&outAEReply)
{
	StAEDescriptor	value, mode;			// Extract value to which to set
	
	//try 
	//{
		SetDebugThrow_(debugAction_Nothing);
		value.GetParamDesc(inAppleEvent, keyAEData, typeText);
		SetDebugThrow_(debugAction_Alert);
		SetAEProperty(pContents, value, outAEReply);
	/*}
	catch(LException & theErr)
	{
		if(theErr.GetErrorCode() == errAECoercionFail)
		{
			value.GetParamDesc(inAppleEvent, keyAEData, wtTypeNative);
			SetAEProperty(pNative, value, outAEReply);
		}
		else
		{
			throw(theErr);
		}
	}*/
}

//=============================================================================
// SelectAll
//==============================================================================
// For recording the selection of all the text
//==============================================================================

void
WTextModel::SelectAll()
{
	if(mSelection != nil)
	{
		StAEDescriptor		selEvt;
		SendEventToSelf(kAEMiscStandards, kAESelect, selEvt);
		StAEDescriptor aeReply;
	
		AESendMode sendModifiers = kAEWaitReply;
		ThrowIfOSErr_(::AESend(&selEvt.mDesc, &aeReply.mDesc, sendModifiers, kAENormalPriority,
							kAEDefaultTimeout, nil, nil));
	}
	else
		mTE->SelectAll();
}

//=====================================================================
//	DoSetStyle
//==============================================================================
// Setting the style property is the most complex of the text properties. WTextModel
// handles it by defining an AERecord that contains the style enums, bold, italic,
// underline, etc. This is what gets sent to this method. The routine extracts each 
// AEDesc in the AERecord, finds out what it represents and then modified the style
// property accordingly.
//==============================================================================

void
WTextModel::DoSetStyle(
				AEDesc inValue)
{
	Style				theFace = 0;
	//this isn't very general!
	TextStyle		theStyle;
	StAEDescriptor	onStyles, style[7]; 
	DescType	type;
	SInt32		numItems = 0, i;
	bool		doToggle = false;
	OSType		styleEnum;
	
	//first coerce it to a list
	OSErr anErr = ::AECoerceDesc(&inValue, typeAEList, &onStyles.mDesc); 
	ThrowIfOSErr_(anErr);
	
	//now count the items
	anErr = ::AECountItems(&onStyles.mDesc, &numItems);
	ThrowIfOSErr_(anErr);

	for(i = 0; i < numItems ; ++i){
		//Grab each desc
		anErr = AEGetNthDesc(&onStyles.mDesc, (i + 1), typeEnumerated, &type, &style[i].mDesc);
		ThrowIfOSErr_(anErr);
		//Extract the Enumeration from the desc
		UExtractFromAEDesc::TheEnum(style[i], styleEnum);
		//find out what the enum is and set theFace accordingly
		switch (styleEnum){
			case kAEBold:
				theFace |= 1;
				break;
			case kAEItalic:
				theFace |= (1 << 1);
				break;
			case kAEUnderline:
				theFace |= (1 << 2);
				break;
			case kAEOutline:
				theFace |= (1 << 3);
				break;
			case kAEShadow: 
				theFace |= (1 << 4);
				break;
			case kAECondensed:
				theFace |= (1 << 5);
				break;
			case kAEExpanded:
				theFace |= (1 << 6);
				break;
			case typeTrue:
				//This is a special case. If true is in the list, then that
				//means we want to toggle the face
				doToggle = true;
				break;
		}//end switch
	}//end for
	theStyle.tsFace = theFace;
	if(doToggle)
		SetStyle(weDoFace + weDoToggleFace, theStyle);
	else
		SetStyle(weDoFace, theStyle);
}

//==============================================================================
//	¥SetParaStyle
//==============================================================================
// Purpose: Right now this only sets the alignment of the paragraph. As I get 
// more time I will add more utility to this.
//
// Revision:
//==============================================================================

#if WASTE_VERSION > 0x02000000

void
WTextModel::SetParaStyle(
					AEDesc	inValue)
{
	OSType	justification;
	UExtractFromAEDesc::TheEnum(inValue, justification);
	WERuler			theParaStyle;
	WERulerMode		theMode = weDoAlignment;

	switch(justification)
	{
		case kAELeftJustified:
			theParaStyle.alignment = weFlushLeft;
			break;
		
		case kAERightJustified:	
			theParaStyle.alignment = weFlushRight;
			break;
		
		case kAEFullyJustified:	
			theParaStyle.alignment = weJustify;
			break;
		
		case kAECentered:
			theParaStyle.alignment = weCenter;
			break;
			
		default:
			theParaStyle.alignment = weFlushDefault;
			break;
	}
	
	mTE->AESetRuler(theMode, theParaStyle, false);
}
#endif

void
WTextModel::SetStyle(
					WEStyleMode		inMode,
					TextStyle	inStyle)
{
	mTE->FocusDraw();
	//call the WTextView set ruler function,
	//but tell it not to send and appleevent
	//this fucntion got called becase of an apple
	//event and we don't want a circular loop
	mTE->AESetStyle(inMode, inStyle, false);
	
}
//=====================================================================
//  AESetStyle
//==============================================================================
//  Prepares an apple event that sets the style of the selected text and then
// sends it. We send an Apple Event to ourselves for recording purposes
//==============================================================================

void
WTextModel::CreateStyle(
			short 				inMode, 
			TextStyle	  &		inStyle,
			AppleEvent	&		outStyleEvent)
{
	Assert_(mSelection != nil);
	StAEDescriptor		theSpecifier, thePropSpecifier;
	OSErr				err = noErr;
	OSType				theType = typeNull;
	bool				toggleOption = false;
	
	//Make a specifier for the selection object, since this is what
	//will be sent the appleevent
	mSuperModel->SetDefaultSubModel(nil);
	MakeSpecifier(theSpecifier.mDesc);
	mSuperModel->SetDefaultSubModel(this);
	//now make the apple event, we want to send a set event, which is in the
	//core suite
	UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAESetData, outStyleEvent);
	//Now determine what we actually have to add to the AppleEvent.
	//This depends upon what we are actually setting, determined by the inMode
	//variable.
	switch(inMode){
		
		case weDoFont:
		{
			short		fontNum;
			Str255		fontName;
			//find out the font number
			fontNum = inStyle.tsFont;
			//get the name cause that's what apple script likes
			::GetFontName(fontNum, fontName);
			//put it in a descriptor record
			StAEDescriptor	fontData(fontName);
			//now add the data to the appleevent
			err = ::AEPutParamDesc(&outStyleEvent, keyAEData, &fontData.mDesc);
			ThrowIfOSErr_(err);	
			
			//the property type is font
			theType = pFont;
			//create the property descriptor
			StAEDescriptor	propFont(theType);
			//create the object specifier for the property. The MakeSpecifier above
			//identifies the selection object, but does not id the property we want
			//we do that by creating a property object specifier inside the one
			//that identifies the CSelectionText inModelobject
			err = ::CreateObjSpecifier(cProperty, &theSpecifier.mDesc, formPropertyID,
										&propFont.mDesc, false, &thePropSpecifier.mDesc);
			ThrowIfOSErr_(err);				
			break;
		}
		case weDoFace: //this is to set it back to plain
		case (weDoFace + weDoToggleFace):
		{
			Style			theFace;
			DescType		type = kAEBold;
			StAEDescriptor 	theStyleRecord;
			SInt32			descTypeSize = sizeof(DescType);
			if(inStyle.tsFace != normal){
				theFace = inStyle.tsFace;
				StAEDescriptor	boldDesc(typeEnumeration, &type, descTypeSize);
				//check the bold bit
				type = kAEBold;
				if (theFace & 1)//bold bit set
					UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, boldDesc.mDesc);
				
				//check the italic bit
				type = kAEItalic;
				StAEDescriptor	italicDesc(typeEnumeration, &type, descTypeSize);
				if(theFace & (1 << 1))//italic bit set
					UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, italicDesc.mDesc);
				
				//check the underline setting
				type = kAEUnderline;
				StAEDescriptor underlineDesc(typeEnumeration, &type, descTypeSize);
				if(theFace & (1 << 2))//underline bit set
					UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, underlineDesc.mDesc);
				
				//check the outline setting
				type = kAEOutline;
				StAEDescriptor	outlineDesc(typeEnumeration, &type, descTypeSize);
				if(theFace & (1 << 3))//outline bit set
					UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, outlineDesc.mDesc);
				
				//check the shadow setting
				type = kAEShadow;
				StAEDescriptor shadowDesc(typeEnumeration, &type, descTypeSize);	
				if(theFace & (1 << 4))//shadow
					UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, shadowDesc.mDesc);
				
				//check the condense setting
				type = kAECondensed;
				StAEDescriptor condenseDesc(typeEnumeration, &type, descTypeSize);
				if(theFace & (1 << 5))//Condense bit
					UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, condenseDesc.mDesc);
				
				//check the extended setting
				type = kAEExpanded;
				StAEDescriptor extendDesc(typeEnumeration, &type, descTypeSize);
				if(theFace & (1 << 6)) //Extended bit
					UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, extendDesc.mDesc);
			} else {
				//only the plain command is going to arrive here, so just add plain
				type = kAEPlain;
				StAEDescriptor plainDesc(typeEnumeration, &type, descTypeSize);
				UAEDesc::AddDesc(&theStyleRecord.mDesc, 0, plainDesc.mDesc);
			}	
			toggleOption = inMode & weDoToggleFace;
			//finally add the toggle option to the style record.
			UAEDesc::AddPtr(&theStyleRecord.mDesc, 0, typeBoolean, 
											&toggleOption, sizeof(Boolean));
			
			//add it to the apple event
			err = ::AEPutParamDesc(&outStyleEvent, keyAEData, &theStyleRecord.mDesc);
			ThrowIfOSErr_(err);
			
			//package the property in the object specifier.
			theType = pTextStyles;
			StAEDescriptor	propStyles(theType);
			err = ::CreateObjSpecifier(cProperty, &theSpecifier.mDesc, formPropertyID,
										&propStyles.mDesc, false, &thePropSpecifier.mDesc);
			ThrowIfOSErr_(err);
			break;
		}
		case weDoSize:
		{
			SInt16	pointSize = inStyle.tsSize;
			//create a size descriptor
			StAEDescriptor	sizeData(pointSize);
			err = ::AEPutParamDesc(&outStyleEvent, keyAEData, &sizeData.mDesc);
			//package the property
			theType = pTextPointSize;
			StAEDescriptor propSize(theType);
			err = ::CreateObjSpecifier(cProperty, &theSpecifier.mDesc, formPropertyID,
										&propSize.mDesc, false, &thePropSpecifier.mDesc);
			ThrowIfOSErr_(err);
			break;
		}
		case weDoColor:
		{
			RGBColor 			theColor = inStyle.tsColor;
			DescType			color = typeRGBColor;
			//create a color AEDesc
			StAEDescriptor		colorData(color, &theColor, sizeof(RGBColor));
			err = ::AEPutParamDesc(&outStyleEvent, keyAEData, &colorData.mDesc);
			//package the property
			//reuse it to save an insignificant amount of memory
			theType = pColor;
			StAEDescriptor	propColor(theType);
			err = ::CreateObjSpecifier(cProperty, &theSpecifier.mDesc, formPropertyID,
										&propColor.mDesc, false, &thePropSpecifier.mDesc);
			ThrowIfOSErr_(err);
			break;
		}
				
	}//end switch	
	//add the finished object specifier to the apple event
	err = ::AEPutParamDesc(&outStyleEvent, keyDirectObject, 
											&thePropSpecifier.mDesc);
}

#if WASTE_VERSION > 0x02000000

void
WTextModel::CreateParaStyle(
			WERulerMode		inMode, 
			WERuler  &		inStyle,
			AppleEvent	&	outStyleEvent)
{
	#pragma unused(inMode)
	
	Assert_(mSelection != nil);
	StAEDescriptor		theSpecifier, thePropSpecifier;
	OSErr				err = noErr;
	OSType				theType = typeNull;
	bool				toggleOption = false;
	
	mSuperModel->SetDefaultSubModel(nil);
	MakeSpecifier(theSpecifier.mDesc);
	mSuperModel->SetDefaultSubModel(this);
	//now make the apple event, we want to send a set event, which is in the
	//core suite
	UAppleEventsMgr::MakeAppleEvent(kAECoreSuite, kAESetData, outStyleEvent);
	//Now add the justification information to the apple event..
	OSType	justif;
	
	switch(inStyle.alignment)
	{
		case weFlushLeft:
			justif = kAELeftJustified;
			break;
			
		case weFlushRight:
			justif = kAERightJustified;
			break;
			
		case weCenter:
			justif = kAECentered;
			break;
			
		case weJustify:
			justif = kAEFullyJustified;
			break;
		
		default:
			justif = kAEDefaultJustified;
			break;
	}
	
	StAEDescriptor	styleData(typeEnumerated, &justif, sizeof(justif));
	err = ::AEPutParamDesc(&outStyleEvent, keyAEData, &styleData.mDesc);
	ThrowIfOSErr_(err);
	//package the property
	theType = pJustification;
	StAEDescriptor propAlign(theType);
	err = ::CreateObjSpecifier(cProperty, &theSpecifier.mDesc, formPropertyID,
								&propAlign.mDesc, false, &thePropSpecifier.mDesc);
	ThrowIfOSErr_(err);
	
	//add the finished object specifier to the apple event
	err = ::AEPutParamDesc(&outStyleEvent, keyDirectObject, 
											&thePropSpecifier.mDesc);
											
	ThrowIfOSErr_(err);	
}

#endif
//=====================================================================
//	MakeRangeSpecifier
//==============================================================================
// Purpose:This routine is issued just before a text modifying command is done. We wait
// until that time, since it would be silly to record selections on their own
//
// Revision:
// 4/3/98. I changed this to specify a range of text to modify instead of sending
// sending a select apple event before doing the other appleevnet. This makes
// undo much easier to implement and is cleaner apple script IMHO
//
//
//==============================================================================

void
WTextModel::MakeRangeSpecifier(
							AEDesc	&inSuperSpecifier,
							AEDesc	&outSelfSpecifier) const
{
	StAEDescriptor		theSpecifier, rangeSpecifier;
	OSErr				err = noErr;
	OSType				theType = typeNull;
	SInt32				selStart, selEnd;
	DescType			modelType = cChar;
	
	mTE->GetTextSelection(selStart, selEnd);
	
	if(CanUseParagraphs(selStart, selEnd))
	{
		modelType = cParagraph;
	}
	else if(CanUseWords(selStart, selEnd))
	{
		modelType = cWord;
	}
	else
	{
		SInt8	charSize;
		selStart++;	
		charSize = FindNthChar(nil, selStart);
		selStart += charSize;
		
		charSize = FindNthChar(nil, selEnd);
		selEnd += charSize;
	}
	//make a range descriptor, thank you PPlant authors.	
	UAEDesc::MakeRange(inSuperSpecifier, modelType, selStart, selEnd, &outSelfSpecifier);
}

//==============================================================================*
//	CanUseParagraphs
//==============================================================================
// Purpose: This routine basically Finds paragraphs near the text selStart 
// and selEnd. If ends of the paragraph match the selection, then we can use
// that when describing the range
//
// Revision: 5/28/98. I added code to reset the start value back to its
// old value when the start is at a word boundary, but the end is not. This
// was messing up the selection
//==============================================================================

bool
WTextModel::CanUseParagraphs(
			SInt32 & ioStart, 
			SInt32 & ioEnd) const
{
	SInt32		start = ioStart, end = ioEnd;
	SInt32		numParas = CountParagraphs();
	SInt32		paraStart, paraEnd;
	//return if we have no text
	if (numParas <= 0)
		return false;
	//do a binary search to find the start para
	SInt32	minPara = 1;
	SInt32	maxPara = numParas;
	SInt32	midPara;
	
	while(minPara < midPara || midPara < maxPara)
	{
	
		midPara = (minPara + maxPara) >> 1;
		
		//now find the offsets for this paragraph
		FindNthParagraph(nil, midPara, paraStart, paraEnd);
		if(paraStart <= start && paraEnd > start)
		{	
			//we found the para!
			if(start != paraStart)
			{
				return false;
			}
			else
			{
				ioStart = midPara;
				break;
			}
		}
		//not the right para.
		if(paraStart > start)
			maxPara = midPara;
		else
		{
			minPara = midPara + 1;
			if(minPara > maxPara)
				return false;
		}
	}
	
	if(ioStart != midPara)
	{
		ioStart = start;
		return false;
	}			
	if(start == end){ //we are looking for an insertion
		ioStart = midPara - 1;
		ioEnd = ioStart;
		//since we were at the edge of a para
		//just get outta here.
		return true;
	}
	//we do not want to include a return character in the last
	//paragraph, so check and get rid of it if needed.
	if(mTE->GetChar(end) == '\r')
		end--;
	//now search for the end para
	minPara = ioStart;
	maxPara = numParas;
	while(minPara < midPara || midPara < maxPara)
	{
	
		midPara = (minPara + maxPara) >> 1;
		
		//now find the offsets for this paragraph
		FindNthParagraph(nil, midPara, paraStart, paraEnd);
		if(paraStart < end && paraEnd >= end)
		{	
			//we found the para!
			if( (end == paraEnd) || (end + 1 == paraEnd)) //accounts for return char not being selected.
			{
				ioEnd = midPara;
				return true;
			}
			else
			{
				//set the start back to its old value
				break;
			}
		}
		//not the right para.
		if(paraStart > end)
			maxPara = midPara;
		else
		{
			minPara = midPara + 1;
			if(minPara > maxPara)
				break;
		}
	}
	//reset the values back to originals
	ioStart = start;
	ioEnd = end;
	return false;
}

//=====================================================================
//	CanUseWords
//==============================================================================
// Purpose: The logic and flow of this routine is identical to CanUseParagraphs
// I will leave it at that.
//
// Revision: 5/28/98. I added code to reset the start value back to its
// old value when the start is at a word boundary, but the end is not. This
// was messing up the selection 
//==============================================================================

bool
WTextModel::CanUseWords(
			SInt32 & ioStart, 
			SInt32 & ioEnd) const
{
	SInt32		start = ioStart, end = ioEnd;
	SInt32		numWords = CountWords();
	SInt32		wordStart, wordEnd;
	//return if we have no text
	if (numWords <= 0)
		return false;
	//do a binary search to find the start word
	SInt32	minWord = 1;
	SInt32	maxWord = numWords;
	SInt32	midWord;
	
	while(minWord < midWord || midWord < maxWord)
	{
	
		midWord = (minWord + maxWord) >> 1;
		
		//now find the offsets for this wordgraph
		FindNthWord(nil, midWord, wordStart, wordEnd);
		if(wordStart <= start && wordEnd > start)
		{	
			//we found the word!
			if(start != wordStart)
			{
				return false;
			}
			else
			{
				ioStart = midWord;
				break;
			}
		}
		//not the right word.
		if(wordStart > start)
		{
			if(maxWord == midWord && minWord == midWord)
				return false;
			maxWord = midWord;
		}
		else
		{	
			minWord = midWord + 1;
			if(minWord > maxWord)
				return false;
		}
	}
	
	
	if(start == end){ //we are looking for an insertion
		ioStart = midWord - 1;
		ioEnd = ioStart;
		//since we were at the edge of a word
		//just get outta here.
		return true;
	}
	
	//now search for the end word
	minWord = ioStart;
	maxWord = numWords;
	
	if(minWord == maxWord)
	{
		if(end == wordEnd)
		{
			ioEnd= midWord;
			return true;
		}
	}
	
	while(minWord < midWord || midWord < maxWord)
	{
	
		midWord = (minWord + maxWord) >> 1;
		
		//now find the offsets for this wordgraph
		FindNthWord(nil, midWord, wordStart, wordEnd);
		if(wordStart < end && wordEnd >= end)
		{	
			//we found the word!
			if(end != wordEnd)
			{
				//reset the start value back to it's old value
				ioStart = start;
				break;
			}
			else
			{
				ioEnd = midWord;
				return true;
			}
		}
		//not the right word.
		if(wordStart > end)
			maxWord = midWord;
		else
		{
			minWord = midWord + 1;
			if(minWord > maxWord)
				break;
		}
	}
	ioStart = start;
	return false;
}

//=====================================================================**
//	MakeInsertSpecifier
//==============================================================================
//	 This routine is issued just before a text modifying command is done. We wait
// until that time, since it would be silly to record selections on their own
//
// 4/3/98. I changed this to specify an insertion point to instead of sending
// a select apple event before doing the other appleevnet. This makes
// undo much easier to implement and is cleaner apple script IMHO
//==============================================================================

void
WTextModel::MakeInsertSpecifier(
							AEDesc	&inSuperSpecifier,
							AEDesc	&outSelfSpecifier) const
{
	mSelection->MakeSelfSpecifier(inSuperSpecifier, outSelfSpecifier);
}	

//==============================================================================
// MakeSelfSpecifier
//==============================================================================
// Make a specifier that refers to this object. In PPlant this is easy. Just
// create a specifier for this object
//==============================================================================


void
WTextModel::MakeSelfSpecifier(	AEDesc	&inSuperSpecifier,
											AEDesc	&outSelfSpecifier) const
{
	
	//create a specifier that specifies a submodel.
	//it doesn't exist right now, but will be created when the
	//event is handled. That's why we have to to it here
	SInt32	selStart, selEnd;
	mTE->GetTextSelection(selStart, selEnd);
	
	if(selStart != selEnd)
	{
		MakeRangeSpecifier(inSuperSpecifier, outSelfSpecifier);
	}
	else
	{
		MakeInsertSpecifier(inSuperSpecifier, outSelfSpecifier);
	}
}
//=====================================================================*
//	SendEventToSelf
//==============================================================================
// A conglomerate function that sends several type of events for recording
// purposes
//==============================================================================

void
WTextModel::SendEventToSelf(
					SInt32			suite, 
					SInt32			event, 
					AppleEvent &	outAppleEvent,
					bool				specifyThis)
{
	try {
		StAEDescriptor		theSpecifier;
		
		//Make a specifier for the selection object, since this is what
		//will be sent the appleevent
		//We have to inactivate the default submodel to get this to build
		//correctly
		
		if(specifyThis)
			mSuperModel->MakeSpecifier(theSpecifier.mDesc);
		else
		{
			mSuperModel->SetDefaultSubModel(nil);
			MakeSpecifier(theSpecifier.mDesc);
			mSuperModel->SetDefaultSubModel(this);
		}
		
		//make the apple event
		UAppleEventsMgr::MakeAppleEvent(suite, event, outAppleEvent);
		//add the the objSpec to the apple event.
		OSErr err = ::AEPutParamDesc(&outAppleEvent, keyDirectObject, 
												&theSpecifier.mDesc);
		ThrowIfOSErr_(err);
		
		AddInsertionLocation(outAppleEvent);
		
	}
	
	catch (...) {
		//don't throw an error
	}
}

void
WTextModel::AddInsertionLocation(
						AppleEvent		& outAppleEvent)
{
	SInt32	selStart, selEnd;
	DescType	relType = kAEAfter;
		
	mTE->GetTextSelection(selStart, selEnd);
	// if we have an insertion location make a insert loc spec
	// and add it to the apple event.
	if(selStart == selEnd)
	{
		StAEDescriptor		theSpecifier, insertObj, nullObj, insertSpec;
		OSErr				err = noErr;
		DescType			modelType = cChar, relType = kAEAfter;
		
		//check to see is we can use a paragraph or a word to
		//id where the user clicked
		if(CanUseParagraphs(selStart, selEnd))
			modelType = cParagraph;
		else if(CanUseWords(selStart, selEnd))
			modelType = cWord;
		
		//create a AEDesc containing that location
		if(selStart == 0)
		{
			selStart = 1;
			relType = kAEBefore;
		}
		StAEDescriptor	elementPosition(selStart);	
		//Create the position object specifier that is used to record where the 
		//user clicked
		err = ::CreateObjSpecifier(modelType, &nullObj.mDesc, formAbsolutePosition, &elementPosition.mDesc,
				false, &insertSpec.mDesc);
		ThrowIfOSErr_(err);
		
		//Now make an insertionLoc specifier Thanks again PPlant
		UAEDesc::MakeInsertionLoc(insertSpec.mDesc, relType, &insertObj.mDesc);
		//add the insertionLoc specifier to the Apple Event
		err = ::AEPutParamDesc(&outAppleEvent, keyAEData, &insertObj.mDesc);
		ThrowIfOSErr_(err);
	}
}

//=====================================================================
//	FindTheTextForModel
//==============================================================================
// This routine assumes that theTextHandle is an empty valid handle.
//==============================================================================

void
WTextModel::FindTheTextForModel(
					Handle		theTextHandle, 
					SInt32	&	outStart, 
					SInt32 	&	outEnd) const
{	
	outStart = mStart;
	outEnd = mEnd;
	
	if(theTextHandle != nil)
		mTE->StreamRange(mStart, mEnd, kTypeText, 0L, theTextHandle);
}
//=====================================================================
//	GetAEProperty
//==============================================================================
// Get the requested property. This text model supports getting the contents 
// (the text) and text characteristics, font size, etc. The method is the same
// get the requested property and stuff it in outPropertyDesc.
//==============================================================================

void
WTextModel::GetAEProperty(
	DescType		inProperty,
	const AEDesc	&inRequestedType,
	AEDesc			&outPropertyDesc) const
{
	TextStyle			theStyle;
	StAEDescriptor 	theStyleRecord, totalStyles;
	SInt32				start, end;

	WERunInfo	theInfo;
	mTE->GetRunInfo(mStart, theInfo);
	theStyle = theInfo.runStyle;
	
	switch (inProperty) {
	
		case pContents:
		{	
			//copy all the text
			//make sure we have a valid handle.
			StHandleBlock	theText(0L);
			FindTheTextForModel(theText, start, end);
			StHandleLocker lock(theText);
			//stuuf it in the outDesc
			UAEDesc::AddPtr(&outPropertyDesc, 0, typeChar, *(theText.Get()), end - start);
			break;
		}
		case pBestType:		
			
			break;
		
		case pColor:				
			RGBColor	theColor;
			theColor = theStyle.tsColor;
			UAEDesc::AddPtr(&outPropertyDesc, 0, typeRGBColor, &theColor, sizeof(RGBColor));
			break;
				
		case pFont:
			Str255		fontName;
			short		fontNum;
			fontNum  = theStyle.tsFont;
			::GetFontName(fontNum, fontName);
			UAEDesc::AddPtr(&outPropertyDesc, 0, typeChar, &fontName[1], fontName[0]);
			break;
			
		case pTextStyles:
		{
			//the face has to be converted into a pascal string and passed back
			SInt32			descTypeSize = sizeof(DescType);
			SInt16			mask = 1;
			//get the run information at the begining of the text
			Style theFace = theStyle.tsFace;
			//build the style AEList
			BuildStyleAEDesc(&theStyleRecord.mDesc, theFace);
				
			UAEDesc::AddDesc(&outPropertyDesc, 0, theStyleRecord.mDesc);
			
			break;
		}		
		case pTextPointSize:
		case pPointSize:
			short	size = theStyle.tsSize;
			UAEDesc::AddPtr(&outPropertyDesc, 0, typeShortInteger, &size, sizeof(short));
			break;
		
		case pUniformStyles:
		{
			DoContinuousStyles(mStartPosition, mEndPosition, &theStyleRecord.mDesc);
			UAEDesc::AddDesc(&outPropertyDesc, 0, theStyleRecord.mDesc);
			break;
		}
		
		#if WASTE_VERSION > 0x02000000

		case pJustification:
		{
			WEParaInfo	theParaInfo;
			mTE->GetParaInfo(mStart, theParaInfo);
			OSType	justif;
			
			switch(theParaInfo.paraRuler.alignment)
			{
				case weFlushLeft:
					justif = kAELeftJustified;
					break;
					
				case weFlushRight:
					justif = kAERightJustified;
					break;
					
				case weCenter:
					justif = kAECentered;
					break;
					
				case weJustify:
					justif = kAEFullyJustified;
					break;
				
				default:
					justif = kAEDefaultJustified;
					break;
			}
			//stuuf it in the outDesc
			UAEDesc::AddPtr(&outPropertyDesc, 0, typeType, &justif, sizeof(OSType));
			break;
		}
		#endif
		
		default:
			LModelObject::GetAEProperty(inProperty, inRequestedType,
											outPropertyDesc);
			break;
	}
}

//==============================================================================
//	SetAEProperty
//==============================================================================
// Set various text attributes. If this recieves the event we want to change all the 
// text so select it. Note that not all properties can be set.
//==============================================================================

void
WTextModel::SetAEProperty(
	DescType		inProperty,
	const AEDesc	&inValue,
	AEDesc&			outAEReply)
{
	//if we are looking for an insertion location. Then pass it off to the CSelecin object
	if(inProperty == pInsertionLoc){
		WSelectionText	*theSelection = DebugCast_((mTE->GetTextModel())->GetModelProperty(pSelection),
														LModelObject, WSelectionText);
		ValidateObject_(theSelection);
		theSelection->SetAEProperty(inProperty, inValue, outAEReply);
	}
	TextStyle		theStyle;	 
	
	mTE->FocusDraw();
	mTE->SetTextSelection(mStart, mEnd);
	
	switch (inProperty) {
	
		case pContents:
		{
			SInt32 insertSize;
			#if TARGET_API_MAC_CARBON == 1
			insertSize = ::AEGetDescDataSize(&inValue);
			if(insertSize == 0)
				ThrowOSErr_(errAEParamMissed);
			StPointerBlock	dataPtr(insertSize);
			AEGetDescData(&inValue, dataPtr, insertSize);
			mTE->FocusDraw();
			mTE->InsertText(dataPtr, insertSize, nil, nil, nil, nil,
					wRefreshAfterInsert);
			#else
			insertSize = ::GetHandleSize(inValue.dataHandle);
			if(insertSize == 0)
				ThrowOSErr_(errAEParamMissed);
			StHandleLocker	lock(inValue.dataHandle);
			mTE->FocusDraw();
			mTE->InsertText(*(inValue.dataHandle), insertSize, nil, nil, nil, nil,
					wRefreshAfterInsert);
			#endif
			break;
		}
		case pColor:				
			RGBColor	newColor;
			UExtractFromAEDesc::TheRGBColor(inValue, newColor);
			theStyle.tsColor = newColor;
			mTE->FocusDraw();
			SetStyle(weDoColor, theStyle);
			break;
				
		case pFont:
			Str255	fontName;
			SInt16	fontNum;
			UExtractFromAEDesc::ThePString(inValue, fontName);
			::GetFNum(fontName, &fontNum);
			theStyle.tsFont = fontNum;
			mTE->FocusDraw();
			SetStyle(weDoFont, theStyle);
			break;
			
		case pTextStyles:
			mTE->FocusDraw();
			DoSetStyle(inValue);
			break;

#if WASTE_VERSION > 0x02000000

		case pJustification:
			mTE->FocusDraw();
			SetParaStyle(inValue);
			break;
#endif
			
		case pTextPointSize:
		case pPointSize:
			SInt16	fontSize;
			UExtractFromAEDesc::TheInt16(inValue, fontSize);
			theStyle.tsSize = fontSize;
			mTE->FocusDraw();
			SetStyle(weDoSize, theStyle);
			break;
		default:
			LModelObject::SetAEProperty(inProperty, inValue, outAEReply);
			break;
	}
}

bool
WTextModel::AEPropertyExists(
				DescType	inProperty) const
{
	bool	exists = false;
	
	switch (inProperty) {
	
		case pContents:
		//case pNative:
		case pColor:				
		case pFont:
		case pTextStyles:
		case pJustification:
		case pTextPointSize:
		case pPointSize:
		case pInsertionLoc:
			exists = true;
			break;
			
		default:
			exists = LModelObject::AEPropertyExists(inProperty);
			break;
	}
		
	return exists;
}
//=====================================================================
//	DoContinuousStyle
//==============================================================================
// Determine what attributes are continusous for the text. It is passed back as
// an AERecord in the outDesc.
//==============================================================================

void
WTextModel::DoContinuousStyles(
					SInt32 selStart, 
					SInt32 selEnd, 
					AEDesc *outDesc) const
{
	TextStyle			style;
	unsigned short			mode = weDoAll;
	Style			theStyle;
	{
		StSetSelectionForRoutine	setRange(selStart, selEnd, mTE->GetWEHandle());	
		mTE->ContinuousStyle(mode, style);
	}
	//the face has to be converted into a pascal string and passed back
	theStyle = style.tsFace;
	
	//Build the Style AEList
	BuildStyleAEDesc(outDesc, theStyle);
}

//=============================================================================
//	BuildStyleAEDesc
//==============================================================================
// Build an AERecord from the style parameter that is passed in
//==============================================================================

void
WTextModel::BuildStyleAEDesc(
					AEDesc *ioStyleDesc, 
					Style inStyle) const
{
	//check the bold bit
	DescType	type = kAEBold;
	SInt32			descTypeSize = sizeof(DescType);

	if(inStyle != 0){
		if (inStyle & 1){//bold bit set
			StAEDescriptor	boldDesc(typeEnumeration, &type, descTypeSize);
			UAEDesc::AddDesc(ioStyleDesc, 0, boldDesc.mDesc);
		}
		
		//check the italic bit
		if(inStyle & (1 << 1)){//italic bit set
			type = kAEItalic;
			StAEDescriptor	italicDesc(typeEnumeration, &type, descTypeSize);
			UAEDesc::AddDesc(ioStyleDesc, 0, italicDesc.mDesc);
		}		
		//check the underline setting
		if(inStyle & (1 << 2)){//underline bit set
			type = kAEUnderline;
			StAEDescriptor underlineDesc(typeEnumeration, &type, descTypeSize);
			UAEDesc::AddDesc(ioStyleDesc, 0, underlineDesc.mDesc);
		}
		//check the outline setting
		if(inStyle & (1 << 3)){//outline bit set
			type = kAEOutline;
			StAEDescriptor	outlineDesc(typeEnumeration, &type, descTypeSize);		
			UAEDesc::AddDesc(ioStyleDesc, 0, outlineDesc.mDesc);
		}
		//check the shadow setting
		if(inStyle & (1 << 4)){//shadow
			type = kAEShadow;
			StAEDescriptor shadowDesc(typeEnumeration, &type, descTypeSize);	
			UAEDesc::AddDesc(ioStyleDesc, 0, shadowDesc.mDesc);
		}
		
		//check the condense setting
		if(inStyle & (1 << 5)){//Condense bit
			type = kAECondensed;
			StAEDescriptor condenseDesc(typeEnumeration, &type, descTypeSize);		
			UAEDesc::AddDesc(ioStyleDesc, 0, condenseDesc.mDesc);
		}
		//check the extended setting
		if(inStyle & (1 << 6)){ //Extended bit
			type = kAEExpanded;
			StAEDescriptor extendDesc(typeEnumeration, &type, descTypeSize);
			UAEDesc::AddDesc(ioStyleDesc, 0, extendDesc.mDesc);
		}
	} else {
		type = kAEPlain;
		StAEDescriptor	plainDesc(typeEnumeration, &type, descTypeSize);
		UAEDesc::AddDesc(ioStyleDesc, 0, plainDesc.mDesc);		
	}
}
