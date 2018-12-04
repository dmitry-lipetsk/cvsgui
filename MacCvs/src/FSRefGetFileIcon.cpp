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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2002
 */
						
#include <Icons.h>
#include <Finder.h>
#include <Folders.h>
#include <MacErrors.h>
#include <Resources.h>
#include <LowMem.h>

#include "FSRefGetFileIcon.h"
#include "SortList.h"

static CSortList<CIconRef> fEntries(20, CIconRef::Compare);

CIconRefData::CIconRefData(IconSuiteRef suite, IconRef icon) : m_ref(1),
	m_suite(suite), m_icon(icon)
{
}

CIconRefData::~CIconRefData()
{
	SetIconSuite(0L);
	SetIcon(0L);
}

void CIconRefData::SetIconSuite(IconSuiteRef suite)
{
	if(m_suite)
		::DisposeIconSuite(m_suite, true);
	m_suite = suite;
}

void CIconRefData::SetIcon(IconRef icon)
{
	if(m_icon)
		::ReleaseIconRef(m_icon);
	m_icon = icon;
}


OSStatus GetSysIcon(OSType inIconType, IconSelectorValue inIconSelector, CIconRef & outIcon) // static
{
  IconRef   iconRef;
  OSStatus  err;
  
  err = GetIconRef(kOnSystemDisk, kSystemIconsCreator, inIconType, &iconRef);
  if ( err != noErr ) return err;
  
	// search if the icon ref already exists in the cache
	CIconRefData *data = new CIconRefData(0L, iconRef);
	CIconRef dataRef(data);
	data->UnRef();

	int index;
	if(fEntries.InsideAndInsert(dataRef, &index))
	{
		outIcon = fEntries.Get(index);
		return noErr;
	}
	
	if ( inIconSelector != 0 )
	{
  	IconFamilyHandle iconFamily = 0L;
  	err = ::IconRefToIconFamily(iconRef, inIconSelector, &iconFamily);
  	if( err != noErr || iconFamily == 0L )
  		return err;

  	IconSuiteRef theSuite;
  	err = ::IconFamilyToIconSuite(iconFamily, inIconSelector, &theSuite);
  	::DisposeHandle((Handle)iconFamily);
  	if( err != noErr || theSuite == 0L )
  		return err;

  	data->SetIconSuite(theSuite);
  }
	
	outIcon = dataRef;

	return( err );
}

/*	------------------------------------------------------------------
	GetFileIcon		Given a file specification for a file, folder, or
					volume, create an appropriate icon suite
					and find its label color.
	------------------------------------------------------------------ */
OSStatus GetFileIcon(
/* --> */	FSRef				*thing,
/* --> */	IconSelectorValue	iconSelector,
/* <-- */	CIconRef &			res)
{
	FSCatalogInfo	info;
	OSStatus		err;
	FSCatalogInfoBitmap inWhichInfo = kFSCatInfoNodeFlags | kFSCatInfoVolume | kFSCatInfoFinderInfo |
		kIconServicesCatalogInfoMask;

	err = FSGetCatalogInfo( thing, inWhichInfo, &info, NULL, NULL, NULL );

	if( err != noErr )
		return err;
	
	IconRef outIconRef = 0L;
	SInt16 outLabel;
	err = GetIconRefFromFileInfo(thing, 0, NULL, inWhichInfo, &info, kIconServicesNormalUsageFlag,
		&outIconRef, &outLabel);
	if( err != noErr || outIconRef == 0L )
		return err;
	
	// search if the icon ref already exists in the cache
	CIconRefData *data = new CIconRefData(0L, outIconRef);
	CIconRef dataRef(data);
	data->UnRef();

	int index;
	if(fEntries.InsideAndInsert(dataRef, &index))
	{
		res = fEntries.Get(index);
		return noErr;
	}
	
	IconFamilyHandle iconFamily = 0L;
	err = ::IconRefToIconFamily(outIconRef, iconSelector, &iconFamily);
	if( err != noErr || iconFamily == 0L )
		return err;

	IconSuiteRef theSuite;
	err = ::IconFamilyToIconSuite(iconFamily, iconSelector, &theSuite);
	::DisposeHandle((Handle)iconFamily);
	if( err != noErr || theSuite == 0L )
		return err;

	data->SetIconSuite(theSuite);
	
	res = dataRef;

	return( err );
}

