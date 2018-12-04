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
 * CBrowserTable.cpp --- main browser class
 */

#include "GUSIInternal.h"
#include "GUSIFileSpec.h"
#include <unistd.h>
#include <sys/stat.h>

#include "CBrowserTable.h"

#include <LTableMultiGeometry.h>
#include <LOutlineKeySelector.h>
#include <LOutlineRowSelector.h>
#include <LOutlineItem.h>
#include <UAttachments.h>
#include <LStaticText.h>

#include "CBrowserDirItem.h"
#include "CBrowserFileItem.h"
#include "MacCvsApp.h"
#include "MacCvsConstant.h"
#include "TextBinary.h"
#include "stdfolder.h"
#include "VolsPaths.h"
#include "AppConsole.h"
#include "CBrowserPopup.h"
#include "MacMisc.h"
#include "BrowseViewColumn.h"
#include "BrowseViewModel.h"
#include "CvsCommands.h"
#include "umain.h"
#include "PythonGlue.h"
#include "FileTraversal.h"
#include "CvsPrefs.h"
#include "MultiString.h"

#include <LBevelButton.h>
#include <LCFString.h>
#include <stack>
#include "CvsAlert.h"

#if __profile__
#include <profiler.h>
#endif

namespace {

  bool CheckForCFMStyleSandbox(const char* dir);
  
  class PersistentColumnWidths : public TMPersistent< std::pair<int,int> >
  {
  public:
	  PersistentColumnWidths(const char* uniqueName, kClassPersistent pclass = kNoClass) : 
	      TMPersistent(uniqueName, pclass) {}
	  virtual unsigned int SizeOf(void) const
	  {
	    return sizeof(int) + GetList().size() * 2 * sizeof(int);
	  }
	  virtual const void* GetData(void) const
	  {
    	m_buffer.AdjustSize(SizeOf());
    	char* tmp = m_buffer;
    	*(int*) tmp = GetList().size();
    	tmp += sizeof(int) / sizeof(char);
    	
    	for(typename list_t::const_iterator i = GetList().begin(); i != GetList().end(); ++i)
    	{
	      *(int*)tmp = i->first;
    	  tmp += sizeof(int) / sizeof(char);
	      *(int*)tmp = i->second;
    	  tmp += sizeof(int) / sizeof(char);
    	}
    	
    	return (char*)m_buffer;
	  }
	  virtual void SetData(const void* ptr, unsigned int size)
	  {
	    if ( size >= sizeof(int) ) {
    	  const char* tmp = (const char*)ptr;
	      GetList().resize(*(int*)tmp);
    	  tmp += sizeof(int) / sizeof(char);
      	for(typename list_t::iterator i = GetList().begin(); i != GetList().end(); ++i)
      	{
      	  i->first = *(int*)tmp;
      	  tmp += sizeof(int) / sizeof(char);
      	  i->second = *(int*)tmp;
      	  tmp += sizeof(int) / sizeof(char);
      	}
	    }
	    else GetList().clear();
	  }
	  
	  bool HasWidth(int columnType) const
	  {
    	for(typename list_t::const_iterator i = GetList().begin(); i != GetList().end(); ++i)
    	  if ( i->first == columnType ) return true;
    	return false;
	  }
	  
	  int GetWidth(int columnType) const
	  {
    	for(typename list_t::const_iterator i = GetList().begin(); i != GetList().end(); ++i)
    	  if ( i->first == columnType ) return i->second;
    	return 0;
    }

    void SetWidth(int columnType, int width)
    {
    	TouchTimeStamp();
    	for(typename list_t::iterator i = GetList().begin(); i != GetList().end(); ++i)
    	  if ( i->first == columnType ) {
    	    i->second = width;
    	    return;
    	  }
    	
    	GetList().push_back( list_t::value_type(columnType, width) );
    }
    
    void Clear()
    {
      GetList().clear();
    }
    
	protected:
	  mutable	CStaticAllocT<char> m_buffer;
  };
  
} // anonymous namespace

Handle CBrowserTable::mIconH[mMaxIcons];
bool CBrowserTable::mIconInit = false;
CPersistentInt gFileViewSortRegular("P_FileViewSort", EntnodeData::kName, kAddSettings);
CPersistentBool gFileViewSortAscRegular("P_FileViewSortAsc", false, kAddSettings);
PersistentColumnWidths  gFileViewColumnWidthsRegular("P_FileViewColumnWidths", kAddSettings);
CPersistentInt gFileViewSortRecursive("P_FileViewSortFlat", EntnodeData::kName, kAddSettings);
CPersistentBool gFileViewSortAscRecursive("P_FileViewSortAscFlat", false, kAddSettings);
PersistentColumnWidths  gFileViewColumnWidthsRecursive("P_FileViewColumnWidthsFlat", kAddSettings);
CPersistentInt  gBrowserDoubleClickWarnThreshold("P_WarnThresholdForDoubleClickingBrowserItems", 10);

static bool gTurnOffBroadcast = false;

#define kDragFlavorTypeColumn 'COLU'
#define kColumnsBtnID 2000

static int sIndIcons[mMaxIcons] =
{
	kFileIconText,
	kFileIconBinary,
	kFileIconUnknown,
	kFileIconTextConflict,
	kFileIconBinConflict,
	kFileIconMiss,
	kFolderIcon,
	kFolderIconUnknown,
	kFolderIconMiss,
	kFileIconTextMod,
	kFileIconBinaryMod,
	kFileIconNoState,
	kFolderIconIgnore,
	kFileIconIgnore,
	kFileLocked,
	kFileUnlocked,
	kFileIconAdded,
	kFileIconRemoved,
	kFileIconAddedBin,
	kFileIconRemovedBin
};

class LColumnAttachment : public LAttachment
{
public:
	LColumnAttachment(int type);
	virtual ~LColumnAttachment();
	
	int m_typeColumn;
};

LColumnAttachment::LColumnAttachment(int type) : LAttachment((MessageT)-1), m_typeColumn(type)
{
}

LColumnAttachment::~LColumnAttachment()
{
}

class LMyTableMultiGeometry : public LTableMultiGeometry
{
public:
						LMyTableMultiGeometry(
								LTableView*			inTableView,
								UInt16				inColWidth,
								UInt16				inRowHeight);

	virtual				~LMyTableMultiGeometry();

	virtual void		SetColWidth(
								UInt16		inWidth,
								TableIndexT inFromCol,
								TableIndexT	inToCol);
};

LMyTableMultiGeometry::LMyTableMultiGeometry(
								LTableView*			inTableView,
								UInt16				inColWidth,
								UInt16				inRowHeight) : LTableMultiGeometry(inTableView, inColWidth, inRowHeight)
{
}

LMyTableMultiGeometry::~LMyTableMultiGeometry()
{
}

void		LMyTableMultiGeometry::SetColWidth(
							UInt16		inWidth,
							TableIndexT inFromCol,
							TableIndexT	inToCol)
{
	LTableMultiGeometry::SetColWidth(inWidth, inFromCol, inToCol);
	
	CBrowserTable *table = dynamic_cast<CBrowserTable *>(mTableView);
	ThrowIfNil_(table);
	
	LPane *topWind = table->GetTopWindow();
	LView *columns = dynamic_cast<LView*>
		(topWind->FindPaneByID(item_BrowserColumns));
	Assert_(columns != 0L);
	if(columns == 0L)
		return;
	
	TArrayIterator<LPane*>	iterator(columns->GetSubPanes());
	LPane	*subPane;
	int cnt = 1;
	SPoint32 loc;
	SDimension16 dim;
	int rightH;
	while (iterator.Next(subPane))
	{
		LBevelButton *btn = dynamic_cast<LBevelButton *>(subPane);
		Assert_(btn != 0L);
		if(btn != 0L)
		{
			ControlButtonContentInfo inInfo;
			btn->GetContentInfo(inInfo);
			if(inInfo.contentType != kControlContentTextOnly)
			{
				inInfo.contentType = kControlContentTextOnly;
				btn->SetContentInfo(inInfo);
			}
		}

		if(cnt < inFromCol)
		{
		}
		else if(cnt == inFromCol)
		{
			subPane->GetFrameSize(dim);
			subPane->ResizeFrameTo(inWidth, dim.height, true);
			subPane->GetFrameLocation(loc);
			rightH = loc.h + inWidth;
		}
		else
		{
			subPane->GetFrameLocation(loc);
			subPane->MoveBy(rightH - loc.h, 0, true);
			subPane->GetFrameSize(dim);
			subPane->GetFrameLocation(loc);
			rightH = loc.h + dim.width;
		}
		
		cnt++;
	}
}

SInt16 CBrowserTable::GetIconId(int index)
{
	Assert_(index < mMaxIcons);
	return sIndIcons[index];
}

Handle CBrowserTable::GetIconHdl(int id)
{
	if(!mIconInit)
	{
		InitializeIcons();
		mIconInit = true;
	}

	int cnt;
	for(cnt = 0; cnt < mMaxIcons; cnt++)
	{
		if(sIndIcons[cnt] == id)
			break;
	}
	Assert_(cnt != mMaxIcons);

	return mIconH[cnt];
}

void CBrowserTable::InitializeIcons(void)
{
	for(int i = 0; i < mMaxIcons; i++)
	{
		OSErr err = ::GetIconSuite( &mIconH[i], GetIconId(i), kSelectorAllAvailableData );
		ThrowIfOSErr_(err);	
		ThrowIfResFail_(mIconH[i]);
		
		::MoveHHi(mIconH[i]);
		::HLock(mIconH[i]);
	}
}

