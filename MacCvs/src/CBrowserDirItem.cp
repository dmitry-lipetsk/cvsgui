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
 * CBrowserDirItem.cpp --- directory item for the browser
 */

#include "GUSIInternal.h"
#include "GUSIFileSpec.h"
#include <unistd.h>

#include <UGAColorRamp.h>

#include "CBrowserDirItem.h"
#include "CBrowserFileItem.h"
#include "FileTraversal.h"
#include "AppConsole.h"
#include "MacCvsApp.h"
#include "MacMisc.h"
#include "stdfolder.h"
#include "CvsIgnore.h"
#include "BrowseViewColumn.h"
#include "BrowseViewModel.h"
#include "FSRefGetFileIcon.h"


CBrowserDirItem::CBrowserDirItem( EntnodeData *data )
{
	Assert_(data != 0L);
	mData = data->Ref();
	fEntriesMod = 0;
	fEntriesLogMod = 0;
	mFileSpec = data->GetMacSpec();
	
	GetEntriesModTime(fEntriesMod, fEntriesLogMod);
	
	// store the FSSpec for the file associated with this item.
	
	// Note that even tho we are storing data for an item within
	// the outline item, this doesn't provide any sort of persistant
	// storage of data within the table (see comments in COutlineTable's
	// ctor about mTableStorage).
	
	// The reason for this is how the outline table class deals with
	// expansion and collapsing of hierarchical nodes... when you expand
	// the item's ExpandSelf method is called. And as things are expanded
	// and such, items are created.. as collapsed, they are deleted. So
	// there is no persistance of the outline items and their stored data.
	
	// This is also why you might have a deep expansion of nested hierarchies
	// in the table, but you collapse a top-level parent, reexpand that and
	// the expand/nesting hierarchy is not maintained... everything was
	// deleted and has to be recreated.

	// try to get the folder's real icon. If it doesn't exist,
	// we'll use our own
	
	mIconH = CBrowserTable::GetImageForEntry(mData);
	if(!mData->IsIgnored() && !mData->IsUnknown() && !mData->IsMissing())
	{
		GetFileIcon(&mFileSpec, kSelectorAllAvailableData, mIconRef);
		if (mIconRef.GetIconSuite() != 0L)
			mIconH = 0L;
	}
}

CBrowserDirItem::~CBrowserDirItem()
{
}

void CBrowserDirItem::GetEntriesModTime(time_t & newEntriesMod,
	time_t & newEntriesLogMod)
{
	newEntriesMod = 0;
	newEntriesLogMod = 0;

	GUSIFileSpec dirSpec(mFileSpec);

	dirSpec += "CVS";
	if(!dirSpec.Exists())
		return;

	GUSIFileSpec tagSpec(dirSpec);
	tagSpec += "Tag";
	if(tagSpec.Exists())
		Tag_Open(fTag, dirSpec.FullPath());
		
	GUSIFileSpec entrySpec(dirSpec);
	entrySpec += "Entries";
	if(entrySpec.Exists())
	{
		struct stat sb;
		if (stat(entrySpec.FullPath(), &sb) != -1)
			newEntriesMod = sb.st_mtime;
	}

	entrySpec = dirSpec;
	entrySpec += "Entries.log";
	if(entrySpec.Exists())
	{
		struct stat sb;
		if (stat(entrySpec.FullPath(), &sb) != -1)
			newEntriesMod = sb.st_mtime;
	}
}

// determine what it is that we're to draw in a given column
void CBrowserDirItem::GetDrawContentsSelf(const STableCell & inCell,
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
  		if ( str ) UTF8_to_PString_Copy(ioDrawContents.outTextString, str);

			Assert_(str != 0L);
			break;
		}
		case EntnodeData::kPath:
		case EntnodeFile::kExt:
  		if ( str ) UTF8_to_PString_Copy(ioDrawContents.outTextString, str);
		  break;
		case EntnodeFile::kTag:
		{
			if(!fTag.empty())
			{
				ioDrawContents.outTextString[0] = '\0';
				c2pstrcpy(ioDrawContents.outTextString, fTag);
			}
			break;
		}
		default:
		  if ( str ) c2pstrcpy(ioDrawContents.outTextString, str);
	}
}

// by default, rows are not expanders.. but since this is a directory
// and we're showing hierarchy, of course we need to be able to expand.
// So we must make sure the table is aware of that fact.
Boolean
CBrowserDirItem::CanExpand() const
{	
	return !mData->IsUnknown() && !mData->IsMissing();
}

// just to be cute, draw an adornment of a gray background
void CBrowserDirItem::DrawRowAdornments(const Rect & inLocalRowRect)
{
	ShadeRow(UGAColorRamp::GetColor(colorRamp_Gray2), inLocalRowRect);
}

void CBrowserDirItem::DrawCell(const STableCell & inCell, const Rect & inLocalCellRect)
{
	CBrowserItem::DrawCell(inCell, inLocalCellRect);
	
	RGBColor c = UGAColorRamp::GetColor(colorRamp_White);
	RGBForeColor(&c);
	MoveTo(inLocalCellRect.right - 1, inLocalCellRect.top);
	LineTo(inLocalCellRect.right - 1, inLocalCellRect.bottom);
}

class CTraversalExpand : public TraversalReport
{
public :
	CBrowserDirItem *fDirItem;
	CBrowserTable *fTable;
	LOutlineItem *fLastItem;
	CSortList<ENTNODE> fEntries;
	std::vector<CStr> & fIgnList;
	CSortParam & fSort;
	bool m_isRecursive;		
	bool m_isIgnore;
	KoFilterModel *m_filter;
	UFSSpec m_FileSpec;
	
