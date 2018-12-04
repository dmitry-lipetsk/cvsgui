// ===========================================================================
//	CWASTEEdit.cp				©1995-97 Timothy Paustian All rights reserved.
// ===========================================================================

#include <LStream.h>
#include <UDrawingState.h>
#include <UKeyFilters.h>
#include <PP_KeyCodes.h>
#include <PP_Messages.h>
#include <PP_Constants.h>
#include <PP_Resources.h>
#include <UTextTraits.h>
#include <LWindow.h>
#include <LScroller.h>
#include <UMemoryMgr.h>
#include <LTabGroup.h>
#include <LScrollerView.h>
#include <UCursor.h>

#ifndef __MEMORY__
#include <Memory.h>
#endif

#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

#ifndef __SCRIPT__
#include <Script.h>
#endif

#ifndef __SCRAP__
#include <Scrap.h>
#endif

#ifndef __RESOURCES__
#include <Resources.h>
#endif

#ifndef __WINDOWS__
#include <Windows.h>
#endif

#ifndef __FONTS__
#include <Fonts.h>
#endif

#include "CWASTEEdit.h"
#include <LString.h>

//#define PROFILE_ME
#ifdef PROFILE_ME
#include <Profiler.h>
Boolean doUpdateForProf = true;
#endif

const SInt32 kWhite = 0xFFFF;
SInt32 CWASTEEdit::sMaxScrollDelta = 32;

// ---------------------------------------------------------------------------
//		¥ CWASTEEdit
// ---------------------------------------------------------------------------
//	Default Contructor

CWASTEEdit::CWASTEEdit()
:
	LDragAndDrop(
		UQDGlobals::GetCurrentPort(),
		this)
{
	mForeColor.red = 0;
	mForeColor.green = 0;
	mForeColor.blue = 0;
	
	mBackColor.red = 0xFFFF;
	mBackColor.green = 0xFFFF;
	mBackColor.blue = 0xFFFF;
	
	mInitialTextID = 0;
	mInitialResFile = -1;
	
	mTextAttributes = WEDefaultAttrs;
	UInt32 wasteFlags = WEDefaultFlags;
	
	InitWASTEEdit(wasteFlags);	// Initialize member variables
	AlignWASTEEditRects();
}


// ---------------------------------------------------------------------------
//		¥ CWASTEEdit
// ---------------------------------------------------------------------------
//	Construct from input parameters

CWASTEEdit::CWASTEEdit(
	const SPaneInfo &	inPaneInfo,
	const SViewInfo &	inViewInfo,
	UInt16				inTextAttributes,	// = WEDefaultAttrs
	ResIDT				inTextTraitsID,		// = 0
	ResIDT				inInitialTextID,		// = 0
	SInt32				inWasteFlags)		// = 0
:
	LView(
		inPaneInfo,
		inViewInfo),
	LDragAndDrop(
		UQDGlobals::GetCurrentPort(),
		this)
{
	mForeColor = Color_Black; // JRB
	mBackColor = Color_White; // JRB

	mTextTraitsID = inTextTraitsID;
	mTextAttributes = inTextAttributes;
	mInitialTextID = inInitialTextID;
	mInitialResFile = -1;
	InitWASTEEdit(inWasteFlags);
	AlignWASTEEditRects();
}


// ---------------------------------------------------------------------------
//		¥ CWASTEEdit
// ---------------------------------------------------------------------------
//	Contruct a WASTEEdit from the data in a Stream
//
//	Revisions
//	1/13/97 Added code to handle the new CWASTEEdit info 

CWASTEEdit::CWASTEEdit(
	LStream *	inStream)
:
	LView(
		inStream),
	LDragAndDrop(
		UQDGlobals::GetCurrentPort(),
		this)
{	
	//clear the attributes field, necessary for backward compatibility
	mTextAttributes = 0;
	
	// get the Text attributes (multistyle, editable, selectable wordwrap.
	inStream->ReadData(&mTextAttributes, sizeof(UInt16));
	
	// read in text traits ID
	inStream->ReadData(&mTextTraitsID, sizeof(ResIDT));
	
	// get text resource ID
	inStream->ReadData(&mInitialTextID, sizeof(ResIDT));
	
	UInt32 wasteFlags;
	
	mForeColor = Color_Black; // JRB
	
	// get the WASTE attributes
	inStream->ReadData(&wasteFlags, sizeof(UInt32));
	// read in the background color.
	inStream->ReadData(&mBackColor, sizeof(RGBColor));

	InitWASTEEdit(wasteFlags);
	AlignWASTEEditRects();
}
 
CWASTEEdit::CWASTEEdit(
				const CWASTEEdit& inOriginal)	
	
	: LView(inOriginal),	
	LDragAndDrop(UQDGlobals::GetCurrentPort(),this)
{
	LongRect    	viewRect, destRect;
	SInt32			textLength;
	UInt32			wasteFlags;
	StHandleBlock  	text(0L);
	StHandleBlock	style(0L);
	StHandleBlock  	soup(0L );
	
	//LView's copy constructor puts this view inside nothing,
	//but we'll need to be inside a view by the time we get to 
	//InitWasteEdit. The user will have to PutInside() after construction
	//if they want a different superview.
	mSuperView = inOriginal.mSuperView;	
	
	//get member data from the old instance
	mInScroller = inOriginal.mInScroller;

	mSavePos = inOriginal.mSavePos;
	mPrintPanels = inOriginal.mPrintPanels;
	sMaxScrollDelta = inOriginal.sMaxScrollDelta;
	mTextAttributes = inOriginal.mTextAttributes;
	mForeColor = inOriginal.mForeColor;
	mBackColor = inOriginal.mBackColor;

	mTextTraitsID = inOriginal.mTextTraitsID;
	mScroller = inOriginal.mScroller;
	
	mInitialTextID = inOriginal.mInitialTextID;
	//we will find this if needed in InitWASTEEdit
	mInitialResFile = -1;
	//get data from the WASTE handle
	WEGetDestRect(&destRect, inOriginal.mWASTEEditH);
	WEGetViewRect(&viewRect, inOriginal.mWASTEEditH);
	inOriginal.GetTextStyleSOUPData(text, style, soup);
	textLength = inOriginal.GetTextLength();
	
	//set the waste flags
	wasteFlags = 0;
	if(inOriginal.FeatureFlag(weFInhibitColor, weBitTest))
		wasteFlags |= (1UL << weFInhibitColor);
	if(inOriginal.FeatureFlag(weFMonoStyled, weBitTest))
		wasteFlags |= (1UL << weFMonoStyled);
	if(inOriginal.FeatureFlag(weFInhibitRedraw, weBitTest))
		wasteFlags |= (1UL << weFInhibitRedraw);
	if(inOriginal.FeatureFlag(weFInhibitColor, weBitTest))
		wasteFlags |= (1UL << weFInhibitColor);
	if(inOriginal.FeatureFlag(weFDrawOffscreen, weBitTest))
		wasteFlags |= (1UL << weFDrawOffscreen);
	if(inOriginal.FeatureFlag(weFUseTempMem, weBitTest))
		wasteFlags |= (1UL << weFUseTempMem);
	if(inOriginal.FeatureFlag(weFInhibitRecal, weBitTest))
		wasteFlags |= (1UL << weFInhibitRecal);
	if(inOriginal.FeatureFlag(weFDragAndDrop, weBitTest))
		wasteFlags |= (1UL << weFDragAndDrop);
	if(inOriginal.FeatureFlag(weFIntCutAndPaste, weBitTest))
		wasteFlags |= (1UL << weFIntCutAndPaste);
	if(inOriginal.FeatureFlag(weFUndo, weBitTest))
		wasteFlags |= (1UL << weFUndo);
	if(inOriginal.FeatureFlag(weFReadOnly, weBitTest))
		wasteFlags |= (1UL << weFReadOnly);
	if(inOriginal.FeatureFlag(weFOutlineHilite, weBitTest))
			wasteFlags |= (1UL << weFOutlineHilite);
	if(inOriginal.FeatureFlag(weFAutoScroll, weBitTest))
		wasteFlags |= (1UL << weFAutoScroll);
	
	//now set the values for this instance
	InitWASTEEdit(wasteFlags);
	
	mMargins = inOriginal.mMargins;
	
	WESetViewRect(&viewRect, mWASTEEditH);
	WESetDestRect(&destRect,mWASTEEditH);
	AlignWASTEEditRects();
	Insert(text,textLength,  (StScrpHandle)style.Get(), soup, true, true);
}
// ---------------------------------------------------------------------------
//		¥ InitWASTEEdit
// ---------------------------------------------------------------------------
//	Initialize member variables of a WASTEEdit to default values

