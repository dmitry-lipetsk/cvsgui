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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 2000
 */

/*
 * CBrowserWind.cpp --- main browser class
 */

#include "GUSIInternal.h"
#include "GUSIFileSpec.h"
#include <unistd.h>
#include <memory>
#include "CBrowserWind.h"
#include "MacCvsApp.h"
#include "MacCvsConstant.h"
#include "CBrowserTable.h"
#include "PromptFiles.h"
#include "MacrosSetup.h"
#include "CvsCommands.h"
#include "CvsPrefs.h"
#include "TclGlue.h"
#include "MultiFiles.h"
#include "CvsArgs.h"
#include "MoveToTrash.h"
#include "AppConsole.h"
#include "VolsPaths.h"
#include "stdfolder.h"
#include "TextBinary.h"
#include "PythonGlue.h"
#include "UPseudoSupport.h"
#include "BrowseViewHandlers.h"
#include "BrowseViewModel.h"
#include "LEditText.h"
#include "LCmdHelpBevelBtn.h"
#include "MacMisc.h"

#include "umain.h"

#include <LPopupButton.h>

enum {
	item_LocateBrowserPopup		= 'POPP'
};

namespace {
  template <class T> class tmp_set
  {
    T   mOldValue;
    T&  mTarget;
  public:
    tmp_set(T& inTarget, const T& inNewValue) : mTarget(inTarget), mOldValue(inTarget) { inTarget = inNewValue; }
    ~tmp_set() { mTarget = mOldValue; }
  };
  typedef tmp_set<bool> tmp_set_bool;
  
} // end of anonymous namespace

/// CmdHelpBevelButton class that will refresh it's state from the parent window, not the current command target
class LLocalCmdHelpBevelButton : public LCmdHelpBevelButton
{
public:
	enum {class_ID = 'lHBB'};

	LLocalCmdHelpBevelButton(LStream *inStream) : LCmdHelpBevelButton(inStream) {}
	
  void SpendTime(const EventRecord&	/* inMacEvent */)
  {
  		// Enable or disable the button based on the state of
  		// its associated command

  		// Check if command status could have changed and if
  		// we have a useful command

  	if ( LCommander::GetUpdateCommandStatus() &&
  		 (mCommand != cmd_Nothing) ) {

  		 	// Query our container window about the status of our command
  		LCommander*	target;
      LView*      super(this->GetSuperView());
      if ( super )
      {
        while ( super->GetSuperView() )
          super = super->GetSuperView();
        
        target = dynamic_cast<CBrowserWind*>(super);
      }
  		if (target != nil) {

  			Boolean		cmdEnabled = false;
  			Boolean		usesMark   = false;
  			UInt16		mark;
  			Str255		cmdName;

  			target->ProcessCommandStatus(mCommand, cmdEnabled, usesMark,
  											mark, cmdName);

  			if (cmdEnabled) {		// Match visual state of Button
  				Enable();			//   to the state of its command
  			} else {
  				Disable();
  			}


  			if (usesMark) {			// For Buttons associated with a
  									//   markable menu item, Button is
  									//   on when item is marked.
  				SInt32	value = Button_Off;
  				if (mark != noMark) {
  					value  = Button_On;
  				}
  				SetValue(value);
  			}
  		}
  	}
  }
};

UStr CBrowserWind::sLastActivatedPath;

extern "C" {
	static int sortFiles(CBrowserCell *s1, CBrowserCell *s2);
}

static int sortFiles(CBrowserCell *s1, CBrowserCell *s2)
{
	return strcmp(s1->fData->GetFolderPath(), s2->fData->GetFolderPath());
}

static SInt32 GetScrollbarWidth()
{
  SInt32  value;
  return GetThemeMetric(kThemeMetricScrollBarWidth, &value) ? 16 : value;
}

// this one is usefull because GetNextSelectedCell returns
// each cells of each row
static void GetNextSelectedRow(LTableView *tab, STableCell & ioCell)
{
	TableIndexT	row = ioCell.row;
	while(!ioCell.IsNullCell())
	{
		if(!tab->GetNextSelectedCell(ioCell))
			break;
		if(row != ioCell.row)
			return;
	}
	ioCell.row = 0;
	ioCell.col = 0;
}