	CTraversalExpand(CBrowserDirItem *diritem, CBrowserTable *table,
		std::vector<CStr> & ignlist,
		CSortParam & sort, const UFSSpec & spec) :
		fDirItem(diritem), fTable(table),
		fLastItem(0L), fIgnList(ignlist), fSort(sort),
		m_FileSpec(spec), fEntries(200, ENTNODE::Compare)
	{
		Assert_(diritem != 0L);
		Assert_(table != 0L);
		
		m_isRecursive = table->GetRecursionModel()->IsShowRecursive();
		m_isIgnore = table->GetIgnoreModel()->IsShowIgnored();
		m_filter = table->GetFilterModel();
	}
	virtual ~CTraversalExpand()
	{
	}
	
	virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname, const UFSSpec * macspec)
	{
		Entries_Open (fEntries, fullpath);

		fIgnList.clear();
		
		GUSIFileSpec ignFile(*macspec);
		ignFile += ".cvsignore";
		if(ignFile.Exists())
			BuildIgnoredList(fIgnList, fullpath);
		
		return kContinueTraversal;
	}
	
	virtual kTraversal ExitDirectory(const char *fullpath)
	{
		// add the missing files
		Entries_SetMissing(fEntries);
		int numEntries = fEntries.NumOfElements();
		for(int i = 0; i < numEntries; i++)
		{
			const ENTNODE & theNode = fEntries.Get(i);
			EntnodeData *data = ((ENTNODE *)&theNode)->Data();
			if(!data->IsMissing() || !m_filter->IsMatch(data))
				continue;
			
			GUSIFileSpec spec = m_FileSpec;
			spec += (*data)[EntnodeData::kName];
			data->MacSpec() = spec;
			
			if(m_isRecursive && data->GetType() != ENT_FILE)
				continue;
			
			LOutlineItem *theItem;
			if(data->GetType() == ENT_FILE)
				theItem = NEW CBrowserFileItem(data);
			else
				theItem = NEW CBrowserDirItem(data);
			ThrowIfNil_(theItem);
			fTable->InsertSortedItem( &fSort, theItem, fDirItem, fLastItem );
			fLastItem = theItem;
		}

		fIgnList.clear();

		return kContinueTraversal;
	}

	virtual kTraversal OnError(const char *err, int errcode)
	{
		cvs_err(err);
		cvs_err("\n");
		return kTraversalError;
	}

	virtual kTraversal OnIdle(const char *fullpath)
	{
		CMacCvsApp::gApp->DoSpinCursor();
		if ( CMacCvsApp::gApp->CheckForUserCancel() ) return kStopTraversal;
		
		return kContinueTraversal;
	}

	virtual kTraversal OnDirectory(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const UFSSpec * macspec)
	{
		if(stricmp("CVS", name) == 0)
			return kSkipFile;
			
		EntnodeData *data = Entries_SetVisited(fullpath, fEntries, name,
			dir, true, &fIgnList);
		data->MacSpec() = *macspec;
		
		if (!m_isIgnore && data->IsIgnored())
		{
			return kSkipFile;
		}
		
    if (m_isRecursive)
    {
      if (!data->IsIgnored() && !data->IsUnknown() && !data->IsMissing()) 
			{
				CTraversalExpand traverse(fDirItem, fTable, fIgnList, fSort, *macspec);
				kTraversal res = FileTraverse(fullname, traverse);
				if ( res == kStopTraversal  ||  res == kTraversalError ) return res;
			}
			return kSkipFile;			
    }

		CBrowserDirItem *theItem = NEW CBrowserDirItem(data);
		ThrowIfNil_(theItem);
		
		fTable->InsertSortedItem( &fSort, theItem, fDirItem, fLastItem );
		fLastItem = theItem;

		return kSkipFile;
	}

	virtual kTraversal OnFile(const char *fullpath,
		const char *fullname,
		const char *name,
		const struct stat & dir, const UFSSpec * macspec)
	{
		EntnodeData *data = Entries_SetVisited(fullpath, fEntries, name,
			dir, false, &fIgnList);
		data->MacSpec() = *macspec;
		
		if (!m_isIgnore && data->IsIgnored())
			return kSkipFile;
		
		if(!m_filter->IsMatch(data))
			return kSkipFile;
			
		LOutlineItem *theItem = NEW CBrowserFileItem(data);
		ThrowIfNil_(theItem);
		
		fTable->InsertSortedItem( &fSort, theItem, fDirItem, fLastItem );
		fLastItem = theItem;

		return kSkipFile;
	}
};

// this is the magic of what we do when someone tries to
// expand this item. We create outline items for all possible subitems.
// We do not need to destruct these (in CollapseSelf) as that is
// handled for us automatically.. see comments in LOutlineItem.cp for
// more information
void CBrowserDirItem::ExpandSelf()
{
	CStr aPath;
	OSErr err = GetFullPath(aPath);
	if(err != noErr)
	{
		cvs_err("MacCvs: Unable to convert path name (error %d)\n", err);
		return;
	}

	CBrowserTable *table = dynamic_cast<CBrowserTable *>(mOutlineTable);
	Assert_(table != 0L);
	
	KoColumnContext context(aPath);
	KiColumnModel* model = table->GetColumnModel();
	CSortParam sortParam(table->IsAscendant(), true, model->GetAt(table->GetSortColumnIndex()), model->GetAt(0), &context);
	
	CTraversalExpand traverse(this, table, fIgnList, sortParam, mFileSpec);
	kTraversal res = FileTraverse(aPath, traverse);
}