void CBrowserTable::recursionChanged(LPane *pane)
{
	CBrowserTable *table = dynamic_cast<CBrowserTable *>(pane);
	Assert_(table != 0L);
	if(table != 0L)
		table->OnRecursionChanged();
}

void CBrowserTable::filteringChanged(LPane *pane)
{
	CBrowserTable *table = dynamic_cast<CBrowserTable *>(pane);
	Assert_(table != 0L);
	if(table != 0L)
		table->OnFilteringChanged();
}

CBrowserTable::CBrowserTable(LStream *inStream) : LOutlineTable( inStream ),
	 LDragAndDrop(GetMacWindow(), this),
	 fCount(0), fTarget((LStaticText *)-1), m_removingAll(false), m_InRefresh(false)
{
	m_sort = (int)gFileViewSortRegular;
	m_ascendant = (bool)gFileViewSortAscRegular;

	m_filter = new KoFilterModel();
	m_recursion = new KoRecursionModel();
	m_ignore = new KoIgnoreModel();

	// make sure browser is not flat by default
	if ( m_recursion->IsShowRecursive() )
	  m_recursion->ToggleRecursion();
	// since with multiple browsers, filter settings should neither be global, nor stick with a browse location,
	// we'll reset the filters back to a default set whenever we create a new browser
  if ( m_ignore->IsShowIgnored() )
    m_ignore->ToggleIgnore();
  if ( m_filter->HasFilters() )
    m_filter->ClearAll();
    
	m_isRecursive = m_recursion->IsShowRecursive();
	SetRecursive(m_isRecursive);
	
	m_filter->GetNotificationManager()->CheckIn(this, CBrowserTable::filteringChanged);
	m_recursion->GetNotificationManager()->CheckIn(this, CBrowserTable::recursionChanged);
	m_ignore->GetNotificationManager()->CheckIn(this, CBrowserTable::filteringChanged);
}

CBrowserTable::~CBrowserTable()
{
	if(m_filter) {
		m_filter->GetNotificationManager()->CheckOut(this);
		delete m_filter;
  }
	if(m_recursion) {
		m_recursion->GetNotificationManager()->CheckOut(this);
		delete m_recursion;
  }
	if(m_ignore) {
		m_ignore->GetNotificationManager()->CheckOut(this);
	  delete m_ignore;
	}
	CMacCvsApp::gApp->RemoveWindowFromListByView(this);
}

void CBrowserTable::UpdateSortSettings()
{
	if ( m_isRecursive )
	{
	  gFileViewSortRecursive = m_sort;
  	gFileViewSortAscRecursive = m_ascendant;
  }
  else
  {
	  gFileViewSortRegular = m_sort;
  	gFileViewSortAscRegular = m_ascendant;
  }
}

bool CBrowserTable::FreshenFileMask(void) 
{
#if 0
	CString strFileMask;
	m_comboFilterMask.GetWindowText(strFileMask);
	return m_filter.SetMask(strFileMask);
#else
	return 0;
#endif
}

void CBrowserTable::OnFilteringChanged()
{
	ResetBrowser(true);
}

void CBrowserTable::OnRecursionChanged()
{
	ChangeModel(this->GetRecursionModel()->IsShowRecursive());
}

static LColumnAttachment *GetColumnAttachment(LBevelButton *btn)
{
	const TArray<LAttachment*>* theAttachments = btn->GetAttachmentsList();
	if(theAttachments == 0L)
		return 0L;
	
	TArrayIterator<LAttachment*> iterate(*theAttachments);

	LAttachment*	theAttach;
	LColumnAttachment *attach = 0L;
	while (iterate.Next(theAttach)) {
		attach = dynamic_cast<LColumnAttachment*>(theAttach);
		if(attach != 0L)
			break;
	}
	
	return attach;
}

void CBrowserTable::HighlightSortButton(void)
{
	USemaphore policeman(gTurnOffBroadcast);

	LPane *topWind = GetTopWindow();
	LView *columns = dynamic_cast<LView*>
		(topWind->FindPaneByID(item_BrowserColumns));
	Assert_(columns != 0L);
	if(columns == 0L)
		return;
	
	TArrayIterator<LPane*>	iterator(columns->GetSubPanes());
	LPane	*subPane;
	while (iterator.Next(subPane))
	{
		LBevelButton *btn = dynamic_cast<LBevelButton *>(subPane);
		Assert_(btn != 0L);
		if(btn == 0L || !btn->IsVisible())
			continue;

		LColumnAttachment *attach = GetColumnAttachment(btn);
		Assert_(attach != 0L);
		if(attach == 0L)
			continue;
		
		ControlButtonContentInfo inInfo;
		if(attach->m_typeColumn == m_sort)
		{
			btn->SetValue(Button_On);
			inInfo.contentType = kControlContentCIconRes;
			inInfo.u.resID = m_ascendant ? 164 : 165;
			btn->SetContentInfo(inInfo);
		}
		else
		{
			btn->SetValue(Button_Off);
			btn->GetContentInfo(inInfo);
			if(inInfo.contentType != kControlContentTextOnly)
			{
				inInfo.contentType = kControlContentTextOnly;
				btn->SetContentInfo(inInfo);
			}
		}
	}
}

void	CBrowserTable::ListenToMessage(
						MessageT		inMessage,
						void*			ioParam)
{
	if(gTurnOffBroadcast)
		return;

	USemaphore policeman(gTurnOffBroadcast);

	if(inMessage >= kColumnsBtnID && inMessage <= (kColumnsBtnID + 10))
	{
		LPane *topWind = GetTopWindow();
		LBevelButton *btn = dynamic_cast<LBevelButton*>(topWind->FindPaneByID(inMessage));
		Assert_(btn != 0L);
		if(btn == 0L)
			return;
		
		btn->SetValue(Button_On);

		LColumnAttachment *attach = GetColumnAttachment(btn);
		Assert_(attach != 0L);
		if(attach == 0L)
			return;

		int newSort = attach->m_typeColumn;
		if(newSort == m_sort)
			m_ascendant = !m_ascendant;
		else
		{
			m_sort = newSort;
			KiColumnModel* model = GetColumnModel();
			m_ascendant = model->GetAt(model->GetColumn(m_sort))->IsDefaultAscending();
		}

		ResetBrowser(true);
	}
}

void CBrowserTable::RememberGeometry(void)
{
cvs_out("CBrowserTable::RememberGeometry: recursive=%s, table geometry=%p\n", m_isRecursive?"true":"false", GetTableGeometry());
  PersistentColumnWidths&   columnWidths = m_isRecursive ? gFileViewColumnWidthsRecursive : gFileViewColumnWidthsRegular;
	columnWidths.Clear();
	LTableGeometry	*tableGeometry = GetTableGeometry();
	if(tableGeometry != 0L) {
	  int numColumns = m_modelColumns->GetCount();
	  for(int i = 1; i <= numColumns; i++)
		  columnWidths.SetWidth(m_modelColumns->GetType(i - 1), tableGeometry->GetColWidth(i));
	}
}

void CBrowserTable::ResetGeometry(void)
{
  PersistentColumnWidths&   columnWidths = m_isRecursive ? gFileViewColumnWidthsRecursive : gFileViewColumnWidthsRegular;
  
	// set the table geometry
	LTableMultiGeometry *geom = NEW LMyTableMultiGeometry(this, 20, 20);
	ThrowIfNil_(geom);
		
	LTableGeometry	*inTableGeometry = GetTableGeometry();
	if(inTableGeometry != 0L) {
		delete inTableGeometry;
	}
	
	SetTableGeometry(geom);

	TableIndexT outRows, outCols;
	GetTableSize(outRows, outCols);
	if(outCols != 0)
		RemoveCols(outCols, 1, false);

	int numColumns = m_modelColumns->GetCount();
	InsertCols( numColumns, 0, nil, nil, false );
	for(int i = 1; i <= numColumns; i++)
	{
		LV_COLUMN column;
		m_modelColumns->GetAt(i - 1)->GetSetupData(&column);
		geom->SetColWidth(columnWidths.HasWidth(m_modelColumns->GetType(i - 1)) ? columnWidths.GetWidth(m_modelColumns->GetType(i - 1)) : column.width, i, i);
	}

	LPane *topWind = GetTopWindow();
	LView *columns = dynamic_cast<LView*>
		(topWind->FindPaneByID(item_BrowserColumns));
	Assert_(columns != 0L);
	int pos = 0;
	if(columns != 0L)
	{
		TArrayIterator<LPane*>	iterator(columns->GetSubPanes());
		LPane	*subPane = 0L;
		MessageT msgv = kColumnsBtnID;
		while (iterator.Next(subPane))
		{
			LBevelButton *btn = dynamic_cast<LBevelButton *>(subPane);
			Assert_(btn != 0L);
			if(btn == 0L)
				continue;

			if(pos >= numColumns)
			{
				btn->Hide();
				continue;
			}
			
			btn->Show();

			ControlButtonContentInfo inInfo;
			btn->GetContentInfo(inInfo);
			if(inInfo.contentType != kControlContentTextOnly)
			{
				inInfo.contentType = kControlContentTextOnly;
				btn->SetContentInfo(inInfo);
			}
			
			LV_COLUMN column;
			m_modelColumns->GetAt(pos)->GetSetupData(&column);

			LStr255 pname(column.name);
			btn->SetDescriptor(pname);
			btn->AddListener(this);
			btn->SetValueMessage(msgv);
			btn->SetPaneID(msgv++);
			LColumnAttachment *attach = GetColumnAttachment(btn);
			if(attach == 0L)
			{
				attach = new LColumnAttachment(m_modelColumns->GetType(pos));
                                btn->AddAttachment((LAttachment *)attach);
			}
			else
				attach->m_typeColumn = m_modelColumns->GetType(pos);
			
			pos++;
		}
	}
}

