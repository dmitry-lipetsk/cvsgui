#ifndef UPSEUDOSUPPORT_H
#define UPSEUDOSUPPORT_H

#include "uwidget.h"
#include <LOutlineItem.h>
#include <LCmdBevelButton.h>
#include <LPlaceHolder.h>

#include <map>
#include <vector>

extern bool gTurnOffBroadcast;

struct UPseudoCmdStatusWrapper
{
	Boolean *outEnabled;
	Boolean *outUsesMark;
	UInt16 *outMark;
	unsigned char *outName;
};

class UPseudoRotButton : public LBevelButton
{
public:
	enum { class_ID = FOUR_CHAR_CODE('UPRB') };
	
	UPseudoRotButton(LStream *inStream, ClassIDT inImpID = LBevelButton::imp_class_ID);
	virtual ~UPseudoRotButton();

	virtual void DoRotate(int icon, bool refresh = true);
	inline int GetCurrentState(void) const { return fCurrentIcon; }
protected:
	void ReadIconsID(LStream *inStream, std::vector<short> & res);

	std::vector<short> fIconSuite;
	int fCurrentIcon;
};

class UPseudoPlaceHolder : public LPlaceHolder, public LListener
{
public:
	enum { class_ID = FOUR_CHAR_CODE('UPLH') };
	
	UPseudoPlaceHolder(LStream *inStream);
	virtual ~UPseudoPlaceHolder();

	void DoRotate(int entry);
	inline void SetWidID(int widid) { fWidid = widid; }
	inline LView *GetCurrentView(void) { return fCurrentView; }

	virtual void	ListenToMessage(
							MessageT		inMessage,
							void*			ioParam);

protected:
	virtual void	FinishCreateSelf();
	
	std::vector<short> fViewSuite;
	LView *fCurrentView;
	PaneIDT fController;
	int fWidid;
};

class UPseudoPageHider : public LAttachment, public LListener
{
public:
	enum { class_ID = FOUR_CHAR_CODE('UPPH') };
	
	UPseudoPageHider(LStream *inStream);
	virtual ~UPseudoPageHider();

	inline void SetWidID(int widid) { fWidid = widid; }

	virtual void	ListenToMessage(
							MessageT		inMessage,
							void*			ioParam);

protected:
	void DoRotate(int entry);
	
	virtual void	ExecuteSelf(
							MessageT		inMessage,
							void*			ioParam);

	std::vector<PaneIDT> fViewSuite;
	int fWidid;
	int fCurSel;
	class LPageController *fPageCtrl;
};

class LMainPseudoAttachment : public LAttachment, public LPeriodical
{
public:
	LMainPseudoAttachment();
	virtual ~LMainPseudoAttachment();

	virtual	void		SpendTime(
								const EventRecord&	inMacEvent);

protected:
	virtual void	ExecuteSelf(
							MessageT		inMessage,
							void*			ioParam);
};

class LPseudoWinAttachment : public LAttachment, public LListener
{
public:
	LPseudoWinAttachment(int widid);
	virtual ~LPseudoWinAttachment();

	int GetPageValue(int cmdid) const;
	void SetPageValue(int cmdid, int value);
	
	void CheckWindowSize(void);
	
	int SearchWidID(LPane *pane);
		// since several pseudo-widgets can be attached to
		// the window (ex: tab controller), we need to search
		// for the correct Widget to broadcast the event.
	
	void InvalidCache(UWidget *tab_wid);
		// when a view is removed (from a tab window)
		// we need to invalidate the cache used by 'SearchWidID'.
	
protected:
	virtual void	ExecuteSelf(
							MessageT		inMessage,
							void*			ioParam);
	virtual void	ListenToMessage(
							MessageT		inMessage,
							void*			ioParam);
	
	int m_widid;
	Rect m_lastRect;
	std::map<int, int> m_PageValues;
	std::map<LPane *, int> m_CacheWidgets;
};

class UPseudoFakePane : public LPane
{
public:
	UPseudoFakePane(const SPaneInfo	&inPaneInfo, LPseudoWinAttachment *inAttach);
	virtual ~UPseudoFakePane();

	virtual void		AdaptToSuperFrameSize(
								SInt32		inSurrWidthDelta,
								SInt32		inSurrHeightDelta,
								Boolean		inRefresh);
	
protected:
	LPseudoWinAttachment *m_Attach;

	virtual void		ActivateSelf();
	virtual void		DeactivateSelf();
};

class CPseudoItem : public LOutlineItem
{
public:
	CPseudoItem(int widid, int cmdid);
	virtual ~CPseudoItem();
	
	std::map<int, UStr> m_texts;
	void *m_data;

protected:
	virtual void GetDrawContentsSelf(const STableCell& inCell, SOutlineDrawContents& ioDrawContents);

	virtual void DrawRowAdornments(const Rect& inLocalRowRect);
	virtual void			DrawCell(
									const STableCell&		inCell,
									const Rect&				inLocalCellRect);

	virtual void DoubleClick(const STableCell& inCell, const SMouseDownEvent& inMouseDown,
		const SOutlineDrawContents&	inDrawContents, Boolean inHitText);
	virtual void			SingleClick(
									const STableCell&			inCell,
									const SMouseDownEvent&		inMouseDown,
									const SOutlineDrawContents&	inDrawContents,
									Boolean						inHitText);

	int m_widid, m_cmdid;
private:
	// defensive programming
	CPseudoItem();
	CPseudoItem( const CPseudoItem &inOriginal );
	CPseudoItem&	operator=( const CPseudoItem &inOriginal );
};

class LTextTableView;

extern void macpp_fecthview(int widid, LPane *view);
extern void macpp_linkbroadcasters(LPane *view, LListener *listener);
extern void macpp_preparetable(LOutlineTable *table);
extern void macpp_preparetable(void *win, LTextTableView *table);
extern LPseudoWinAttachment *macpp_getattachwindow(UWidget *wid);
extern LPseudoWinAttachment *macpp_getattachwindow(LWindow *wind);

#define UTextTableView_SingleClick FOUR_CHAR_CODE('UTVS')
#define UTextTableView_DoubleClick FOUR_CHAR_CODE('UTVD')

#endif
