// =================================================================================
//	CTextView.cp					©1996-2000 Metrowerks Inc. All rights reserved.
// =================================================================================

#include "CTextView.h"

#include <LStream.h>


// ---------------------------------------------------------------------------------
//	¥ CTextView												[public]
// ---------------------------------------------------------------------------------
//	LStream constructor

CTextView::CTextView(
	LStream*	inStream)

	: LTextEditView(inStream)
{
	mIsDirty = false;
}


// ---------------------------------------------------------------------------------
//	¥ ~CTextView											[public, virtual]
// ---------------------------------------------------------------------------------
//	Destructor

CTextView::~CTextView()
{
	// nothing
}


// ---------------------------------------------------------------------------------
//	¥ UserChangedText										[public, virtual]
// ---------------------------------------------------------------------------------
//	Called when a user action changes the text

void
CTextView::UserChangedText()
{
	if (IsDirty() == false) {

			// Set the update menus flag.
		SetUpdateCommandStatus(true);
		
			// Set the dirty flag.
		SetDirty(true);
	}
}


// ---------------------------------------------------------------------------------
//	¥ SavePlace												[public, virtual]
// ---------------------------------------------------------------------------------

void
CTextView::SavePlace(
	LStream*	outPlace )
{
		// Call inherited.
	LTextEditView::SavePlace(outPlace);
	
		// Save the image size.
	outPlace->WriteData(&mImageSize, sizeof(SDimension32));
}


// ---------------------------------------------------------------------------------
//	¥ RestorePlace											[public, virtual]
// ---------------------------------------------------------------------------------

void
CTextView::RestorePlace(
	LStream*	inPlace )
{
		// Call inherited.
	LTextEditView::RestorePlace(inPlace);
	
		// Restore the image size.
	inPlace->ReadData(&mImageSize, sizeof(SDimension32));
	
		// Realign the text edit rects.
	AlignTextEditRects();
}
