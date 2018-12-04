#include "stdafx.h"

#include "UPseudoSupport.h"
#include "uconsole.h"
#include "udraw.h"
#include "umenu.h"

#include <stdio.h>
#include <stdlib.h>

#include <UGAColorRamp.h>
#include <LTableMonoGeometry.h>
#include <LTableMultiGeometry.h>
#include <LTableSingleSelector.h>
#include <LOutlineKeySelector.h>
#include <LOutlineTable.h>
#include <LTextTableView.h>
#include <LCellSizeToFit.h>
#include <LTableArrayStorage.h>
#include <LTextArray.h>
#include <LTableSingleRowSelector.h>
#include <LEditText.h>
#include <LPopupButton.h>
#include <LScrollBar.h>
#include <LPageController.h>
#include <LTabsControl.h>
#include <LSlider.h>
#include <LLittleArrows.h>
#include <LCheckBoxGroupBox.h>

#ifndef NEW
#	define NEW new
#endif

#ifdef __GNUC__
#   include <typeinfo>
#else
#   if __option(RTTI)
#	include <typeinfo>
#   else
#	error "RTTI option disabled -- must be enabled for LPaneTree to function"
#   endif
#endif

bool gTurnOffBroadcast = false;

class UPPMenu : public UMenu
{
public:
	UPPMenu();
	virtual ~UPPMenu();
	
	static int sPPMenuWidID;
};

class LPseudoDrawAttachment : public LAttachment
{
public:
	LPseudoDrawAttachment(int widid, int cmdid);
	virtual ~LPseudoDrawAttachment();

protected:
	virtual void	ExecuteSelf(
							MessageT		inMessage,
							void*			ioParam);
	
	int m_widid, m_cmdid;
};

static bool macpp_getcmdid(LPane *view, int & cmdid)
{
	PaneIDT atype = view->GetPaneID();
	const char *atypestr = (char *)&atype;
	if(atypestr[0] != 'p' && atypestr[1] != 's')
		return false;
	
	if(atypestr[2] < '0' || atypestr[2] > '9')
		return false;
	if(atypestr[3] < '0' || atypestr[3] > '9')
		return false;
	cmdid = (int)(atypestr[2] - '0') * 10 + (int)(atypestr[3] - '0') + EV_COMMAND_START;
	return true;
}

static bool macpp_getcmdid(PaneIDT atype, int & cmdid)
{
	const char *atypestr = (char *)&atype;
	if(atypestr[0] != 'p' && atypestr[1] != 's')
		return false;
	
	if(atypestr[2] < '0' || atypestr[2] > '9')
		return false;
	if(atypestr[3] < '0' || atypestr[3] > '9')
		return false;
	cmdid = (int)(atypestr[2] - '0') * 10 + (int)(atypestr[3] - '0') + EV_COMMAND_START;
	return true;
}

LPseudoWinAttachment *macpp_getattachwindow(UWidget *wid)
{
	LPane *win = (LPane *)wid->GetWidget(kUMainWidget);
	LWindow *wind = dynamic_cast<LWindow *>((LPane *)win);
	
	if(wind != 0L)
	{
		return macpp_getattachwindow(wind);
	}
	return 0L;
}

LPseudoWinAttachment *macpp_getattachwindow(LWindow *wind)
{
	if(wind != 0L)
	{
		const TArray<LAttachment*>*	theAttachments = wind->GetAttachmentsList();

		if (theAttachments != nil)
		{
			TArrayIterator<LAttachment*> iterate(*theAttachments);
			
			LAttachment*	theAttach;
			while (iterate.Next(theAttach))
			{
				LPseudoWinAttachment*	theWinAttach = dynamic_cast<LPseudoWinAttachment*>(theAttach);
				if (theWinAttach != nil)
				{
					return theWinAttach;
				}
			}
		}
	}
	return 0L;
}

