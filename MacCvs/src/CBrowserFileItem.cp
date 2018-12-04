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
 * CBrowserFileItem.cpp --- file item for the browser
 */

#include "CBrowserFileItem.h"

#include <UGAColorRamp.h>

#include "MacCvsApp.h"
#include "BrowseViewColumn.h"
#include "CvsCommands.h"
#include "FSRefGetFileIcon.h"
#include "GUSIFileSpec.h"
#include "MacMisc.h"


CBrowserFileItem::CBrowserFileItem( EntnodeData *data )
{
	Assert_(data != 0L);
	mData = data->Ref();
	mStateH = 0L;
	mLockH = 0L;
	mFileSpec = data->GetMacSpec();

	// store the FSSpec for the file associated with this item.
	
	// Note that even tho we are storing data for an item within
	// the outline item, this doesn't provide any sort of persistant
	// storage of data within the table (see comments in COutlineTable's
	// ctor about mTableStorage).
	
	// The reason for this is how the outline table class deals with
	// expansion and collapsing of hierarchical nodes... when you expand
	// the item's ExpandSelf method is called (see CDirectoryItem for this
	// as file items of course can't be expanded). And as things are expanded
	// and such, items are created.. as collapsed, they are deleted. So
	// there is no persistance of the outline items and their stored data.
	
	// This is also why you might have a deep expansion of nested hierarchies
	// in the table, but you collapse a top-level parent, reexpand that and
	// the expand/nesting hierarchy is not maintained... everything was
	// deleted and has to be recreated.
	
	// get the icon for this file, use our defaults if we can't get one.

	if(!mData->IsMissing())
		GetFileIcon(&mFileSpec, kSelectorAllAvailableData, mIconRef);
	if (mIconRef.GetIconSuite() == 0L)
		mIconH = CBrowserTable::GetIconHdl(kFileIconNoState);
	
	const char *info = (*data)[EntnodeFile::kOption];
	bool isbin = info != 0L && strcmp(info, "-kb") == 0;
	
	mStateH = CBrowserTable::GetImageForEntry(data);
	
	if(!data->IsIgnored() && !data->IsMissing() && !data->IsUnknown())
		mLockH = data->IsLocked() ? CBrowserTable::GetIconHdl(kFileLocked) :
			CBrowserTable::GetIconHdl(kFileUnlocked);
	
	// and roughly calculate the size of the file, for the second column of
	// the table.
	DetermineFileSize();
}

CBrowserFileItem::~CBrowserFileItem()
{
}

// this is the routine called to know what to draw within the
// table cell. See the comments in LOutlineItem.cp for more info.
void CBrowserFileItem::GetDrawContentsSelf(const STableCell & inCell,
	SOutlineDrawContents & ioDrawContents)
{
	CBrowserTable *table = dynamic_cast<CBrowserTable *>(mOutlineTable);
	Assert_(table != 0L);

	int atype = table->GetColumnModel()->GetType(inCell.col - 1);
		
	ioDrawContents.outTextString[0] = '\0';
	ioDrawContents.outShowSelection = true;
	ioDrawContents.outHasIcon = false;
	ioDrawContents.outTextTraits.style = 0;
	const char *str = (*mData)[atype];

	switch (atype)
	{
		case EntnodeData::kName:
		{
			ioDrawContents.outIconSuite = mIconH != 0L ? mIconH : mIconRef.GetIconSuite();
			ioDrawContents.outHasIcon = ioDrawContents.outIconSuite != 0L;
			if(!mData->IsUnmodified() || (*mData)[EntnodeFile::kConflict] != 0L)
				ioDrawContents.outTextTraits.style |= bold;
  		if ( str ) UTF8_to_PString_Copy(ioDrawContents.outTextString, str);

			Assert_(str != 0L);
			break;
		}
		case EntnodeData::kPath:
		case EntnodeFile::kExt:
  		if ( str ) UTF8_to_PString_Copy(ioDrawContents.outTextString, str);
		  break;
		case EntnodeFile::kVN:
		{
			ioDrawContents.outHasIcon = true;
			ioDrawContents.outIconSuite = mStateH;
		  if ( str ) c2pstrcpy(ioDrawContents.outTextString, str);
			break;
		}
		case EntnodeFile::kOption:
		{
			ioDrawContents.outHasIcon = mLockH != 0L;
			ioDrawContents.outIconSuite = mLockH;
		  if ( str ) c2pstrcpy(ioDrawContents.outTextString, str);
			break;
		}
		default:
		  if ( str ) c2pstrcpy(ioDrawContents.outTextString, str);
	}
}

// just to be cute, we'll draw an adornment (again, see the LOutlineItem.cp
// comments for more information). We'll draw a groovy gray background
void CBrowserFileItem::DrawRowAdornments(const Rect & inLocalRowRect)
{
	ShadeRow(UGAColorRamp::GetColor(colorRamp_White), inLocalRowRect);
	RGBColor c = UGAColorRamp::GetColor(colorRamp_Gray2);
	RGBForeColor(&c);
	MoveTo(inLocalRowRect.right, inLocalRowRect.top);
	LineTo(inLocalRowRect.right, inLocalRowRect.bottom);
}

void CBrowserFileItem::DrawCell(const STableCell & inCell, const Rect & inLocalCellRect)
{
	CBrowserItem::DrawCell(inCell, inLocalCellRect);
	
	RGBColor c = UGAColorRamp::GetColor(colorRamp_Gray2);
	RGBForeColor(&c);
	MoveTo(inLocalCellRect.right - 1, inLocalCellRect.top);
	LineTo(inLocalCellRect.right - 1, inLocalCellRect.bottom);
}

// see how big the file actually is, and make a size string for printing
// in the second column
void CBrowserFileItem::DetermineFileSize()
{
	LStr255 fileName;
	UInt8 utf8path[PATH_MAX];
	if(FSRefMakePath(&mFileSpec, utf8path, sizeof(utf8path)) == noErr)
	{
		UStr uppath, filename;
		SplitPath(utf8_to_posix(utf8path), uppath, filename);
		fileName = filename;
	}
	
	FSCatalogInfo info;
	if(FSGetCatalogInfo(&mFileSpec, kFSCatInfoDataSizes | kFSCatInfoRsrcSizes, &info, NULL, NULL, NULL) != noErr)
	{
		mSizeStr = (SInt32)0;
		mSizeStr += "\p K";
		return;
	}

	SInt32	fileSize = info.dataPhysicalSize + info.rsrcPhysicalSize;

	if ( fileSize >= 1024 ) {
		fileSize /= 1024;
		
		mSizeStr = fileSize;
		mSizeStr += "\p K";
	} else {
		
		mSizeStr = fileSize;
		mSizeStr += "\p bytes";
	}
}
