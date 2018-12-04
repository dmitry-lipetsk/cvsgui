/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 1, or (at your option)
** any later version.

** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.

** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * LCvsWasteEdit.cpp --- wrapper for CWasteEdit
 */

#include "LCVSWasteEdit.h"
#include "TclGlue.h"
#include "PythonGlue.h"
#include "CPStr.h"
#include "CvsPrefs.h"
#include "AppConsole.h"
#include "MacCvsApp.h"
#include <PP_DebugMacros.h>
#include "WTextModel.h"

CCvsWasteEdit::CCvsWasteEdit(LStream *inStream) : WTextView(inStream)
{
}

CCvsWasteEdit::~CCvsWasteEdit()
{
}

bool CCvsWasteEdit::HandleEnterKey(void)
{
	SInt32 selStart, selEnd;
	GetTextSelection(selStart, selEnd);
	Handle txt = GetText();
	Assert_(txt != 0L);
	if(txt == 0L)
		return false;
	
	CStr ascript;
	long len = GetTextLength();
	
	{
		StHandleLocker lock(txt);
		
		if(selStart == selEnd)
		{
			// rewind until we get the beginning of the line
			while(selStart > 0 && (*txt)[--selStart] != '\r')
			{
			}
			
			if((*txt)[selStart] == '\r')
				selStart++;
			
			// now get the end of the line
			while(selEnd < len && (*txt)[selEnd] != '\r')
			{
				selEnd++;
			}
		}
		
		int scriptLen = selEnd - selStart;
		if(scriptLen > 0)
			ascript.set((*txt) + selStart, scriptLen);
	}
	
	// execute the script
	if(!ascript.empty())
	{
		if(len != 0 && (*txt)[len - 1] != '\r')
		{
			cvs_out("\n");
		}
		
		if(gCvsPrefs.GetUseShell() == SHELL_PYTHON && CPython_Interp::IsAvail())
		{
			CPython_Interp interp;
			interp.DoScript(ascript);
		}
		else if(CTcl_Interp::IsAvail())
		{
			CTcl_Interp interp;
			interp.DoScript(ascript);
		}
		
		return true;
	}
	
	return false;
}