void CBrowserTable::ChangeModel(bool isRecursive)
{
  RememberGeometry();
	SetRecursive(isRecursive);
	ResetGeometry();
	ResetBrowser(true);
}

void CBrowserTable::FinishCreateSelf()
{
	LOutlineTable::FinishCreateSelf();

	// set the table selector
	
	SetTableSelector(NEW LOutlineRowSelector( this ) );
	
	ResetGeometry();
	
	// and note that we don't set the table storage....
	
	// most of the table classes not only maintain the graphical
	// representation of your data but also the data itself. But
	// LOutlineTable doesn't really do this...it mostly handles
	// graphical representation... you need to handle your data
	// maintenance elsewhere by yourself.
	
	// insert the columns (Name, Rev., Option, Tag,
	// Status, Date, Conflict)
	
	// Set up keyboard selection and scrolling.

	AddAttachment(NEW LOutlineKeySelector(this, msg_AnyMessage));
	AddAttachment(NEW LKeyScrollAttachment(this));

	// Try to become default commander in the window.

	if (mSuperCommander != nil) {
		mSuperCommander->SetLatentSub(this);
	}
}

bool CBrowserTable::ResetBrowser(const char *path)
{
	bool isReload = path != 0L && !fPath.empty() && stricmp(fPath, path) == 0;

	fPath = path;
	
	if(!isReload)
	{
  	if ( m_recursion->IsShowRecursive() )
  	  m_recursion->ToggleRecursion();
		
		if(HasPersistentSettings(fPath))
		{
		  LoadPersistentSettings(fPath);
		  //if ( m_filter ) m_filter->LoadSettings();
		  //if ( m_recursion ) m_recursion->LoadSettings();
		  //if ( m_ignore ) m_ignore->LoadSettings();
		}
		else
		{
			// check if the path has a CVS folder. If not,
			// we don't want the user to be prompted (like when 
			// WinCvs is starting for the first time).
			UStr cvsFolder(fPath);
			if(!cvsFolder.endsWith(kPathDelimiter))
				cvsFolder << kPathDelimiter;
			cvsFolder << "CVS";
			struct stat sb;
			if (stat(cvsFolder, &sb) != -1 && S_ISDIR(sb.st_mode))
				AskCreatePersistentSettings(fPath);
		}
	}
	
  // check whether this is an old style (i.e. CFM) sandbox and
  // if it is, ask the user whether (s)he wants to convert the files
  //
  if ( !CheckForCFMStyleSandbox(fPath) ) return false;

  // reload sort settings, since we might have a new set of settings loaded
	if ( m_isRecursive )
	{
  	m_sort = gFileViewSortRecursive;
  	m_ascendant = gFileViewSortAscRecursive;
	}
	else
	{
  	m_sort = gFileViewSortRegular;
  	m_ascendant = gFileViewSortAscRegular;
  }

	ResetBrowser(true);
	
	return true;
}

void CBrowserTable::FetchExpandedItems(char ** & relPaths, int & numPaths, CStr *curPath, CBrowserItem *superItem)
{
	CStr curPathStart;
	EntnodeData *data;
	LOutlineItem *item;
	
	if(curPath == 0L)
	{
		// first recursion
		relPaths = 0L;
		numPaths = 0;
		Boolean res = GetFirstLevelItems().FetchItemAt(1, &item);
		if(!res)
			return;
		superItem = dynamic_cast<CBrowserItem*>(item);
		Assert_(superItem != 0L);
		if(superItem == 0L)
			return;
		data = superItem->GetData();
		curPath = &curPathStart;
		curPathStart = (*data)[EntnodeData::kName];
	}
	
	CStr & dir = *curPath;
	if(!dir.endsWith(kPathDelimiter))
		dir << kPathDelimiter;

	const LArray *subItems = superItem->GetSubItems();
	if(subItems != 0L)
	{
		int i = 1;
		while(subItems->FetchItemAt(i++, &item))
		{
			CBrowserItem *bitem = dynamic_cast<CBrowserItem*>(item);
			Assert_(bitem != 0L);
			if(bitem == 0L)
				continue;
			
			data = bitem->GetData();
			if(data->GetType() == ENT_FILE)
				continue;
			
			if(bitem->IsExpanded())
			{
				CStr newCurPathStart((const char *)dir);
				newCurPathStart << (*data)[EntnodeData::kName];
				FetchExpandedItems(relPaths, numPaths, &newCurPathStart, bitem);
			}
		}
	}
	
	if(relPaths == 0L)
		relPaths = (char **)malloc(sizeof(char *));
	else
		relPaths = (char **)realloc(relPaths, (numPaths + 1) * sizeof(char *));
	ThrowIfNil_(relPaths);
	char *str = (char *)malloc((dir.length() + 1) * sizeof(char));
	ThrowIfNil_(str);
	strcpy(str, dir);
	relPaths[numPaths++] = str;
}

void CBrowserTable::RestoreExpandedItems(char * const *relPaths, int numPaths, const char *curPath, CBrowserItem *superItem)
{
	EntnodeData *data;
	LOutlineItem *item;
	const char *curName = 0L;
	size_t curNameLen;
	int i;
	
	if(curPath == 0L)
	{
		// first recursion
		Boolean res = GetFirstLevelItems().FetchItemAt(1, &item);
		if(!res)
			return;
		superItem = dynamic_cast<CBrowserItem*>(item);
		Assert_(superItem != 0L);
		if(superItem == 0L)
			return;
		data = superItem->GetData();
		curName = (*data)[EntnodeData::kName];
		curNameLen = strlen(curName);
		for(i = 0; i < numPaths; i++)
		{
			const char *apath = relPaths[i];
			if(strncmp(apath, curName, curNameLen) == 0 && apath[curNameLen] == kPathDelimiter)
			{
				if(!superItem->IsExpanded())
					superItem->Expand();
				if(apath[curNameLen + 1] != '\0')
					RestoreExpandedItems(0L, 0, apath + curNameLen + 1, superItem);
			}
		}
	}
	else
	{
		const LArray *subItems = superItem->GetSubItems();
		if(subItems != 0L)
		{
			i = 1;
			while(subItems->FetchItemAt(i++, &item))
			{
				CBrowserItem *bitem = dynamic_cast<CBrowserItem*>(item);
				Assert_(bitem != 0L);
				if(bitem == 0L)
					continue;
				
				data = bitem->GetData();
				if(data->GetType() == ENT_FILE)
					continue;
				
				curName = (*data)[EntnodeData::kName];
				curNameLen = strlen(curName);
				
				if(strncmp(curPath, curName, curNameLen) == 0 && curPath[curNameLen] == kPathDelimiter)
				{
					if(!bitem->IsExpanded())
						bitem->Expand();
					if(curPath[curNameLen + 1] != '\0')
						RestoreExpandedItems(0L, 0, curPath + curNameLen + 1, bitem);
					break;
				}
			}
		}
	}
}

void CBrowserTable::FetchSelectedItems(char ** & relPaths, int & numPaths, CStr *curPath, CBrowserItem *superItem)
{
	CStr curPathStart;
	EntnodeData *data;
	LOutlineItem *item;
	
	if(curPath == 0L)
	{
		// first recursion
		relPaths = 0L;
		numPaths = 0;
		Boolean res = GetFirstLevelItems().FetchItemAt(1, &item);
		if(!res)
			return;
		superItem = dynamic_cast<CBrowserItem*>(item);
		Assert_(superItem != 0L);
		if(superItem == 0L)
			return;
		data = superItem->GetData();
		curPath = &curPathStart;
		curPathStart = (*data)[EntnodeData::kName];
	}

	// add the root cell if selected
	CStr & dir = *curPath;
	if(!dir.endsWith(kPathDelimiter))
		dir << kPathDelimiter;

	TableIndexT rowIndex = FindRowForItem(superItem);
	Assert_(rowIndex != LArray::index_Bad);
	if(rowIndex == LArray::index_Bad)
		return;

	STableCell inSuperCell(rowIndex, 1);	
	if(CellIsSelected(inSuperCell))
	{
		if(relPaths == 0L)
			relPaths = (char **)malloc(sizeof(char *));
		else
			relPaths = (char **)realloc(relPaths, (numPaths + 1) * sizeof(char *));
		ThrowIfNil_(relPaths);
		char *str = (char *)malloc((dir.length() + 1) * sizeof(char));
		ThrowIfNil_(str);
		strcpy(str, dir);
		relPaths[numPaths++] = str;
	}
	
	if(!superItem->IsExpanded())
		return;

	// add the children cells if selected
	const LArray *subItems = superItem->GetSubItems();
	CStr newCurPathStart;
	if(subItems != 0L)
	{
		int i = 1;
		while(subItems->FetchItemAt(i++, &item))
		{
			CBrowserItem *bitem = dynamic_cast<CBrowserItem*>(item);
			Assert_(bitem != 0L);
			if(bitem == 0L)
				continue;
			
			data = bitem->GetData();
			
			if(data->GetType() == ENT_FILE)
			{
				rowIndex = FindRowForItem(bitem);
				Assert_(rowIndex != LArray::index_Bad);
				if(rowIndex == LArray::index_Bad)
					continue;
				
				STableCell inCell(rowIndex, 1);
				if(CellIsSelected(inCell))
				{
					newCurPathStart = (const char *)dir;
					newCurPathStart << (*data)[EntnodeData::kName];
					newCurPathStart << kPathDelimiter;
					if(relPaths == 0L)
						relPaths = (char **)malloc(sizeof(char *));
					else
						relPaths = (char **)realloc(relPaths, (numPaths + 1) * sizeof(char *));
					ThrowIfNil_(relPaths);
					char *str = (char *)malloc((newCurPathStart.length() + 1) * sizeof(char));
					ThrowIfNil_(str);
					strcpy(str, newCurPathStart);
					relPaths[numPaths++] = str;
				}
			}
			else
			{
				// recurse
				newCurPathStart = (const char *)dir;
				newCurPathStart << (*data)[EntnodeData::kName];
				FetchSelectedItems(relPaths, numPaths, &newCurPathStart, bitem);
			}
		}
	}	
}