CBrowserWind::CBrowserWind(LStream *inStream) : LWindow( inStream ), mCurrentFilterStyle(ColumnIndex::kEnd), mHandlingFilterChange(false)
{
}

CBrowserWind::~CBrowserWind()
{
	CBrowserTable *browse = dynamic_cast<CBrowserTable*>
		(FindPaneByID(item_BrowserTable));
	if ( browse )
	{
	  browse->UpdateSortSettings();
	  browse->RememberGeometry();
  	UStr path(browse->GetPath());
  	if(path == sLastActivatedPath)
  	  SavePersistentSettings(path);
  }
}

void CBrowserWind::FinishCreateSelf()
{
	LPopupButton *popup = dynamic_cast<LPopupButton*>(FindPaneByID(cmd_FilterMaskType));
	ThrowIfNil_(popup != 0L);
	popup->AddListener(this);
	
	popup = dynamic_cast<LPopupButton*>(FindPaneByID(item_LocateBrowserPopup));
	ThrowIfNil_(popup != 0L);
	popup->SetMacMenuH(::NewMenu(::UniqueID('MENU'), "\p"), true);
	popup->AddListener(this);
	UpdateBrowserMenu();
}

bool CBrowserWind::LocateBrowser(const char *dir)
{
	// set the window's title to the folder name			
	CStr uppath, file;
	SplitPath(dir, uppath, file);
	SetDescriptor(UTF8_to_LString(file));
	
	//	Handle Proxy icons
	if (UEnvironment::HasGestaltAttribute (gestaltWindowMgrAttr, 0)) do {
		GUSIFileSpec	spec (dir);
		if (!spec.Exists ()) break;
		
		AliasHandle		alias = nil;
		OSErr			e = ::FSNewAlias (nil, spec, &alias);
		if (noErr != e) break;
		
		StGrafPortSaver		port;				// Mac OS 8.5 needs this
		::SetWindowProxyAlias (GetMacWindow (), alias);
		::SetWindowModified	(GetMacWindow (), false);
		} while (false);

	CBrowserTable *browse = dynamic_cast<CBrowserTable*>
		(FindPaneByID(item_BrowserTable));
	ThrowIfNil_(browse);
	return browse->ResetBrowser(dir);
}

void CBrowserWind::UpdateBrowserMenu()
{
	LPopupButton *popup = dynamic_cast<LPopupButton*>(FindPaneByID(item_LocateBrowserPopup));
	ThrowIfNil_(popup != 0L);
	
	bool	wasBroadcasting (popup->IsBroadcasting ());
	popup->StopBroadcasting ();
	
	popup->DeleteAllMenuItems();
	
	LMenu *browserMenu	= LMenuBar::GetCurrentMenuBar()->FetchMenu(menu_RecentBrowser);
	
	const std::vector<CStr> & allPaths = sBrowserLoc.GetList();	
	std::vector<CStr>::const_iterator j;
	for(j = allPaths.begin(); j != allPaths.end(); ++j)
	{
		Str255	menuText;
		GetMenuItemText (browserMenu->GetMacMenuH (), 
						 browserMenu->IndexFromCommand (cmd_FirstBrowser + (j - allPaths.begin())),
						 menuText);
		popup->InsertMenuItem(menuText, max_Int16 - 1);
	}
	if(allPaths.size() > 0)
	{
		CBrowserTable *browse = dynamic_cast<CBrowserTable*>
			(FindPaneByID(item_BrowserTable));
		ThrowIfNil_(browse);
		CMString::list_t::const_iterator i = sBrowserLoc.Find (browse->GetPath ());
		if (i != sBrowserLoc.end ())
			popup->SetCurrentMenuItem(1 + i - sBrowserLoc.begin ());
	}
	
	if (wasBroadcasting) popup->StartBroadcasting ();
}