Boolean CCvsWasteEdit::HandleKeyPress(const EventRecord & inKeyEvent)
{
	Boolean		keyHandled = true;
	Boolean		doScrollCheck = true;
	EKeyStatus	theKeyStatus = keyStatus_Input;
	SInt16		theKey = inKeyEvent.message & charCodeMask;
#ifdef qMacCvsPP
	EventRecord	myKeyEvent = inKeyEvent;
#	define inKeyEvent myKeyEvent
#endif

	if (!FeatureFlag(weFReadOnly, weBitTest)) 
	{
		//remember the target. We need to check this before doing scroll synch
		LCommander	*theTarget = GetTarget();
		
		SInt32	selStart, selEnd;
		GetTextSelection(selStart, selEnd );
		Boolean beforeSel = (selStart == selEnd);
		
		FocusDraw();
		// UKeyFilters tells us what kind of key was hit and
		// from there we can take the appropriate action
		theKeyStatus = UKeyFilters::PrintingCharField(inKeyEvent);
		
		if (inKeyEvent.modifiers & cmdKey & !(theKeyStatus == keyStatus_TECursor)) 
		{										// Always pass up when the
			theKeyStatus = keyStatus_PassUp;	// command key is down
		}
		
#ifdef qMacCvsPP
		if((inKeyEvent.modifiers & (cmdKey|optionKey|controlKey)) == 0)
		{
			if(theKey == char_Home)
			{
				theKey = char_UpArrow;
				inKeyEvent.modifiers |= cmdKey;
				theKeyStatus = keyStatus_TECursor;
			}
			else if(theKey == char_End)
			{
				theKey = char_DownArrow;
				inKeyEvent.modifiers |= cmdKey;
				theKeyStatus = keyStatus_TECursor;
			}
			else if(theKey == char_PageUp)
			{
				theKey = char_UpArrow;
				inKeyEvent.modifiers |= optionKey;
				theKeyStatus = keyStatus_TECursor;
			}
			else if(theKey == char_PageDown)
			{
				theKey = char_DownArrow;
				inKeyEvent.modifiers |= optionKey;
				theKeyStatus = keyStatus_TECursor;
			}
		}
#endif

		// now check out what key was typed and take the correct action
		switch (theKeyStatus) {
		
			case keyStatus_Input:
				//when we first start typing, then update the menus
				//after that (when in the middle of a typing action
				//don't mess with menus. This greatly speeds things up.
				if(!IsTyping())
					SetUpdateCommandStatus(true);
				FocusDraw();
				Key(theKey, inKeyEvent.modifiers);
				UserChangedText();
				break;

			case keyStatus_TEDelete:
				FocusDraw();
				Key(theKey, inKeyEvent.modifiers);
				UserChangedText();
				if(!mDeleting)
				{
					mDeleting = true;
					SetUpdateCommandStatus(true);
				}
				break;
				
			case keyStatus_TECursor:
			{	//get some preliminary info about run
				//and paragraph data
				SInt32	runIndex = OffsetToRun(selStart);
				
				#if WASTE_VERSION > 0x02000000
				WEParaInfo rulerInfo;
				GetParaInfo(selStart, rulerInfo);
				#endif
				
				//OK now do the key stuff
				FocusDraw();	
				Key(theKey, inKeyEvent.modifiers);
				//Finally see if things have changed.
				SInt32 newStart, newEnd;
				GetTextSelection(newStart, newEnd);
				
				#if WASTE_VERSION > 0x02000000
				WEParaInfo newRulerInfo;
				GetParaInfo(newStart, newRulerInfo);
				#endif
				
				if(runIndex != OffsetToRun(newStart) 
					
					#if WASTE_VERSION > 0x02000000
					||
					(rulerInfo.paraRuler.alignment != newRulerInfo.paraRuler.alignment)
					#endif
					
					 )
				{
					SetUpdateCommandStatus(true);
				}
			}	
				break;
				
			case keyStatus_ExtraEdit:
				if (theKey == char_FwdDelete
#ifdef qMacCvsPP
					|| theKey == char_Home
					|| theKey == char_End
					|| theKey == char_PageUp
					|| theKey == char_PageDown
#endif
				)
				{
					FocusDraw();
					Key(theKey, inKeyEvent.modifiers);
					UserChangedText();
					if(!mDeleting)
					{
						mDeleting = true;
						SetUpdateCommandStatus(true);
					}
				}
				else {
					// these really aren't handled.
					keyHandled = LCommander::HandleKeyPress(inKeyEvent);
				}
				break;
				
			case keyStatus_Reject:
				// +++ Do something
				doScrollCheck = false;
				::SysBeep(1);
				break;
				
			case keyStatus_PassUp:
#ifdef qMacCvsPP
				if ((CTcl_Interp::IsAvail() || CPython_Interp::IsAvail()) && (theKey == char_Enter || theKey == char_Return)) {
					// launch a TCL script
					if(HandleEnterKey())
					{
						FocusDraw();
						UserChangedText();
						SetUpdateCommandStatus(true);
						break;
					}
				}
#endif
				if ((theKey == char_Return) || (theKey==char_Tab)) 
				{
					if(!IsTyping())
						SetUpdateCommandStatus(true);
					FocusDraw();
					Key(theKey, inKeyEvent.modifiers);
					UserChangedText();
					
				}
				else 
				{
					keyHandled = LCommander::HandleKeyPress(inKeyEvent);
				}
				break;
		}
		//if the selection changed from no selection
		//to a selection or the otherway around, we
		//want to update things.
		GetTextSelection( selStart, selEnd );
		if ( beforeSel != ( selStart == selEnd ) ) 
		{
			SetUpdateCommandStatus(true);
		}	
		
		if(mTextModel != nil 
			&&	(theKeyStatus == keyStatus_Input
			||	theKeyStatus == keyStatus_TEDelete
			||	theKey == char_FwdDelete) )
		{
			//if we are doing applescript, we need to update the search pointers
			//to all the text.
			ValidateObj_(mTextModel);
			mTextModel->SetSearchPointers(0, LONG_MAX);
		}
		//an optimization. We only check the menus at the begining of a delete sequence
		//here we set mDeleting back to false, if it is true and the user did not
		//type delete or fwd_delete
		if(mDeleting && !(keyStatus_TEDelete == theKeyStatus || theKey == char_FwdDelete))
		{
			mDeleting = false;
		}
		
		if (doScrollCheck && (GetTarget() == theTarget) ) 
		{
			//1/8/01 - a new modification, let me know if this is a problem
			CheckScroll();
			AdjustImageToText();
			CheckScroll();
		}
	}
	else 
	{
		if (keyStatus_TECursor == theKeyStatus) 
		{ 
			FocusDraw();	
			Key(theKey, inKeyEvent.modifiers);
			keyHandled = true;
		}
		else 
		{		
			keyHandled = LCommander::HandleKeyPress(inKeyEvent);
		}
	}
	return keyHandled;
}

