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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * CBrowserTable.h --- main browser class
 */

#pragma once

#include <LOutlineTable.h>
#include <LOutlineItem.h>
#include <LPeriodical.h>
#include <LDragAndDrop.h>
#include <LCommander.h>
#include "CvsEntries.h"
#include "CPStr.h"
#include <LCMAttachment.h>
#include "FSRefGetFileIcon.h"


enum
{
	kFileIconText = 134,
	kFileIconBinary = 135,
	kFileIconUnknown = 136,
	kFileIconTextConflict = 147,
	kFileIconBinConflict = 147,
	kFileIconMiss = 137,
	kFolderIcon = 131,
	kFolderIconUnknown = 132,
	kFolderIconMiss = 133,
	kFileIconTextMod = 140,
	kFileIconBinaryMod = 141,
	kFileIconNoState = 142,
	kFolderIconIgnore = 143,
	kFileIconIgnore = 144,
	kFileLocked = 145,
	kFileUnlocked = 146,
	kFileIconAdded = 148,
	kFileIconRemoved = 149,
	kFileIconAddedBin = 150,
	kFileIconRemovedBin = 151
};

class CBrowserItem;
class LStaticText;
class EntnodeData;
class KiColumnModel;
struct CSortParam;
class CBrowserPopup;
class KoFilterModel;
class KoRecursionModel;
class KoIgnoreModel;


class CBrowserTable : public LOutlineTable, public LCommander,
	public LPeriodical, public LListener, public LDragAndDrop
{
public:
	enum { class_ID = 'CBRT' };

	CBrowserTable( LStream *inStream );
	virtual ~CBrowserTable();
	
	bool ResetBrowser(const char *path);
	void ResetBrowser(bool forceReload = true);
		// set the root of the browser

	void RemoveAllItems(void);
		// remove all the items

	void ChangeModel(bool isRecursive);

	static Handle GetIconHdl(int id);
	static Handle GetImageForEntry(EntnodeData *data);

	virtual Boolean HandleKeyPress(const EventRecord & inKeyEvent);
	virtual void BeTarget();
	virtual void DontBeTarget();
		// from LCommander
	
	virtual void SpendTime(const EventRecord & inMacEvent);
		// from LPeriodical

	virtual void SelectionChanged();
		// from LTableView
	
	virtual void		ScrollImageBy(
								SInt32				inLeftDelta,
								SInt32				inTopDelta,
								Boolean				inRefresh);
#if __PowerPlant__	< 0x02118000
	virtual void		AdjustCursorSelf(
								Point				inPortPt,
								const EventRecord	&inMacEvent);
#else
	virtual void		AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn);
#endif
		// From LView
	
	inline const char *GetPath(void) const { return fPath; }
	
	TableIndexT IsMouseInColumn(Point inPortPt) const;
		// says if the mouse is in a column, 0 if not
	
	virtual void	ListenToMessage(
							MessageT		inMessage,
							void*			ioParam);
		// From LListener
	
	LPane * GetTopWindow(void);
	
	inline bool IsAscendant(void) const { return m_ascendant; }
	int GetSortColumnIndex(void) const;

	// retrieves columnar model of the data
	inline KiColumnModel* GetColumnModel() const { return m_modelColumns; }

	virtual void			InsertSortedItem(
									CSortParam*			inSorter,
									LOutlineItem*		inOutlineItem,
									LOutlineItem*		inSuperItem,
									LOutlineItem*		inAfterItem,
									Boolean				inRefresh = true,
									Boolean				inAdjustImageSize = true);

	inline bool IsRemovingAll() const { return m_removingAll; }
	
	void BumpNumOfFiles(void);
	
	void AdjustSelectionForContextualMenuClick(const EventRecord& inEvent);
	
  SDimension16 CalcStandardSize() const;

	// KiFilterModel implementation
	inline KoFilterModel* GetFilterModel()
	{
		return m_filter;
	}

	// KiRecursionModel implementation
	inline KoRecursionModel* GetRecursionModel()
	{
		return m_recursion;
	}

	// KiRecursionModel implementation
	inline KoIgnoreModel* GetIgnoreModel()
	{
		return m_ignore;
	}

	bool FreshenFileMask(void);
	void UpdateSortSettings();
  void RememberGeometry(void);

