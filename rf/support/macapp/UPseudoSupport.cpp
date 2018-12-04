#include "stdafx.h"

#include <stdlib.h>
#include <Errors.h>

#include "UPseudoSupport.h"
#include "UPopup.h"
#include "UDialogTEView.h"
#include "UEditText.h"
#include "UWindow.h"
#include "USlider.h"

#include "uconsole.h"

#define MA_CHECK(cond) Assertion_AC(cond, #cond)

extern "C" char *strdup(const char *str)
{
	if(str == 0L)
		return 0L;
		
	size_t len = strlen(str);
	char *res = (char *)malloc((len + 1) * sizeof(char));
	if(res == 0L)
		throw CException_AC(mFulErr);
	strcpy(res, str);
	return res;
}

#undef Inherited
#define Inherited TBehavior

MA_DEFINE_CLASS_M1(TPseudoWinBehavior, Inherited);

TPseudoWinBehavior::TPseudoWinBehavior() : TBehavior(kPseudoWinBehavior), fWidID(-1)
{
}

TPseudoWinBehavior::~TPseudoWinBehavior()
{
}

void TPseudoWinBehavior::DoEvent(EventNumber eventNumber, TEventHandler* source, TEvent* event)
{
	// find out the command who fired the event (if any)
	int cmdid = -1;
	
	// the typing event is originated by a sub-view of an TEditText field
	TEventHandler* ourSource = source;
	if(eventNumber == cTyping && MA_MEMBER(ourSource, TDialogTEView))
	{
		ourSource = ((TDialogTEView *)ourSource)->fEditText;
	}
	
	if(fWidID != -1)
		cmdid = UEventSendMessage(fWidID, EV_FETCHCMD, 0, ourSource);
	
	if(cmdid != -1) switch(eventNumber)
	{
		case mBecameTarget:
		case mResignedTarget:
			UEventNotifyMessage(EV_UPDATEFOCUS, eventNumber == mResignedTarget, ourSource);
			break;
		case mButtonHit:
		case mRadioHit:
		case mCheckBoxHit:
			UEventSendMessage(fWidID, EV_CMD, cmdid, 0L);
			break;
		case mHScrollBarHit:
		case mVScrollBarHit:
			if(MA_MEMBER(ourSource, TSlider) || MA_MEMBER(ourSource, TScrollBar))
			{
				TCtlMgr *slider = (TCtlMgr *)ourSource;
				UEventSendMessage(fWidID, EV_SCROLLCHANGE, cmdid, (void *)slider->GetValue());
			}
			break;
		case mPopupHit:
			if(MA_MEMBER(ourSource, TPopup))
			{
				TPopup *popup = (TPopup *)ourSource;
				short cur = popup->GetCurrentItem();
				CStr255_AC pstr = popup->GetItemText(cur);
				UPStr content(pstr);
				UEventSendMessage(fWidID, EV_COMBO_SEL, UMAKEINT(cmdid, cur - 1),
					(void *)(const char *)content);
			}
			break;
		case mControlHit:
			if(MA_MEMBER(ourSource, TSlider))
			{
				TSlider *slider = (TSlider *)ourSource;
				UEventSendMessage(fWidID, EV_SCROLLCHANGE, cmdid, (void *)slider->GetValue());
			}
			break;
		case cTyping:
			if(MA_MEMBER(ourSource, TEditText))
			{
				UEventSendMessage(fWidID, EV_EDITCHANGE, cmdid, 0L);
			}
			break;
		case TPseudoTextListView::kSelectionChanged:
		case TPseudoTextListView::kMultiClickSelection:
			if(MA_MEMBER(ourSource, TPseudoTextListView))
			{
				TPseudoTextListView *txt = (TPseudoTextListView *)ourSource;
				short entry = txt->FirstSelectedItem();
				CChar255_AC name;
				if(entry > 0)
					name = txt->GetItemText(entry);
					
				UEventSendMessage(fWidID,
					eventNumber == TPseudoTextListView::kSelectionChanged ?
					EV_LIST_SELECTING : EV_LIST_DBLCLICK, UMAKEINT(cmdid, entry - 1),
					(void *)(const char *)name);
			}
			break;
	}
	
	Inherited::DoEvent(eventNumber, source, event);
}