void
CWASTEEdit::InitWASTEEdit(
	UInt32	inWasteFlags)
{
	LongRect	viewLRect;
	LongRect	destLRect;
	Rect		viewRect;
	Rect		destRect;

	// Get the size of the frame
	CalcLocalFrameRect(viewRect);
	
	mScroller = nil;
	AdaptToEnclosingScroller();
	
	destRect = viewRect;
	
	// if wrapping is on, use the default small size
	if (mTextAttributes & wasteAttr_WordWrap) {
		destRect.right = destRect.left + mFrameSize.width;
	}
	else {
		if (mImageSize.width > 0)
		{
			//Check to make sure this is not too small.
			//if you want it less than 200 remove this code.
			SignalIf_(mImageSize.width < 200); 
			destRect.right = destRect.left + mImageSize.width;
		}
		else	// otherwise use the large size
			destRect.right = destRect.left + WE_DEFAULT_NO_WRAP_WIDTH;
	}
	
	// we need to protect ourselves since the WASTE instance
	// is not built yet.

	Boolean oldRecon = mReconcileOverhang;
	mReconcileOverhang = false;
	// resize the image to whatever we decided above
	ResizeImageTo(	(destRect.right - destRect.left),
					(destRect.bottom - destRect.top), false);
	mReconcileOverhang = oldRecon;
	
	// make LongRects for WENew call
	WERectToLongRect(&viewRect, &viewLRect);
	WERectToLongRect(&destRect, &destLRect);
					
	WENew(&destLRect, &viewLRect, inWasteFlags, &mWASTEEditH);
	
	// Store the wasteEdit handle in the refcon
	// we use this in the clickloop
	WESetInfo(weRefCon, &this, mWASTEEditH);
	
	// set up the clickLoop. Note that this uses UPPs
	// therefore it is stable on PPC as well as 68k
	static WEClickLoopUPP		clickLoop = nil;
	if (clickLoop == nil)
		clickLoop = NewWEClickLoopProc(MyClickLoop);
	WESetInfo(weClickLoop, (Ptr)&clickLoop, mWASTEEditH);
	
	static WEHiliteDropAreaUPP	dragProc = nil;
	if (dragProc == nil)
		dragProc = NewWEHiliteDropAreaProc(MyDragHiliteProc);
	WESetInfo(weHiliteDropAreaHook, (Ptr)&dragProc, mWASTEEditH);

	//----------------------------------------------------------------
	// Change added 25th September 1995 ASL
	// Check for a TextTraitsID other than zero. If the TextTraitsID
	// is set, then use that resource for the font etc info, otherwise
	// use the defaults

	TextTraitsH	theTextTraits = NULL;
	TextStyle	ts;
	RGBColor	defaultColor = WE_DEFAULT_COLOR;
	
	if (mTextTraitsID != 0)
		theTextTraits = UTextTraits::LoadTextTraits(mTextTraitsID);

	if (theTextTraits == NULL) {
			// Set the default font, size and face.
		ts.tsFont = WE_DEFAULT_FONT;
		ts.tsSize = WE_DEFAULT_SIZE;
		ts.tsFace = WE_DEFAULT_FACE;
		ts.tsColor = defaultColor;

		SetStyle( weDoAll, &ts);
				
			// and alignment
				
		SetAlignment( weFlushDefault);
	}
	else {
		ts.tsFont = (*theTextTraits)->fontNumber;
		ts.tsSize = (*theTextTraits)->size;
		ts.tsFace = (*theTextTraits)->style;
		ts.tsColor = (*theTextTraits)->color;
				
		SetStyle( weDoAll, &ts);
				
#ifdef WE_JUSTIFY_BY_DEFAULT
		if ((*theTextTraits)->justification == weFlushDefault) {
			(*theTextTraits)->justification = weJustify;
		}
#endif

		SetAlignment( (*theTextTraits)->justification);
	}

#ifdef WE_USE_TABS
	if (wasteAttr_TabSupport & mTextAttributes) {
		WEInstallTabHooks(mWASTEEditH);
	}
#endif	
	
	// a list of the panels offsets of a print record.
	// here we create an empty list with each item being the size of
	// and SInt32
	mPrintPanels = NEW LArray(sizeof(SInt32));
	
	// if this is a non-multistyle pane, then we want to set the 
	// monostyle flag
	// 1/11/97
	if ( !(mTextAttributes & wasteAttr_MultiStyle) ) {
		FeatureFlag(weFMonoStyled, weBitSet);
	}
	// if this isn't editable, then it is read only, so set the flag.
	if ( !(mTextAttributes & wasteAttr_Editable) ) {
		FeatureFlag(weFReadOnly, weBitSet);
	}
	
	//added support for style and soup resources
	if(mInitialTextID != 0)
	{
		Handle 	initialTextH	= ::GetResource('TEXT', mInitialTextID);
		if (initialTextH != nil) {
			::LoadResource(initialTextH);
			mInitialResFile = ::HomeResFile(initialTextH);
			SInt32 textSize = ::GetHandleSize(initialTextH);
			SInt8 prevUndo = FeatureFlag(weFUndo, weBitClear);
			SInt16 curRes = ::CurResFile();
			::UseResFile(mInitialResFile);
			Handle initialStyleH = ::Get1Resource('styl', mInitialTextID);
			if (initialStyleH) {
				::LoadResource(initialStyleH);
			}
			Handle initialSoupH = ::Get1Resource('SOUP', mInitialTextID);
			if (initialSoupH) {
				::LoadResource(initialSoupH);
			}
			::UseResFile(curRes);
			Insert(initialTextH, textSize, (StScrpRec**)initialStyleH, 
						initialSoupH, true, false);
			FeatureFlag(weFUndo, prevUndo);
			::ReleaseResource(initialTextH);
			::ReleaseResource(initialStyleH);
			::ReleaseResource(initialSoupH);
			SetSelection(0, 0);
		}
	}
}

// ---------------------------------------------------------------------------
//		¥ ~CWASTEEdit
// ---------------------------------------------------------------------------
//	Destructor

CWASTEEdit::~CWASTEEdit()
{
	if (mWASTEEditH != nil) {
		WEDispose(mWASTEEditH);
	}
	// get rid of the info since we are dying. This was causing a memory leak.
	// 12/10/96
	delete mPrintPanels;
}


// ---------------------------------------------------------------------------
//		¥ AdaptToNewSurroundings
// ---------------------------------------------------------------------------

void
CWASTEEdit::AdaptToNewSurroundings()
{
	LView::AdaptToNewSurroundings();

	AdaptToEnclosingScroller();
}


// ---------------------------------------------------------------------------
//		¥ AdaptToEnclosingScroller
// ---------------------------------------------------------------------------

void
CWASTEEdit::AdaptToEnclosingScroller()
{
	//You can now set this flag in the PPob.
	if(mTextAttributes & wasteAttr_DoMargins){
		
		// figure out the margin between the scroller (if there is one)
		// and this viewRect. This gets used later
		LView *theSuperView = GetSuperView();
		while (theSuperView && (dynamic_cast<LScroller *>(theSuperView) == nil)
					&&  (dynamic_cast<LScrollerView *>(theSuperView) == nil))
		{
			theSuperView = theSuperView->GetSuperView();
		}
		
		if (theSuperView == nil) {
			mMargins.v = mMargins.h = 0;
			mInScroller = false;
			mScroller = nil;
		}
		else {
			Rect portViewRect;
			CalcPortFrameRect(portViewRect);
			Rect scrollRect;
			theSuperView->CalcPortFrameRect(scrollRect);

			mMargins.v = portViewRect.top - scrollRect.top;
			mMargins.h = portViewRect.left - scrollRect.left;
			mInScroller = true;
			// This is safe since above we determined this is a LScroller
			mScroller = theSuperView;
			
			if (!mFrameBinding.left || !mFrameBinding.right) {
				mMargins.h = 0;
			}
			if (!mFrameBinding.top || !mFrameBinding.bottom) {
				mMargins.v = 0;
			}
		}
	}
	else {
		mMargins.v = mMargins.h = 0;
		mInScroller = false;
		mScroller = nil;
	}
}


// ---------------------------------------------------------------------------
//		¥ GetTextHandle
// ---------------------------------------------------------------------------
//	Return a Handle to the text in Waste
//
//	The Handle is the actual text Handle
//	if you need the raw text outside of CWASTEEdit, use this routine to get it 
//	Treat the handle as read only

Handle
CWASTEEdit::GetTextHandle() const
{
	return WEGetText(mWASTEEditH);
}


// ---------------------------------------------------------------------------
//		¥ GetWEHandle
// ---------------------------------------------------------------------------
//	Return a Handle to the Waste Text Engine.
//
//	Caller may use this in calls to the Waste engine. However, caller must
//	not dispose of the Handle!.

WEHandle
CWASTEEdit::GetWEHandle()
{
	return mWASTEEditH;
}

// ---------------------------------------------------------------------------
//		¥ FocusDraw
// ---------------------------------------------------------------------------
//	Focus the view.

Boolean
CWASTEEdit::FocusDraw( 
						LPane * inSubPane)
{
	Boolean focused = LView::FocusDraw(inSubPane );

	if (focused) {
		// 10/17/96 expand the ClipRect a little to allow
		// Drawing in the margins. If you don't want this to happen
		// make sure the margins are set to 0
		if (mMargins.h != 0 || mMargins.v != 0) {
			Rect frame;
			CalcLocalFrameRect(frame);
			::InsetRect(&frame, -mMargins.h, -mMargins.v);
			::ClipRect(&frame);
		}
		// this is pretty much fixed now. 9/30/96
		ApplyForeAndBackColors();
	}
	
	return focused;
}


//-------------------------------------------------------------------------------
//	¥ ApplyForeAndBackColors
//-------------------------------------------------------------------------------
// Apply the Fore and Back Colors to the current pane. The fore and back
// colors are set in SetForeAndBackColors

void
CWASTEEdit::ApplyForeAndBackColors() const
{
	::RGBForeColor(&mForeColor);
	::RGBBackColor(&mBackColor);
}


// ---------------------------------------------------------------------------
//		¥ SetForeAndBackColors
// ---------------------------------------------------------------------------
//	Specify the foreground and/or background colors for this CWASTEEdit
//
//	Specify nil for inForeColor and/or inBackColor to leave that
//	color trait unchanged

void
CWASTEEdit::SetForeAndBackColors(
	const RGBColor *	inForeColor,
	const RGBColor *	inBackColor)
{
	if (inForeColor != nil) {
		mForeColor = *inForeColor;
	}
	
	if (inBackColor != nil) {
		mBackColor = *inBackColor;
	}
}


// ---------------------------------------------------------------------------
//		¥ EraseTextArea
// ---------------------------------------------------------------------------

void
CWASTEEdit::EraseTextArea()
{
	// now erase the text since it's going to change.
	Rect theRect;
	CalcLocalFrameRect(theRect);
	
	FocusDraw();
	::EraseRect(&theRect);
	InvalPortRect(&theRect);
}


// ---------------------------------------------------------------------------
//		¥ EraseUpdateArea
// ---------------------------------------------------------------------------
//	Purpose: This is used to erase the update rgn when the background is
//	not white. If not done, the background color is not updated properly.
//
//	Revision: 9/30/96
 