void macpp_fecthview(int widid, LPane *pane)
{
	int cmdid;
	if(macpp_getcmdid(pane, cmdid))
	{
		UEventSendMessage(widid, EV_INIT_WIDGET, cmdid, pane);

		// look for any attachement we know of
		const TArray<LAttachment*>*	theAttachments = pane->GetAttachmentsList();
		if (theAttachments != nil)
		{
			TArrayIterator<LAttachment*> iterate(*theAttachments);

			LAttachment*	theAttach;
			while (iterate.Next(theAttach))
			{
				UPseudoPageHider *hider = dynamic_cast<UPseudoPageHider*>(theAttach);
				if(hider != 0L)
					hider->SetWidID(widid);
			}
		}

		// for any control, set the broadcast message to be the Pane ID
		LControl *control = dynamic_cast<LControl*>(pane);
		if(control != 0L && control->GetValueMessage() == 0)
		{
			control->SetValueMessage(pane->GetPaneID());
		}
		
		// idem
		LCheckBoxGroupBox *groupbox = dynamic_cast<LCheckBoxGroupBox*>(pane);
		if(groupbox != 0L && groupbox->GetValueMessage() == 0)
		{
			groupbox->SetValueMessage(pane->GetPaneID());
		}
		
		// set the widget id for some custom controls
		UPseudoPlaceHolder *holder = dynamic_cast<UPseudoPlaceHolder*>(pane);
		if(holder != 0L)
		{
			holder->SetWidID(widid);
			return; // don't fetch inside, it'll be owned by another pseudo widget
		}

		// this attachment let us fire a EV_CUSTOM_DRAW event when painting
		if(typeid(*pane) == typeid(LView))
		{
			LPseudoDrawAttachment *attach = NEW LPseudoDrawAttachment(widid, cmdid);
			pane->AddAttachment(attach);
		}
	}

	// menus with an unspecified menu id need to be created if we want to use
	// later on the EV_COMBO_APPEND message on it.
	LPopupButton *controler = dynamic_cast<LPopupButton*>(pane);
	if(controler != 0L)
	{
		if(controler->GetMenuID() == MENU_Unspecified && controler->GetMacMenuH() == 0L)
			controler->SetMacMenuH(::NewMenu(::UniqueID('MENU'), "\p"), true);
	}

	// recurse
	LView *view = dynamic_cast<LView*>(pane);
	if(view == 0L)
		return;

	TArrayIterator<LPane*> iterator(view->GetSubPanes());
	LPane	*theSub;
	while (iterator.Next(theSub))
	{
		macpp_fecthview(widid, theSub);
	}
}

void macpp_linkbroadcasters(LPane *pane, LListener *listener)
{
	LBroadcaster *broadcaster = dynamic_cast<LBroadcaster *>(pane);
	if(broadcaster != 0L && !broadcaster->HasListener(listener))
		broadcaster->AddListener(listener);
	
	LView *view = dynamic_cast<LView*>(pane);
	if(view == 0L)
		return;
	
	TArrayIterator<LPane*> iterator(view->GetSubPanes());
	LPane	*theSub;
	while (iterator.Next(theSub))
	{
		macpp_linkbroadcasters(theSub, listener);
	}
}

void macpp_preparetable(LOutlineTable *table)
{
	if(table->GetTableGeometry() != 0L)
		return;
	
	// try to guess a good height for the geometry
	int height = 20;
	TextTraitsH traitsH = UTextTraits::LoadTextTraits(table->GetTextTraits());
	if(traitsH != 0L)
		height = ((**traitsH).size * 6) / 4;

	LTableMultiGeometry *geom = NEW LTableMultiGeometry(table, 50, height);
	if(geom != 0L)
		table->SetTableGeometry(geom);
	table->SetTableSelector(NEW LTableSingleRowSelector( table ));
	table->AddAttachment(NEW LOutlineKeySelector(table, msg_AnyMessage));
	table->AddAttachment(NEW LKeyScrollAttachment(table));
}