#undef Inherited
#define Inherited TTextListView
MA_DEFINE_CLASS_M0(TPseudoTextListView);

TPseudoTextListView::TPseudoTextListView()
{
}



TPseudoTextListView::~TPseudoTextListView()
{
}


IDType TPseudoTextListView::GetStandardSignature(void) const
{
	return TPseudoTextListView::kTextStringListViewSignature;
}

CStr255_AC TPseudoTextListView::GetItemText(GridCoordinate anItem) const
{
	MA_CHECK(this->fNumOfRows == this->fStringContainer.size());
	MA_CHECK(1 <= anItem && this->fStringContainer.size() >= anItem);

	CStr255_AC aString = this->fStringContainer[anItem - 1];
	return aString;
}

void TPseudoTextListView::InsStringBefore(short anItem, const char * string)
{
	MA_CHECK(1 <= anItem && this->fStringContainer.size() >= anItem - 1);

	std::vector<CStr255_AC>::iterator iter = this->fStringContainer.end();

	try
	{
		iter = this->fStringContainer.insert(
					this->fStringContainer.begin() + (anItem - 1), string);
	}
	catch(CException_AC& theException)
	{
		if (iter != this->fStringContainer.end())
		{
			this->fStringContainer.erase(iter);
				// Put it back like it was.  This will maintain internal consistency.
		}
		throw;
	}

	this->InsItemBefore(anItem, 1);
}

void TPseudoTextListView::InsStringFirst(const char * string)
{
	this->InsStringBefore(1, string);
}

void TPseudoTextListView::InsStringLast(const char * string)
{
	this->InsStringBefore(this->fNumOfRows + 1, string);
}

void TPseudoTextListView::DelItemAt(GridCoordinate anItem, GridCoordinate numOfItems)
{
	this->fStringContainer.erase(
			this->fStringContainer.begin() + (anItem - 1),
			this->fStringContainer.begin() + (anItem - 1 + numOfItems));

	this->Inherited::DelItemAt(anItem, numOfItems);
}

void TPseudoTextListView::RemoveAll(void)
{
	this->DelItemFirst(this->fNumOfRows);
}

bool TPseudoTextListView::DoMouseCommand_SelectCells(GridCell whichCell, TToolboxEvent * event)
{
	MA_CHECK(nil != event);

	bool selectionChanged = false;
		// Will be true if the selection changed.

	if (this->CanSelectCell(whichCell))
	{
		bool shifted = event->IsShiftKeyPressed();
		bool commandKey = event->IsCommandKeyPressed();

		if (commandKey)
		{
			this->SelectCell(whichCell, !this->fSingleSelection,
					kHighlight, !this->IsCellSelected(whichCell));

			selectionChanged = true;
		}
		else if (shifted && !this->fSingleSelection)
		{
			GridCell runner = whichCell;

			if (this->IsCellSelected(whichCell))
			{
				for ( ; runner.v <= this->fNumOfRows; runner.v++)
				{
					if (!this->IsCellSelected(runner))
					{
						break;
					}

					this->SelectCell(runner, kExtend, kHighlight, kDeSelect);
				}
			}
			else
			{
				for ( ; runner.v > 0; runner.v--)
				{
					if (this->IsCellSelected(runner))
					{
						break;
					}

					this->SelectCell(runner, kExtend, kHighlight, kSelect);
				}
			}

			selectionChanged = true;
		}
		else if (!this->IsCellSelected(whichCell))
		{
			this->SelectCell(whichCell, kDontExtend, kHighlight, kSelect);

			selectionChanged = true;
		}
	}

	return selectionChanged;
}

void TPseudoTextListView::DoMouseCommand(CViewPoint & theMouse, TToolboxEvent * event, CPoint_AC)
{
	GridCell aCell;

	if (badChoice != this->IdentifyPoint(theMouse, aCell))
	{
		this->DoMouseCommand_SelectCells(aCell, event);
			// First be sure to select/deselect the cell they clicked on.

		if (event->GetClickCount() > 1)
		{
			this->HandleEvent(kMultiClickSelection, this, event);
		}
	}
}