void CBrowserTable::RestoreSelectedItems(char * const *relPaths, int numPaths, const char *curPath, CBrowserItem *superItem)
{
	EntnodeData *data;
	LOutlineItem *item;
	const char *curName = 0L;
	size_t curNameLen;
	int i;
	TableIndexT rowIndex;
	
	if(curPath == 0L)
	{
		// first recursion
		Boolean res = GetFirstLevelItems().FetchItemAt(1, &item);
		if(!res)
			return;
		superItem = dynamic_cast<CBrowserItem*>(item);
		Assert_(superItem != 0L);
		if(superItem == 0L)
			return;
		
		if(!superItem->IsExpanded())
			return;
		
		data = superItem->GetData();
		curName = (*data)[EntnodeData::kName];
		curNameLen = strlen(curName);
		
		for(i = 0; i < numPaths; i++)
		{
			const char *apath = relPaths[i];
			if(strncmp(apath, curName, curNameLen) == 0 && apath[curNameLen] == kPathDelimiter)
			{
				if(apath[curNameLen + 1] == '\0')
				{
					// that's it
					rowIndex = FindRowForItem(superItem);
					Assert_(rowIndex != LArray::index_Bad);
					if(rowIndex == LArray::index_Bad)
						return;
					
					STableCell inSuperCell(rowIndex, 1);
					if(!CellIsSelected(inSuperCell))
						SelectCell(inSuperCell);
					return;
				}
				else
					RestoreSelectedItems(0L, 0, apath + curNameLen + 1, superItem);
			}
		}
	}
	else
	{
		if(!superItem->IsExpanded())
			return;

		const LArray *subItems = superItem->GetSubItems();
		if(subItems != 0L)
		{
			i = 1;
			while(subItems->FetchItemAt(i++, &item))
			{
				CBrowserItem *bitem = dynamic_cast<CBrowserItem*>(item);
				Assert_(bitem != 0L);
				if(bitem == 0L)
					continue;
				
				data = bitem->GetData();
				
				curName = (*data)[EntnodeData::kName];
				curNameLen = strlen(curName);
				
				if(strncmp(curPath, curName, curNameLen) == 0 && curPath[curNameLen] == kPathDelimiter)
				{
					if(curPath[curNameLen + 1] == '\0')
					{
						// that's it
						rowIndex = FindRowForItem(bitem);
						Assert_(rowIndex != LArray::index_Bad);
						if(rowIndex == LArray::index_Bad)
							continue;
						
						STableCell inCell(rowIndex, 1);
						if(!CellIsSelected(inCell))
							SelectCell(inCell);

					}
					else if(bitem->IsExpanded())
					{
						RestoreSelectedItems(0L, 0, curPath + curNameLen + 1, bitem);
					}
					break;
				}
			}
		}
	}
}

bool CBrowserTable::HasToReload(void)
{
	// now select the matching item
	CBrowserItem *item;
	TableIndexT inRow = 1;
	while((item = dynamic_cast<CBrowserItem*>(FindItemForRow(inRow))) != 0L)
	{
		CBrowserDirItem *dirItem = dynamic_cast<CBrowserDirItem*>(item);
		if(dirItem != 0L)
		{
			time_t newEntriesMod, newEntriesLogMod;
			dirItem->GetEntriesModTime(newEntriesMod, newEntriesLogMod);
			if(dirItem->EntriesModTime() != newEntriesMod ||
				dirItem->EntriesLogModTime() != newEntriesLogMod)
					return true;
		}
		inRow++;
	}
	
	return false;
}

void CBrowserTable::BumpNumOfFiles(void)
{
	m_numoffiles++;
#if 1
	fCount = ::TickCount();

	LStaticText *txt = GetCaptionTarget();
	if(txt != 0L)
	{
		LStr255 value((SInt32)m_numoffiles);
		txt->SetText(value);
		txt->Draw(nil);
	}
#endif
}

void CBrowserTable::ResetBrowser(bool forceReload)
{
	if(!forceReload && !HasToReload())
		return;
	
	USemaphore setToOn(m_InRefresh);

#if 0 //__profile__
	ProfilerInit(collectDetailed, bestTimeBase, 200, 200);
	ProfilerSetStatus(1);
#endif

	HighlightSortButton();
	
	m_numoffiles = 0;
	
	char **relExpandedPaths = 0L;
	int numExpandedPaths = 0;
	char **relSelectedPaths = 0L;
	int numSelectedPaths = 0;
	
	// try to remember the expanded items as well as the
	// selected items so we have a chance to restore it after
	FetchExpandedItems(relExpandedPaths, numExpandedPaths);
	FetchSelectedItems(relSelectedPaths, numSelectedPaths);
	
	RemoveAllItems();

	// reload
	CStr dir((const char *)fPath);
	CStr uppath;
	CStr folder;
	if(!dir.endsWith(kPathDelimiter))
		dir << kPathDelimiter;
	bool res = SplitPath(dir, uppath, folder);
	ThrowIfNot_(res);

	EntnodePath *thePath = NEW EntnodePath(uppath);
	ThrowIfNil_(thePath);
	
	EntnodeData *data = NEW EntnodeFolder(folder, thePath);
	thePath->UnRef();
	ThrowIfNil_(data);
	
	GUSIFileSpec spec((const char *)fPath);
//	data->SetEncoding(_i18n("Folder"));
	data->MacSpec() = spec;
	
	LOutlineItem *theItem = NEW CBrowserDirItem(data);
	data->UnRef();
	ThrowIfNil_(theItem);
	InsertItem(theItem, 0L, 0L);
	theItem->Expand();
		
	RestoreExpandedItems(relExpandedPaths, numExpandedPaths);
	RestoreSelectedItems(relSelectedPaths, numSelectedPaths);
	
	int i;
	for(i = 0; i < numExpandedPaths; i++)
	{
		free(relExpandedPaths[i]);
	}
	if(relExpandedPaths != 0L)
		free(relExpandedPaths);
	for(i = 0; i < numSelectedPaths; i++)
	{
		free(relSelectedPaths[i]);
	}
	if(relSelectedPaths != 0L)
		free(relSelectedPaths);

	AdjustImageSize(true);
	Refresh();
	
#if 0 //__profile__
	ProfilerDump("\pSources:Browser.prof");
	ProfilerTerm();
#endif
}

void CBrowserTable::RemoveAllItems(void)
{
	USemaphore setToOn(m_removingAll);
	
	LArrayIterator iter(GetFirstLevelItems());
	LOutlineItem* item;
	while (iter.Next(&item))
	{
		RemoveItem(item, false, false);
	}
	AdjustImageSize(false);
	Refresh();
}

CBrowserItem::CBrowserItem()
{
	mIconH = 0L;
	mData = 0L;
}

CBrowserItem::~CBrowserItem()
{
	mData->UnRef();
}

OSErr CBrowserItem::GetFullPath(CStr & fpath)
{
	UStr aPath;
	GUSIFileSpec spec = mFileSpec;
	
	aPath = spec.FullPath();
	if(spec.Error() != noErr)
		return spec.Error();

	fpath = aPath;

	return noErr;
}

static void FetchDragableItems(CBrowserTable *table, std::vector<CBrowserItem *> & result, CBrowserItem *superItem)
{
	EntnodeData *data;
	LOutlineItem *item;
	
	if(superItem == 0L)
	{
		Boolean res = table->GetFirstLevelItems().FetchItemAt(1, &superItem);
		if(!res)
			return;
	}

	data = superItem->GetData();
	if(!data->IsMissing())
	{
		TableIndexT rowIndex = table->FindRowForItem(superItem);
		Assert_(rowIndex != LArray::index_Bad);
		if(rowIndex != LArray::index_Bad)
		{
			STableCell inCell(rowIndex, 1);
			if(table->CellIsSelected(inCell))
				result.push_back(superItem);
		}
	}

	const LArray *subItems = superItem->GetSubItems();
	if(subItems != 0L)
	{
		int i = 1;
		while(subItems->FetchItemAt(i++, &item))
		{
			CBrowserItem *bitem = dynamic_cast<CBrowserItem*>(item);
			Assert_(bitem != 0L);
			if(bitem == 0L)
				continue;
			
			FetchDragableItems(table, result, bitem);
		}
	}
}