void macpp_preparetable(void *win, LTextTableView *table)
{
	if(table->GetTableGeometry() != 0L)
		return;
	
	SDimension16	tableSize;
	table->SetCellSizer(NEW LCellSizeToFit());
	table->GetFrameSize(tableSize);
	table->SetTableGeometry(NEW LTableMonoGeometry(table, 
		tableSize.width/1 /* col width = width/num Cols */, 
		12  /* row height */));
	table->SetTableSelector(NEW LTableSingleRowSelector( table ));
	table->SetTableStorage(NEW LTableArrayStorage(table, (UInt32)0));
	table->SetDoubleClickMessage(UTextTableView_DoubleClick);
	table->SetSingleClickMessage(UTextTableView_SingleClick);
	table->SetCustomHilite(true); // cause there is a hilite bug if we don't
}

static LWindow *macpp_findwindow(LPane *pane)
{
	LPane *topView = 0L;
	while(pane != 0L)
	{
		topView = pane;
		pane = pane->GetSuperView();
	}
	return dynamic_cast<LWindow *>(topView);
}

LPseudoDrawAttachment::LPseudoDrawAttachment(int widid, int cmdid) : LAttachment(msg_DrawOrPrint),
	m_widid(widid), m_cmdid(cmdid)
{
}

LPseudoDrawAttachment::~LPseudoDrawAttachment()
{
}

void LPseudoDrawAttachment::ExecuteSelf(MessageT inMessage, void *ioParam)
{
	SetExecuteHost(true);
	LView *view = dynamic_cast<LView*>(GetOwnerHost());
	UEventSendMessage(m_widid, EV_CUSTOM_DRAW, m_cmdid, view);
}

LMainPseudoAttachment::LMainPseudoAttachment() : LAttachment(msg_AnyMessage)
{
	StartIdling(0.1);
}

LMainPseudoAttachment::~LMainPseudoAttachment()
{
}

void LMainPseudoAttachment::SpendTime(const EventRecord&	inMacEvent)
{
	::UEventGiveTime();
}

void LMainPseudoAttachment::ExecuteSelf(MessageT inMessage, void *ioParam)
{
	SetExecuteHost(true);
	
	if(UPPMenu::sPPMenuWidID == -1)
	{
		NEW UPPMenu();
	}
	
	if((inMessage > 0 && inMessage < 800) || inMessage >= EV_COMMAND_START)
	{
		int handledFlag;
		int res = UEventSendMessageExt(UPPMenu::sPPMenuWidID, EV_CMD,
			inMessage, 0L, &handledFlag);
		if(res == 0 && handledFlag)
		{
			SetExecuteHost(false);
			return;
		}
		else
			SetExecuteHost(true);
	}

	switch (inMessage)
	{
		case msg_CommandStatus:
		{
			SCommandStatus *status = static_cast<SCommandStatus*> (ioParam);
			CommandT cmdid = status->command;
			UPseudoCmdStatusWrapper wrapper;
			wrapper.outEnabled = status->enabled;
			wrapper.outUsesMark = status->usesMark;
			wrapper.outMark = status->mark;
			wrapper.outName = status->name;
			
			// first forward the command to the focused widget : if it doesn't want it
			// we give it to the next pseudo widget.
			int widFocus = UWidget::GetFocus();
			if(widFocus != -1)
			{
				UCmdUI pUI(cmdid);
				pUI.m_wrapper = &wrapper;
				UEventSendMessage(widFocus, EV_UPDTCMD, pUI.m_nID, &pUI);
				if(!pUI.CanContinue())
				{
					SetExecuteHost(false);
					break;
				}
			}

			STD map<int, UCmdTarget *>::const_iterator i;
			const STD map<int, UCmdTarget *> & targets = UEventGetTargets();
			for(i = targets.begin(); i != targets.end(); ++i)
			{
				int widid = (*i).first;
				if(widid == widFocus)
					continue;
				
				UCmdUI pUI(cmdid);
				pUI.m_wrapper = &wrapper;
				UEventSendMessage(widid, EV_UPDTCMD, pUI.m_nID, &pUI);
				if(!pUI.CanContinue())
					break;
			}
			
			if(i != targets.end())
				SetExecuteHost(false);
			break;
		}

		case msg_Event:
		{
			EventRecord *ev = static_cast<EventRecord*> (ioParam);

#if 0
			if(ev->what == activateEvt)
			{
				bool activate = (ev->modifiers & activeFlag) != 0;
				LWindow	*theWindow = LWindow::FetchWindowObject((WindowPtr)ev->message);
				if(theWindow != 0L)
				{
					UEventNotifyMessage(EV_UPDATEFOCUS, !activate, theWindow);
						// so the pseudo widget update its current widget.
						// it's important for the menu handling.
				}
			}
#endif
			if(ev->what == activateEvt ||
				ev->what == mouseDown ||
				ev->what == mouseUp ||
				ev->what == keyDown ||
				ev->what == keyUp ||
				ev->what == autoKey)
			{
				UWidget::DoValidUI(false);
				LCommander::SetUpdateCommandStatus(true);
			}
			break;
		}
	}
}