Boolean
CBrowserWind::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled = true;
	
	if((inCommand >= cmd_BrowserStart && inCommand < cmd_BrowserEnd) ||
		inCommand == cmd_Checkout ||
		inCommand == cmd_CmdLine ||
		inCommand == cmd_RtagCreate ||
		inCommand == cmd_RtagDelete ||
		inCommand == cmd_RtagBranch ||
		inCommand == cmd_Import)
	{
		return ObeyCommandBrowser(inCommand, ioParam);
	}
	
	if(inCommand >= cmd_FilterStart && inCommand < cmd_FilterEnd)
  {
  	CBrowserTable *brows = dynamic_cast<CBrowserTable*>
  		(FindPaneByID(item_BrowserTable));
  	if(brows == 0L)
  		return true;
    KoFilterModel *filter = brows->GetFilterModel();
    if ( filter == NULL )
      return true;
    KoIgnoreModel *ignore = brows->GetIgnoreModel();
    if ( ignore == NULL )
      return true;
    KoRecursionModel *recursion = brows->GetRecursionModel();
    if ( recursion == NULL )
      return true;
      
    switch ( inCommand )
    {
    	case cmd_FilterCommitable :
    		filter->ToggleModified();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterChanged :
    		filter->ToggleChanged();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterAdded :
    		filter->ToggleAdded();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterRemoved :
    		filter->ToggleRemoved();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterConflict :
    		filter->ToggleConflict();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterShowUnknown :
    		filter->ToggleUnknown();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterHideUnknown :
    		filter->ToggleHideUnknown();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterMissing :
    		filter->ToggleMissing();
    		brows->FreshenFileMask();
    		filter->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterIgnored :
    		ignore->ToggleIgnore();
    		ignore->GetNotificationManager()->NotifyAll();
    		break;
    	case cmd_FilterFlat :
    		recursion->ToggleRecursion();
    		recursion->GetNotificationManager()->NotifyAll();
    		break;
      case cmd_FilterMaskType:
    	case cmd_FilterEdit :
    	  if ( !mHandlingFilterChange )
    	  {
      	  tmp_set_bool   in_handler(mHandlingFilterChange, true);
    			LEditText *edit = dynamic_cast<LEditText*>
    				(this->FindPaneByID(cmd_FilterEdit));
    			LPopupButton *style = dynamic_cast<LPopupButton*>
    				(this->FindPaneByID(cmd_FilterMaskType));
    			if(edit != 0L && style != 0L)
    			{
      		  static const int   filterStyles[] = { ColumnIndex::kEnd,
        		                                      ColumnIndex::kName, 
        		                                      ColumnIndex::kExtention, 
        		                                      ColumnIndex::kRevision, 
        		                                      ColumnIndex::kOption,
        		                                      ColumnIndex::kState,
        		                                      ColumnIndex::kStickyTag,
        		                                      ColumnIndex::kTimeStamp,
        		                                      ColumnIndex::kConflict,
        		                                      ColumnIndex::kPath };
    				int     filterStyle(ColumnIndex::kName);
    				SInt32  selectedStyle(style->GetValue());
    				if ( selectedStyle >= 1  &&  selectedStyle <= sizeof(filterStyles)/sizeof(*filterStyles) )
    				  filterStyle = filterStyles[selectedStyle-1];
    				  
    				LStr255 txt;
    				edit->GetText(txt);

    				// determine what really to do: enable or disable filter
    				if ( filterStyle == ColumnIndex::kEnd )
    				{
      				if ( inCommand == cmd_FilterMaskType )
      				{
      				  // empty text edit field if not yet empty
        				if ( txt.Length() > 0 ) {
        				  edit->SetText(LStr255());
        				  txt = "";
        				}
      				}
      				else if ( txt.Length() > 0 ) {
      				  style->SetValue(2);
      				  filterStyle = ColumnIndex::kName; // user entered some filter text -> default to name filter
      				}
            }
            else
            {
              // if user removed all text, reset filter type back to 'none'
      				if ( inCommand == cmd_FilterEdit  &&  txt.Length() == 0 )
      				{
      				  style->SetValue(1);
      				  filterStyle = ColumnIndex::kEnd;
      				}
            }
            
            // now adjust filter according to style & mask
    				if ( filterStyle == ColumnIndex::kEnd )
    				{
      				if ( mCurrentFilterStyle != ColumnIndex::kEnd ) filter->SetMask(mCurrentFilterStyle, "");
      				mCurrentFilterStyle = ColumnIndex::kEnd;
      				if ( filter->IsFilterMasksEnable() ) filter->ToggleFilterMasksEnable();
    				}
    				else
    				{
      				UStr cstr(txt);
      				filter->SetMask(filterStyle, cstr.c_str());
      				if ( filterStyle != mCurrentFilterStyle && mCurrentFilterStyle != ColumnIndex::kEnd ) filter->SetMask(mCurrentFilterStyle, "");
      				mCurrentFilterStyle = filterStyle;
      				if ( !filter->IsFilterMasksEnable() ) filter->ToggleFilterMasksEnable();
            }
    				filter->GetNotificationManager()->NotifyAll();
    			}
        }
    		break;

    	case cmd_FilterPopup:
    	{
    		SInt32 *value = static_cast<SInt32 *>(ioParam);
  			LBevelButton *btn = dynamic_cast<LBevelButton*>
  				(this->FindPaneByID(cmd_FilterPopup));
  			if(btn != 0L)
  			{
  				LStr255 txt;
  				btn->GetMenuItemText(*value, txt);
  				UStr cstr(txt);
  				filter->SetMask(ColumnIndex::kName, cstr);
  				filter->GetNotificationManager()->NotifyAll();
  				
  				LEditText *edit = dynamic_cast<LEditText*>
  					(this->FindPaneByID(cmd_FilterEdit));
  				if(edit != 0L)
  					edit->SetText(txt);
  			}

    		break;
    	}
    }
    return true;
  }
  
	switch (inCommand) {
    		
		case cmd_SaveSettings:
		{
			CBrowserTable *browse = dynamic_cast<CBrowserTable*>
				(FindPaneByID(item_BrowserTable));
			ThrowIfNil_(browse);
			
			::AskCreatePersistentSettings(browse->GetPath());
			break;
		}
		
		case cmd_LocateBrowser:
		{
			CStr dir;
			
			if ( BrowserGetDirectory("Select a directory to browse :", dir) )
			{
				if ( LocateBrowser (dir) )
  				CMacCvsApp::gApp->UpdateBrowserMenu(dir);
			}
			break;
		}
		
		case cmd_RememberBrowserLocation:
		{
			CBrowserTable *browse = dynamic_cast<CBrowserTable*>
				(FindPaneByID(item_BrowserTable));
			ThrowIfNil_(browse);
			CMacCvsApp::gApp->UpdateBrowserMenu(browse->GetPath());
		  break;
		}
			
		default:
			cmdHandled = LWindow::ObeyCommand(inCommand, ioParam);
			break;
	}
		
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus
// ---------------------------------------------------------------------------
//	Pass back whether a Command is enabled and/or marked (in a Menu)

