// ===========================================================================
//	CWASTEEdit.h				©1995-97 Timothy Paustian All rights reserved.
// ===========================================================================

#ifndef _H_CWASTEEdit
#define _H_CWASTEEdit
#pragma once

// Define USE_HAND_CURSOR to use the grab hand cursor instead
// of the standard WASTE behavior.
// See AdjustCursorSelf in CWASTEEdit.cp

#define WE_USE_TABS
//#define USE_HAND_CURSOR 
#define WE_DEFAULT_IMAGE_WIDTH		540
#define WE_DEFAULT_NO_WRAP_WIDTH	4000
#define WE_DEFAULT_FONT				applFont
#define WE_DEFAULT_SIZE				12
#define WE_DEFAULT_FACE				normal
#define WE_DEFAULT_COLOR			{0,0,0}

#define WASTE_REDRAW_FLAG			1

#define WE_JUSTIFY_BY_DEFAULT		1

#include <LView.h>
#include <LCommander.h>
#include <LPeriodical.h>
#include <LDragAndDrop.h>

class LScroller;

#include "WASTE.h"

#ifdef	WE_USE_TABS
	#include "WETabs.h"
#endif

#if WASTE_OBJECTS
	#include "WASTE_Objects.h"
#endif

enum {
	wasteAttr_MultiStyle	=	0x8000,
	wasteAttr_Editable		=	0x4000,
	wasteAttr_Selectable	=	0x2000,
	wasteAttr_WordWrap		=	0x1000,
	wasteAttr_TabSupport	=	0x0800,
	wasteAttr_DoMargins		= 	0x0400,
	wasteAttr_ActDeactScroll = 	0x0200
};

const UInt16 WEDefaultAttrs =	wasteAttr_MultiStyle	+
								wasteAttr_Editable		+
								wasteAttr_Selectable	+
								wasteAttr_WordWrap;

const UInt32 WEDefaultFlags =	weDoAutoScroll		+
								weDoOutlineHilite	+
								weDoUndo			+
								weDoIntCutAndPaste	+
								weDoDragAndDrop		+
								weDoUseTempMem		+
								weDoDrawOffscreen;

#define grabHandCursor	1104
#define openHandCursor	1108
#define rUndoStringsID	400