LPseudoWinAttachment::LPseudoWinAttachment(int widid) : LAttachment(msg_GrowZone), m_widid(widid)
{
	::SetRect(&m_lastRect, 0, 0, 0, 0);

	CheckWindowSize();
}

LPseudoWinAttachment::~LPseudoWinAttachment()
{
	UEventSendMessage(m_widid, EV_DESTROY, 0, 0L);
}

int LPseudoWinAttachment::GetPageValue(int cmdid) const
{
	std::map<int, int>::const_iterator i = m_PageValues.find(cmdid);
	if(i != m_PageValues.end())
		return (*i).second;
	return 1;
}

void LPseudoWinAttachment::SetPageValue(int cmdid, int value)
{
	std::map<int, int>::value_type val(cmdid, value);
	m_PageValues.erase(cmdid);
	m_PageValues.insert(val);
}

void LPseudoWinAttachment::ExecuteSelf(MessageT inMessage, void *ioParam)
{
	SetExecuteHost(true);
	
	// not used yet
}

void LPseudoWinAttachment::CheckWindowSize(void)
{
	LWindow *wind = dynamic_cast<LWindow*>(GetOwnerHost());
	if(wind != 0L)
	{
		Rect r;
		wind->GetGlobalBounds(r);
		if(!::EqualRect(&r, &m_lastRect))
		{
			m_lastRect = r;
			URect ur(r);
			UEventSendMessage(m_widid, EV_RESIZE, kUMainWidget, &ur);
		}
	}
}

int LPseudoWinAttachment::SearchWidID(LPane *pane)
{
	int res = m_widid;

	// first ask the cache
	std::map<LPane *, int>::iterator i = m_CacheWidgets.find(pane);
	if(i != m_CacheWidgets.end())
		return (*i).second;
	
	// then look if it belongs to a tab
	UCmdTarget *target = UEventQueryWidget(m_widid);
	UWidget *wid = UDYNAMIC_CAST(UWidget, target);
	if(wid == 0L)
		return res;
	
	std::map<int, UWidget *>::iterator t;
	for(t = wid->m_tabs.begin(); t != wid->m_tabs.end(); ++t)
	{
		UWidget *tab = (*t).second;
		std::map<int, void *>::iterator s;
		for(s = tab->m_widgets.begin(); s != tab->m_widgets.end(); ++s)
		{
			if((*s).second == (void *)pane)
			{
				res = tab->GetWidID();
				break; // it's a tab widget
			}
		}
	}

	// so we guess it's in the main widget !
	std::map<LPane *, int>::value_type val(pane, res);
	m_CacheWidgets.insert(val);
	
	return res;
}

void LPseudoWinAttachment::InvalidCache(UWidget *tab_wid)
{
	std::map<int, void *>::iterator s;
	for(s = tab_wid->m_widgets.begin(); s != tab_wid->m_widgets.end(); ++s)
	{
		std::map<LPane *, int>::iterator i = m_CacheWidgets.find((LPane *)(*s).second);
		if(i != m_CacheWidgets.end())
			m_CacheWidgets.erase(i);
	}
}


