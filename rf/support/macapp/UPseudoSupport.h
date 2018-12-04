#ifndef UPSEUDOSUPPORT_H
#define UPSEUDOSUPPORT_H

#include "uwidget.h"
#include "umenu.h"

#include "UBehavior.h"
#include "UGridView.h"
#include "UEditText.h"
#include "UIconButton.h"
#include <vector>

class TBuggyPopup : public TPopup
{
	MA_DECLARE_CLASS;

	TBuggyPopup();
	virtual ~TBuggyPopup();

	virtual void ReadFields(CStream_AC* aStream);
	virtual void WriteFields(CStream_AC* aStream) const;
	virtual void DoPostCreate(TDocument* itsDocument);
protected:
	int fRealWidth;
};

class TPseudoScrollBar : public TScrollBar
{
	MA_DECLARE_CLASS;

	TPseudoScrollBar();
	virtual ~TPseudoScrollBar();

	virtual void TrackScrollBar(short partCode);

	int fPageStep;
	int fButtonStep;
};

class TPseudoIconPopup : public TIconPopup
{
	MA_DECLARE_CLASS;

	TPseudoIconPopup();
	virtual ~TPseudoIconPopup();

	virtual void ReadFields(CStream_AC* aStream);
	virtual void WriteFields(CStream_AC* aStream) const;
};

class TPseudoIconButton : public TIconButton
{
	MA_DECLARE_CLASS;

	TPseudoIconButton();
	virtual ~TPseudoIconButton();

	virtual void ReadFields(CStream_AC* aStream);
	virtual void WriteFields(CStream_AC* aStream) const;
};

class TPseudoRotateIconButton : public TPseudoIconButton
{
	MA_DECLARE_CLASS;

	TPseudoRotateIconButton();
	virtual ~TPseudoRotateIconButton();

	virtual void ReadFields(CStream_AC* aStream);
	virtual void WriteFields(CStream_AC* aStream) const;
	
	void DoRotate(int icon);

protected:
	std::vector<short> fIconSuite;
	int fCurrentIcon;
};

class TPseudoIconCheckBox : public TIconCheckBox
{
	MA_DECLARE_CLASS;

	TPseudoIconCheckBox();
	virtual ~TPseudoIconCheckBox();

	virtual void ReadFields(CStream_AC* aStream);
	virtual void WriteFields(CStream_AC* aStream) const;
};

class UMacAppMenu : public UMenu
{
	UDECLARE_DYNAMIC(UMacAppMenu)

public:
	UMacAppMenu();
	virtual ~UMacAppMenu();
	
	static int sMAMenuWidID;

	ev_msg void OnUpdateUI(void);

	UDECLARE_MESSAGE_MAP()
};

class TPseudoAppBehavior : public TBehavior
{
	MA_DECLARE_CLASS;
public:
	TPseudoAppBehavior();
	virtual ~TPseudoAppBehavior();

	virtual bool DoIdle(IdlePhase phase);
	virtual void DoMenuCommand(CommandNumber aCommandNumber);
	virtual void DoSetupMenus();
	virtual bool DoToolboxEvent(TToolboxEvent* event);
	
protected:
};

class TPseudoWinBehavior : public TBehavior
{
	MA_DECLARE_CLASS;
public:

	enum
	{
		kPseudoWinBehavior = 'pswb'
	};
	
	TPseudoWinBehavior();
	virtual ~TPseudoWinBehavior();

	virtual void DoEvent(EventNumber eventNumber, TEventHandler* source, TEvent* event);

	inline int GetWidID() const { return fWidID; }
protected:
	inline void SetPseudoWidget(int widid) { fWidID = widid; }
	
	int fWidID;
	
	friend class UWidget;
};

class TPseudoWinAdorner : public TAdorner
{
	MA_DECLARE_CLASS;
public:

	enum
	{
		kPseudoWinAdorner = 'pswa'
	};
	
	TPseudoWinAdorner();
	virtual ~TPseudoWinAdorner();

	virtual void ViewChangedFrame (TView* itsView,
											const CViewRect& oldFrame,
											const CViewRect& newFrame,
											bool	invalidate);

protected:
	inline void SetPseudoWidget(int widid) { fWidID = widid; }
	
	int fWidID;
	
	friend class UWidget;
};

class TPseudoCustomView : public TView
{
	MA_DECLARE_CLASS;
public:

	TPseudoCustomView();
	virtual ~TPseudoCustomView();

	virtual void Draw(const CViewRect& area);
};

class TPseudoTextListView : public TTextListView
{
	MA_DECLARE_CLASS;

public:
	enum
	{
		kTextStringListViewSignature = 'TxLv',

		kSelectionChanged = 'TxSC',
		kMultiClickSelection = 'TxMu'
	};

	TPseudoTextListView();
	virtual ~TPseudoTextListView();

	virtual IDType GetStandardSignature(void) const;

	virtual CStr255_AC GetItemText(GridCoordinate) const;

	virtual void InsStringBefore(short anItem, const char * string);
	virtual void InsStringFirst(const char * string);
	virtual void InsStringLast(const char * string);

	virtual void DelItemAt(GridCoordinate anItem, GridCoordinate numOfItems);
	virtual void RemoveAll(void);

	virtual void DoKeyEvent(TToolboxEvent * event);
	virtual void DoMouseCommand(CViewPoint & theMouse, TToolboxEvent * event, CPoint_AC);
	virtual void SetSelection(const TGridViewDesignator* cellsToSelect,
						bool extendSelection, bool highlight, bool select);

protected:
	virtual bool DoMouseCommand_SelectCells(GridCell whichCell, class TToolboxEvent * event);

	std::vector<CStr255_AC> fStringContainer;
};

class TPseudoDoubleNumberText : public TEditText
{
	MA_DECLARE_CLASS;
	
public:
	enum
	{
		kDefaultPrecision = 5,
		kStdDoubleNumberText = 'dNum'
	};
		
	TPseudoDoubleNumberText();
	virtual ~TPseudoDoubleNumberText();

	virtual IDType GetStandardSignature() const;
	virtual void ReadFields(CStream_AC* aStream);
	virtual void WriteFields(CStream_AC* aStream) const;
	virtual long GetValidationError();
		// Overrides

	virtual double GetValue();
	virtual void SetValue(double newValue, bool redraw);
protected:
	double fMinimum;
	double fMaximum;
	short fPrecision;
};

void macapp_workaround_adlib_bug(TView *view);

#endif