void
CBrowserWind::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	if((inCommand >= cmd_BrowserStart && inCommand < cmd_BrowserEnd) ||
		(inCommand >= cmd_FirstMacro && inCommand < cmd_LastMacro) ||
		inCommand == cmd_Checkout ||
		inCommand == cmd_CmdLine ||
		inCommand == cmd_RtagCreate ||
		inCommand == cmd_RtagDelete ||
		inCommand == cmd_RtagBranch ||
		inCommand == cmd_Import)
	{
		FindCommandStatusBrowser(inCommand, outEnabled, outUsesMark,
			outMark, outName);
		return;
	}

	if(inCommand >= cmd_FilterStart && inCommand < cmd_FilterEnd)
	{
  	CBrowserTable *brows = dynamic_cast<CBrowserTable*>
  		(FindPaneByID(item_BrowserTable));
    KoFilterModel   *filter = brows ? brows->GetFilterModel() : NULL;
    KoIgnoreModel   *ignore = brows ? brows->GetIgnoreModel() : NULL;
    KoRecursionModel   *recursion = brows ? brows->GetRecursionModel() : NULL;
		if(brows == NULL  ||  filter == NULL || ignore == NULL || recursion == NULL)
		{
			outEnabled = false;
			return;
		}
		switch ( inCommand )
		{
    	case cmd_FilterCommitable :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsModified() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterChanged :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsChanged() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterAdded :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsAdded() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterRemoved :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsRemoved() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterConflict :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsConflict() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterShowUnknown :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsUnknown() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterHideUnknown :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsHideUnknown() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterMissing :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = filter->IsMissing() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterIgnored :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = ignore->IsShowIgnored() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterFlat :
    		outEnabled = true;
    		outUsesMark = true;
    		outMark = recursion->IsShowRecursive() ? kMenuCheckmarkGlyph : noMark;
    		break;
    	case cmd_FilterPopup :
    		outEnabled = true;
    		break;
      default:
        outEnabled = false;
        break;
    }
    return;
	}

	switch (inCommand) {
	
		case cmd_LocateBrowser:
		case cmd_SaveSettings:
			outEnabled = !CMacCvsApp::gApp->IsCvsRunning();
			break;
		
		case cmd_RememberBrowserLocation:
		{
  	  CBrowserTable *browse = dynamic_cast<CBrowserTable*>
  		  (FindPaneByID(item_BrowserTable));
		  outEnabled = (sBrowserLoc.Find(browse->GetPath()) == sBrowserLoc.end());
		  break;
		}
		  	
		default:
			LWindow::FindCommandStatus(inCommand, outEnabled,
									outUsesMark, outMark, outName);
			break;
	}
}