void LPseudoWinAttachment::ListenToMessage(
							MessageT		inMessage,
							void*			ioParam)
{
	int cmdid;
	LPane *pane;
	
	if(gTurnOffBroadcast)
		return;
	
	if(macpp_getcmdid(inMessage, cmdid))
	{
		LWindow *wind = dynamic_cast<LWindow*>(GetOwnerHost());
		if(wind != 0L)
		{
			pane = wind->FindPaneByID(inMessage);
		
			if(pane != 0L)
			{
				int widid = SearchWidID(pane);
				
				LEditText *edit;
				LControl *control;
				LMenuController *popup;
				LScrollBar *bar;
				LSlider *slider;
				LLittleArrows *arrows;
				LCheckBoxGroupBox *groupbox;
				
				if((edit = dynamic_cast<LEditText*>(pane)) != 0L)
					UEventSendMessage(widid, EV_EDITCHANGE, cmdid, 0L);
				else if((popup = dynamic_cast<LMenuController*>(pane)) != 0L &&
					popup->GetMacMenuH() != 0L)
				{
					SInt16 item = popup->GetCurrentMenuItem();
					LStr255 pstr;
					if(item > 0)
						popup->GetMenuItemText(item, pstr);
					UStr content(pstr);
					UEventSendMessage(widid, EV_COMBO_SEL, UMAKEINT(cmdid, item - 1),
						(void *)(const char *)content);
				}
				else if((bar = dynamic_cast<LScrollBar*>(pane)) != 0L)
					UEventSendMessage(widid, EV_SCROLLCHANGE, cmdid, (void *)bar->GetValue());
				else if((slider = dynamic_cast<LSlider*>(pane)) != 0L)
					UEventSendMessage(widid, EV_SCROLLCHANGE, cmdid, (void *)slider->GetValue());
				else if((arrows = dynamic_cast<LLittleArrows*>(pane)) != 0L)
					UEventSendMessage(widid, EV_SCROLLCHANGE, cmdid, (void *)arrows->GetValue());
				else if((control = dynamic_cast<LControl*>(pane)) != 0L)
					UEventSendMessage(widid, EV_CMD, cmdid, 0L);
				else if((groupbox = dynamic_cast<LCheckBoxGroupBox*>(pane)) != 0L)
					UEventSendMessage(widid, EV_CMD, cmdid, 0L);
			}
		}
	}
	else switch (inMessage)
	{
		case msg_ScrollAction:
		{
			LScrollBar::SScrollMessage *message = static_cast<LScrollBar::SScrollMessage *>(ioParam);
			LScrollBar*	scrollBar = message->scrollBar;
			int fPageStep = 1;
			if(macpp_getcmdid(scrollBar->GetPaneID(), cmdid))
				fPageStep = GetPageValue(cmdid);
			int fButtonStep = 1;
			switch(message->hotSpot)
			{
			case kControlPageUpPart:
				scrollBar->SetValue(scrollBar->GetValue() - fPageStep);
				break;
			case kControlUpButtonPart:
				scrollBar->SetValue(scrollBar->GetValue() - fButtonStep);
				break;
			case kControlPageDownPart:
				scrollBar->SetValue(scrollBar->GetValue() + fPageStep);
				break;
			case kControlDownButtonPart:
				scrollBar->SetValue(scrollBar->GetValue() + fButtonStep);
				break;
			}
			break;
		}
		case UTextTableView_SingleClick:
		case UTextTableView_DoubleClick:
			pane = static_cast<LPane *>(ioParam);
			
			if(pane != 0L && macpp_getcmdid(pane, cmdid))
			{
				UStr name;
				int entry = 0;
				LTextTableView *table = dynamic_cast<LTextTableView *>(pane);
				if(table != 0L)
				{
					STableCell cell = table->GetFirstSelectedCell();
					if(!cell.IsNullCell())
					{
						char tmp[255];
						UInt32 ioDataSize = 255;
						table->GetCellData(cell, tmp, ioDataSize);
						name.set(tmp, ioDataSize);
						entry = cell.row;
					}
				}
				
				UEventSendMessage(SearchWidID(pane),
					inMessage == UTextTableView_SingleClick ?
					EV_LIST_SELECTING : EV_LIST_DBLCLICK, UMAKEINT(cmdid, entry - 1),
					(void *)(const char *)name);
			}
			break;
	}
}