void CBrowserItem::DoubleClick(
	const STableCell & /* inCell */,
	const SMouseDownEvent & /* inMouseDown */,
	const SOutlineDrawContents & /* inDrawContents */,
	Boolean /* inHitText */)
{
	CBrowserTable *table = dynamic_cast<CBrowserTable*>(mOutlineTable);
	if ( table == NULL )
	  return;
	  
  std::vector<CBrowserItem*>  selectedItems;
	FetchDragableItems(table, selectedItems, NULL);
	
	std::vector<UFSSpec>  selectedFiles;
	std::vector<CBrowserItem*>  selectedFolders;
	
	for ( std::vector<CBrowserItem*>::const_iterator i(selectedItems.begin()); i != selectedItems.end(); ++i )
	{
	
  	if((*i)->mData->IsMissing())
  		continue;

  	if((*i)->mData->GetType() == ENT_FILE)
  	  selectedFiles.push_back((*i)->mFileSpec);
  	else
  	  selectedFolders.push_back(*i);
  }
  
	if ( selectedItems.size() > gBrowserDoubleClickWarnThreshold  ||  selectedFolders.size() > 1  ||  (!selectedFiles.empty()  &&  !selectedFolders.empty()))
	{
	  char    primaryMsg[256];
	  const char*   secondaryMsg(NULL);
	  sprintf(primaryMsg, _i18n("Are you sure you want to open the selected %u %s?"), selectedItems.size(),
	          (selectedFiles.size() == 0 ? _i18n("folders") : (selectedFolders.size() == 0 ? _i18n("files") : _i18n("files and folders"))));
	  if ( !selectedFiles.empty()  &&  !selectedFolders.empty() )
	    secondaryMsg = _i18n("You have selected a mixture of files and folders. If you continue, the files will be opened in their default editors, whereas the folders will be opened as new browser windows in MacCvs.");
	  else if ( selectedFolders.size() > 1 )
	    secondaryMsg = _i18n("A new browser window will be opened for each of the selected folders.");
	  if ( CvsAlert(kCvsAlertQuestionIcon, primaryMsg, secondaryMsg).ShowAlert() != kCvsAlertOKButton )
	    return;
	}
	
  if ( !selectedFiles.empty() ) CBrowserPopup::SendFinderOpenAE(selectedFiles.size(), &selectedFiles[0]);
  
  for ( std::vector<CBrowserItem*>::const_iterator i(selectedFolders.begin()); i != selectedFolders.end(); ++i )
  {  	
  	CStr aPath;
  	OSErr err = (*i)->GetFullPath(aPath);
  	if(err != noErr)
  	{
  		cvs_err("MacCvs: Unable to extract path for folder '%s' (error %d)\n", (*i)->GetData()->GetName(), err);
  		continue;
  	}

  	CMacCvsApp::gApp->NewBrowser(aPath);
  }
}

class LDragSelection : public LDragTask
{
public:
					LDragSelection(
							CBrowserTable *table,
							const EventRecord&		inEventRecord,
							const Rect&				inItemRect,
							std::vector<CBrowserItem *> & inItems,
							FlavorFlags				inFlags);
	virtual			~LDragSelection();
};

LDragSelection::LDragSelection(CBrowserTable *table,
							const EventRecord&		inEventRecord,
							const Rect&				inItemRect,
							std::vector<CBrowserItem *> & inItems,
							FlavorFlags				inFlags) :
								LDragTask(inEventRecord)
{
	std::vector<CBrowserItem *>::iterator it;
	ItemReference inItemRef = 1;
	for(it = inItems.begin(); it != inItems.end(); ++it, ++inItemRef)
	{
		CBrowserItem *bitem = *it;
		HFSFlavor data = {0, 0, 0};
		Boolean trueValue = true;
		ThrowIfOSErr_(FSGetCatalogInfo(&bitem->GetFileSpec(), kFSCatInfoNone, NULL, NULL, &data.fileSpec, NULL));
		::AddDragItemFlavor(mDragRef, inItemRef, flavorTypeHFS,
			&data, sizeof(HFSFlavor), inFlags);
    ::AddDragItemFlavor(mDragRef, inItemRef, 'mCVS', &trueValue, sizeof(trueValue), flavorSenderOnly|flavorNotSaved);

		TableIndexT rowIndex = table->FindRowForItem(bitem);
		Assert_(rowIndex != LArray::index_Bad);
		if(rowIndex == LArray::index_Bad)
			continue;
		
		STableCell inCell(rowIndex, 1);
		Rect outCellFrame;
		if(table->GetLocalCellRect(inCell, outCellFrame))
		{
			::LocalToGlobal(&topLeft(outCellFrame));
			::LocalToGlobal(&botRight(outCellFrame));
			AddRectDragItem(inItemRef, outCellFrame);
		}
	}	

	::TrackDrag(mDragRef, &mEventRecord, mDragRegion);
}

LDragSelection::~LDragSelection()
{
}

void CBrowserItem::TrackDrag(
	const STableCell & inCell,
	const SMouseDownEvent & inMouseDown,
	const SOutlineDrawContents & inDrawContents)
{
	CBrowserTable *table = dynamic_cast<CBrowserTable*>(mOutlineTable);
	if(table == 0L)
		return;

	std::vector<CBrowserItem *> items;
	FetchDragableItems(table, items, 0L);
	if(items.size() == 0)
		return;

	Rect where;
	SPoint32 lt = {inDrawContents.ioCellBounds.left, inDrawContents.ioCellBounds.top};
	SPoint32 rb = {inDrawContents.ioCellBounds.right, inDrawContents.ioCellBounds.bottom};
	mOutlineTable->ImageToLocalPoint(lt, ((Point *)&where)[0]);
	mOutlineTable->ImageToLocalPoint(rb, ((Point *)&where)[1]);

	LDragSelection theDragTask(table, inMouseDown.macEvent, where, items, 0L);
}

void CBrowserItem::Collapse()
{
	CBrowserTable *table = dynamic_cast<CBrowserTable*>(mOutlineTable);
	bool isRemovingAll = table->IsRemovingAll(); // so let's make it faster
	
	// Redraw the disclosure triangle.

	mExpanded = false;
	if(!isRemovingAll)
		RefreshDisclosureTriangle();

	// Allow subclass to collapse data model.

	CollapseSelf();

	// Remove the subitems.

	if (mSubItems != nil) {
		LArrayIterator iter(*mSubItems);
		LOutlineItem* item;

		while (iter.Next(&item)) {
			mOutlineTable->RemoveItem(item, true, false);
		}
		if(!isRemovingAll)
			mOutlineTable->AdjustImageSize(true);
	}
}

void CBrowserTable::SelectionChanged(void)
{
	LCommander::SetUpdateCommandStatus(true);
}

Boolean CBrowserTable::HandleKeyPress(const EventRecord & inKeyEvent)
{
	SInt16 theKey = inKeyEvent.message & charCodeMask;
	EKeyStatus theKeyStatus = UKeyFilters::PrintingCharField(inKeyEvent);
	if((inKeyEvent.modifiers & cmdKey) == 0 && theKeyStatus == keyStatus_Input)
	{
		if(fCount == 0)
			fTyped = "";
		
		fCount = inKeyEvent.when;
		fTyped << (char)theKey;
		
		// now select the matching item
		CBrowserItem *item;
		TableIndexT inRow = 1;
		while((item = dynamic_cast<CBrowserItem*>(FindItemForRow(inRow))) != 0L)
		{
			EntnodeData *data = item->GetData();
			if(strnicmp((*data)[EntnodeData::kName], fTyped, fTyped.length()) == 0)
			{
				UnselectAllCells();
		
				STableCell cell(inRow, 1);
				SelectCell(cell);
				ScrollCellIntoFrame(cell);
				break;
			}
			inRow++;
		}
		
		LStaticText *txt = GetCaptionTarget();
		if(txt == 0L)
			return true;

		txt->SetText((Ptr)(const char *)fTyped, fTyped.length());
		return true;
	}
	
	return LCommander::HandleKeyPress(inKeyEvent);
}

void CBrowserTable::BeTarget()
{
	StartIdling(0.5 * kEventDurationSecond); // Start tracking the keyboard
}

void CBrowserTable::DontBeTarget()
{
	StopIdling();
}

LPane * CBrowserTable::GetTopWindow(void)
{
	LPane * topWind = this;
	while(topWind != 0L && topWind->GetSuperView() != 0L)
		topWind = topWind->GetSuperView();

	Assert_(topWind != 0L);
	return topWind;
}

LStaticText *CBrowserTable::GetCaptionTarget(void)
{
	// find the target the first time
	if(fTarget == (LStaticText *)-1)
	{
		LPane * topWind = GetTopWindow();

		fTarget = dynamic_cast<LStaticText*>
			(topWind->FindPaneByID(item_BrowserStaticText));
		Assert_(fTarget != 0L);
	}
	
	return fTarget;
}

void CBrowserTable::SpendTime(const EventRecord & /* inMacEvent */)
{
	if(fCount == 0)
		return;
	
	if((::TickCount() - fCount) < 30) // half a second
		return;
	
	LStaticText *txt = GetCaptionTarget();
	if(txt == 0L)
		return;
	
	char empty = '\0';
	txt->SetText((Ptr)&empty, 0);
	fCount = 0;
}

Handle CBrowserTable::GetImageForEntry(EntnodeData *data)
{
	Handle result = 0L;
	if(data->GetType() == ENT_FILE)
	{
		const char *info = (*data)[EntnodeFile::kOption];
		bool isbin = info != 0L && strcmp(info, "-kb") == 0;
		const char* vn = (*data)[EntnodeFile::kVN];
		bool isAdded = vn && (strcmp(vn, "0") == 0);
		
		if(data->IsIgnored())
			result = GetIconHdl(kFileIconIgnore);
		else if(data->IsUnknown())
			result = GetIconHdl(kFileIconUnknown);
		else if(data->GetConflict() != 0L)
			result = GetIconHdl(isbin ? kFileIconBinConflict :
				kFileIconTextConflict);
		else if(data->IsRemoved())
			result = GetIconHdl(isbin ? kFileIconRemovedBin : kFileIconRemoved);
		else if(isAdded)
			result = GetIconHdl(isbin ? kFileIconAddedBin : kFileIconAdded);
		else if(data->IsMissing())
			result = GetIconHdl(kFileIconMiss);
		else if(isbin)
			result = GetIconHdl(data->IsUnmodified() ?
				kFileIconBinary : kFileIconBinaryMod);
		else
			result = GetIconHdl(data->IsUnmodified() ?
				kFileIconText : kFileIconTextMod);
	}
	else
	{
		if(data->IsIgnored())
			result = GetIconHdl(kFolderIconIgnore);
		else if(data->IsUnknown())
			result = GetIconHdl(kFolderIconUnknown);
		else if(data->IsMissing())
			result = GetIconHdl(kFolderIconMiss);
		else
			result = GetIconHdl(kFolderIcon);
	}

	return result;
}