void TPseudoTextListView::DoKeyEvent(TToolboxEvent* event)// override 
{
	Boolean handledCharacter = false;

	if (this->IsEnabled() && this->Focus())		// if view is not enabled then we don't take ANY keystrokes
	{
		GridCell selected;
		unsigned char ch = event->GetText()[1];
		
		if (ch == chUp)
		{
			selected = this->FirstSelectedCell();
			selected.v -= 1;
			 
			this->DoMouseCommand_SelectCells(selected, event);
			
			handledCharacter = true;
		}
		else if (ch == chDown)
		{
			selected = this->LastSelectedCell();
			selected.v += 1;
			 
			this->DoMouseCommand_SelectCells(selected, event);
			
			handledCharacter = true;
		}
	}

	if (!handledCharacter)
		Inherited::DoKeyEvent(event);
}

void TPseudoTextListView::SetSelection(const TGridViewDesignator* cellsToSelect,
						bool extendSelection, bool highlight, bool select)
{
	this->Inherited::SetSelection(cellsToSelect, extendSelection, highlight, select);

	this->Changed(kSelectionChanged, this);
	this->HandleEvent(kSelectionChanged, this, nil);
}

#undef Inherited
#define Inherited TView

MA_DEFINE_CLASS_M1(TPseudoCustomView, Inherited);

TPseudoCustomView::TPseudoCustomView()
{
}

TPseudoCustomView::~TPseudoCustomView()
{
}

void TPseudoCustomView::Draw(const CViewRect& area)
{
	TWindow *wind = GetWindow();
	if(wind != 0L)
	{
		TPseudoWinBehavior *pseudo = dynamic_cast_AC(TPseudoWinBehavior*,
			wind->GetBehaviorWithIdentifier(TPseudoWinBehavior::kPseudoWinBehavior));
		if(pseudo != 0L)
		{
			int cmdid = UEventSendMessage(pseudo->GetWidID(), EV_FETCHCMD, 0, this);
			if(cmdid != -1)
				UEventSendMessage(pseudo->GetWidID(), EV_CUSTOM_DRAW, cmdid, this);
		}
	}
}


#undef Inherited
#define Inherited TEditText

MA_DEFINE_CLASS_M1(TPseudoDoubleNumberText, Inherited);

TPseudoDoubleNumberText::TPseudoDoubleNumberText() : fMaximum(DBL_MAX), fMinimum(DBL_MIN),
	fPrecision(kDefaultPrecision)
{
}

TPseudoDoubleNumberText::~TPseudoDoubleNumberText()
{
}

IDType TPseudoDoubleNumberText::GetStandardSignature() const
{
	return kStdDoubleNumberText;
}

void TPseudoDoubleNumberText::ReadFields(CStream_AC* aStream)
{
	Inherited::ReadFields(aStream);

	CStr255_AC theString;
	theString = aStream->ReadString(255);
	sscanf(CChar255_AC(theString), "%lf", &fMinimum);
	theString = aStream->ReadString(255);
	sscanf(CChar255_AC(theString), "%lf", &fMaximum);
	fPrecision = aStream->ReadInteger();
}

void TPseudoDoubleNumberText::WriteFields(CStream_AC* aStream) const
{
	Inherited::WriteFields(aStream);

	char tmp[255];
	sprintf(tmp, "%lf", fMinimum);
	aStream->WriteString(CStr255_AC(tmp));
	sprintf(tmp, "%lf", fMaximum);
	aStream->WriteString(CStr255_AC(tmp));
	aStream->WriteLong(fPrecision);
}


double TPseudoDoubleNumberText::GetValue()
{
	double theValue = 0.0;
	CStr255_AC theString = GetText();

	if (!theString.IsEmpty())
	{
		sscanf(CChar255_AC(theString), "%lf", &theValue);
	}

	return theValue;
}