CPseudoItem::CPseudoItem(int widid, int cmdid) : m_widid(widid), m_cmdid(cmdid), m_data(0L)
{
}

CPseudoItem::~CPseudoItem()
{
}

// this is the routine called to know what to draw within the
// table cell. See the comments in LOutlineItem.cp for more info.
void
CPseudoItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	std::map<int, UStr>::iterator i = m_texts.find(inCell.col);
	if(i != m_texts.end())
	{
		ioDrawContents.outShowSelection = true;
		LStr255 pstr((*i).second);
		LString::CopyPStr(pstr, ioDrawContents.outTextString);
	}
}

// just to be cute, we'll draw an adornment (again, see the LOutlineItem.cp
// comments for more information). We'll draw a groovy gray background
void
CPseudoItem::DrawRowAdornments(
	const Rect&		inLocalRowRect )
{
	ShadeRow(UGAColorRamp::GetColor(colorRamp_White), inLocalRowRect);
	RGBColor c = UGAColorRamp::GetColor(colorRamp_Gray2);
	RGBForeColor(&c);
	MoveTo(inLocalRowRect.right, inLocalRowRect.top);
	LineTo(inLocalRowRect.right, inLocalRowRect.bottom);
}

void CPseudoItem::DrawCell(const STableCell & inCell, const Rect & inLocalCellRect)
{
	LOutlineItem::DrawCell(inCell, inLocalCellRect);
	
	RGBColor c = UGAColorRamp::GetColor(colorRamp_Gray2);
	RGBForeColor(&c);
	MoveTo(inLocalCellRect.right - 1, inLocalCellRect.top);
	LineTo(inLocalCellRect.right - 1, inLocalCellRect.bottom);
}

void CPseudoItem::SingleClick(
								const STableCell&			inCell,
								const SMouseDownEvent&		inMouseDown,
								const SOutlineDrawContents&	inDrawContents,
								Boolean						inHitText)
{
	UStr name;
	std::map<int, UStr>::iterator i = m_texts.find(inCell.col - 1);
	if(i != m_texts.end())
	{
		name = (*i).second;
	}
	UEventSendMessage(m_widid,
		EV_LIST_SELECTING, UMAKEINT(m_cmdid, inCell.row - 1),
		(void *)(const char *)name);
	
	LOutlineItem::SingleClick(inCell, inMouseDown, inDrawContents, inHitText);
}

void
CPseudoItem::DoubleClick(
	const STableCell&			inCell,
	const SMouseDownEvent&		inMouseDown,
	const SOutlineDrawContents&	inDrawContents,
	Boolean						inHitText)
{
	UStr name;
	std::map<int, UStr>::iterator i = m_texts.find(inCell.col - 1);
	if(i != m_texts.end())
	{
		name = (*i).second;
	}
	UEventSendMessage(m_widid,
		EV_LIST_DBLCLICK, UMAKEINT(m_cmdid, inCell.row - 1),
		(void *)(const char *)name);
	
	LOutlineItem::DoubleClick(inCell, inMouseDown, inDrawContents, inHitText);
}

int UPPMenu::sPPMenuWidID = -1;

UPPMenu::UPPMenu() : UMenu(::UEventGetWidID())
{
	sPPMenuWidID = GetWidID();
}

UPPMenu::~UPPMenu()
{
}

UPseudoRotButton::UPseudoRotButton(LStream *inStream, ClassIDT inImpID) : LBevelButton(inStream, inImpID), fCurrentIcon(0)
{
	ReadIconsID(inStream, fIconSuite);
	
	if(fIconSuite.size() != 0)
		DoRotate(0, false);
}

UPseudoRotButton::~UPseudoRotButton()
{
}

void UPseudoRotButton::ReadIconsID(LStream *inStream, std::vector<short> & res)
{
	LStr255 pstr;
	
	inStream->ReadPString(pstr);
	UStr str(pstr);
	const char *ptr = str;
	while(ptr != 0L)
	{
		int v;
		const char *tmp = strchr(ptr, ',');
		if(tmp != 0L)
		{
			UStr stmp;
			size_t offset = tmp - ptr;
			stmp.set(ptr, offset);
			ptr += offset + 1;
			if(sscanf(stmp, "%d", &v) == 1)
				res.push_back(v);
		}
		else
		{
			if(sscanf(ptr, "%d", &v) == 1)
				res.push_back(v);
			ptr = 0L;
		}
	}
}