void
CWASTEEdit::EraseUpdateArea(
	RgnHandle	inEraseRgn)
{
	::EraseRgn(inEraseRgn);
}


// ---------------------------------------------------------------------------
//		¥ Draw
// ---------------------------------------------------------------------------
//	This is being overidden in CWASTEEdit to make sure margins are
//	included in the drawing. There was problem with them not being
//	correctly updated if nothing else in the view was changed.
//
// 6/2/98 added a feature flag text to make sure that we don't
// draw in a port that is inhibited by the weFInhibitRedraw flag.
// This will also inhibit redraw of any subpanes inside the port
// which I think is a good thing. 
void
CWASTEEdit::Draw(
	RgnHandle	inSuperDrawRgnH)
{
	if (IsVisible() && FocusDraw() && 
		!FeatureFlag(weFInhibitRedraw, weBitTest)) {
		// here is the change. We expand the revealed rect
		// by the size of the margins.
		//but only if the margins are greater than 0
		Rect theRevealRect = mRevealedRect;
		SInt16	hInset = mMargins.h > 0 ? -(mMargins.h - 1) : 0;
		SInt16	vInset = mMargins.v > 0 ? -(mMargins.v - 1) : 0;
		
		if(mTextAttributes & wasteAttr_DoMargins && 
				((hInset < 0) || (vInset < 0)))
			::InsetRect(&theRevealRect, hInset, vInset);

		// now back to the regular routine.
		::RectRgn(mUpdateRgn, &theRevealRect);
		if (inSuperDrawRgnH != nil) {
			::SectRgn(inSuperDrawRgnH, mUpdateRgn, mUpdateRgn);
		}
		if (!::EmptyRgn(mUpdateRgn)) {
									// Some portion needs to be drawn
			Rect frame;
			CalcLocalFrameRect(frame);
			if (ExecuteAttachments(msg_DrawOrPrint, &frame)) {
									// A View is visually behind its
									//	 SubPanes so it draws itself first,
									//	 then its SubPanes			
				DrawSelf();
			}
			LArrayIterator iterator(mSubPanes, LArrayIterator::from_Start);
			LPane	*theSub;
			while (iterator.Next(&theSub)) {
				theSub->Draw(mUpdateRgn);
			}
		}
		::SetEmptyRgn(mUpdateRgn);	// Emptying update region frees up memory
									//	 if this region wasn't rectangular
	}
}


// ---------------------------------------------------------------------------
//		¥ DrawSelf
// ---------------------------------------------------------------------------
//	Draw the text that is visible.

void
CWASTEEdit::DrawSelf()
{
	// We have to change it to the current port in case we are drawing
	// into a port that is not the owner port. This happens when we are
	// printing or drawing into an offscreen port.
	
	FocusDraw();
	
	GrafPtr savePort;
	ThrowIfOSErr_(WEGetInfo(wePort, &savePort, mWASTEEditH));

	GrafPtr newPort = UQDGlobals::GetCurrentPort();
	ThrowIfOSErr_(WESetInfo(wePort, &newPort, mWASTEEditH));
	
	RgnHandle updateRgn = GetLocalUpdateRgn();

	// somehwhat of a hack, if the background color of the view is not white
	// then we need to erase the rgn that is being updated to get it to look 
	// right 9/30/96
	// I removed the color check. This really isn't costing much and it takes 
	// care of the margins 10/18/96
	if (mMargins.v != 0 || mMargins.h != 0) {
		RgnHandle	marginRgn = NewRgn();
		RgnHandle	frameRgn = NewRgn();
		Rect		frame, marginFrame;
		CalcLocalFrameRect(frame);
		marginFrame = frame;
		::InsetRect(&marginFrame, -(mMargins.h - 1), -(mMargins.v - 1));
		::RectRgn(frameRgn, &frame); 
		::RectRgn(marginRgn, &marginFrame);
		::DiffRgn(marginRgn, frameRgn, marginRgn);
		::UnionRgn(marginRgn, updateRgn, marginRgn);			
		EraseUpdateArea(marginRgn);
		::DisposeRgn(marginRgn);
		::DisposeRgn(frameRgn);
	}
	else {
		EraseUpdateArea(updateRgn);
	}
	// the first parameter is the region to be updated. If we pass nil,
	// the viewRect is used as the update region.
#ifdef PROFILE_ME
	// profiling Timp 11/1/96
	if (doUpdateForProf) {
		if (ProfilerInit(collectDetailed, bestTimeBase, 20, 7) != noErr) {
			ExitToShell();
		}
	}
#endif
	WEUpdate(updateRgn, mWASTEEditH);
	
	#ifdef PROFILE_ME
	if (doUpdateForProf) {
		OSErr anErr = ProfilerDump("\pSegmentLoopProfile");
		ProfilerTerm();
	}
	#endif
	
	DisposeRgn(updateRgn);
	ThrowIfOSErr_(WESetInfo(wePort, &savePort, mWASTEEditH));
}

// ---------------------------------------------------------------------------
//		¥ ClickSelf
// ---------------------------------------------------------------------------
//	Respond to Click inside an WASTEEdit window

void
CWASTEEdit::ClickSelf(
	const SMouseDownEvent &	inMouseDown)
{
	if (!(mTextAttributes & wasteAttr_Selectable)) 		//¥AST move this up to simplify	
		return;
		
	Point	mouseLoc;

	mouseLoc = inMouseDown.wherePort;
	mouseLoc.v += mPortOrigin.v;
	mouseLoc.h += mPortOrigin.h;

	// if its not selectable, then don't let them edit
	if (!IsTarget()) 
	{				
		//if (!IsInSelection(mouseLoc)) 		//¥AST  Activate text pane, if its not a 
		//	SwitchTarget(this);				// drag of the existing selection		
		if (FeatureFlag(weFDragAndDrop, weBitTest) &&
				WaitMouseMoved ( inMouseDown.macEvent.where )) 
		{
			// this is the begining of a drag so let WEClick handle it
			// 1/16/97 timp
			// if the end of the text is not in view, don't allow a drop.
			BeginDrag(mouseLoc, inMouseDown.macEvent);
		}
		// You do not want to view to become active if the
		// selection or cursor is not in a visible part of the view.

		SInt32 selStart, selEnd;
		GetTextSelection(&selStart, &selEnd); 
		if (!CursorInView(selStart)) {
			if ((selStart == selEnd) || (!CursorInView(selEnd))) {
				return;
			}
		}
		
		// if we have outline hilite and we are on top of the selection
		// then assume the user wants to do something with the selection
		SInt8 outlineHilite = FeatureFlag(weFOutlineHilite, weBitTest);
		if (outlineHilite && IsInSelection(mouseLoc)) {
			SwitchTarget(this);
			return;
		}
		else {
		// In this case we are not on the selection.
		// assume the user wants to start a cursor at the mousepoint.
			SInt8 prevScroll = FeatureFlag(weFAutoScroll, weBitClear);
			FocusDraw();
			WESetSelection(0, 0, mWASTEEditH);
			FeatureFlag(weFAutoScroll, prevScroll);
			SwitchTarget(this);
		}
	}
	else
	{
		FocusDraw();
		// we need to fool waste so that it calcs things right
		// shift over by the port origin. 9/28/96
		mouseLoc = inMouseDown.wherePort;
		mouseLoc.v += mPortOrigin.v;
		mouseLoc.h += mPortOrigin.h;
		EventModifiers modifiers = inMouseDown.macEvent.modifiers;
		UInt32 when = inMouseDown.macEvent.when;
		if (FeatureFlag(weFDragAndDrop, weBitTest) &&
				WaitMouseMoved ( inMouseDown.macEvent.where )) 
		{
			// this is the begining of a drag so let WEClick handle it
			// 1/16/97 timp
			// if the end of the text is not in view, don't allow a drop.
			BeginDrag(mouseLoc, inMouseDown.macEvent);
		} else {
			WEClick(mouseLoc, modifiers, when, mWASTEEditH); 
		}
		
		AdjustImageToText();
		// added timp 9/28/96
		CheckScroll();
		// removed SelView call 10/16/96
	}
}


//-----------------------------------------------------------------------------
//		¥ IsInSelection
//-----------------------------------------------------------------------------
//	Function takes in a mouse locaton in global coordinates and
//	determines if it is over the selction.
//
//	Revision: initial version 1/31/97

Boolean
CWASTEEdit::IsInSelection(
	Point	inMouseLoc)
{
	SInt32	selStart, selEnd;
	char	dummyEdge = '\0';
	Boolean	onTopOfSelection = false;
	
	// get the selection Range
	GetTextSelection(&selStart, &selEnd);
	// if we have a selection range
	if (selEnd != selStart) {
		RgnHandle hiliteRgn = GetHiliteRgn(selStart, selEnd);
		OffsetRgn(hiliteRgn, -mPortOrigin.h, -mPortOrigin.v);
		// if this is on top of the selection then we want to adjust
		// the cursor we may need to adjust the ptLocation
		onTopOfSelection = ::PtInRgn(inMouseLoc, hiliteRgn);
		::DisposeRgn(hiliteRgn);
	}
	return onTopOfSelection;
}


// ---------------------------------------------------------------------------
//		¥ AdjustCursorSelf
// ---------------------------------------------------------------------------
// This method adjusts the cursor depending upon where it is. I am
// intentionally not using the WEAdjustCursor call. PowerPlant has this
// very useful function and it is quite easy to manipulate the cursor.
// This way if someone wants to overide the below behavior, they can just
// inherit from CWASTEEdit and change it. To get different behavior using
// WASTE I would have to change WEAdjustCursor and update it each time a
// new release came out. I think the below implementation is easier