static void NumberToString_US(double n, UStr & s, int decimalPlaces)
{
	char tmp[255];
	if(decimalPlaces > 0)
		sprintf(tmp, "%0.*f", decimalPlaces, n);
	else
		sprintf(tmp, "%ld", round(n));
	s = tmp;
}

void TPseudoDoubleNumberText::SetValue(double newValue, bool redraw)
{
	newValue = fMinimum >= newValue ? fMinimum : newValue;
	newValue = newValue <= fMaximum ? newValue : fMaximum;

	UStr aString;
	NumberToString_US(newValue, aString, fPrecision);

	CStr255_AC tempString = aString; 
	SetText(tempString, redraw);
}

long TPseudoDoubleNumberText::GetValidationError()
{
	CStr255_AC theString = GetText();

	if (!theString.IsEmpty())
	{
		double extValue;
		bool valid = sscanf(CChar255_AC(theString), "%lf", &extValue) == 1;

		if (!valid)
			return kNonNumericCharacters;
		else if (extValue < fMinimum)
			return kValueTooSmall;
		else if (extValue > fMaximum)
			return kValueTooLarge;
	}
	
	return kValidValue;
}

#undef Inherited
#define Inherited TBehavior

MA_DEFINE_CLASS_M1(TPseudoAppBehavior, Inherited);

TPseudoAppBehavior::TPseudoAppBehavior() : TBehavior()
{
	SetIdleFreq(0); // ASAP
}

TPseudoAppBehavior::~TPseudoAppBehavior()
{
}

bool TPseudoAppBehavior::DoIdle(IdlePhase phase)
{
	::UEventGiveTime();
	return false;
}

void TPseudoAppBehavior::DoMenuCommand(CommandNumber aCommandNumber)
{
	if(UMacAppMenu::sMAMenuWidID != -1)
	{
		int handledFlag;
		int res = UEventSendMessageExt(UMacAppMenu::sMAMenuWidID, EV_CMD,
			aCommandNumber, 0L, &handledFlag);
		if(res == 0 && handledFlag)
			return;
	}

	// regular MacApp handling
	Inherited::DoMenuCommand(aCommandNumber);
}

void TPseudoAppBehavior::DoSetupMenus()
{
	UWidget::DoValidUI(false);
	::UEventGiveTime();
	
	Inherited::DoSetupMenus();
}

bool TPseudoAppBehavior::DoToolboxEvent(TToolboxEvent* event)
{
	if(event && event->GetIdentifier() == activateEvt)
	{
		bool activate = ((event->GetModifiers()) & activeFlag) != 0;
		TWindow * aWindow = TWindow::WMgrToWindow((WindowRef)(event->GetMessage()));
		if(aWindow)
		{
			UEventNotifyMessage(EV_UPDATEFOCUS, !activate, aWindow);
				// so the pseudo widget update its current widget.
				// it's important for the menu handling.
		}
	}
	
	return Inherited::DoToolboxEvent(event);
}

#undef Inherited
#define Inherited TIconButton

MA_DEFINE_CLASS_M1(TPseudoIconButton, Inherited);

TPseudoIconButton::TPseudoIconButton()
{
}

TPseudoIconButton::~TPseudoIconButton()
{
}

void TPseudoIconButton::ReadFields(CStream_AC* aStream)
{
	Inherited::ReadFields(aStream);

	*aStream >> fContentType;
	SetContentRsrcID(fContentType, fRsrcID);
}

void TPseudoIconButton::WriteFields(CStream_AC* aStream) const
{
	Inherited::WriteFields(aStream);

	*aStream << fContentType;
}

#undef Inherited
#define Inherited TPopup

MA_DEFINE_CLASS_M1(TBuggyPopup, Inherited);

TBuggyPopup::TBuggyPopup()
{
}

TBuggyPopup::~TBuggyPopup()
{
}

void TBuggyPopup::ReadFields(CStream_AC* aStream)
{
	Inherited::ReadFields(aStream);

	*aStream >> fRealWidth;
}

void TBuggyPopup::WriteFields(CStream_AC* aStream) const
{
	Inherited::WriteFields(aStream);

	*aStream << fRealWidth;
}