Boolean CCvsWasteEdit::DragIsAcceptable(DragReference inDragRef)
{
	Boolean	isAcceptable = true;
	
	UInt16	itemCount;
	::CountDragItems(inDragRef, &itemCount);
	
	if ( itemCount < 10 ) // limit potential number of new browser windows to open
	{
  	for (UInt16 item = 1; item <= itemCount; item++)
  	{
  		ItemReference	itemRef;
  		::GetDragItemReferenceNumber(inDragRef, item, &itemRef);
  		
  		FlavorFlags theFlags;
  		isAcceptable = GetFlavorFlags(inDragRef, itemRef, flavorTypeHFS, &theFlags) == noErr;

  		if (!isAcceptable)
  			break;
  	}
  }
  else isAcceptable = false;
  	
	if(isAcceptable)
		return true;

	return WTextView::DragIsAcceptable(inDragRef);
}

void CCvsWasteEdit::DoDragReceive(DragReference	inDragRef)
{
	UInt16	itemCount, item;
	ItemReference itemRef;
	FlavorFlags theFlags;
	::CountDragItems(inDragRef, &itemCount);
	
	for (item = 1; item <= itemCount; item++)
	{
		::GetDragItemReferenceNumber(inDragRef, item, &itemRef);
		
		Boolean isAcceptable = GetFlavorFlags(inDragRef, itemRef, flavorTypeHFS, &theFlags) == noErr;

		if (!isAcceptable)
		{
			WTextView::DoDragReceive(inDragRef);
			return;
		}
	}
	
	for (item = 1; item <= itemCount; item++)
	{
		::GetDragItemReferenceNumber(inDragRef, item, &itemRef);
		
		LView::OutOfFocus(nil);
		FocusDropArea();

		Size theDataSize;	// How much data there is for us.
		HFSFlavor theFlavorData;	// Where we will put that data.
		
		if (GetFlavorFlags(inDragRef, itemRef, flavorTypeHFS, &theFlags) == noErr) {
			::GetFlavorDataSize(inDragRef, itemRef, flavorTypeHFS, &theDataSize);
			if (theDataSize) {
				ThrowIf_(theDataSize != sizeof(HFSFlavor));	// sanity check
				//
				// Get the data about the rectangle we are receiving.
				//
				::GetFlavorData(inDragRef, itemRef, flavorTypeHFS, &theFlavorData, &theDataSize, 0L);

				FSRef ref;
				FSpMakeFSRef(&theFlavorData.fileSpec, &ref);
				CMacCvsApp::gApp->NewBrowser(ref);
				
			  ProcessSerialNumber currentProcess = { 0, kCurrentProcess };
			  SetFrontProcessWithOptions(&currentProcess, kSetFrontProcessFrontWindowOnly);
			}
		}
	}
}