void CBrowserWind::ListenToMessage(MessageT inMessage, void *ioParam)
{
	switch (inMessage) {
		case item_LocateBrowserPopup:
			{
			SInt32 *value = static_cast<SInt32 *>(ioParam);
			LPopupButton *popup = dynamic_cast<LPopupButton*>(FindPaneByID(item_LocateBrowserPopup));
			ThrowIfNil_(popup != 0L);

			const std::vector<CStr> & allPaths = sBrowserLoc.GetList();
			CStr newpath = allPaths[*value - 1];
			
			if ( LocateBrowser (newpath) )
  			CMacCvsApp::gApp->UpdateBrowserMenu(newpath);
			break;
			} // case
		
		case cmd_FilterMaskType:
		  this->ObeyCommand(cmd_FilterMaskType, ioParam);
		  break;
		  	
		case msg_UpdateBrowserMenu:
			UpdateBrowserMenu ();
			break;
	} // switch
}

void CBrowserWind::ActivateSelf()
{
	LWindow::ActivateSelf();
	
	CBrowserTable *browse = dynamic_cast<CBrowserTable*>
		(FindPaneByID(item_BrowserTable));
	ThrowIfNil_(browse);
	
	UStr path(browse->GetPath());
	if(path != sLastActivatedPath)
	{
//		StartIdling();
	  LoadPersistentSettings(path);
		sLastActivatedPath = path;
	}
}

void CBrowserWind::DeactivateSelf()
{
	LWindow::DeactivateSelf();
	
	CBrowserTable *browse = dynamic_cast<CBrowserTable*>
		(FindPaneByID(item_BrowserTable));
	if ( browse )
	{
	  browse->UpdateSortSettings();
  	UStr path(browse->GetPath());
  	if(path == sLastActivatedPath)
  	  SavePersistentSettings(path);
  }
}

void CBrowserWind::SpendTime(const EventRecord&	inMacEvent)
{
	// we cannot load the settings directly from ActivateSelf due
	// to a dark unknown Yield problem of GUSI
//	LoadPersistentSettings(sLastActivatedPath);
	StopIdling();
}

bool CBrowserWind::ProcessSelectionCommand(KiSelectionHandler & handler)
{
	struct cvs_running
	{
	  cvs_running() { CMacCvsApp::gApp->SetCvsRunning(true); }
	  ~cvs_running() { CMacCvsApp::gApp->SetCvsRunning(false); }
	};
	
	cvs_running   running;
	
	for(int i = 0; i < m_allFolders.size(); i++)
	{
		handler.OnFolder(m_allFolders[i]);
	}
	if(m_mf.NumDirs() != 0)
		handler.OnFiles(&m_mf);
	
	return m_mf.NumDirs() != 0 || m_allFolders.size() != 0;
}