void
CWASTEEdit::AdjustCursorSelf(
	Point				inPortPt,
	const EventRecord &	/* inMacEvent*/)
{	
	CursHandle	theCursH = nil;
#ifdef USE_HAND_CURSOR
	// timp 9/28/96
	inPortPt.v += mPortOrigin.v;
	inPortPt.h += mPortOrigin.h;
	
	//	to get this to work you have to hack WEMouse.
	if ((wasteAttr_Selectable & mTextAttributes)) {
		
		if (IsInSelection(inPortPt) && FeatureFlag(weFDragAndDrop, weBitTest)) {
			unsigned char clickCount = WEGetClickCount(mWASTEEditH);
			// if the mouse is down (the btnState is 0, Not 1) then show a
			// grabbing hand
			// btnState is a mask for the modifiers field.
			if (!(inMacEvent.modifiers & btnState)) {
				if (clickCount < 1) {
					// if we are down, show the grabhand
					theCursH = UCursor::SetTheCursor(grabHandCursor);
				}
				else {
					// if the click count is > 1 then show an IBeam
					// since we are seleting text.
					theCursH = UCursor::SetIBeam();
				}
			}
			else {
				theCursH = UCursor::SetTheCursor(openHandCursor);
			}
		}
		else {
			theCursH = UCursor::SetIBeam(iBeamCursor);
		}
	}
	else {
		// Convert the point to Global coordinates
		// That's what WEAdjustCursor likes.
		PortToGlobalPoint(inPortPt);
		if (!WEAdjustCursor(inPortPt, nil, mWASTEEditH)) {
			// Waste failed and we arn't on a selection
			// so show the Ibeam cursor
			UCursor::SetIBeam();
		}
	}
//--------------------Alternate standard behavior------------------------------
#else
	if (FocusDraw()) {
		PortToGlobalPoint(inPortPt);
		if (!WEAdjustCursor(inPortPt, nil, mWASTEEditH)) {
			// Waste failed
			CursHandle	theAltCursH = ::GetCursor(iBeamCursor);
			if (theAltCursH != nil) {
				::SetCursor(*theAltCursH);
			}
		}
	}
#endif	
}


// ---------------------------------------------------------------------------
//		¥ ObeyCommand
// ---------------------------------------------------------------------------
// added doScrollCheck feature. Now after every command, the alignment
// and scrolling of the rects are checked.
// 9/28/96