Boolean CBrowserTable::ItemIsAcceptable(DragReference inDragRef, ItemReference inItemRef)
//
//	ItemIsAcceptable will be called whenever the Drag Manager wants to know if
//	the item the user is currently dragging contains any information that we
//	can accept.
//
{
	FlavorFlags		theFlags;
  //
  // allow files/folders to be dropped, but only if it's not a drag initiated by us
  // (this will avoid the problem where an inexact click is interpreted as a drag and	opens a new browser)
  //
	if (GetFlavorFlags(inDragRef, inItemRef, flavorTypeHFS, &theFlags) == noErr &&
	    GetFlavorFlags(inDragRef, inItemRef, 'mCVS', &theFlags) == badDragFlavorErr )
	{
	  //
	  // only allow single file/folder to be dropped to open new browser
	  // (otherwise there might be a proliferation of browser windows opening)
	  //
		UInt16    numItems;
		ThrowIfOSErr_(CountDragItems(inDragRef, &numItems));
		return ( numItems == 1 );
	}
	
	if (GetFlavorFlags(inDragRef, inItemRef, kDragFlavorTypeColumn, &theFlags) == noErr)
		return true;
	
	return false;
}

void CBrowserTable::ReceiveDragItem(DragReference inDragRef, DragAttributes inDragAttrs,
	ItemReference inItemRef, Rect &inItemBounds)
{
	//
	// Information about the drag contents we'll be needing.
	//
	FlavorFlags theFlags;		// We actually only use the flags to see if a flavor exists
	Size theDataSize;	// How much data there is for us.
	
	//
	// Check to make sure the drag contains a cItemType item.
	//
	if (GetFlavorFlags(inDragRef, inItemRef, flavorTypeHFS, &theFlags) == noErr) {
		::GetFlavorDataSize(inDragRef, inItemRef, flavorTypeHFS, &theDataSize);
		if (theDataSize) {
			ThrowIf_(theDataSize != sizeof(HFSFlavor));	// sanity check
			
			//
			// Get the data about the file we are receiving.
			//
			HFSFlavor theFlavorData;	// Where we will put that data.
			::GetFlavorData(inDragRef, inItemRef, flavorTypeHFS, &theFlavorData, &theDataSize, 0L);

			FSRef ref;
			FSpMakeFSRef(&theFlavorData.fileSpec, &ref);
			
			CMacCvsApp::gApp->NewBrowser(ref);
			ProcessSerialNumber currentProcess = { 0, kCurrentProcess };
			SetFrontProcessWithOptions(&currentProcess, kSetFrontProcessFrontWindowOnly);
		}
	}

	if (GetFlavorFlags(inDragRef, inItemRef, kDragFlavorTypeColumn, &theFlags) == noErr) {
		::GetFlavorDataSize(inDragRef, inItemRef, kDragFlavorTypeColumn, &theDataSize);
		if (theDataSize) {
			ThrowIf_(theDataSize != sizeof(TableIndexT));	// sanity check
			//
			// Get the data about the rectangle we are receiving.
			//
			TableIndexT theFlavorData;
			::GetFlavorData(inDragRef, inItemRef, kDragFlavorTypeColumn, &theFlavorData, &theDataSize, 0L);
			
			Point localPt = topLeft(inItemBounds);
			LocalToPortPoint(localPt);
			int offset = localPt.h - mStartColumnH;
			UInt16 newWidth = mTableGeometry->GetColWidth(theFlavorData) + offset;
			mTableGeometry->SetColWidth(newWidth, theFlavorData, theFlavorData);
		}
	}
}

// when the view scrolls, so should the columns
void CBrowserTable::ScrollImageBy(
							SInt32				inLeftDelta,
							SInt32				inTopDelta,
							Boolean				inRefresh)
{
	LOutlineTable::ScrollImageBy(inLeftDelta, inTopDelta, inRefresh);
	
	if(inLeftDelta == 0)
		return;
	
	LPane * topWind = GetTopWindow();	
	LView *columns = dynamic_cast<LView*>
		(topWind->FindPaneByID(item_BrowserColumns));
	Assert_(columns != 0L);
	if(columns == 0L)
		return;
	
	columns->MoveBy(-inLeftDelta, 0, inRefresh);
}

TableIndexT CBrowserTable::IsMouseInColumn(Point inPortPt) const
{
	SPoint32 imgPt;
	PortToLocalPoint(inPortPt);
	LocalToImagePoint(inPortPt, imgPt);
	imgPt.h -= 2;
	TableIndexT ind1 = mTableGeometry->GetColHitBy(imgPt);
	imgPt.h += 4;
	TableIndexT ind2 = mTableGeometry->GetColHitBy(imgPt);

	return ind1 == (ind2 - 1) ? ind1 : 0;
}

#if __PowerPlant__	< 0x02118000
void		CBrowserTable::AdjustCursorSelf(
							Point				inPortPt,
							const EventRecord	&inMacEvent)
#else
void		CBrowserTable::AdjustMouseSelf(
								Point				inPortPt,
								const EventRecord&	inMacEvent,
								RgnHandle			outMouseRgn)
#endif
{
	if(IsMouseInColumn(inPortPt))
	{
		UCursor::SetTheCursor(curs_SizeColumns);
		return;
	}

#if __PowerPlant__	< 0x02118000
	LOutlineTable::AdjustCursorSelf(inPortPt, inMacEvent);
#else
	LOutlineTable::AdjustMouseSelf(inPortPt, inMacEvent, outMouseRgn);
#endif
}

void CBrowserTable::SetRecursive(bool isRecursive)
{
  // remember old sort settings if model changed
	if ( m_isRecursive != isRecursive ) this->UpdateSortSettings();
	
  m_isRecursive = isRecursive;
  
  // and adjust model and sort settings
	if ( isRecursive )
	{
  	m_sort = gFileViewSortRecursive;
  	m_ascendant = gFileViewSortAscRecursive;
  	m_modelColumns = KiColumnModel::GetRecursiveModel();
	}
	else
	{
  	m_sort = gFileViewSortRegular;
  	m_ascendant = gFileViewSortAscRegular;
	  m_modelColumns = KiColumnModel::GetRegularModel();
  }
}

class LDragColumnTask : public LDragTask
{
public:
					LDragColumnTask(
							const EventRecord&		inEventRecord,
							const Rect&				inItemRect,
							ItemReference			inItemRef,
							FlavorType				inFlavor,
							void*					inDataPtr,
							Size					inDataSize,
							FlavorFlags				inFlags,
							int minH, int maxH);
	virtual			~LDragColumnTask();

	OSErr OnDragInput(Point *mouse, SInt16 *modifiers, void *dragInputRefCon, DragRef theDrag);
	
protected:
	DragInputUPP m_dragInput;
	int mPosV, mMinH, mMaxH;
};

static pascal OSErr DragInputProc(Point *mouse, SInt16 *modifiers, void *dragInputRefCon, DragRef theDrag)
{
	LDragColumnTask *task = static_cast<LDragColumnTask *>(dragInputRefCon);
	return task->OnDragInput(mouse, modifiers, dragInputRefCon, theDrag);
}

LDragColumnTask::LDragColumnTask(const EventRecord&		inEventRecord,
							const Rect&				inItemRect,
							ItemReference			inItemRef,
							FlavorType				inFlavor,
							void*					inDataPtr,
							Size					inDataSize,
							FlavorFlags				inFlags,
							int minH, int maxH) :
								LDragTask(inEventRecord), m_dragInput(0L)
{
	m_dragInput = NewDragInputUPP(DragInputProc);
	ThrowIfNil_(m_dragInput);
	
	ThrowIfOSErr_(::SetDragInputProc(mDragRef, m_dragInput, this));
	Point mouse = inEventRecord.where;
	mPosV = mouse.v;

	mouse.h = minH;
	::LocalToGlobal(&mouse);
	mMinH = mouse.h;
	
	mouse.h = maxH;
	::LocalToGlobal(&mouse);
	mMaxH = mouse.h;
	
	::AddDragItemFlavor(mDragRef, inItemRef, inFlavor, inDataPtr, inDataSize,
							inFlags);

	Rect	globalRect = inItemRect;
	::LocalToGlobal(&topLeft(globalRect));
	::LocalToGlobal(&botRight(globalRect));
	AddRectDragItem(inItemRef, globalRect);

	::TrackDrag(mDragRef, &mEventRecord, mDragRegion);
}

LDragColumnTask::~LDragColumnTask()
{
	if(m_dragInput != 0L)
		DisposeDragInputUPP(m_dragInput);
}

OSErr LDragColumnTask::OnDragInput(Point *mouse, SInt16 *modifiers, void *dragInputRefCon, DragRef theDrag)
{
	mouse->v = mPosV;
	if(mouse->h < mMinH)
		mouse->h = mMinH;
	if(mouse->h > mMaxH)
		mouse->h = mMaxH;
	return noErr;
}