Boolean CBrowserWind::ObeyCommandBrowser(CommandT inCommand, void *ioParam)
{	
	CBrowserTable *brows = dynamic_cast<CBrowserTable*>
		(FindPaneByID(item_BrowserTable));
	if(brows == 0L)
		return true;

	int numFiles = 0;
	int numFolders = 0;
	EntnodeData *data;
	CBrowserItem *item;
	STableCell cell;

	// sort a file list by directory in order to build
	// a multi-file object
	m_mf.reset();
	std::vector<CBrowserCell> allFiles;
	m_allFolders.erase(m_allFolders.begin(), m_allFolders.end());
	
	cell = brows->GetFirstSelectedCell();
	while(!cell.IsNullCell())
	{
		item = dynamic_cast<CBrowserItem*>
			(brows->FindItemForRow(cell.row));
		Assert_(item != 0L);
		if(item != 0L)
		{
			data = item->GetData();
			Assert_(data != 0L);
			if(data != 0L)
			{
				if(data->GetType() == ENT_FILE)
				{
					allFiles.push_back(CBrowserCell(data->GetMacSpec(), data));
				}
			}
		}
		GetNextSelectedRow(brows, cell);
	}
	numFiles = allFiles.size();

	// we used to create several directories for the MultiFiles. In fact WinCvs
	// and gCvs avoid that. Instead they take the root and make all he sub files
	// relative to the root. It is much more efficient this way, and cvs knows how
	// to deal with it anyway.
#if 0
	qsort(&*allFiles.begin(), numFiles, sizeof(CBrowserCell), (int (*)(const void*, const void*))sortFiles); 

	int offset = 0;
	int f = 0;
	while(offset < numFiles)
	{
		m_mf.newdir(allFiles[offset].fData->GetFolderPath());
		for(f = offset; f < numFiles; f++)
		{
			if(sortFiles(&allFiles[offset], &allFiles[f]) != 0)
				break;

			m_mf.newfile(allFiles[f].fData->GetName(), &allFiles[f].fSpec);
		}
		offset = f;
	}
#else
	UStr rootPath = brows->GetPath();
	if(!rootPath.endsWith(kPathDelimiter))
		rootPath << kPathDelimiter;
	std::vector<CBrowserCell>::const_iterator i;
	for(i = allFiles.begin(); i != allFiles.end(); ++i)
	{
		if(m_mf.NumDirs() == 0)
			m_mf.newdir(rootPath);
		UStr fullpath((*i).fData->GetFolderPath());
		if(!fullpath.endsWith(kPathDelimiter))
			fullpath << kPathDelimiter;
		fullpath << (*i).fData->GetName();
		m_mf.newfile(fullpath.c_str() + rootPath.length(), &(*i).fSpec);
	}
#endif

	allFiles.erase(allFiles.begin(), allFiles.end());

	// get all the directories
	cell = brows->GetFirstSelectedCell();
	
	while(!cell.IsNullCell())
	{
		item = dynamic_cast<CBrowserItem*>
			(brows->FindItemForRow(cell.row));
		Assert_(item != 0L);
		if(item != 0L)
		{
			data = item->GetData();
			Assert_(data != 0L);
			if(data != 0L)
			{
				if(data->GetType() == ENT_SUBDIR)
				{
					char aPath[512];
					GUSIFileSpec spec = data->GetMacSpec();
					char* specPath = spec.FullPath();
					if ( specPath ) {
  					strcpy(aPath, specPath);
          }
  			  ThrowIfOSErr_(spec.Error());
					m_allFolders.push_back(CBrowserCell(aPath, data));
				}
			}
		}
		GetNextSelectedRow(brows, cell);
	}
	numFolders = m_allFolders.size();

	CBrowserTable *browse = dynamic_cast<CBrowserTable*>
		(FindPaneByID(item_BrowserTable));
	ThrowIfNil_(browse);
	
	std::auto_ptr<KiSelectionHandler>   handler;	
	bool                                fallbackToRootPathIfNoSelection(false);
	bool                                forceRefresh(false);
	
	switch(inCommand)
	{
		case cmd_SelUpdate :
			handler.reset(new KoUpdateHandler);
			break;

		case cmd_SelQuery :
			handler.reset(new KoQueryUpdateHandler);
			break;

		case cmd_SelCommit :
			handler.reset(new KoCommitHandler);
			break;

		case cmd_SelAdd :
			handler.reset(new KoAddHandler);
			break;

		case cmd_SelAddBin :
			handler.reset(new KoAddBinaryHandler);
			break;

		case cmd_SelRemove :
			handler.reset(new KoFileRemoveHandler);
			break;

		case cmd_SelLogTree :
			handler.reset(new KoGraphHandler);
			break;

		case cmd_SelDiff :
			handler.reset(new KoDiffHandler);
			break;

		case cmd_SelLog :
			handler.reset(new KoLogHandler);
			break;

		case cmd_SelStatus :
			handler.reset(new KoStatusHandler);
			break;

		case cmd_SelAnnotate :
			handler.reset(new KoAnnotateHandler);
			break;

		case cmd_SelLock :
			handler.reset(new KoLockHandler);
			break;

		case cmd_SelUnLock :
			handler.reset(new KoUnlockHandler);
			break;

		case cmd_SelWatchOn :
			handler.reset(new KoWatchOnHandler);
			break;

		case cmd_SelWatchOff :
			handler.reset(new KoWatchOffHandler);
			break;

		case cmd_SelEdit :
			handler.reset(new KoEditHandler);
			break;

		case cmd_SelUnEdit :
			handler.reset(new KoUneditHandler);
			break;

		case cmd_SelWatchers :
			handler.reset(new KoWatchersHandler);
			break;

		case cmd_SelEditors :
			handler.reset(new KoEditorsHandler);
			break;

		case cmd_SelTagCreate :
			handler.reset(new KoCreateTagHandler);
			break;

		case cmd_SelTagDelete :
			handler.reset(new KoDeleteTagHandler);
			break;

		case cmd_SelTagBranch :
			handler.reset(new KoBranchTagHandler);
			break;

		case cmd_SelRelease :
			handler.reset(new KoReleaseHandler);
			break;

		case cmd_SelDelete :
			handler.reset(new KoMoveToTrashHandler);
			forceRefresh = true;
			break;

		case cmd_SelReload :
			brows->ResetBrowser(true);
			break;
			
		case cmd_Checkout :
			handler.reset(new KoCheckoutHandler);
			fallbackToRootPathIfNoSelection = true;
			break;

		case cmd_Import :
			handler.reset(new KoImportHandler);
			fallbackToRootPathIfNoSelection = true;
			break;

		case cmd_CmdLine :
			handler.reset(new KoCommandLineHandler);
			fallbackToRootPathIfNoSelection = true;
			break;

		case cmd_RtagCreate :
			handler.reset(new KoRtagCreateHandler);
			fallbackToRootPathIfNoSelection = true;
			break;

		case cmd_RtagDelete :
			handler.reset(new KoRtagDeleteHandler);
			fallbackToRootPathIfNoSelection = true;
			break;

		case cmd_RtagBranch :
			handler.reset(new KoRtagBranchHandler);
			fallbackToRootPathIfNoSelection = true;
			break;

		default:
			return false;
	}
	
	if ( handler.get() != NULL ) {
	  if ( !ProcessSelectionCommand(*handler)  &&  fallbackToRootPathIfNoSelection )
	    handler->OnFolder(browse->GetPath());
	  if ( handler->GetNeedViewReset() ) CMacCvsApp::gApp->RefreshBrowsers(forceRefresh);
	}
	
	return true;
}

