// =================================================================================
//	CTextDocument.h					©1996-2000 Metrowerks Inc. All rights reserved.
// =================================================================================

#ifndef _H_CTextDocument
#define _H_CTextDocument
#pragma once
#include <UStandardDialogs.h>
#include <LSingleDoc.h>
#include <LListener.h>

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


#ifndef	__WASTE__
#include "waste.h"
#endif

const CommandT	cmd_HierFontMenu = 9000;
const CommandT	cmd_HierSizeMenu = 9001;
const CommandT	cmd_HierStyleMenu = 9002;
const CommandT	cmd_HierAlignMenu = 9003;
const CommandT	cmd_HierColorMenu = 9004;
const MessageT menu_NoItem				= -1; //no item is selected in size and text menus

const ResIDT	main_MenuFont = 132;
const ResIDT	main_MenuSize = 133;
const ResIDT	main_MenuStyle = 134;
const ResIDT	main_MenuColor = 136;


class WTextView;

class CTextDocument : public LSingleDoc,
							 public LListener {
public:
							CTextDocument(
								LCommander*			inSuper,
								FSSpec*				inFileSpec);

	virtual					~CTextDocument();

	virtual Boolean			IsModified();
	
	virtual void			DoAESave(
								FSSpec&				inFileSpec,
								OSType				inFileType);
	virtual void			DoSave();
	virtual void			DoRevert();
	virtual void			DoPrint();

	virtual Boolean			AskSaveAs(
								FSSpec&				outFSSpec,
								Boolean				inRecordIt);

	virtual OSType			GetFileType() const;

	virtual Boolean			ObeyCommand(
								CommandT			inCommand,
								void*				ioParam = nil);
								
	virtual void			FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);					

	static	bool			HasResourceFork(
								const FSSpec&		inFile);
	static	bool			HasResourceFork(
								const LFile&		inFile);
	
	virtual WTextView*	GetTextView() {return mTextView;}
	
	virtual void		ListenToMessage(
								MessageT	inMessage,
								void*		ioParam);
protected:

	virtual void			NameNewDoc();
	virtual void			OpenFile(
								FSSpec&				inFileSpec);
	virtual void			SetPrintFrameSize();

			WTextView*		mTextView;
	bool				mIsNative;
	
	PP_StandardDialogs::LFileDesignator*	mFileDesignator;	

private:
	SInt16			mFontItemChecked;
	SInt16			mSizeItemChecked;
	SInt16			mButtonFontChecked;
	SInt16			mButtonSizeChecked;
	
	static	PicHandle		sFontPopupIcon;
	static	PicHandle		sSizePopupIcon;
	static	PicHandle		sStylePopupIcon;
	static	PicHandle		sColorPopupIcon;
	static	PicHandle		sLeftButtonIcon;
	static	PicHandle		sCenterButtonIcon;
	static	PicHandle		sRightButtonIcon;
	static	PicHandle		sFullButtonIcon;
	
	CTextDocument();
							CTextDocument(const CTextDocument& inOriginal);
	CTextDocument&	operator=(const CTextDocument& inRhs);
	
	void				InitializePopupIcons();
	
	void				LinkButtonToPicture(
							PaneIDT		inPaneID,
							PicHandle	inPicture);
	
	void				SendDataToEngine();
	
	void				FindTheFontForMenu(
									ResIDT		inMenuRes,
									SInt16	&	ioLastChecked);
	void				FindTheSizeForMenu(
									ResIDT		inMenuRes,
									SInt16	&	ioLastChecked);
	void				DoTableDialog();
	void				DoMenuCheckMarks(
							ResIDT		inMenuRes,
							LStr255 &	inCheckText,
							SInt16	&	ioLastMark,
							bool		inIsTextStyle);

	bool				CheckAlignment(
							WEAlignment	inAlignment);
	void				DoColorMenu();
	void				DoStyleMenu();
};

#endif // _H_CTextDocument