class CWASTEEdit :	public LView,
					public LCommander,
					public LPeriodical,
					public LDragAndDrop
{

public:

	enum { class_ID = FOUR_CHAR_CODE('nWST')};
	
						CWASTEEdit();

						CWASTEEdit(
							const SPaneInfo &	inPaneInfo,
							const SViewInfo &	inViewInfo,
							UInt16				inTextAttributes = WEDefaultAttrs,
							ResIDT				inTextTraitsID = 0,
							ResIDT				inInitialTextID = 0,
							SInt32				inWasteFlags = 0);

						CWASTEEdit(
							LStream *			inStream);
						
						CWASTEEdit(
							const CWASTEEdit& inOriginal);
	

	virtual				~CWASTEEdit();
	
	virtual void		AdaptToNewSurroundings();

	virtual void		AdaptToEnclosingScroller();

	virtual Handle		GetTextHandle() const;

	WEHandle			GetWEHandle();
	
	virtual Boolean		ObeyCommand(
							CommandT			inCommand,
							void *				ioParam);

	virtual void		FindCommandStatus(
							CommandT			inCommand,
							Boolean &			outEnabled,
							Boolean &			outUsesMark,
							UInt16 &			outMark,
							Str255				outName);

	virtual void		SpendTime(
							const EventRecord & inMacEvent);

	virtual Boolean		HandleKeyPress(
							const EventRecord & inKeyEvent);

	virtual void		ResizeFrameBy(
							SInt16				inWidthDelta,
							SInt16				inHeightDelta,
							Boolean				inRefresh);

	virtual void		MoveBy(
							SInt32				inHorizDelta,
							SInt32				inVertDelta,
							Boolean				inRefresh);

	virtual void		ScrollImageBy(
							SInt32				inLeftDelta,
							SInt32				inTopDelta,
							Boolean				inRefresh);

	virtual Boolean		FocusDraw( 
						LPane * inSubPane = nil);

	virtual void		Draw(
							RgnHandle			inSuperDrawRgnH);

	virtual void		SetForeAndBackColors(
							const RGBColor *	inForeColor,
							const RGBColor *	inBackColor);

	virtual void		EraseTextArea();

	virtual void		EraseUpdateArea(
							RgnHandle			inEraseRgn);

	virtual void		SelectAll();

	virtual void		UserChangedText();
	
	virtual void		GetTextStyleSOUPData(
							Handle				outTextH,
							Handle				outStylesH,
							Handle				outSOUPH) const;

	virtual void		InsertPtr(
							void *				inTextPtr,
							SInt32				inTextLength,
							StScrpHandle		inStylesH,
							Handle				inSoupH,
							Boolean				inDeleteAllBeforeInsertion = false,
							Boolean				inRefresh = true);

	virtual void		InsertHandle(
							Handle				inTextH,
							StScrpHandle		inStylesH,
							Handle				inSoupH,
							Boolean				inDeleteAllBeforeInsertion = false,
							Boolean				inRefresh = true);

	virtual void		Insert(
							Handle				inTextH, 
							SInt32				inTextLength, 
							StScrpHandle		inStylesH, 
							Handle				inSoupH, 
							Boolean				inDeleteAllBeforeInsertion = false, 
							Boolean				inRefresh = true);
										
	virtual void		SetStyle(
							short				inMode,
							const TextStyle *	inTextStyle);

	virtual OSErr		UseStyleScrap(
							StScrpHandle		inStylesH);

	virtual OSErr		UseText(
							Handle				inTextH);

	virtual void		SetAlignment(
							char				inAlignment);

	virtual void		SetSelection(
							SInt32				inSelStart,
							SInt32				inSelEnd);

	virtual RgnHandle	GetHiliteRgn(
							SInt32				inRangeStart,
							SInt32				inRangeEnd);

	virtual void		Delete();
	
	virtual void		AdjustImageToText();

	virtual void		AlignWASTEEditRects();
	
	// printing code for this view
	
	virtual Boolean		ScrollToPanel(
							const PanelSpec &	inPanel);

	virtual void		CountPanels(
							UInt32	&			outHorizPanels,
							UInt32	&			outVertPanels);
	
	virtual void		SuperPrintPanel(
							const PanelSpec &inSuperPanel, 
							RgnHandle 		inSuperPrintRgnH);
							
	virtual void		SavePlace(
							LStream		*outPlace);

	virtual void		RestorePlace(
							LStream		*inPlace);
													
	// inlined routines. These are bascially for utility so a user doesn't
	// have to keep getting a handle to mWASTEEditH before calling. 
	
	virtual short	FeatureFlag(short feature, short action) const
	{return WEFeatureFlag(feature, action, mWASTEEditH);}
	
	virtual OSErr	GetInfo(OSType selector, void *info) const
	{return WEGetInfo(selector, info, mWASTEEditH);}
	
	virtual OSErr	SetInfo(OSType selector, const void *info) const
	{return WESetInfo(selector, info, mWASTEEditH);}
	
	virtual Boolean	ContinuousStyle(unsigned short *mode,
						TextStyle *ts) const
	{return WEContinuousStyle(mode, ts, mWASTEEditH);}
	
	virtual void	GetRunInfo(SInt32 offset, WERunInfo *runInfo) const
	{WEGetRunInfo(offset, runInfo, mWASTEEditH);}

	virtual SInt32	GetOffset(const LongPt *thePoint, signed char *edge)
	{return	 WEGetOffset(thePoint, edge, mWASTEEditH);}
	
	virtual void	GetPoint(SInt32 offset, SInt16 direction, LongPt *thePoint,
						short *lineHeight) const
	{WEGetPoint(offset, direction, thePoint, lineHeight, mWASTEEditH);}
	
	virtual void	FindWord(SInt32 offset, char edge, SInt32 *wordStart,
						SInt32 *wordEnd) const
	{WEFindWord(offset, edge, wordStart, wordEnd, mWASTEEditH);}
	
	virtual void	FindLine(SInt32 offset, char edge, SInt32 *lineStart,
						SInt32 *lineEnd) const
	{WEFindLine(offset, edge, lineStart, lineEnd, mWASTEEditH);}
	
	virtual void	FindParagraph(SInt32 offset, char edge, SInt32 *paraStart, SInt32 *paraEnd) const
	{WEFindParagraph(offset, edge, paraStart, paraEnd, mWASTEEditH);}
	
	virtual OSErr	CopyRange(SInt32 rangeStart, SInt32 rangeEnd, Handle hText, 
						StScrpHandle hStyles, Handle hSOUP) const
	{return WECopyRange(rangeStart, rangeEnd, hText, hStyles, hSOUP,
						mWASTEEditH);}
	
	virtual char	GetAlignment() const
	{return WEGetAlignment(mWASTEEditH);}
	
	virtual short	GetChar(SInt32 offset) const
	{return WEGetChar(offset, mWASTEEditH);}
	
	virtual SInt32	GetTextLength() const
	{return WEGetTextLength(mWASTEEditH);}
	
	virtual SInt32	CountLines() const
	{return WECountLines(mWASTEEditH);}
	
	virtual SInt32	GetHeight(SInt32 startLine, SInt32 endLine) const
	{return WEGetHeight(startLine, endLine, mWASTEEditH);}
	
	virtual void	GetTextSelection(SInt32 *selStart, SInt32 *selEnd)const
	{ WEGetSelection(selStart, selEnd, mWASTEEditH );}
	
	virtual void	SelView() const
	{WESelView(mWASTEEditH);}
	
	virtual void	CalText() const
	{WECalText(mWASTEEditH);}
	
#if WASTE_OBJECTS

	virtual SInt32	FindNextObject(SInt32 offset,
						WEObjectReference *objectRef) const
	{return WEFindNextObject(offset, objectRef, mWASTEEditH);}
	 
	virtual OSType	GetObjectType(WEObjectReference objectRef) const
	{return WEGetObjectType(objectRef);}

	virtual Handle	GetObjectDataHandle(WEObjectReference objectRef) const
	{return WEGetObjectDataHandle(objectRef);}

	virtual Point	GetObjectSize(WEObjectReference objectRef) const
	{return WEGetObjectSize(objectRef);}

	virtual WEHandle	GetObjectOwner(WEObjectReference objectRef) const
	{return WEGetObjectOwner(objectRef);}

	virtual SInt32	GetObjectRefCon(WEObjectReference objectRef) const
	{return WEGetObjectRefCon(objectRef);}

	virtual void	SetObjectRefCon(WEObjectReference objectRef,
						SInt32 refCon) const
	{WESetObjectRefCon(objectRef, refCon);}
	
	void	GetSelectedObject(WEObjectReference *outObject) const
	{WEGetSelectedObject(outObject, mWASTEEditH);}
	
	void	UseSoup(WESoupHandle hSoup) const
	{WEUseSoup(hSoup, mWASTEEditH);}
#endif
	
	virtual SInt32	OffsetToLine(SInt32 offset) const
	{return WEOffsetToLine(offset, mWASTEEditH);}
	
	Boolean	IsActive() const
	{return WEIsActive(mWASTEEditH);}

	void	GetLineRange(SInt32 lineNo, SInt32 *lineStart, SInt32 *lineEnd) const
	{WEGetLineRange(lineNo, lineStart, lineEnd, mWASTEEditH);}
		
	void	SetAlignment(WEAlignment alignment) const
	{WESetAlignment(alignment, mWASTEEditH);}
	
	unsigned long	GetModCount() const
	{return WEGetModCount(mWASTEEditH);}

	void	ResetModCount() const
	{WEResetModCount(mWASTEEditH);}

	Point				mMargins; //intialize to zero others can change if they like
	Boolean				mInScroller;

protected:

	WEHandle			mWASTEEditH;
	Point				mSavePos;
	LArray *			mPrintPanels;
	static SInt32		sMaxScrollDelta;
	UInt16				mTextAttributes;
	RGBColor			mForeColor;
	RGBColor			mBackColor;
	ResIDT				mTextTraitsID;
	LView *				mScroller;
	ResIDT				mInitialTextID;
	SInt16				mInitialResFile;
	
	virtual void		DrawSelf();

	virtual void		ReconcileFrameAndImage(
							Boolean				inRefresh);

	virtual void		ApplyForeAndBackColors() const; 
	
	virtual void		ClickSelf(
							const SMouseDownEvent &	inMouseDown);

	virtual void		AdjustCursorSelf(
							Point				inPortPt,
							const EventRecord &	inMacEvent);
	
	virtual void		BeTarget();

	virtual void		DontBeTarget();
	
	// Drag and Drop overrides
	virtual void		BeginDrag(
							const Point inMouseLoc,
							const EventRecord & inEventRecord);
							
	virtual Boolean		DragIsAcceptable(
							DragReference		inDragRef);

	virtual void		DoDragReceive(
							DragReference		inDragRef);
							
	virtual void		HiliteDropArea(
							DragReference	inDragRef);

	virtual void		EnterDropArea(
							DragReference		inDragRef,
							Boolean				inDragHasLeftSender);

	virtual void		LeaveDropArea(
							DragReference		inDragRef);

	virtual void		InsideDropArea(
							DragReference		inDragRef);

	virtual Boolean		PointInDropArea(
							Point				inPoint);

	virtual void		ScrolledImageReport(
							SInt32				inLeftDelta,
							SInt32				inTopDelta);

	virtual void		CheckScroll();

	virtual void		NormalizePortForWE();

	virtual void		ProtectSelection();

	virtual void		GetUpdateRgn(
							WindowPtr			inWindow,
							RgnHandle			outRgnLocal);
	
	virtual Boolean		IsInSelection(
							Point				inMouseLoc);
	
	static pascal Boolean	MyClickLoop(
								WEHandle			hWE);

	static pascal OSErr		MyDragHiliteProc(
								DragReference		drag,
								Boolean				hiliteFlag,
								WEHandle			hWE);
private:

	void				InitWASTEEdit(
							UInt32				inWasteFlags);

	Boolean				CursorInView(
							SInt32				inPosition);


};

#endif