void CBrowserWind::FindCommandStatusBrowser(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	UInt16		&outMark,
	Str255		outName)
{
	if(gCvsPrefs.empty() || CMacCvsApp::gApp->IsCvsRunning())
	{
		outEnabled = false;
		return;
	}

	static int numFiles = 0;
	static int numFolders = 0;
	static int numUnknowns = 0;
	static int numMissing = 0;
	static int numTotal = 0;
	static int numRemoved = 0;
	bool fillCache = !PyIsUICacheValid();

	if(fillCache)
	{
		PyValidUICache();
		
		numFiles = 0;
		numFolders = 0;
		numUnknowns = 0;
		numMissing = 0;
		numTotal = 0;
		numRemoved = 0;

		CBrowserTable *brows = dynamic_cast<CBrowserTable*>
			(FindPaneByID(item_BrowserTable));

		STableCell cell = brows->GetFirstSelectedCell();
		while(!cell.IsNullCell())
		{
			CBrowserItem *item = dynamic_cast<CBrowserItem*>
				(brows->FindItemForRow(cell.row));
			Assert_(item != 0L);
			if(item != 0L)
			{
				EntnodeData *data = item->GetData();
				Assert_(data != 0L);
				if(data != 0L)
				{
					PyAppendCache(data);
					
					if(data->GetType() == ENT_FILE)
						numFiles++;
					else
						numFolders++;
					
					if(data->IsMissing())
						numMissing++;
					else if(data->IsUnknown())
						numUnknowns++;
					
					if(data->IsRemoved())
						numRemoved++;
				}
				numTotal++;
			}
			GetNextSelectedRow(brows, cell);
		}
	}
	
	if(inCommand >= cmd_FirstMacro && inCommand < cmd_LastMacro)
	{
		UPseudoCmdStatusWrapper wrapper;
		wrapper.outEnabled = &outEnabled;
		wrapper.outUsesMark = &outUsesMark;
		wrapper.outMark = &outMark;
		wrapper.outName = outName;
		UCmdUI cmdui(inCommand);
		cmdui.m_wrapper = &wrapper;
		PyDoCmdUI(&cmdui);
		return;
	}

	switch(inCommand)
	{
	case cmd_SelQuery :
	case cmd_SelUpdate :
	case cmd_SelCommit :
	case cmd_SelDiff :
	case cmd_SelLog :
	case cmd_SelStatus :
	case cmd_SelAnnotate :
	case cmd_SelLock :
	case cmd_SelUnLock :
	case cmd_SelWatchOn :
	case cmd_SelWatchOff :
	case cmd_SelEdit :
	case cmd_SelUnEdit :
	case cmd_SelWatchers :
	case cmd_SelEditors :
	case cmd_SelTagCreate :
	case cmd_SelTagDelete :
	case cmd_SelTagBranch :
		outEnabled = numTotal > 0 && numUnknowns == 0;
		break;
	case cmd_SelAdd :
	case cmd_SelAddBin :
		outEnabled = (numTotal == numUnknowns && numUnknowns > 0) ||
			(numTotal == numMissing && numTotal == numRemoved && numTotal > 0);
		break;
	case cmd_SelLogTree :
	case cmd_SelRemove :
		outEnabled = numTotal > 0 && numFolders == 0 && numUnknowns == 0;
		break;
	case cmd_SelDelete :
		outEnabled = numTotal > 0 && numFolders == 0 && numMissing == 0;
		break;
	case cmd_SelReload :
		outEnabled = true;
		break;
	case cmd_SelRelease :
		outEnabled = numFiles == 0 && numFolders > 0 && numUnknowns == 0 && numMissing == 0;
		break;
	case cmd_Checkout:
	case cmd_Import:
	case cmd_CmdLine:
	case cmd_RtagCreate:
	case cmd_RtagDelete:
	case cmd_RtagBranch:
		outEnabled = true;
		break;
	}
}