void		CBrowserTable::ClickSelf(
								const SMouseDownEvent &inMouseDown)
{
	TableIndexT column;
	if((column = IsMouseInColumn(inMouseDown.wherePort)) > 0)
	{
		UInt16 oldWidth = mTableGeometry->GetColWidth(column);
		
		Rect	portRect;
		CalcPortFrameRect(portRect);
		Rect itemRect = portRect;
		itemRect.left = inMouseDown.wherePort.h - 1;
		itemRect.right = inMouseDown.wherePort.h + 1;
		mStartColumnH = inMouseDown.wherePort.h;
		
		SInt16 minH = portRect.left;
		if((inMouseDown.wherePort.h - oldWidth + 10) > minH)
			minH = inMouseDown.wherePort.h - oldWidth + 10;
		SInt16 maxH = portRect.right;
		LPane * topWind = GetTopWindow();
		topWind->FocusDraw();
		LDragColumnTask task(inMouseDown.macEvent, itemRect, 1, kDragFlavorTypeColumn, &column, sizeof(column), 0,
			minH, maxH);
		return;
	}

	LOutlineTable::ClickSelf(inMouseDown);

	if (mSuperCommander != nil) {
		mSuperCommander->SetLatentSub(this);
		SwitchTarget(this);
	}
}

int CBrowserTable::Location(CSortParam *inSorter, LOutlineItem *inSuperItem, EntnodeData *datasrc)
{
	Assert_(inSuperItem != 0L);

	const LArray *subItems = inSuperItem->GetSubItems();
	int fNum = subItems == 0L ? 0 : subItems->GetCount();

	int left, right, cmp_left = 0, cmp_right = 0;
	int cmp_middle, middle;
	computeSort compute;
	bool fNewOne;
	LOutlineItem *item;
	
	if(fNum == 0)
	{
		fNewOne = true;
		return 0;
	}
	
	fNewOne = false;
	left = 0;
	right = fNum - 1;
	compute = computeLR;
#	define Entry(i) (subItems->FetchItemAt(i + 1, &item), dynamic_cast<CBrowserItem*>(item))
#	define fMethod(a, b) _Compare((a)->GetData(), datasrc, inSorter)

	while(1)
	{
		middle = (left + right) / 2;
		
		if(compute == computeLR)
		{
			// firstime only
			cmp_left = fMethod(Entry(left), entry);
			cmp_right = fMethod(Entry(right), entry);
		}
		cmp_middle = fMethod(Entry(middle), entry);
		
		// entry already exist (E == L || E == R)
		if(cmp_left == 0 || cmp_right == 0)
		{
			return cmp_left == 0 ? left : right;
		}
		
		// found an entry superior to the right (E > R && E < R + 1)
		if(cmp_right < 0)
		{
			fNewOne = true;
			return right + 1;
		}
		
		// found an entry inferior to the left (L - 1 < E < L)
		if(cmp_left > 0)
		{
			fNewOne = true;
			return left;
		}
		
		// found an entry inferior to the right (E > R - 1 && E < R)
		if(middle == left)
		{
			fNewOne = true;
			return right;
		}
		
		// entry already exist (E == M)
		if(cmp_middle == 0)
			return middle;
		// restart search (E > M && E < R)
		else if(cmp_middle < 0)
		{
			compute = computeNone;
			left = middle;
			cmp_left = cmp_middle;
			cmp_right = 1;
		}
		// restart search (E > L && E < M)
		else
		{
			compute = computeNone;
			right = middle;
			cmp_right = cmp_middle;
			cmp_left = -1;
		}
	}
	assert(0);
	return 0;
}

int CBrowserTable::GetSortColumnIndex(void) const
{
	return m_modelColumns->GetColumn(m_sort);
}

void CBrowserTable::InsertSortedItem(
								CSortParam*			inSorter,
								LOutlineItem*		inOutlineItem,
								LOutlineItem*		inSuperItem,
								LOutlineItem*		inAfterItem,
								Boolean				inRefresh,
								Boolean				inAdjustImageSize)
{
	CBrowserItem *srcItem = dynamic_cast<CBrowserItem*>(inOutlineItem); 
	Assert_(inSuperItem != 0L && srcItem != 0L);
	
	EntnodeData *datasrc = srcItem->GetData();
	int loc = Location(inSorter, inSuperItem, datasrc);
	if(loc == 0)
	{
		// insert first
		inAfterItem = 0L;
	}
	else
	{
		const LArray *subItems = inSuperItem->GetSubItems();
		Assert_(subItems != 0L);
		subItems->FetchItemAt(loc, &inAfterItem);
	}
	
	BumpNumOfFiles();
	
	if(m_InRefresh)
	{
		inRefresh = false;
		inAdjustImageSize = false;
	}
	
	LOutlineTable::InsertItem(inOutlineItem, inSuperItem, inAfterItem, inRefresh, inAdjustImageSize);
}

void CBrowserTable::AdjustSelectionForContextualMenuClick(const EventRecord& inEvent)
{
	Point						mouseLoc(inEvent.where);
	STableCell			hitCell;
	SPoint32				imagePt;
	bool						updateCommandStatus(false);
	
	this->GlobalToPortPoint(mouseLoc);
	this->PortToLocalPoint(mouseLoc);
	this->LocalToImagePoint(mouseLoc, imagePt);

	if ( this->GetCellHitBy(imagePt, hitCell) )
	{
		if ( !this->CellIsSelected(hitCell) )
		{
			if (!(inEvent.modifiers & shiftKey))
				this->UnselectAllCells();
			this->SelectCell(hitCell);
			this->UpdatePort();
			updateCommandStatus = true;
		}
	}
	else
	{
		this->UnselectAllCells();
		this->UpdatePort();
		updateCommandStatus = true;
	}
	
	if (mSuperCommander != nil) {
		mSuperCommander->SetLatentSub(this);
		SwitchTarget(this);
	}
	
	if ( updateCommandStatus )
	{
		PyInvalUICache();
		CMacCvsApp::gApp->UpdateMenus();
	}
}

SDimension16 CBrowserTable::CalcStandardSize() const
{
  SDimension16    result = {0,0};
  UInt32          width, height;
  const_cast<CBrowserTable*>(this)->GetTableGeometry()->GetTableDimensions(width, height);
  result.width = std::min(width, UInt32(max_Int16));
  result.height = std::min(height, UInt32(max_Int16));
  return result;
}


CBrowserCMAttachment::~CBrowserCMAttachment()
{
  if ( mPathPopup )
  {
    mPathPopup->RemoveMenu();
    delete mPathPopup;
  }
}

void CBrowserCMAttachment::PreCMSelect( const EventRecord& inMacEvent ) //Override
{
	LCommander*		target = this->FindCommandTarget();
	if ( target )
	{
		CBrowserTable*		browserTable = dynamic_cast<CBrowserTable*>(target);
		if ( browserTable )
		{
		  browserTable->AdjustSelectionForContextualMenuClick(inMacEvent);

      bool        oneSelected(false);
      CStr        pathSelected;
      
      STableCell  firstSelected(browserTable->GetFirstSelectedCell());
      if ( !firstSelected.IsNullCell() )
      {
        CBrowserItem*   browserItem = dynamic_cast<CBrowserItem*>(browserTable->FindItemForRow(firstSelected.row));
        if ( browserItem )
        {
          browserItem->GetFullPath(pathSelected);
          mOneSelectedIsFile = (dynamic_cast<CBrowserFileItem*>(browserItem) != NULL);
        }
        ++firstSelected.row;
        oneSelected = !(browserTable->CellIsSelected(firstSelected) || browserTable->GetNextSelectedCell(firstSelected));
      }

      // if only one item selected, provide the "Reveal in Finder" and path submenu
      //
      if ( oneSelected  &&  !pathSelected.empty() )
        mPathPopup = new CBrowserPopup(pathSelected);
		}
	}
}

bool
CBrowserCMAttachment::AddCommand(
	MenuHandle		inMenuH,
	CommandT		inCommand,
	ConstStringPtr	inItemText,
	SInt16			inAfterItem,
	bool			inOverrideDisable)
{
  if ( inCommand == cmd_FilePath )
  {
    if ( mPathPopup )
    {
			SInt16 numItems = (SInt16) ::CountMenuItems(inMenuH);
			if (inAfterItem > numItems) {
				inAfterItem = numItems;
			} else if (inAfterItem < 0) {
				inAfterItem = 0;
			}
      bool result(LCMAttachment::AddCommand(inMenuH, inCommand, mOneSelectedIsFile ? "\pFile Path" : "\pFolder Path", inAfterItem, inOverrideDisable));
      if ( result ) mPathPopup->InstallAsSubmenu(inMenuH, inAfterItem+1);
      return result;
    }
  }
  else
  {
    if ( inCommand == cmd_RevealInFinder  &&  inItemText[0] == 0  &&  mPathPopup != NULL )
      inItemText = "\pReveal In Finder";
    return LCMAttachment::AddCommand(inMenuH, inCommand, inItemText, inAfterItem, inOverrideDisable);
  }
  return false;
}

void
CBrowserCMAttachment::DispatchCommand(
	UInt32		inSelectionType,
	CommandT	inCommand)
{
  bool    handled(false);
  if ( inSelectionType == kCMMenuItemSelected )
  {
    if ( inCommand >= cmd_FirstPathComponent  &&  inCommand < cmd_LastPathComponent )
      handled = mPathPopup->ProcessMenuCommand(inCommand);
    else if ( inCommand == cmd_RevealInFinder )
      handled = mPathPopup->ProcessMenuCommand(cmd_FirstPathComponent);
  }
  // we're done with the path submenu -> release it
  // (unfortunately, we can't do this is PostCMSelect, since we still need the menu and object in here
  //  and PostCMSelect is called before DispatchCommand...)
  //
  if ( mPathPopup ) 
  {
    mPathPopup->RemoveMenu();
    delete mPathPopup;
    mPathPopup = NULL;
  }
  
  // let base class handle dispatching the command if it was none of our path submenu
  //
  if ( !handled ) LCMAttachment::DispatchCommand(inSelectionType, inCommand);
}