Boolean
CWASTEEdit::ObeyCommand( 
	CommandT	inCommand,
	void *		ioParam)
{
	Boolean	cmdHandled = true;
	Boolean	doScrollCheck = true;
	FocusDraw();
#ifdef NO_AUTOSCROLL
	SInt8 prevScroll = FeatureFlag(weFAutoScroll, weBitClear);
#endif
	switch (inCommand) {
		case cmd_Undo:
			if (!(mTextAttributes & wasteAttr_Editable))
				break;
			ThrowIfOSErr_(WEUndo(mWASTEEditH));
			UserChangedText();
			SetUpdateCommandStatus(true);
			break;
			
		case cmd_Cut:
			if (!(mTextAttributes & wasteAttr_Editable))
				break;
			ThrowIfOSErr_(WECut(mWASTEEditH));
			UserChangedText();
			SetUpdateCommandStatus(true);
			break;
			
		case cmd_Copy:
			if (!(mTextAttributes & wasteAttr_Selectable))
				break;
			ThrowIfOSErr_(WECopy(mWASTEEditH));
			doScrollCheck = false;
			break;
			
		case cmd_Paste:
			if (!(mTextAttributes & wasteAttr_Editable))
				break;
			ThrowIfOSErr_(WEPaste(mWASTEEditH));
			UserChangedText();
			SetUpdateCommandStatus(true);
			break;
			
		case cmd_Clear: 
			if (!(mTextAttributes & wasteAttr_Editable))
				break;
			ThrowIfOSErr_(WEDelete(mWASTEEditH));
			UserChangedText();
			SetUpdateCommandStatus(true);
			break;
			
		case msg_TabSelect:
			if (!IsEnabled()) {
				cmdHandled = false;
				break;
			} 
			// WEKey// else FALL THRU to SelectAll()
			
		case cmd_SelectAll:
			if (!(mTextAttributes & wasteAttr_Selectable))
				break;
			SelectAll();
			break;
		
		default:
			doScrollCheck = false;
			cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
			break;
	}
#ifdef NO_AUTOSCROLL
	FeatureFlag(prevScroll, weBitSet);
#endif
	if (doScrollCheck) {
		// timp 9/30/96
		AdjustImageToText();
		CheckScroll();
	}		
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//		¥ FindCommandStatus
// ---------------------------------------------------------------------------

void
CWASTEEdit::FindCommandStatus(
	CommandT	inCommand,
	Boolean	&	outEnabled,
	Boolean &	outUsesMark,
	UInt16 &	outMark,
	Str255		outName)
{
	outUsesMark = false;
	
	FocusDraw();
	
	switch (inCommand) {
	
		case cmd_Undo:
			if (!(mTextAttributes & wasteAttr_Editable)) {
				outEnabled = false;
			}
			else {
				Boolean temp;
				// actionKind is the action to perform for the undo
				// cut, copy, drag, whatever.
				WEActionKind actionKind = WEGetUndoInfo(&temp, mWASTEEditH);
				// This tests to see whether we are undoing or redoing
				short boolNum = 0;
				if (temp)
					boolNum = 1;
				if ((!temp) && (actionKind == 0))
					boolNum = 1;
				
				// Get the string to put in the undo menu item
				Str255	itemString;
				short	strID=(2*actionKind) + boolNum;
				::GetIndString(itemString, rUndoStringsID, strID);
				
				// Now copy it into outName to change the menu
				LString::CopyPStr(itemString, outName);
				// is nothing to undo
				outEnabled =	(strID > 1);
			}
			break;
			
		case cmd_Copy:
			if (!(mTextAttributes & wasteAttr_Selectable)) {
				break;
			}
			else {
				SInt32	start, end;
				GetTextSelection(&start, &end);
				outEnabled = (start != end);
			}
			break;

		case cmd_Cut:				// Cut, Copy, and Clear enabled
		case cmd_Clear:				//	 if something is selected
			if (!(mTextAttributes & wasteAttr_Editable)) {
				outEnabled = false;
			}
			else {
				SInt32	start, end;
				GetTextSelection(&start, &end);
				outEnabled = (start != end);
			}
			break;
					
		case cmd_Paste:
			if (!(mTextAttributes & wasteAttr_Editable)) {
				outEnabled = false;
			}
			else {			
				outEnabled = WECanPaste(mWASTEEditH);
			}
			break;
		
		case cmd_SelectAll:			// Check if any characters are present
			outEnabled = (GetTextLength() > 0);
			break;
			
		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}



Boolean
CWASTEEdit::HandleKeyPress(
	const EventRecord &	inKeyEvent)
{
	Boolean		keyHandled = true;
	Boolean		doScrollCheck = true;
	EKeyStatus	theKeyStatus = keyStatus_Input;
	SInt16		theKey = inKeyEvent.message & charCodeMask;
		
	if (wasteAttr_Editable & mTextAttributes) {
		//remember the target. We need to check this before doing scroll synch
		LCommander	*theTarget = GetTarget();
		
		SInt32	selStart, selEnd;
		GetTextSelection( &selStart, &selEnd );
		Boolean beforeSel = (selStart == selEnd);
		
		FocusDraw();
		
		if (inKeyEvent.modifiers & cmdKey) {	// Always pass up when the
			theKeyStatus = keyStatus_PassUp;	// command key is down
		}
		else {
			// UKeyFilters tells us what kind of key was hit and
			// from there we can take the appropriate action
			theKeyStatus = UKeyFilters::PrintingCharField(inKeyEvent);
		}
		
		// now check out what key was typed and take the correct action
		switch (theKeyStatus) {
		
			case keyStatus_Input:
				FocusDraw();
				WEKey(theKey, inKeyEvent.modifiers, mWASTEEditH);
				UserChangedText();
				break;

			case keyStatus_TEDelete:
				FocusDraw();
				WEKey(theKey, inKeyEvent.modifiers, mWASTEEditH);
				UserChangedText();
				break;
				
			case keyStatus_TECursor:
				FocusDraw();	
				WEKey(theKey, inKeyEvent.modifiers, mWASTEEditH);
				break;
				
			case keyStatus_ExtraEdit:
				if (theKey == char_FwdDelete) {
					FocusDraw();
					WEKey(theKey, inKeyEvent.modifiers, mWASTEEditH);
					UserChangedText();
				}
				else {
					// these really aren't handled.
					keyHandled = LCommander::HandleKeyPress(inKeyEvent);
				}
				break;
				
			case keyStatus_Reject:
				// +++ Do something
				doScrollCheck = false;
				SysBeep(1);
				break;
				
			case keyStatus_PassUp:
				if ((theKey == char_Return) || (theKey==char_Tab)) {
					FocusDraw();
					WEKey(theKey, inKeyEvent.modifiers, mWASTEEditH);
					UserChangedText();
				}
				else {
					keyHandled = LCommander::HandleKeyPress(inKeyEvent);
				}
				break;
		}
		// This is to keep track of whether we are inputing text
		// we only want to update the menus at the begining of such
		// a sequence.
		GetTextSelection( &selStart, &selEnd );
		if ( beforeSel != ( selStart == selEnd ) ) {
			SetUpdateCommandStatus(true);
		}	
		// 9/30/96 added new system for better scroll synchronization
		// CheckScroll is a new routine
		if (doScrollCheck && (GetTarget() == theTarget) ) {
			AdjustImageToText();
			CheckScroll();
		}
	}
	else {
		if (keyStatus_TECursor == theKeyStatus) { 
			FocusDraw();	
			WEKey(theKey, inKeyEvent.modifiers, mWASTEEditH);
			keyHandled = true;
		}
		else {		
			keyHandled = LCommander::HandleKeyPress(inKeyEvent);
		}
	}
	return keyHandled;
}


// ---------------------------------------------------------------------------
//		¥ SelectAll
// ---------------------------------------------------------------------------
//	Select entire contents of an WASTEEdit

void
CWASTEEdit::SelectAll()
{
	SInt8 prevScroll = FeatureFlag(weFAutoScroll, weBitClear);
	FocusDraw();
	WESetSelection(0, max_Int32, mWASTEEditH);
	FeatureFlag(weFAutoScroll, prevScroll);
}


// ---------------------------------------------------------------------------
//		¥ AlignWASTEEditRects
// ---------------------------------------------------------------------------
//	Align the view and destination rectangles of the WASTEEdit
//	This routine insures that the Frame.topLeft == viewRect.topLeft and
//	Image.topLeft == destRect.topLeft

void
CWASTEEdit::AlignWASTEEditRects()
{
	Rect textFrame;
	
	if (FocusDraw() && CalcLocalFrameRect(textFrame)) {
		// make sure the frame and the viewRect are the same.
		LongRect theLongViewRect, theLongDestRect;
		WERectToLongRect(&textFrame, &theLongViewRect);
		WESetViewRect(&theLongViewRect, mWASTEEditH);
		
		// make sure the image and the destRect are the same
		
		theLongDestRect.top = mImageLocation.v + mPortOrigin.v;
		theLongDestRect.left = mImageLocation.h + mPortOrigin.h;
		theLongDestRect.bottom = theLongDestRect.top + mImageSize.height;
		theLongDestRect.right = theLongDestRect.left + mImageSize.width;

		WESetDestRect(&theLongDestRect, mWASTEEditH);
		
		if (mTextAttributes & wasteAttr_WordWrap) {
			CalText();
		}
	}
}


// ---------------------------------------------------------------------------
//		¥ AdjustImageToText
// ---------------------------------------------------------------------------
// This adjusts the image size to match the destRect. 

void
CWASTEEdit::AdjustImageToText()
{
	LongRect theLongDestRect;
	WEGetDestRect(&theLongDestRect, mWASTEEditH);
	
	// we want the total height and waste will keep it in range
	// SInt32 height = WEGetHeight(0, max_Int32, mWASTEEditH);
	
	ResizeImageTo((theLongDestRect.right - theLongDestRect.left),
			(theLongDestRect.bottom - theLongDestRect.top), true);
	
	// 9/28/96 timp
	// removed the scroll checking nonsense. This is handled in a new routine
}


// ---------------------------------------------------------------------------
//		¥ ResizeFrameBy
// ---------------------------------------------------------------------------
//	Change the Frame size by the specified amounts
//
//		inWidthDelta and inHeightDelta specify, in pixels, how much larger
//		to make the Frame. Positive deltas increase the size, negative deltas
//		reduce the size.
//
// 6/2/98 This was totally revamped by a shameless rip off of PPlant code
// from LTextEditView.cp. Thanks to John Daub! 

void
CWASTEEdit::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	LView::ResizeFrameBy(inWidthDelta, inHeightDelta, Refresh_No);
	
	//now resize the image. This is only needed if word wrap is
	//turned on
	if (mTextAttributes & wasteAttr_WordWrap) {
		ResizeImageTo(mFrameSize.width, mImageSize.height, Refresh_No);	
	}
	AlignWASTEEditRects();
	
	LView::OutOfFocus(this);
	
	AdjustImageToText();
	
	if(mTextAttributes & wasteAttr_WordWrap) {
		SelView();
		CheckScroll();
	}
	
	if (inRefresh) {				// It's safe to refresh now that
		Refresh();					//   everything is in synch
	}
	
	// 1/16/97 added to change parameters if inside scroller
	// This is very important if you are inside a scroller and 
	// the frame binding is changing
	if (mInScroller) {
		AdaptToEnclosingScroller();
	}
}


// ---------------------------------------------------------------------------
//		¥ MoveBy
// ---------------------------------------------------------------------------
//	Move the location of the Frame by the specified amounts
//
//		inHorizDelta and inVertDelta specify, in pixels, how far to move the
//		Frame (within its surrounding Image). Positive horiz deltas move to
//		the left, negative to the right. Positive vert deltas move down,
//		negative up.

void
CWASTEEdit::MoveBy(
	SInt32		inHorizDelta,
	SInt32		inVertDelta,
	Boolean		inRefresh)
{
	LView::MoveBy(inHorizDelta, inVertDelta, inRefresh);
	AlignWASTEEditRects();
}


// ---------------------------------------------------------------------------
//		¥ ScrollImageBy
// ---------------------------------------------------------------------------
//	Scroll the Text

void
CWASTEEdit::ScrollImageBy(
	SInt32		inLeftDelta,	// Pixels to scroll horizontally
	SInt32		inTopDelta,		// Pixels to scroll vertically
	Boolean		inRefresh)
{
	FocusDraw();
	// inhibit redraw PPlant will do this later
	SInt8 prevDraw = FeatureFlag(weFInhibitRedraw, weBitSet);
	// Scroll the destRect
	WEScroll(-inLeftDelta, -inTopDelta, mWASTEEditH);
	
	// set redraw back if we inhibited it
	FeatureFlag(weFInhibitRedraw, prevDraw);
	// added timp 9/28/96
	CheckScroll();
	
	if (inRefresh) {
		FocusDraw();
	}
}


// ---------------------------------------------------------------------------
//		¥ ReconcileFrameAndImage
// ---------------------------------------------------------------------------
//	Adjusts the Image so that it fits within the Frame
//
//	This is useful for short text blocks in a frame that you want to always
//	be at the top when the dest rect shrinks.
//	Timp v 1.5 1/28/97

void
CWASTEEdit::ReconcileFrameAndImage(
	Boolean	inRefresh)
{
	LView::ReconcileFrameAndImage(inRefresh);
	if (mReconcileOverhang) {
		LongRect destRect, viewRect;
		::WEGetDestRect(&destRect, mWASTEEditH);
		::WEGetViewRect(&viewRect, mWASTEEditH);
		SInt32 destHeight = destRect.bottom - destRect.top;
		if (destHeight < viewRect.bottom - viewRect.top) {
			if (destRect.top != viewRect.top) {
				destRect.top = viewRect.top;
				destRect.bottom = destHeight;
				::WESetDestRect(&destRect, mWASTEEditH);
				::WEUpdate(nil, mWASTEEditH);
				CheckScroll();
			}		
		}
	}
}


// ---------------------------------------------------------------------------
//		¥ CountPanels
// ---------------------------------------------------------------------------

void
CWASTEEdit::CountPanels(
	UInt32 &	outHorizPanels,
	UInt32 &	outVertPanels)
{
	// Remove any items in the print record. 
	SInt32 count = mPrintPanels->GetCount();
	if (count > 0)
		mPrintPanels->RemoveItemsAt(count, 1);
	
	SDimension32 imageSize;
	GetImageSize(imageSize);
	
	// note that the frameSize is the size of the page if you have set
	// up your placeholder correctly.
	SDimension16 frameSize;
	GetFrameSize(frameSize);
	
	SInt32	lineNo = 0;
	SInt32	lineCount = WECountLines(mWASTEEditH);
	SInt16	panelHeight = 0;
	SInt32	totalHeight = 0;

	outVertPanels = 0;
	
	while( (imageSize.height > totalHeight) && (lineNo < lineCount)) {
		// remember the offset for this panel
		mPrintPanels->InsertItemsAt(1, LArray::index_Last, (void*)&(totalHeight));
		// keep adding lines until we extend past the frame
		// also keep it in the bounds of the total line count
		while((panelHeight < frameSize.height) && (lineNo < lineCount)) {
			panelHeight += WEGetHeight(lineNo, lineNo + 1, mWASTEEditH);
			lineNo++;
		}
		if (lineNo < lineCount) {
			// get rid of the last line since it is over the frame
			panelHeight -= WEGetHeight(lineNo - 1, lineNo, mWASTEEditH);
			lineNo--;
		}
		// we have reached a panel end so add to the number of panels
		outVertPanels++;
		// add this panel to the totalHeight
		totalHeight += panelHeight;
		panelHeight = 0;
	}
	// we don't support more than one horizontal panel, override if
	// you want to do this
	outHorizPanels = 1;
}


// ---------------------------------------------------------------------------
//		¥ ScrollToPanel
// ---------------------------------------------------------------------------

Boolean
CWASTEEdit::ScrollToPanel(
	const PanelSpec &	inPanel)
{
	Boolean			panelInImage = false;
	LongPt			thePoint;
	SInt32			bottom;
	SDimension16	frameSize = mFrameSize;
	UInt32			horizPanelCount = 1;
	UInt32			vertPanelCount = mPrintPanels->GetCount();
	
	// we need to rewrite this so that we get the offset by going
	// to the class variable. 
	if ((inPanel.horizIndex <= horizPanelCount) &&
		(inPanel.vertIndex <= vertPanelCount)) {
		mPrintPanels->FetchItemAt(inPanel.vertIndex, (void *) &thePoint.v);
		
		// calculate the bottom of this panel, we use this to set the frame
		SInt32	lineCount = WECountLines(mWASTEEditH);
		if (inPanel.vertIndex != vertPanelCount)
			mPrintPanels->FetchItemAt(inPanel.vertIndex + 1, (void *) &bottom);
		else
			bottom = WEGetHeight(0, lineCount, mWASTEEditH);
			
		thePoint.h = 0;
		
		// faster than calling the PPlant ResizeFrameTo
		mFrameSize.height =	 bottom - thePoint.v;
		AlignWASTEEditRects();
		ScrollImageTo(thePoint.h, thePoint.v, false);
		panelInImage = true;
	}
	
	return panelInImage;
}


// ---------------------------------------------------------------------
//    * SuperPrintPanel
// ---------------------------------------------------------------------
//This over ride allows us to set the port on the WASTE WEHandle to the printing
//port.  This way we won't see the scolling on the screen during printing.
//Need to do this because CWASTEEdit's DrawSelf function sets the port to the
//print port for the drawing but sets it back when its done.

void
CWASTEEdit::SuperPrintPanel(
				const PanelSpec &inSuperPanel, 
				RgnHandle 		inSuperPrintRgnH)
{
        //Set the WEHandle port to the printer port
        GrafPtr newPort = UQDGlobals::GetCurrentPort();
        ThrowIfOSErr_(WESetInfo(wePort, &newPort, mWASTEEditH));


        LView::SuperPrintPanel(inSuperPanel, inSuperPrintRgnH);
}


// ---------------------------------------------------------------------------
//		¥ BeTarget
// ---------------------------------------------------------------------------
//	WASTEEdit is becoming the Target

void
CWASTEEdit::BeTarget()
{
	if (FocusDraw()) {				// Show active selection
		WEActivate(mWASTEEditH);
	}
	StartIdling();
	
	// check to see if theSuperView is a LScroller. If it is, we want to 
	// activate it
	if (mScroller != nil && (mTextAttributes & wasteAttr_ActDeactScroll)) {
		mScroller->Activate();
	}
	//removed because it messed up selection during printing
	//ProtectSelection();
}


// ---------------------------------------------------------------------------
//		¥ ProtectSelection
// ---------------------------------------------------------------------------
//	ProtectSelection
//	Purpose: During activation the selection rectange is not being updated
//	correctly if another window was in front of it. Protect selection accounts
//	for this.
//
//	Revision: 10/4/96 - put the routine back in.
//
//			   11/6/98 - I think this is not longer needed WASTE 
//			took care of the problem. I will leave it in case that is not true
 
void
CWASTEEdit::ProtectSelection()
{
	// this is needed to make the updating of the selection rectangle work
	WindowPtr	macWindowP = GetMacPort();
	RgnHandle	updateRgn = NewRgn();
	SInt32	selStart, selEnd;
	
	GetUpdateRgn(macWindowP, updateRgn);
	
	GetTextSelection(&selStart, &selEnd);
	RgnHandle	theSelRgn = GetHiliteRgn(selStart, selEnd);
	::SectRgn(updateRgn, theSelRgn, theSelRgn);

	FocusDraw();
	
	::EraseRgn(theSelRgn);
	::InvalRgn(theSelRgn);
	::DisposeRgn(theSelRgn);
	::DisposeRgn(updateRgn);
}


// ---------------------------------------------------------------------------
//		¥ GetUpdateRgn
// ---------------------------------------------------------------------------

void
CWASTEEdit::GetUpdateRgn(
	WindowPtr	inWindow,
	RgnHandle	outRgnLocal)
{	//{save old update region}
	::CopyRgn(WindowPeek(inWindow)->updateRgn, outRgnLocal);
	//::OffsetRgn(localRgn, mPortOrigin.h, mPortOrigin.v);
	Point	pointOffset = {-mPortOrigin.v, -mPortOrigin.h};
	::LocalToGlobal(&pointOffset);
	::OffsetRgn(outRgnLocal, -pointOffset.h, -pointOffset.v);
}


// ---------------------------------------------------------------------------
//		¥ DontBeTarget
// ---------------------------------------------------------------------------
//	WASTEEdit is no longer the Target
//
//	Remove WASTEEdit from IdleQueue

void
CWASTEEdit::DontBeTarget()
{
	if (FocusDraw()) {				// Show inactive selection
		WEDeactivate(mWASTEEditH);
	}
	StopIdling();					// Stop flashing the cursor
	
	// check to see if theSuperView is a LScroller. If it is, we want to 
	// deactivate it
	if (mScroller != nil && (mTextAttributes & wasteAttr_ActDeactScroll)) {
		mScroller->Deactivate();
	}
}


// ---------------------------------------------------------------------------
//		¥ SpendTime
// ---------------------------------------------------------------------------
//	Idle time: Flash the insertion cursor

void
CWASTEEdit::SpendTime(
	const EventRecord &	/* inMacEvent */)
{	
	if ((mTextAttributes & wasteAttr_Selectable) && FocusDraw() && IsVisible()) {
		SInt32 selStart, selEnd;
		GetTextSelection(&selStart, &selEnd);
		
		if ((selStart == selEnd) && CursorInView(selStart)) {
			// flash the cursor
			WEIdle(nil, mWASTEEditH);
		}

#if WASTE_OBJECTS
		// check to see if any memory tied up for sounds
		// can be deallocated
		DoObjectTasks(mWASTEEditH);		
#endif

	}
}


// ---------------------------------------------------------------------------
//		¥ CursorInView
// ---------------------------------------------------------------------------

Boolean
CWASTEEdit::CursorInView(
	SInt32	inPosition)
{
	Boolean retVal = true;
	// This was added for views inside of scrollers where the cursor
	// may scroll out of view, but the view is partially visible.
	if (mInScroller && (mSuperView != mScroller)) {
		LongPt	longPoint;
		Point	shortPoint;
		Rect	superRect;
		short	lineHeight; 
		
		GetPoint(inPosition, 0, &longPoint, &lineHeight);
		WELongPointToPoint(&longPoint, &shortPoint);
		LocalToPortPoint(shortPoint);
		
		Assert_(mScroller != nil);
		mScroller->CalcPortFrameRect(superRect);
		if (!PtInRect(shortPoint, &superRect))
			retVal = false;
	}
	return retVal;
}


// ---------------------------------------------------------------------------
//		¥ UserChangedText
// ---------------------------------------------------------------------------
//	Text of WASTEEdit has changed as a result of user action
//
//	Override to validate field and/or dynamically update as the user
//	types. This function is not called by routines which is programatically
//	change the text.

void
CWASTEEdit::UserChangedText()
{
}

// ---------------------------------------------------------------------------
//		¥ BeginDrag
// ---------------------------------------------------------------------------
// This is a new routine to allow override of the data that is put in 
// a drag. The default just uses the WASTE impementation 
// 

void
CWASTEEdit::BeginDrag(
		Point mouseLoc,
		const EventRecord &inEventRecord)
{
	WEClick(mouseLoc, inEventRecord.modifiers, inEventRecord.when, mWASTEEditH);
}

// ---------------------------------------------------------------------------
//		¥ DragIsAcceptable
// ---------------------------------------------------------------------------
// The below routines are for implementing drag and drop with
// waste. I basically let WASTE handle the details and try to integrate
// it with PPlants implementation where it makes sense

Boolean
CWASTEEdit::DragIsAcceptable(
	DragReference	inDragRef)
{
	if ( (!(mTextAttributes & wasteAttr_Editable)) || (!FeatureFlag(weFDragAndDrop, weBitTest))) {
		return false;
	} 
	// if we cannot see the end of the text, then don't allow a drop.
	// This only happens when the WASTEEdit pane is inside a scrolling
	// view and all the text is scrolled out of view.
	SInt32	offset = GetTextLength();
	if (!CursorInView(offset)) {
		return false;
	}
	
	FocusDraw();
	return WECanAcceptDrag(inDragRef, mWASTEEditH);
}


// ---------------------------------------------------------------------------
//		¥ DoDragReceive
// ---------------------------------------------------------------------------

void
CWASTEEdit::DoDragReceive(
	DragReference	inDragRef)
{
	if (!(mTextAttributes & wasteAttr_Editable) || (!FeatureFlag(weFDragAndDrop, weBitTest))) {
		return;
	}
			
	// The next three calls get the pane ready to accept the drop 
	LView::OutOfFocus(nil);
	FocusDraw();
	UnhiliteDropArea(inDragRef);

	OSErr	theErr = WEReceiveDrag(inDragRef, mWASTEEditH);
	
	if (theErr == noErr) {
		AdjustImageToText();
		// added timp 9/28/96
		CheckScroll();
		UserChangedText();
		SetUpdateCommandStatus(true);
	}
	else if (theErr != dragNotAcceptedErr) {
		// ignore the dragNotAcceptErr. This means the text was 
		// dragged back on top of the same selection
		ThrowIfOSErr_(theErr);
	}
}


// ---------------------------------------------------------------------------
//		¥ EnterDropArea
// ---------------------------------------------------------------------------

void
CWASTEEdit::EnterDropArea(
	DragReference	inDragRef,
	Boolean			/*inDragHasLeftSender*/)
{
	if (!(mTextAttributes & wasteAttr_Editable) || (!FeatureFlag(weFDragAndDrop, weBitTest))) {
		return;
	}
	else {
		WETrackDrag(kDragTrackingEnterWindow, inDragRef, mWASTEEditH);
	}
}
// ---------------------------------------------------------------------------
//		¥ HiiliteDropArea
// ---------------------------------------------------------------------------
//	Add the drag hilite. We are coming into the area and need to hilite
// the drop area
void
CWASTEEdit::HiliteDropArea(
					DragReference	inDragRef)
{
	if(!mIsHilited)
	{
		ApplyForeAndBackColors();
		Rect	dropRect;
		CalcLocalFrameRect(dropRect);
		//::InsetRect(&dropRect, 1, 1);
		StClipRgnState	theClip(dropRect);
		StRegion	dropRgn(dropRect);
		::ShowDragHilite(inDragRef, dropRgn, true);
		mIsHilited = true;
	}
}


// ---------------------------------------------------------------------------
//		¥ LeaveDropArea
// ---------------------------------------------------------------------------

void
CWASTEEdit::LeaveDropArea(
	DragReference	inDragRef)
{
	if (!(mTextAttributes & wasteAttr_Editable) || (!FeatureFlag(weFDragAndDrop, weBitTest))) {
		return;
	}
	else {
		FocusDraw();
		WETrackDrag(kDragTrackingLeaveWindow, inDragRef, mWASTEEditH);
		mCanAcceptCurrentDrag = false;
		//UnhiliteDropArea(inDragRef);
	}
}


// ---------------------------------------------------------------------------
//		¥ InsideDropArea
// ---------------------------------------------------------------------------

void
CWASTEEdit::InsideDropArea(
	DragReference	inDragRef)
{
	if (!(mTextAttributes & wasteAttr_Editable) || (!FeatureFlag(weFDragAndDrop, weBitTest))) {
		return;
	}
	else {
		FocusDraw();
		WETrackDrag(kDragTrackingInWindow, inDragRef, mWASTEEditH);
	}
}


// ---------------------------------------------------------------------------
//		¥ PointInDropArea
// ---------------------------------------------------------------------------
// I overrode this to make autoscrolling during drags work better
// The default routine compares the point to the size of the 
// CWASTEEdit pane. We want the LDragAndDrop to think it is 
// inside the pane if inPoint is anywhere inside theSuperView.
// (This is often an LScroller Object.)
// This allows the WETrackDrag Function to call the clickLoop
// When it detects that the mouse is outside viewRect but inside
// the superView and we getter better autoscrolling behavior.

Boolean
CWASTEEdit::PointInDropArea(
	Point	inPoint)
{
	if (!(mTextAttributes & wasteAttr_Editable) || (!FeatureFlag(weFDragAndDrop, weBitTest))) {
		return false;
	}
	else {
		Boolean rVal = 0;
		// check to see if theSuperView is a LScroller. If it is, we want to 
		// scroll the view otherwise don't I love RTTI!
		if (mInScroller) {
			// CWASTEEdit is in a scroller or some other view.
			// check to see if theSuperView is hit.
			GlobalToPortPoint(inPoint);
			// The messing with the frame location and size is to give a
			// 16 pixel top and left margin so that autoscrolling during
			// a drag will work when the cursor moves outside the top of
			// left of the viewRect but within 16 pixels. This may need
			// to be overridden depending upon your particular CWASTEEdit
			// view set up. LPane's method is called to prevent undesired
			// adjustment of subimages. They are not needed here since we
			// immeadiately set it back to it's original size. 

			// mSuperView->
			LPane::ResizeFrameBy(2*sMaxScrollDelta, 2*sMaxScrollDelta, false);
			// mSuperView->
			LPane::MoveBy(-sMaxScrollDelta, -sMaxScrollDelta, false);
			
			rVal =	IsHitBy(inPoint.h, inPoint.v);
			
			// mSuperView->
			LPane::ResizeFrameBy(-2*sMaxScrollDelta, -2*sMaxScrollDelta, false);
			// mSuperView->
			LPane::MoveBy(sMaxScrollDelta, sMaxScrollDelta, false);
		}
		else {
			// The superView is the window object. The default method will
			// define the CWASTEEdit area
			rVal = LDragAndDrop::PointInDropArea( inPoint );
		}
		return rVal;
	}
}


// ---------------------------------------------------------------------------
//		¥ MyDragHiliteProc
// ---------------------------------------------------------------------------
//	Purpose: I let WASTE take this over. We call the PPlant functions but waste
//  decides when. This removed a whole bunch of hiliting anomilies during 
// 	scrolling. Yippee!
//
//	Revision: 1/31/97
//			5/4/98 let Waste decide when to hilite/unhilite
//------------------------------------------------------------------------- 

pascal OSErr
CWASTEEdit::MyDragHiliteProc(
	DragReference	 	drag,
	Boolean			 	hiliteFlag,
	WEHandle			hWE)
{
	CWASTEEdit *	wastePtr;
	WEGetInfo(weRefCon, &wastePtr, hWE);
	Point		where;
	
	::GetMouse(&where);
	if(!hiliteFlag )
	{
		if(wastePtr->mIsHilited)
		{
			wastePtr->UnhiliteDropArea(drag);
			wastePtr->mIsHilited = false;
		}
	}
	else
	{
		wastePtr->HiliteDropArea(drag);
	}
	return noErr;
}


// ---------------------------------------------------------------------------
//		¥ NormalizePortForWE
// ---------------------------------------------------------------------------
// This routine resets the port and Rgns so that drawing is 
// correct. Don't use this routine. Just call FocusDraw. I left it
// for backward compatibility
 
void
CWASTEEdit::NormalizePortForWE()
{	
	// fix by Rick Eames. There was a problem with windows
	// that contained the WasteView at anything other than 0, 0 this 
	// fixes it.
	FocusDraw();	
}


// ---------------------------------------------------------------------------
//		¥ ScrolledImageReport
// ---------------------------------------------------------------------------
// Waste and PowerPlant handle scrolling differently. Waste keeps the
// viewRect at 0, 0 and scrolls the destRect. PowerPlant keeps the 
// Image (== destRect) at 0, 0 and moves the frame (== viewRect). We
// have to account for this or really bad things happen. I have choosen
// to let waste win and adjust the frame and image to match the viewRect
// and destRect, respectively.

void
CWASTEEdit::ScrolledImageReport(
	SInt32	inLeftDelta,
	SInt32	inTopDelta)
{
	LArrayIterator iterator(mSubPanes, LArrayIterator::from_Start);
	LPane	*theSub;
	while (iterator.Next(&theSub)) {
		// This adapts any panes inside the WasteEdit
		// I don't know if there will be any, but it's
		// good to put this in. It also caused the frame to move
		// added code here to make sure that subviews are updated 
		// properly. I still have problems with holding down of the 
		// mouse on a control, but it is better.
		// 1/15/97 timp
		Rect subFrame;
		Rect superRevealed;
		
		if (theSub->IsVisible() && theSub->CalcPortFrameRect(subFrame)) {
			GetRevealedRect(superRevealed);
			if (::SectRect(&subFrame, &superRevealed, &subFrame)) {
				OffsetRect( &subFrame, inLeftDelta, inTopDelta);
				InvalPortRect(&subFrame);
			}
		}
		theSub->AdaptToSuperScroll(inLeftDelta, inTopDelta);
		theSub->Draw(nil);
	}
	
	if (mSuperView != nil) {
	// This adjusts the scroll bars
		mSuperView->SubImageChanged(this);
	}
	// removed refresh option. This will be handled in other places.
	// 9/28/96 TimP
}


// ---------------------------------------------------------------------------
//		¥ CheckScroll
// ---------------------------------------------------------------------------
//	Purpose: A new routine to handle adjusting the image and frame whenever
//	WASTE might've moved them. If this does happen, reset the image and frame
//	Then notify sub and super views by calling scrolled image report
//	Revision: 9/28/96
//	I added this routine in hopes of making the scrolling of views make more
//	sense. I think this is better behaved now.

void
CWASTEEdit::CheckScroll()
{
	SPoint32	delta;
	LongRect	viewRect, destRect;
	
	WEGetViewRect(&viewRect, mWASTEEditH);
	WEGetDestRect(&destRect, mWASTEEditH);
	
	// find out if we scrolled, the destRect will scroll if WASTE moves it
	// therefore the diff between these two is the delta. If the destRect
	// moves, we need to adjust the image and the frame location. Remember
	// this is anti powerplant. The frame stays put and the image moves
	delta.h = destRect.left - mImageLocation.h - mPortOrigin.h;
	delta.v = destRect.top - mImageLocation.v - mPortOrigin.v; 
	
	if ( (delta.h != 0) || (delta.v != 0) ) {
		mImageLocation.h = destRect.left - mPortOrigin.h;
		mImageLocation.v = destRect.top - mPortOrigin.v;
	
		// Synchronize PowerPlant view, we reset the frame to the 
		// viewRect
		mFrameLocation.v = viewRect.top - mPortOrigin.v;
		mFrameLocation.h = viewRect.left - mPortOrigin.h;
		
		ScrolledImageReport(delta.h, delta.v);
		
		OutOfFocus(this);
	}
}


// ---------------------------------------------------------------------------
//		¥ GetTextStyleSOUPData
// ---------------------------------------------------------------------------
// Get the data for this WASTE object. typically called during 
// a save operation
//
// 2/3/97
// Fixed a memory leak when an error is throw. If there is an error reading
// data, dispose of any created handles, then throw the error.
//
// 6/2/98
// I made this constant (it really should be) and it is needed by
// the new copy constructor.
//

void
CWASTEEdit::GetTextStyleSOUPData(
	Handle	outTextH,
	Handle	outStylesH,
	Handle	outSOUPH) const
{
	SInt32 length = WEGetTextLength(mWASTEEditH);
	
	// copy the range of selected text to be sent, getting the TEXT
	// styl and SOUP flavors
	OSErr err = CopyRange(0, length, outTextH, (StScrpHandle) outStylesH,
										 outSOUPH);
	ThrowIfOSErr_(err);
}


// ---------------------------------------------------------------------------
//		¥ MyClickLoop
// ---------------------------------------------------------------------------
// I use this instead of LView::AutoScrollImage because I want to 
// to use >= or <=. This makes autoscrolling during a drag work better

pascal Boolean
CWASTEEdit::MyClickLoop(
	WEHandle	hWE)
{
	Point			where;
	Rect			view;
	CWASTEEdit *	WASTEEdit;
	SInt16			scrollUnit;
	
	WEGetInfo(weRefCon, &WASTEEdit, hWE);
	WASTEEdit->CalcLocalFrameRect(view);
	::GetMouse(&where);
	
	// check to see if theSuperView is a LScroller. If it is, we want to 
	// scroll the view otherwise don't I love RTTI!
	if (WASTEEdit->mInScroller) { // see PointInDropArea for explanation
		// is dragged out of the view, but is still inside the superView
		if (where.v >= view.bottom) {
			scrollUnit = where.v - view.bottom;
			scrollUnit = (scrollUnit < sMaxScrollDelta) ? scrollUnit : sMaxScrollDelta;
			WASTEEdit->ScrollPinnedImageBy(0, 1 * scrollUnit, true);
		}							
		else if (where.h >= view.right) {
			scrollUnit = where.h - view.right;
			scrollUnit = (scrollUnit < sMaxScrollDelta) ? scrollUnit : sMaxScrollDelta;
			WASTEEdit->ScrollPinnedImageBy(1 * scrollUnit, 0, true);
		}	
		else if (where.v <= view.top) {
			scrollUnit = view.top - where.v;
			scrollUnit = (scrollUnit < sMaxScrollDelta) ? scrollUnit : sMaxScrollDelta;
			WASTEEdit->ScrollPinnedImageBy(0, -1 * scrollUnit, true);
		}
		else if (where.h <= view.left) {
			scrollUnit = view.left - where.h;
			scrollUnit = (scrollUnit < sMaxScrollDelta) ? scrollUnit : sMaxScrollDelta;
			WASTEEdit->ScrollPinnedImageBy(-1 * scrollUnit, 0, true);
		}
		
	}
	
	// if we don't have a Scroller
	return true;
}

// removed InsetLongRect, since WASTE has this routine.
// timp 9/30/96


// ---------------------------------------------------------------------------
//		¥ InsertPtr
// ---------------------------------------------------------------------------

void
CWASTEEdit::InsertPtr(
	void *			inTextPtr,
	SInt32			inTextLength,
	StScrpHandle	inStylesH,
	Handle			inSoupH,
	Boolean			inDeleteAllBeforeInsertion,
	Boolean			inRefresh)
{
	Assert_(inTextPtr);
	Assert_(inTextLength > 0);

	short prevCal = FeatureFlag(weFInhibitRecal, weBitSet);

	if (inDeleteAllBeforeInsertion)
		SelectAll();

	//
	// normalize the drawing state.
	// If you don't do this, Waste becomes very upset...
	//
	FeatureFlag(weFInhibitRecal, prevCal);
	
	SInt8	prevDraw;
	if(!inRefresh) 
		prevDraw = FeatureFlag(weFInhibitRedraw, weBitSet);

	//
	// insert the text
	//
	SInt8 prevRead = FeatureFlag(weFReadOnly, weBitClear);
	FocusDraw();
	ThrowIfOSErr_(WEInsert(inTextPtr, inTextLength,
					inStylesH, inSoupH, mWASTEEditH));
	FeatureFlag(weFReadOnly, prevRead);
	if(!inRefresh)
		FeatureFlag(weFInhibitRedraw, prevDraw);

	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
	if (inRefresh) {
		FocusDraw();
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//		¥ InsertHandle
// ---------------------------------------------------------------------------

void
CWASTEEdit::InsertHandle(
	Handle			inTextH,
	StScrpHandle	inStylesH,
	Handle			inSoupH,
	Boolean			inDeleteAllBeforeInsertion,
	Boolean			inRefresh)
{
	Assert_(inTextH);

	short prevCal = FeatureFlag(weFInhibitRecal, weBitSet);

	if (inDeleteAllBeforeInsertion)
		SelectAll();

	//
	// Normalize the drawing state.
	// If you don't do this, Waste becomes very upset...
	//
	FeatureFlag(weFInhibitRecal, prevCal);
	
	//inhibit drawing
	SInt8	prevDraw;
	if(!inRefresh) 
		prevDraw = FeatureFlag(weFInhibitRedraw, weBitSet);

	//
	// insert the text
	//
	StHandleLocker	stlock(inTextH);

	FocusDraw();
	SInt8 prevRead = FeatureFlag(weFReadOnly, weBitClear);
		
	ThrowIfOSErr_(WEInsert(*inTextH, GetHandleSize(inTextH),
					inStylesH, inSoupH, mWASTEEditH));
	FeatureFlag(weFReadOnly, prevRead);
	
	if(!inRefresh)
		FeatureFlag(weFInhibitRedraw, prevDraw);

	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
	
	if (inRefresh) {
		FocusDraw();
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//		¥ Insert
// ---------------------------------------------------------------------------

void
CWASTEEdit::Insert(
	Handle			inTextH,
	SInt32			textLength,
	StScrpHandle	inStylesH,
	Handle			inSoupH,
	Boolean			deleteBeforeInsertion,
	Boolean			refresh)
{
	
	SInt8 prevCal = FeatureFlag(weFInhibitRecal, weBitSet);
	// first delete any text that is there. This is important for a revert
	// otherwise not necessary.
	if (deleteBeforeInsertion) {
		SelectAll();
	}
	// first normalize the drawing state. 
	// If you don't do this Waste get's very upset
	FeatureFlag(weFInhibitRecal, prevCal);
	
	//inhibit drawing if necessary
	SInt8	prevDraw;
	if(!refresh) 
		prevDraw = FeatureFlag(weFInhibitRedraw, weBitSet);

	// insert the text
	FocusDraw();
	{
		SInt8 prevRead = FeatureFlag(weFReadOnly, weBitClear);
		StHandleLocker	lock(inTextH);
		ThrowIfOSErr_(WEInsert((Ptr)(*inTextH), textLength, inStylesH, inSoupH, mWASTEEditH));
		FeatureFlag(weFReadOnly, prevRead);
		
	}
	
	if(!refresh)
		FeatureFlag(weFInhibitRedraw, prevDraw);

	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
	
	if (refresh) {
		FocusDraw();
		Refresh();
	}
}


// ---------------------------------------------------------------------------
//		¥ SetStyle
// ---------------------------------------------------------------------------

void
CWASTEEdit::SetStyle(
	short				inMode,
	const TextStyle *	inStyle)
{
	// you can't set the style on a read only instance.
	// should we let the programmer decide this or check it here
	// 1/11/97
	if (FeatureFlag(weFReadOnly, weBitTest))
		return;
	SInt8	prevDraw;
	SInt32	selStart, selEnd;
	
	// if we aren't multistyle then change everything
	// 1/11/97
	if ( !(mTextAttributes & wasteAttr_MultiStyle) ) {
		prevDraw = FeatureFlag(weFInhibitRedraw, weBitSet);
		GetTextSelection(&selStart, &selEnd);
		CWASTEEdit::SelectAll();
	}
	
	SInt8 prevScroll = FeatureFlag(weFAutoScroll, weBitClear);
	OSErr theErr = WESetStyle(inMode, inStyle, mWASTEEditH);
	FeatureFlag(weFAutoScroll, prevScroll);
	// split out error checking so that FeatureFlag was set back.
	// 10/4/96
	ThrowIfOSErr_(theErr);
	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
	
	// 1/11/97
	if ( !(mTextAttributes & wasteAttr_MultiStyle) ) {
		FeatureFlag(weFInhibitRedraw, prevDraw);
		SetSelection(selStart, selEnd);
		Refresh();
	}
	UserChangedText();
}


// ---------------------------------------------------------------------------
//		¥ UseStyleScrap
// ---------------------------------------------------------------------------

OSErr
CWASTEEdit::UseStyleScrap(
	StScrpHandle	inStylesH)
{
	OSErr err = WEUseStyleScrap(inStylesH, mWASTEEditH);
	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
	return err;
}


// ---------------------------------------------------------------------------
//		¥ UseText
// ---------------------------------------------------------------------------

OSErr
CWASTEEdit::UseText(
	Handle	inTextH)
{
	OSErr err = WEUseText(inTextH, mWASTEEditH);
	// bug fix added as suggested by Marty Kent12/17/95
	CalText();
	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
	return err;
}


// ---------------------------------------------------------------------------
//		¥ SetAlignment
// ---------------------------------------------------------------------------

void
CWASTEEdit::SetAlignment(
	char	inAlignment)
{
	WESetAlignment(inAlignment, mWASTEEditH);
}


// ---------------------------------------------------------------------------
//		¥ SetSelection
// ---------------------------------------------------------------------------

void
CWASTEEdit::SetSelection(
 	SInt32 inSelStart,
 	SInt32 inSelEnd)
{ 
	FocusDraw();
	WESetSelection(inSelStart, inSelEnd, mWASTEEditH);
	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
}


// ---------------------------------------------------------------------------
//		¥ GetHiliteRgn
// ---------------------------------------------------------------------------

RgnHandle
CWASTEEdit::GetHiliteRgn(
	SInt32	inRangeStart,
	SInt32	inRangeEnd)
{
	RgnHandle theRegion = nil;
	theRegion = WEGetHiliteRgn(inRangeStart, inRangeEnd, mWASTEEditH);
	return theRegion;
}


// ---------------------------------------------------------------------------
//		¥ Delete
// ---------------------------------------------------------------------------

void
CWASTEEdit::Delete()
{
	if (!(mTextAttributes & wasteAttr_Editable))
		return;
	ThrowIfOSErr_(WEDelete(mWASTEEditH));
	AdjustImageToText();
	// added timp 9/28/96
	CheckScroll();
}


// ---------------------------------------------------------------------------
//		* SavePlace								[public, virtual]
// ---------------------------------------------------------------------------
// JRB
void
CWASTEEdit::SavePlace(
	LStream		*outPlace)
{
	LView::SavePlace(outPlace);

	LongRect theLongViewRect, theLongDestRect;
	WEGetViewRect(&theLongViewRect, mWASTEEditH);
	outPlace->WriteData(&theLongViewRect, sizeof(LongRect));
	WEGetDestRect(&theLongDestRect, mWASTEEditH);
	outPlace->WriteData(&theLongDestRect, sizeof(LongRect));

	GrafPtr savePort;
	ThrowIfOSErr_(WEGetInfo(wePort, &savePort, mWASTEEditH));
	outPlace->WriteData(&savePort, sizeof(savePort));
}

// ---------------------------------------------------------------------------
//		* RestorePlace							[public, virtual]
// ---------------------------------------------------------------------------
// JRB
void
CWASTEEdit::RestorePlace(
	LStream		*inPlace)
{
	LView::RestorePlace(inPlace);

	LongRect theLongViewRect, theLongDestRect;
	inPlace->ReadData(&theLongViewRect, sizeof(LongRect));
	WESetViewRect(&theLongViewRect, mWASTEEditH);
	inPlace->ReadData(&theLongDestRect, sizeof(LongRect));
	WESetDestRect(&theLongDestRect, mWASTEEditH);

	GrafPtr oldPort;
	inPlace->ReadData(&oldPort, sizeof(oldPort));
	ThrowIfOSErr_(WESetInfo(wePort, &oldPort, mWASTEEditH));
}