void
CBrowserWind::CalcStandardBoundsForScreen(
	const Rect&	inScreenBounds,
	Rect&		outStdBounds) const
{
  Rect      screenBounds(inScreenBounds);

  //
  // adjust screenbounds for toolbar
  //
  Rect      toolbarBounds(CMacCvsApp::gApp->GetToolbarGlobalBounds());
  Rect      r;
  
  if ( SectRect(&screenBounds, &toolbarBounds, &r) )
    screenBounds.top = toolbarBounds.bottom - 1;
  
  // calculate standard size based on browser contents
  //
	const CBrowserTable *browse = dynamic_cast<const CBrowserTable*>
		(FindConstPaneByID(item_BrowserTable));
	if ( browse )
	{
	  if ( browse->CalcPortFrameRect(r) )
	  {
	    static SInt32   scrollbarWidth(GetScrollbarWidth());
	    SDimension16    browserBestSize(browse->CalcStandardSize());
	    const_cast<CBrowserWind*>(this)->mStandardSize.width = browserBestSize.width + scrollbarWidth + 2;
	    const_cast<CBrowserWind*>(this)->mStandardSize.height = browserBestSize.height + r.top + scrollbarWidth + 2;
	  }
	}
  
  // and let base class do the remaining calculations
  //
  LWindow::CalcStandardBoundsForScreen(screenBounds, outStdBounds);
}

void CBrowserWind::RegisterClasses()
{
	RegisterClass_(LLocalCmdHelpBevelButton);
}