void TBuggyPopup::DoPostCreate(TDocument* itsDocument)
{
	if(fRealWidth > 0)
	{
		CViewRect r = GetExtent();
		Resize(CViewPoint(fRealWidth, r.Height()), true);
	}
}

#undef Inherited
#define Inherited TIconPopup

MA_DEFINE_CLASS_M1(TPseudoIconPopup, Inherited);

TPseudoIconPopup::TPseudoIconPopup()
{
}

TPseudoIconPopup::~TPseudoIconPopup()
{
}

void TPseudoIconPopup::ReadFields(CStream_AC* aStream)
{
	Inherited::ReadFields(aStream);

	*aStream >> fContentType;
	SetContentRsrcID(fContentType, fRsrcID);
}

void TPseudoIconPopup::WriteFields(CStream_AC* aStream) const
{
	Inherited::WriteFields(aStream);

	*aStream << fContentType;
}

#undef Inherited
#define Inherited TScrollBar

MA_DEFINE_CLASS_M1(TPseudoScrollBar, Inherited);

TPseudoScrollBar::TPseudoScrollBar() : fPageStep(1), fButtonStep(1)
{
}

TPseudoScrollBar::~TPseudoScrollBar()
{
}

void TPseudoScrollBar::TrackScrollBar(short partCode)
{
	switch (partCode)
	{
		case kControlPageUpPart:
			if (GetValue() > GetMinimum())
				DeltaValue(-fPageStep);
			break;
		case kControlUpButtonPart:
			if (GetValue() > GetMinimum())
				DeltaValue(-fButtonStep);
			break;
		case kControlPageDownPart:
			if (GetValue() < GetMaximum())
				DeltaValue(fPageStep);
			break;
		case kControlDownButtonPart:
			if (GetValue() < GetMaximum())
				DeltaValue(fButtonStep);
			break;
	}	
}

#undef Inherited
#define Inherited TPseudoIconButton

MA_DEFINE_CLASS_M1(TPseudoRotateIconButton, Inherited);

TPseudoRotateIconButton::TPseudoRotateIconButton() : fCurrentIcon(0)
{
}

TPseudoRotateIconButton::~TPseudoRotateIconButton()
{
}

void TPseudoRotateIconButton::ReadFields(CStream_AC* aStream)
{
	Inherited::ReadFields(aStream);

	CChar255_AC str(aStream->ReadString(255));
	const char *ptr = str;
	while(ptr != 0L)
	{
		int v;
		const char *tmp = strchr(ptr, ',');
		if(tmp != 0L)
		{
			CChar255_AC stmp;
			size_t offset = tmp - ptr;
			stmp.CopyFrom(ptr, offset);
			ptr += offset + 1;
			if(sscanf(stmp, "%d", &v) == 1)
				fIconSuite.push_back(v);
		}
		else
		{
			if(sscanf(ptr, "%d", &v) == 1)
				fIconSuite.push_back(v);
			ptr = 0;
		}
	}
	
	if(fIconSuite.size() != 0)
		SetContentRsrcID(fContentType, fIconSuite[0]);
}

void TPseudoRotateIconButton::WriteFields(CStream_AC* aStream) const
{
	Inherited::WriteFields(aStream);

	// do we really need to write ?
	*aStream << "";
	UASSERT(0);
}

void TPseudoRotateIconButton::DoRotate(int icon)
{
	if(fCurrentIcon != icon && icon >= 0 && icon < fIconSuite.size())
	{
		fCurrentIcon = icon;
		SetContentRsrcID(fContentType, fIconSuite[fCurrentIcon]);
		ForceRedraw();
	}
}

#undef Inherited
#define Inherited TIconCheckBox

MA_DEFINE_CLASS_M1(TPseudoIconCheckBox, Inherited);

TPseudoIconCheckBox::TPseudoIconCheckBox()
{
}

TPseudoIconCheckBox::~TPseudoIconCheckBox()
{
}

void TPseudoIconCheckBox::ReadFields(CStream_AC* aStream)
{
	Inherited::ReadFields(aStream);

	*aStream >> fContentType;
	SetContentRsrcID(fContentType, fRsrcID);
}