namespace {
  void mac2unix_lineendings(const char* fileName, const char* fullPath)
  {
    CStr  command;
    command << "(tr '\\r' '\\n' < '" << fullPath << "' > '/tmp/" << fileName << "_.#tmp') && (cat '/tmp/" << fileName << "_.#tmp' > '" << fullPath << "') && rm '/tmp/" << fileName << "_.#tmp'";
    if ( system(command) == 0 )
      cvs_out("converted linefeeds for %s\n", (const char*)fullPath);
    else
      cvs_err("converting linefeeds for '%s' failed: %s\n", (const char*)fullPath, strerror(errno));
  }
  
  void DoSpinCursor(bool reset=false)
  {
    static    UInt32    curValue(0);
    static    UInt32    nextSpinTicks(0);
    
    if ( reset ) curValue = 0;
    if ( TickCount() >= nextSpinTicks )
    {
      SetAnimatedThemeCursor(kThemeCountingUpAndDownHandCursor, curValue++);
      nextSpinTicks = TickCount() + 30;
      CheckEventQueueForUserCancel(); // can't really cancel, just to avoid spinning pizza of death
    }
  }
  
  class CTraversalMigrateCFMStyleSandbox : public TraversalReport
  {
    bool                  mIsCVSDir;
    std::vector<UFSSpec>  mDirs;
    std::stack<bool>      mIsCVSDirStack;
  public :

    CTraversalMigrateCFMStyleSandbox(const char* dir) : mIsCVSDir(false) 
    {
      mDirs.push_back(GUSIFileSpec(dir));
    }
    
    const std::vector<UFSSpec>& GetDirs() const { return mDirs; }
    
    virtual kTraversal EnterDirectory(const char *fullpath, const char *dirname, const UFSSpec * macspec)
    {
  	  mIsCVSDirStack.push(mIsCVSDir);
  		if (stricmp("CVS", dirname) == 0)  
  		  mIsCVSDir = true;
  		else
  		{		  
  		  mIsCVSDir = false;
        mDirs.push_back(*macspec);
  		}
      return kContinueTraversal;
    }
    virtual kTraversal ExitDirectory(const char *fullpath)
    {
      mIsCVSDir = mIsCVSDirStack.top();
      mIsCVSDirStack.pop();
      return kContinueTraversal;
    }
  	virtual kTraversal OnDirectory(const char *fullpath,
  		const char *fullname,
  		const char *name,
  		const struct stat & dir, const UFSSpec * macspec)
  	{
  	  DoSpinCursor();
  	  kTraversal    result(kContinueTraversal);
  		if (stricmp("CVS", name) != 0) 
  		{		  
  		  // check whether this directory is under cvs
  		  //
  	    GUSIFileSpec cvsSpec(*macspec);
  	    cvsSpec += "CVS";
  	    if ( cvsSpec.Error()  ||  !cvsSpec.Exists() )
          result = kSkipFile;
  		}
      return result;
    }
  	virtual kTraversal OnFile(const char *fullpath,
  		const char *fullname,
  		const char *name,
  		const struct stat & dir, const UFSSpec * macspec)
  	{
  	  DoSpinCursor();
  	  if ( mIsCVSDir )
  	  {
  	    CStr    filePath(fullpath);
  	    if ( !filePath.endsWith(kPathDelimiter) )
  	      filePath << kPathDelimiter;
  	    filePath << name;
  	    
  	    // convert line endings
  	    //
  	    mac2unix_lineendings(name, filePath);
  	  }
      
      return kContinueTraversal;
  	}
  };

  bool MigrateCFMStyleSandbox(const char* dir)
  {
    cvs_out("migrating old-style sandbox %s...\n", dir);
    
    DoSpinCursor(true);
    CTraversalMigrateCFMStyleSandbox  traverse(dir);
    FileTraverse(dir, traverse);
    for ( std::vector<UFSSpec>::const_iterator i(traverse.GetDirs().begin()); i != traverse.GetDirs().end(); ++i )
    {
      GUSIFileSpec    dir(*i);
      
      // get entry list
      //
      CSortList<ENTNODE>  entries(200, ENTNODE::Compare);
      if ( !Entries_Open(entries, dir.FullPath()) ) return false;
      //
      // transfer filenames into set
      //
      int   entryCount(entries.NumOfElements());
      for ( int i(0); i < entryCount; ++i )
      {
  	    EntnodeData   *nodeData = entries.Get(i).Data();
  	    
  	    if ( nodeData->GetType() == ENT_FILE )
  	    {
    	    // unlock the file
    	    //
    	    bool          wasLocked(false), resetMode(false);
      		FSCatalogInfo info;
      		OSStatus      err;
      		GUSIFileSpec  filespec(dir);
      		filespec += entries.Get(i).Data()->GetName();

      		if((err = FSGetCatalogInfo(filespec, kFSCatInfoNodeFlags|kFSCatInfoPermissions|kFSCatInfoContentMod, &info, NULL, NULL, NULL)) == noErr )
      		{
      		  UInt16    mode(reinterpret_cast<FSPermissionInfo*>(info.permissions)->mode);
      		  
      		  if ((info.nodeFlags & kFSNodeLockedMask) != 0)
      		  {
      			  info.nodeFlags = info.nodeFlags & ~kFSNodeLockedMask;
      			  wasLocked = true;
      			}
            // when Mac Linefeeds are not enabled, convert the linefeeds as well
            //
            if ( !gCvsPrefs.MacLF() )
            {
          		const char *nodeInfo = (*nodeData)[EntnodeFile::kOption];
          		if ( nodeInfo == 0L  ||  strcmp(nodeInfo, "-kb") != 0 )
          		{
          		  // not a binary file -> convert Mac LF into Unix LF
          		  //
        			  reinterpret_cast<FSPermissionInfo*>(info.permissions)->mode = (mode | (S_IWUSR|S_IWGRP));
          			err = FSSetCatalogInfo(filespec, kFSCatInfoNodeFlags|kFSCatInfoPermissions, &info);
                if ( err == noErr )
                {
                  resetMode = true;
                  // convert the lineendings
                  mac2unix_lineendings(entries.Get(i).Data()->GetName(), filespec.FullPath());
                  // and reset the modification date, so the file won't appear modified in our list
        			    FSSetCatalogInfo(filespec, kFSCatInfoContentMod, &info);
                }
              }
            }
              			
      			if ( err == noErr && (wasLocked  ||  resetMode) )
      			{
      			  // make file read-only again when it was locked before
      			  reinterpret_cast<FSPermissionInfo*>(info.permissions)->mode = mode;
      			  if ( wasLocked ) reinterpret_cast<FSPermissionInfo*>(info.permissions)->mode &= ~(S_IWUSR|S_IWGRP|S_IWOTH);
        			err = FSSetCatalogInfo(filespec, kFSCatInfoNodeFlags|kFSCatInfoPermissions, &info);
    	        if ( wasLocked  &&  err == noErr ) cvs_out("unlocked file %s\n", filespec.FullPath());
        		}
      		}
      		if ( err ) cvs_err("couldn't unlock or convert file '%s': error %d\n", filespec.FullPath(), err);
        }
    		DoSpinCursor();
      }
    }
    return true;
  }
  bool CheckForCFMStyleSandbox(const char* dir)
  {
  	GUSIFileSpec entrySpec(dir);

  	entrySpec += "CVS";
  	if(entrySpec.Error()  ||  !entrySpec.Exists())
  		return true;

    entrySpec += "Entries";
    if ( entrySpec.Error()  ||  !entrySpec.Exists() )
      return true;
    
    FILE*   f = fopen(entrySpec.FullPath(), "r");
    if ( f == NULL ) return true;
    
    bool    isMacLF(false);
    while ( !feof(f)  &&  !ferror(f) )
      if ( getc(f) == 0x0d ) // CR
      {
        isMacLF = true;
        break;
      }
    fclose(f);
    
    if ( isMacLF )
    {
      // seems to be an old style sandbox -> ask user whether (s)he wants to migrate the sandbox
      //
  		LCFString		message("The CVS information in this folder seems to have been created by an older version of MacCvs. Do you want to convert it?");
  		LCFString		explanation("Older versions of MacCvs used Mac lineendings in the CVS metafiles and (optionally) used to lock the checked out files. This is incompatible with the current version of MacCvs, which uses unix lineendings and leverages the file permissions to make files read-only.");

  		AlertStdCFStringAlertParamRec		alertParams;
  		if ( GetStandardAlertDefaultParams(&alertParams, kStdCFStringAlertVersionOne) == noErr )
  		{
  			DialogRef 	alert;
  			
  			alertParams.movable = true;
  			alertParams.defaultText = CFSTR("Convert");
  			alertParams.cancelText = CFSTR("Cancel");
  			alertParams.otherText = CFSTR("Don't Convert");
  			alertParams.defaultButton = kAlertStdAlertOKButton;
  			
  			if ( CreateStandardAlert(kAlertCautionAlert, message, explanation, &alertParams, &alert) == noErr )
  			{
  				DialogItemIndex			itemHit;
  				RunStandardAlert(alert, NULL, &itemHit);
  				switch ( itemHit )
  				{
  				  case kAlertStdAlertOKButton:
  				    return MigrateCFMStyleSandbox(dir);
  				  
  				  case kAlertStdAlertCancelButton:
  				    return false;
  				  
  				  default:
  				    return true; // user continues on own risk...
  				}
  		  }
  		}
    }
    return true;
  }
} // end of anonymous namespace