void UPseudoRotButton::DoRotate(int icon, bool refresh)
{
	if(fCurrentIcon != icon && icon >= 0 && icon < fIconSuite.size())
	{
		fCurrentIcon = icon;
		ControlButtonContentInfo info;
		GetContentInfo(info);
		if(info.contentType == kControlContentCIconRes ||
			info.contentType == kControlContentIconSuiteRes ||
			info.contentType == kControlContentICONRes ||
			info.contentType == kControlContentPictRes)
		{
			info.u.resID = fIconSuite[icon];
			SetContentInfo(info);
		}
		if(refresh)
			Refresh();
	}
}

UPseudoPlaceHolder::UPseudoPlaceHolder(LStream *inStream) : LPlaceHolder(inStream),
	fCurrentView(0L), fWidid(-1)
{
	LStr255 pstr;
	inStream->ReadPString(pstr);
	*inStream >> fController;
	
	UStr str(pstr);
	const char *ptr = str;
	while(ptr != 0L)
	{
		int v;
		const char *tmp = strchr(ptr, ',');
		if(tmp != 0L)
		{
			UStr stmp;
			size_t offset = tmp - ptr;
			stmp.set(ptr, offset);
			ptr += offset + 1;
			if(sscanf(stmp, "%d", &v) == 1)
				fViewSuite.push_back(v);
		}
		else
		{
			if(sscanf(ptr, "%d", &v) == 1)
				fViewSuite.push_back(v);
			ptr = 0L;
		}
	}
}

UPseudoPlaceHolder::~UPseudoPlaceHolder()
{
}

void UPseudoPlaceHolder::DoRotate(int entry)
{
	int cmdid;
	if(macpp_getcmdid(GetPaneID(), cmdid) && entry >= 0 && entry < fViewSuite.size())
	{
		LWindow *win = macpp_findwindow(this);
		if(win == 0L)
			return;

		LView* samplePanel = UReanimator::CreateView(fViewSuite[entry],	this, win);
		if(samplePanel == 0L)
			return;
		
		InstallOccupant	(samplePanel);
		
		LView *oldView = fCurrentView;
		
		if (fCurrentView != 0L)
		{
			fCurrentView->Hide ();
			fCurrentView = nil;
		}

		Refresh();
		fCurrentView = samplePanel;
		
		if(fWidid != -1 && !gTurnOffBroadcast)
			UEventSendMessage(fWidid, EV_PAGE_CHANGED, UMAKEINT(cmdid, entry), 0L);

		if (oldView != 0L)
		{
			delete oldView;
		}

	}
}

void UPseudoPlaceHolder::ListenToMessage(MessageT inMessage, void * ioParam)
{
	if(inMessage == fController)
	{
		SInt32 currPageIndex = *static_cast<SInt32 *>(ioParam);
		DoRotate(currPageIndex - 1);
	}
}

void UPseudoPlaceHolder::FinishCreateSelf(void)
{
	DoRotate(0);

	LWindow *win = macpp_findwindow(this);
	if(win == 0L)
		return;

	LPageController *controller = dynamic_cast<LPageController*>(win->FindPaneByID(fController));
	if (controller != 0L)
	{
		controller->StartBroadcasting ();
		controller->AddListener (this);
	}
	LTabsControl *controller2 = dynamic_cast<LTabsControl*>(win->FindPaneByID(fController));
	if (controller2 != 0L)
	{
		controller2->StartBroadcasting ();
		controller2->AddListener (this);
	}
}

