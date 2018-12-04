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
 * LPopupFiller.h --- fills an associated LEditText
 */

#ifndef LPOPUP_FILLER_H
#define LPOPUP_FILLER_H
#pragma once

#include <LPopupButton.h>
#include "MultiString.h"

class MultiFiles;

// used optionnaly to customize the filling/picking
class LFillerWrapper
{
public:
	LFillerWrapper(LMenuController *menu) : fMenu(menu) {}
	
	virtual void OnAddingItem(CStr & str);
		// default fills the menu with the string
	
	virtual bool OnSelectingItem(SInt32 inItem, CStr & str);
		// default fills the string with the menu item title.
		// return false to skip the insertion
protected:
	LMenuController *fMenu;
};

// add a 'Browse...' entry to the poup filler and route it
// to the tag parser
class LTagWrapper : public LFillerWrapper
{
public:
	LTagWrapper(LMenuController *menu, const MultiFiles *mf) :
		LFillerWrapper(menu), mMf(mf), fFirstTime(true)
	{
	}
	
	virtual void OnAddingItem(CStr & str);
	virtual bool OnSelectingItem(SInt32 inItem, CStr & str);
protected:
	const MultiFiles *mMf;
	SInt32 mItem;
	bool fFirstTime;
};

// add a 'Browse...' entry to the poup filler and route it
// to the module parser
class LModuleWrapper : public LFillerWrapper
{
public:
	LModuleWrapper(LMenuController *menu, const MultiFiles *mf) :
		LFillerWrapper(menu), mMf(mf), fFirstTime(true)
	{
	}
	
	virtual void OnAddingItem(CStr & str);
	virtual bool OnSelectingItem(SInt32 inItem, CStr & str);
protected:
	const MultiFiles *mMf;
	SInt32 mItem;
	bool fFirstTime;
};

// this class fills an edit text with the content of a popup-up
class	LPopupFiller : public LPopupButton
{
public:
	enum {class_ID = 'POPF'};

	LPopupFiller(LStream *inStream);
	virtual ~LPopupFiller();

	void DoDataExchange(CMString & mstr, bool putValue, LFillerWrapper *wrapper = 0L);
		// - allows to fill the filler with a persistent multi-string.
		// - the wrapper has to be allocated by new (it'll be deleted)
		// and has to be valid only when putValue = true
		
	virtual void SetValue(SInt32 inItem);
protected:
	virtual void FinishCreateSelf();

	LFillerWrapper mStdWrapper;
	SInt32 mNoneItem;
	LFillerWrapper *mWrapper;
	PaneIDT mEditText;
	bool mFillit;
};

#endif /* LPOPUP_FILLER_H */