void TPseudoIconCheckBox::WriteFields(CStream_AC* aStream) const
{
	Inherited::WriteFields(aStream);

	*aStream << fContentType;
}

#undef Inherited
#define Inherited TAdorner

MA_DEFINE_CLASS_M1(TPseudoWinAdorner, Inherited);

TPseudoWinAdorner::TPseudoWinAdorner() : TAdorner(kPseudoWinAdorner), fWidID(-1)
{
}

TPseudoWinAdorner::~TPseudoWinAdorner()
{
	if(fWidID != -1)
		UEventSendMessage(fWidID, EV_DESTROY, 0, 0L);
}

void TPseudoWinAdorner::ViewChangedFrame (TView* itsView,
							const CViewRect& oldFrame,
							const CViewRect& newFrame,
							bool	invalidate)
{
	TWindow *wind = itsView->GetWindow();
	if(wind != 0L && fWidID != -1)
	{
		int cmdid = UEventSendMessage(fWidID, EV_FETCHCMD, 0, itsView);
		if(cmdid != -1)
		{
			URECT r;
			r.left = newFrame.left;
			r.right = newFrame.right;
			r.top = newFrame.top;
			r.bottom = newFrame.bottom;
			
			UEventSendMessage(fWidID, EV_RESIZE, cmdid, &r);
		}
	}
}

UIMPLEMENT_DYNAMIC(UMacAppMenu, UMenu)

UBEGIN_MESSAGE_MAP(UMacAppMenu, UMenu)
	ON_UPDATEUI(UMacAppMenu)
UEND_MESSAGE_MAP()

int UMacAppMenu::sMAMenuWidID = -1;

UMacAppMenu::UMacAppMenu() : UMenu(::UEventGetWidID())
{
	sMAMenuWidID = GetWidID();
}

UMacAppMenu::~UMacAppMenu()
{
}

// If hierarchical the range of IDs for applications is restricted.  See IM V-236.
// If the menu isn't managed by MacApp and is in a TPopup, its ID must be outside this range.
enum
{
	kHierarchicalMin = 0,
	kHierarchicalMax = 235
};

void UMacAppMenu::OnUpdateUI()
{
	static bool sSemaphore = false;
	USemaphore policeman(sSemaphore);
	if(policeman.IsEnteredTwice())
		return;

	// this is the interesting thing about the toolbar : it resends the
	// update UI to other widgets according to the focus information and
	// allows the update to be handled several times if the handler asks for it
	// (by using UCmdUI::SetContinue)

	const STD map<int, UCmdTarget *> & targets = UEventGetTargets();
	std::map<int, void *>::iterator w;
	int cmdid;
	int widFocus = UWidget::GetFocus();
	
	for(CMenuIterator iter; iter.Current(); ++iter)
	{
		MenuRef menu = iter;
		short menuID = (**menu).menuID;
		
		// see UMenuMgr.cpp from MacApp
		if (menuID < kHierarchicalMin || menuID > kHierarchicalMax)
			continue;
		
		short numitems = ::CountMItems(menu);
		for(int j = 1; j <= numitems; j++)
		{
			cmdid = CommandFromMenuItem(menuID, j);
			if(cmdid < 0)
			{
				UAppConsole("The item %d of the menu %d has not a MacApp cmd\n", j, menuID);
			}
			else
			{
				if(cmdid == kUMainWidget)
					continue;

				// first forward the command to the focused widget : if it doesn't want it
				// we give it to the next pseudo widget.
				if(widFocus != -1 && widFocus != GetWidID())
				{
					UCmdUI pUI(cmdid);
					UEventSendMessage(widFocus, EV_UPDTCMD, pUI.m_nID, &pUI);
					if(!pUI.CanContinue())
						continue;
				}

				STD map<int, UCmdTarget *>::const_iterator i;
				for(i = targets.begin(); i != targets.end(); ++i)
				{
					int widid = (*i).first;
					if(widid == widFocus || widid == GetWidID())
						continue;
					
					UCmdUI pUI(cmdid);
					UEventSendMessage(widid, EV_UPDTCMD, pUI.m_nID, &pUI);
					if(!pUI.CanContinue())
						break;
				}
				if(i == targets.end())
				{
					//UAppConsole("The cmd %d of the menubar has no update handler, it's gonna be disabled\n", cmdid);
					UCmdUI pUI(cmdid);
					pUI.Enable(false);
				}
			}
		}
	}
}