protected:
    void OnFilteringChanged();
    void OnRecursionChanged();
    	// notifiers
    
	static void filteringChanged(LPane *pane);
	static void recursionChanged(LPane *pane);
    	// notifiers
    
	CStr fPath;
	
	UInt32 fCount;
	LStaticText *fTarget;
	CStr fTyped;

	int m_numoffiles;
	
	int m_sort;
		// sort key
	bool m_ascendant;
		// sort flag
	
	KiColumnModel* m_modelColumns;
	// columnar model in use

	KoFilterModel *m_filter;
	KoRecursionModel *m_recursion;
	KoIgnoreModel *m_ignore;

	bool m_isRecursive;
		// true if displaying recursively

	void SetRecursive(bool isRecursive);

	bool m_removingAll;
	bool m_InRefresh;
		// speed flags
	
	void HighlightSortButton(void);
	void ResetGeometry(void);
  
	int Location(CSortParam *inSorter, LOutlineItem *inSuperItem, EntnodeData *datasrc);

#	define mMaxIcons 20
	static SInt16 GetIconId(int index);
	static Handle mIconH[mMaxIcons];
	static bool mIconInit;
	static void InitializeIcons(void);
		// icons used by the browser
	
	// all the tricks of this file are here : how to restore efficiently
	// the expanded folders, selected items after the view is reloaded
	// and has possibly slightly changed.
	void FetchExpandedItems(char ** & relPaths, int & numPaths,
		CStr *curPath = 0L, CBrowserItem *superItem = 0L);
	void FetchSelectedItems(char ** & relPaths, int & numPaths,
		CStr *curPath = 0L, CBrowserItem *superItem = 0L);
	void RestoreExpandedItems(char * const *relExpandedPaths, int numExpandedPaths,
		const char *curPath = 0L, CBrowserItem *superItem = 0L);
	void RestoreSelectedItems(char * const *relExpandedPaths, int numExpandedPaths,
		const char *curPath = 0L, CBrowserItem *superItem = 0L);

	bool HasToReload(void);
	
	LStaticText *GetCaptionTarget(void);
		// this is the static text where we put the string
		// typed by the user

	virtual void ReceiveDragItem(DragReference inDragRef, DragAttributes inDragAttrs,
		ItemReference inItemRef, Rect &inItemBounds);
	virtual Boolean ItemIsAcceptable(DragReference inDragRef, ItemReference inItemRef);
		// from LDragAndDrop

	virtual void		ClickSelf(
								const SMouseDownEvent &inMouseDown);
		// from LOutlineTable

	virtual void	FinishCreateSelf();
		// from LPane
private:
	int mStartColumnH;
	
	// defensive programming
	CBrowserTable();
	CBrowserTable( const CBrowserTable &inOriginal );
	CBrowserTable& operator=( const CBrowserTable &inOriginal );
};

class CBrowserItem : public LOutlineItem
{
public:
	CBrowserItem();
	virtual ~CBrowserItem();

	inline EntnodeData *GetData(void) {return mData;}
	OSErr GetFullPath(CStr & fpath);

	virtual void			Collapse();
		// from LOutlineItem
	
	const UFSSpec & GetFileSpec(void) const { return mFileSpec; }
	
protected:
	UFSSpec mFileSpec;
	EntnodeData *mData;
	Handle mIconH;
	CIconRef mIconRef;

	virtual void TrackDrag(
		const STableCell & inCell,
		const SMouseDownEvent & inMouseDown,
		const SOutlineDrawContents & inDrawContents);

	virtual void DoubleClick(
		const STableCell & inCell,
		const SMouseDownEvent & inMouseDown,
		const SOutlineDrawContents & inDrawContents,
		Boolean inHitText);
		// capture the Double-Click	
};

class CBrowserCMAttachment : public LCMAttachment
{
public:
	enum { class_ID = FOUR_CHAR_CODE('CBcm') };

	CBrowserCMAttachment( LStream* inStream ) : LCMAttachment(inStream), mPathPopup(NULL), mOneSelectedIsFile(false) {}
	~CBrowserCMAttachment();
	
protected:
  CBrowserPopup   *mPathPopup;
  bool            mOneSelectedIsFile;

	virtual	void		PreCMSelect( const EventRecord& inMacEvent ); //Override
  virtual	bool		AddCommand(
  								MenuHandle		inMenuH,
  								CommandT		inCommand,
  								ConstStringPtr	inItemText = Str_Empty,
  								SInt16			inAfterItem = 32000,	// if you have this many items in a menu...
  								bool			inOverrideDisable = false );
  virtual	void		DispatchCommand(
  								UInt32			inSelectionType,
  								CommandT		inCommand); //Override
};