UPseudoPageHider::UPseudoPageHider(LStream *inStream) : LAttachment(inStream),
	fWidid(-1), fCurSel(-1), fPageCtrl(0L)
{
	SetMessage(msg_FinishCreate);
	
	LStr255 pstr;
	inStream->ReadPString(pstr);
	
	UStr str(pstr);
	const char *ptr = str;
	while(ptr != 0L)
	{
		PaneIDT paneID;
		const char *tmp = strchr(ptr, ',');
		if(tmp != 0L)
		{
			UStr stmp;
			size_t offset = tmp - ptr;
			stmp.set(ptr, offset);
			ptr += offset + 1;
			if(stmp.length() == 4)
			{
				paneID = *(PaneIDT *)(char *)stmp.c_str();
				fViewSuite.push_back(paneID);
			}
		}
		else
		{
			if(strlen(ptr) == 4)
			{
				paneID = *(PaneIDT *)ptr;
				fViewSuite.push_back(paneID);
			}
			ptr = 0L;
		}
	}
}

void	UPseudoPageHider::ExecuteSelf(
						MessageT		inMessage,
						void*			ioParam)
{
	if(inMessage == msg_FinishCreate)
	{
		LPane *pane = reinterpret_cast<LPane*>(ioParam);
		fPageCtrl = dynamic_cast<LPageController*>(pane);
		if(fPageCtrl == 0L)
		{
			UAppConsole("The UPseudoPageHider attachement does not have any LPageController to attach to.");
		}
		else
		{
			// add ourself to the broadcast so we receive the changes
			fPageCtrl->StartBroadcasting ();
			fPageCtrl->AddListener (this);
		}
	}
}

UPseudoPageHider::~UPseudoPageHider()
{
}

void UPseudoPageHider::DoRotate(int entry)
{
	if(fPageCtrl == 0L)
		return;
	
	int cmdid;
	if(macpp_getcmdid(fPageCtrl->GetPaneID(), cmdid) && entry >= 0 && entry < fViewSuite.size() &&
		entry != fCurSel)
	{
		fCurSel = entry;
		
		LWindow *win = macpp_findwindow(fPageCtrl);
		if(win == 0L)
			return;

		LPane* samplePanel;
		std::vector<PaneIDT>::iterator i;
		for(i = fViewSuite.begin(); i != fViewSuite.end(); ++i)
		{
			samplePanel = win->FindPaneByID(*i);
			if(samplePanel != 0L)
				samplePanel->Hide();
		}
		
		samplePanel = win->FindPaneByID(fViewSuite[entry]);
		if(samplePanel == 0L)
			return;
		
		samplePanel->Show();
		
		if(fWidid != -1)
			UEventSendMessage(fWidid, EV_CMD, cmdid, (void *)entry);
	}
}

void UPseudoPageHider::ListenToMessage(MessageT inMessage, void * ioParam)
{
	if(fPageCtrl != 0L && inMessage == fPageCtrl->GetPaneID())
	{
		SInt32 currPageIndex = *static_cast<SInt32 *>(ioParam);
		DoRotate(currPageIndex - 1);
	}
}

UPseudoFakePane::UPseudoFakePane(const SPaneInfo	&inPaneInfo, LPseudoWinAttachment *inAttach) :
	LPane(inPaneInfo), m_Attach(inAttach)
{
}

UPseudoFakePane::~UPseudoFakePane()
{
}

void UPseudoFakePane::ActivateSelf()
{
	UEventNotifyMessage(EV_UPDATEFOCUS, 0, dynamic_cast<LWindow *>(GetSuperView()));
}

void UPseudoFakePane::DeactivateSelf()
{
	UEventNotifyMessage(EV_UPDATEFOCUS, 1, dynamic_cast<LWindow *>(GetSuperView()));
}

void UPseudoFakePane::AdaptToSuperFrameSize(
							SInt32		inSurrWidthDelta,
							SInt32		inSurrHeightDelta,
							Boolean		inRefresh)
{
	LPane::AdaptToSuperFrameSize(inSurrWidthDelta, inSurrHeightDelta, inRefresh);
	
	m_Attach->CheckWindowSize();
}


class CDummyClass
{
public:
	CDummyClass()
	{
		RegisterClass_(UPseudoRotButton);
		RegisterClass_(UPseudoPlaceHolder);
		RegisterClass_(UPseudoPageHider);
	}
	
} sDummyStarter;