class CDummyClass
{
public:
	CDummyClass()
	{
		InitUSlider();
		if (!gUGridViewInitialized)
		{
			InitUGridView();
		}
		MA_REGISTER_CLASS(TPseudoAppBehavior);
		MA_REGISTER_CLASS(TPseudoIconButton);
		MA_REGISTER_CLASS(TPseudoIconPopup);
		MA_REGISTER_CLASS(TPseudoScrollBar);
		MA_REGISTER_CLASS(TPseudoRotateIconButton);
		MA_REGISTER_CLASS(TPseudoIconCheckBox);
		MA_REGISTER_CLASS(TBuggyPopup);
		MA_REGISTER_CLASS(TPseudoWinBehavior);
		MA_REGISTER_CLASS(TPseudoCustomView);
		MA_REGISTER_CLASS(TPseudoWinAdorner);
		MA_REGISTER_SIGNATURE(TPseudoTextListView,
				TPseudoTextListView::kTextStringListViewSignature);
		MA_REGISTER_CLASS(TPseudoDoubleNumberText);
	}
	
} sDummyStarter;

typedef struct CViewInfo
{
	TView *view;
};

static void extractInfos(std::vector<CViewInfo> & infos, std::vector<TPopup *> & popups, TView *view)
{
	if(view == 0L)
		return;

	if(MA_MEMBER(view, TPopup) && !MA_MEMBER(view, TIconPopup))
	{
		popups.push_back((TPopup *)view);
	}
	else
	{
		CViewInfo info;
		info.view = view;
		infos.push_back(info);
	}
	
	extractInfos(infos, popups, view->GetNextView());
	extractInfos(infos, popups, view->GetSubView());
}

static bool InBetweenVSide(const CViewRect & popPos, const CViewRect & pos)
{
	return (popPos.top >= pos.top && popPos.top <= pos.bottom) ||
		(popPos.bottom >= pos.top && popPos.bottom <= pos.bottom) ||
		 (pos.top >= popPos.top && pos.top <= popPos.bottom) ||
		(pos.bottom >= popPos.top && pos.bottom <= popPos.bottom);
}

static int findmostright(TPopup *popup, const std::vector<CViewInfo> & infos)
{
	int most_right = popup->GetWindow()->GetExtent().right;
	CViewRect popPos = popup->GetFrame();
	popPos = popup->GetSuperView()->LocalToRootView(popPos);
	
	std::vector<CViewInfo>::const_iterator i;
	for(i = infos.begin(); i != infos.end(); ++i)
	{
		TView *view = (*i).view;
		
		CViewRect pos = view->GetFrame();
		pos = view->GetSuperView()->LocalToRootView(pos);
		if(pos.left > popPos.right && InBetweenVSide(popPos, pos))
		{
			// the top/bottom popup is inside the top/bottom view
			if(pos.left < most_right)
				most_right = pos.left;
		}
	}
	
	return most_right;
}

void macapp_workaround_adlib_bug(TView *view)
{
	std::vector<TPopup *> popups;
	std::vector<CViewInfo> infos;
	extractInfos(infos, popups, view->GetSubView());
#	define OFFSET 40

	std::vector<TPopup *>::iterator p;
	for(p = popups.begin(); p != popups.end(); ++p)
	{
		TPopup *popup = *p;
		
		int most_right = findmostright(popup, infos);
		CViewRect oldPos = popup->GetFrame();
		oldPos = popup->GetSuperView()->LocalToRootView(oldPos);
		if(most_right > (oldPos.right + OFFSET))
		{
			oldPos.right = most_right - OFFSET;
			oldPos = popup->GetSuperView()->RootViewToLocal(oldPos);
			popup->SetFrame(oldPos, true);
		}
	}
}

