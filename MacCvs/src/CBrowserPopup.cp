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
 * CBrowserPopup.cpp --- popup prompted in the browser with Option/Click
 */

#include "GUSIInternal.h"
#include "GUSIFileSpec.h"
#include <unistd.h>

#include "CBrowserPopup.h"

#include "TextBinary.h"
#include "AppConsole.h"
#include "CvsCommands.h"
#include "UProcessUtilities.h"
#include "MacCvsConstant.h"

const ResIDT kPopupTextTraits = 129;

CBrowserPopup::CBrowserPopup(const char * fullPath)
{
	mPath = fullPath;
	mMenuID = ::UniqueID('MENU');
	mMenu = ::NewMenu(mMenuID, "\p");
}

CBrowserPopup::~CBrowserPopup()
{
	if(mMenu != 0L)
		::DisposeMenu(mMenu);
}

void CBrowserPopup::Track(Point inGlobalPoint)
{
	if (mMenu == 0L)
		return;
	
	if(BuildBrowserMenu())
	{
		::InsertMenu(mMenu, hierMenu);
		SInt32 theResult = ::PopUpMenuSelect(mMenu, inGlobalPoint.v, inGlobalPoint.h, 1);
		::DeleteMenu(mMenuID);
		
		OpenMenuItemSelection(HiWord(theResult), LoWord(theResult));
	}
}

void CBrowserPopup::InstallAsSubmenu(MenuHandle inMenu, MenuItemIndex inItemIndex)
{
	if(BuildBrowserMenu())
	{
		::InsertMenu(mMenu, hierMenu);
    ::SetMenuItemHierarchicalMenu(inMenu, inItemIndex, mMenu);
  }
}

void CBrowserPopup::RemoveMenu()
{
  if ( mMenu ) DeleteMenu(mMenuID);
}

void CBrowserPopup::SendFinderOpenAE(UInt32 inFileCount, const UFSSpec *theSpecs)
{
	// from MacCvsPro
	OSErr					error			= noErr;
	AliasHandle				alias 			= NULL;
	AppleEvent				event 			= { typeNull, NULL };
	ProcessSerialNumber 	theFinderPSN	= { 0, 0 };
												
	// only do this if the finder is running			
	if ( UProcessUtils::IsApplicationRunning('MACS', theFinderPSN) )
	{
		StAEDescriptor	editorAddress(	typeProcessSerialNumber, 
										&theFinderPSN, 
										sizeof(theFinderPSN));

		//	Create the AppleEvent.
		error = ::AECreateAppleEvent(	kCoreEventClass, 
											kAEOpen, 
											(const AEAddressDesc *) &editorAddress, 
											0, 
											0, 
											&event);
		
		if ( error != noErr )
			throw( error );
					
		StAEDescriptor	fileList;

	  error = ::AECreateList(NULL, 0, false, &fileList.mDesc);

		if ( error != noErr )
			throw( error );

		for ( UInt32 i(0); i < inFileCount; ++i )
		{
  		//	Create an alias for this item								
  		error = ::FSNewAlias(NULL, &theSpecs[i], &alias);

  		if ( error != noErr )
  			throw( error );

  		//	Stuff the alias in the AppleEvent as the 
  		//	direct parameter.
  		::HLock((Handle)alias);
  		StAEDescriptor	aliasObject(typeAlias, *alias, ::GetHandleSize((Handle)alias));
  		::AEPutDesc(fileList, 0, aliasObject);
  		::HUnlock((Handle)alias);
    }
  	::AEPutParamDesc(&event, keyDirectObject, fileList);
    
		//	Send the open AppleEvent
		UAppleEventsMgr::SendAppleEvent(event, true);		
	}
	
	if ( alias != NULL )
		::DisposeHandle( (Handle)alias );
			
	::AEDisposeDesc( &event );
}

void CBrowserPopup::SendFinderOpenAE(const UFSSpec &theSpec, bool reveal)
{
	if ( reveal )
	{
  	// from MacCvsPro
  	OSErr					error			= noErr;
  	AliasHandle				alias 			= NULL;
  	AppleEvent				event 			= { typeNull, NULL };
  	ProcessSerialNumber 	theFinderPSN	= { 0, 0 };
  												
  	// only do this if the finder is running			
  	if ( UProcessUtils::IsApplicationRunning('MACS', theFinderPSN) )
  	{
  	
  		StAEDescriptor	editorAddress(	typeProcessSerialNumber, 
  										&theFinderPSN, 
  										sizeof(theFinderPSN));

  		//	Create the AppleEvent.
			error = ::AECreateAppleEvent(	kAEMiscStandards, 
											kAEMakeObjectsVisible, 
											(const AEAddressDesc *) &editorAddress, 
											0, 
											0, 
											&event);
  		
  		if ( error != noErr )
  			throw( error );
  					
  		//	Create an alias for this item								
  		error = ::FSNewAlias(NULL, &theSpec, &alias);

  		if ( error != noErr )
  			throw( error );

  		//	Stuff the alias in the AppleEvent as the 
  		//	direct parameter.
  		::HLock((Handle)alias);
  		StAEDescriptor	aliasObject(typeAlias, *alias, ::GetHandleSize((Handle)alias));
  		::AEPutParamDesc(&event, keyDirectObject, aliasObject);
  		::HUnlock((Handle)alias);

  		//	Send the open AppleEvent
  		UAppleEventsMgr::SendAppleEvent(event, true);
  		
  		//if (mFinderSwitch)
  		UProcessUtils::SendAppToFront(theFinderPSN);
  	}
  	
  	if ( alias != NULL )
  		::DisposeHandle( (Handle)alias );
  			
  	::AEDisposeDesc( &event );
  }
  else SendFinderOpenAE(1, &theSpec);
}

bool CBrowserPopup::ProcessMenuCommand(CommandT inCommand)
{
  this->OpenMenuItemSelection(mMenuID, inCommand - cmd_FirstPathComponent + 1);
  return true;
}

void CBrowserPopup::OpenMenuItemSelection(SInt16 menuSelect, SInt16 itemSelect)
{
	if(menuSelect != mMenuID || itemSelect < 1)
		return;
	
	SInt16 size = ::CountMenuItems(mMenu);
	CStr path;
	Str255 itemString;
	char cstr[256];

	path << kPathDelimiter;

	for(SInt16 i = size; i >= itemSelect; i--)
	{
		::GetMenuItemText(mMenu, i, itemString);
		p2cstrcpy(cstr, itemString);
		path << cstr;
		if(i != itemSelect)
			path << kPathDelimiter;
	}
	
	GUSIFileSpec dirSpec((const char *)path);
	if(dirSpec.Error() != noErr)
	{
		cvs_err("Unable to access '%s' (error %d)\n", (const char *)path, dirSpec.Error());
		return;
	}

	SendFinderOpenAE(dirSpec, true);
}

bool CBrowserPopup::BuildBrowserMenu(void)
{
	CStr curPath;
	CStr uppath, folder;
	uppath = (const char *)mPath;
	SInt16  itemIndex = 0;
	UInt32  cmd(cmd_FirstPathComponent);
	do
	{
		curPath = uppath;
		
		if(!SplitPath(curPath, uppath, folder))
		{
			cvs_err("MacCVS : Unknown error\n");
			return false;
		}
	
		LStr255 pstr((const char *)folder);
		::InsertMenuItem(mMenu, pstr, itemIndex++);
		::SetMenuItemCommandID(mMenu, itemIndex, cmd++);
	} while(!uppath.empty());

	return true;
}
