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
 * LPopupFiller.cpp --- fills an associated LEditText
 */

#include <LTextEditView.h>
#include <LEditText.h>
#include "LPopupFiller.h"
#include "ItemListDlg.h"
#include "MacCvsConstant.h"
#include "MacMisc.h"

void LFillerWrapper::OnAddingItem(CStr & str)
{
	fMenu->InsertMenuItem(UTF8_to_LString(str), max_Int16);
}

bool LFillerWrapper::OnSelectingItem(SInt32 inItem, CStr & str)
{
	LStr255 mcontent;	
	fMenu->GetMenuItemText(inItem, mcontent);
	str = LString_to_UTF8(mcontent);
	
	return true;
}

void LTagWrapper::OnAddingItem(CStr & str)
{
	if(fFirstTime)
	{
		fMenu->InsertMenuItem("\pBrowse...", max_Int16);
		mItem = ::CountMenuItems(fMenu->GetMacMenuH());
		fMenu->InsertMenuItem("\p(-", max_Int16, false);
		fFirstTime = false;
	}
	
	LFillerWrapper::OnAddingItem(str);
}
	
bool LTagWrapper::OnSelectingItem(SInt32 inItem, CStr & str)
{
	if(inItem == mItem)
		return CompatGetTagListItem(mMf, str);
	
	return LFillerWrapper::OnSelectingItem(inItem, str);
}

void LModuleWrapper::OnAddingItem(CStr & str)
{
	if(fFirstTime)
	{
		fMenu->InsertMenuItem("\pBrowse...", max_Int16);
		mItem = ::CountMenuItems(fMenu->GetMacMenuH());
		fMenu->InsertMenuItem("\p(-", max_Int16, false);
		fFirstTime = false;
	}
	
	LFillerWrapper::OnAddingItem(str);
}
	
bool LModuleWrapper::OnSelectingItem(SInt32 inItem, CStr & str)
{
	if(inItem == mItem)
		return CompatGetModuleListItem(mMf, str);
	
	return LFillerWrapper::OnSelectingItem(inItem, str);
}

LPopupFiller::LPopupFiller(LStream *inStream) : mFillit(false), LPopupButton(inStream),
	mStdWrapper(this), mWrapper(&mStdWrapper)
{
	inStream->ReadData(&mEditText, sizeof(PaneIDT));
}

LPopupFiller::~LPopupFiller()
{
	if(mWrapper != &mStdWrapper)
		delete mWrapper;
}

void LPopupFiller::FinishCreateSelf()
{
}

void LPopupFiller::SetValue(SInt32 inItem)
{
	LPopupButton::SetValue(inItem);
	
	if(!mFillit || inItem < 1)
		return;
	
	if(inItem == mNoneItem)
		return;
	
	LView *superView = GetSuperView();
	if(superView == 0L)
	{
		Assert_(0);
		return;
	}

	CStr mcontent;	
	if(!mWrapper->OnSelectingItem(inItem, mcontent))
		return;
	
	LEditText *edit = dynamic_cast<LEditText*>
				(superView->FindPaneByID(mEditText));
	if(edit != 0L)
	{
	  CStr    sysEncodedContent(UTF8_to_SysEncoding(mcontent, true));
		edit->SetText((Ptr)(const char *)sysEncodedContent, sysEncodedContent.length());
		edit->UserChangedText();
	}
	else
	{
		LTextEditView *textedit = dynamic_cast<LTextEditView*>
					(superView->FindPaneByID(mEditText));
		if(textedit != 0L)
		{
			CStr sysEncodedContent(UTF8_to_SysEncoding(mcontent, true));
			textedit->SetTextPtr((Ptr)(const char *)sysEncodedContent, sysEncodedContent.length());
			textedit->UserChangedText();
		}
		else
		{
			Assert_(0);
			return;
		}
	}
}

void LPopupFiller::DoDataExchange(CMString & mstr, bool putValue, LFillerWrapper *wrapper)
{
	if(putValue)
	{
		mWrapper = wrapper != 0L ? wrapper : &mStdWrapper;
		mNoneItem = 1;
	
		DeleteAllMenuItems();
		const std::vector<CStr> & list = mstr.GetList();
		std::vector<CStr>::const_iterator i;
		int index;
		InsertMenuItem("\pNone", max_Int16);
		for(index = 1, i = list.begin(); i != list.end(); ++i, ++index)
		{
			CStr addStr(*i);
			mWrapper->OnAddingItem(addStr);
		}

		mFillit = true;
	}
	else
	{
		LView *superView = GetSuperView();
		if(superView == 0L)
		{
			Assert_(0);
			return;
		}

		LEditText *edit = dynamic_cast<LEditText*>
					(superView->FindPaneByID(mEditText));
		if(edit != 0L)
		{
			char str[255];
			Size len;
			edit->GetText(str, 254, &len);
			if(len > 0)
			{
				str[len] = '\0';
				mstr.Insert(SysEncoding_to_UTF8(str));
			}
		}
		else
		{
			LTextEditView *textedit = dynamic_cast<LTextEditView*>
						(superView->FindPaneByID(mEditText));
			if(textedit != 0L)
			{
				Handle hdl = textedit->GetTextHandle();
				Assert_(hdl != 0L);
				if(GetHandleSize(hdl) > 0)
				{
					CStr str;
					str.set(*hdl, GetHandleSize(hdl));
					mstr.Insert(SysEncoding_to_UTF8(str));
				}
			}
			else
			{
				Assert_(0);
				return;
			}
		}
	}
}
