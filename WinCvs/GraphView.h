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

#if !defined(AFX_GRAPHVIEW_H__7A07C311_03F9_11D2_BCB2_000000000000__INCLUDED_)
#define AFX_GRAPHVIEW_H__7A07C311_03F9_11D2_BCB2_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GraphView.h : header file
//

#include "CvsLog.h"
#include "AppGlue.h"

#define STARTPOINT_PT		(CPoint(5, 5))
#define SHADOW_OFFSET_PT	(CPoint(4, 4))
#define ROUND_RECT_PT		(CPoint(6, 6))

#define OCTANGLE_COUNT		8

#define RGB_DEF_SEL				RGB(160, 160, 160)
#define RGB_DEF_SHADOW			RGB(128, 128, 128)
#define RGB_DEF_HEADER			RGB(255, 0, 0)
#define RGB_DEF_TAG				RGB(0, 0, 0)
#define RGB_DEF_BRANCH			RGB(0, 0, 255)
#define RGB_DEF_NODE			RGB(0, 0, 255)
#define RGB_DEF_DEADNODE		RGB(255, 0, 0)
#define RGB_DEF_USERSTATENODE	RGB(0, 128, 64)
#define RGB_DEF_MERGE			RGB(255, 0, 0)
#define STYLE_DEADNODE			PS_DOT
#define STYLE_USERSTATENODE		PS_DASH

/// Node shape type enum
typedef enum
{
	kRectangle,	/*!< Rectangle */
	kRoundRect,	/*!< Rectangle with round corners */
	kOctangle	/*!< Octal */
} kNodeShape;

/// Traverse result enum
typedef enum
{
	kTContinue,	/*!< Continue traverse */
	kTGetDown,	/*!< Get down */
	kTStop		/*!< Stop */
} kTGraph;

// Forward declarations
class EntnodeData;
class CGraphView;
class KiSelectionHandler;

/// Windows log data abstract class
class CWinLogData : public CLogNodeData
{
private:
	typedef CWinLogData						self_type;

	CWinLogData(const self_type&);
	self_type& operator = (const self_type&);

public:
	// Construction
	CWinLogData(CLogNode* node);
	virtual ~CWinLogData();

private:
	// Data members
	CRect m_selfB;	/*!< Self bounds */
	CRect m_totB;	/*!< Total bounds */
	bool  m_sel;	/*!< Selection flag */

protected:
	typedef std::pair<self_type*, self_type*>	data_pair_type;
	typedef std::vector<data_pair_type>			data_pair_vec_type;

	// Methods
	inline static self_type* GetData(CLogNode* node);
	inline static const self_type* GetData(const CLogNode* node);

	void CollectMergePointPairPass1(data_pair_vec_type& mergePair);
	void CollectMergePointPairPass2(data_pair_vec_type& mergePair);
	
	virtual void UpdateSelf(CDC& dc, const EntnodeData* entryInfo)const = 0;
	virtual CSize GetBoundsSelf(CDC& dc, const EntnodeData* entryInfo)const = 0;

	virtual void CollectMergePointPairPass1Self(data_pair_vec_type& mergePair);
	virtual void CollectMergePointPairPass2Self(data_pair_vec_type& mergePair);
	
public:
	// Interface
	inline kLogNode GetType(void) const;
	const char* GetStr(void);
	
	inline CRect& Bounds(void);
	inline const CRect& Bounds(void)const;
	inline CRect& SelfBounds(void);
	inline const CRect& SelfBounds(void)const;
	
	void ComputeBounds(const CPoint& topLeft, CDC& dc, const EntnodeData* entryInfo = 0L);
	void Offset(CPoint offset);
	void Update(CDC& dc, const EntnodeData* entryInfo = 0L)const;
	
	inline bool Selected(void) const;
	void SetSelected(CScrollView* view, bool state);
	void UnselectAll(CScrollView* view);
	void ScrollTo(CScrollView* view);
	CWinLogData* HitTest(CPoint point);
	
	static CWinLogData* CreateNewData(CLogNode* node);
	
	bool IsDiskNode(const EntnodeData* entryInfo)const;
	bool IsDeadNode()const;
	bool IsUserState()const;
	
	void UpdateMergePoints(CDC& dc);

private:
	//Helper structures for ComputeBounds method
	class tag_traits_for_cb;

	//Helper structures for Update method
	class tag_traits_for_update;

	//Helper structure for HitTest method
	class tag_traits_for_ht;
};//class CWinLogData

/// Windows log header node data
class CWinLogHeaderData : public CWinLogData
{
private:
	typedef CWinLogHeaderData				self_type;
	typedef CWinLogData						inherited;

	CWinLogHeaderData(const self_type&);
	self_type& operator = (const self_type&);

public:
	// Construction
	CWinLogHeaderData(CLogNode* node) : inherited(node) {}
	virtual ~CWinLogHeaderData() {}

protected:
	// Methods
	virtual void UpdateSelf(CDC& dc, const EntnodeData* entryInfo)const;
	virtual CSize GetBoundsSelf(CDC& dc, const EntnodeData* entryInfo)const;
};

/// Windows log revision node data
class CWinLogRevData : public CWinLogData
{
private:
	typedef CWinLogRevData					self_type;
	typedef CWinLogData						inherited;

	CWinLogRevData(const self_type&);
	self_type& operator = (const self_type&);

public:
	// Construction
	CWinLogRevData(CLogNode* node) : inherited(node) {}
	virtual ~CWinLogRevData() {}
	
protected:
	// Methods
	virtual void UpdateSelf(CDC& dc, const EntnodeData* entryInfo)const;
	virtual CSize GetBoundsSelf(CDC& dc, const EntnodeData* entryInfo)const;

	virtual void CollectMergePointPairPass1Self(data_pair_vec_type& mergePair);
	virtual void CollectMergePointPairPass2Self(data_pair_vec_type& mergePair);
};

/// Windows log tag node data
class CWinLogTagData : public CWinLogData
{
private:
	typedef CWinLogTagData					self_type;
	typedef CWinLogData						inherited;

	CWinLogTagData(const self_type&);
	self_type& operator = (const self_type&);

public:
	// Construction
	CWinLogTagData(CLogNode* node) : inherited(node) {}
	virtual ~CWinLogTagData() {}
	
protected:
	// Methods
	virtual void UpdateSelf(CDC& dc, const EntnodeData* entryInfo)const;
	virtual CSize GetBoundsSelf(CDC& dc, const EntnodeData* entryInfo)const;
};

/// Windows log branch node data
class CWinLogBranchData : public CWinLogData
{
private:
	typedef CWinLogBranchData				self_type;
	typedef CWinLogData						inherited;

	CWinLogBranchData(const self_type&);
	self_type& operator = (const self_type&);

public:
	// Construction
	CWinLogBranchData(CLogNode* node) : inherited(node) {}
	virtual ~CWinLogBranchData() {}
	
protected:
	// Methods
	virtual void UpdateSelf(CDC& dc, const EntnodeData* entryInfo)const;
	virtual CSize GetBoundsSelf(CDC& dc, const EntnodeData* entryInfo)const;
};

/// Base traverse class
class TGraph
{
public:
	// Interface
	kTGraph TraverseGraph(CLogNode* node);
	virtual kTGraph TraverseNode(CLogNode* node) = 0;

private:
	//Helper structures for TraverseGraph
	class tag_traits_for_tg;
};//class TGraph

/// Graph traverse class
class TGraphRectSelect : public TGraph
{
public:
	// Construction
	TGraphRectSelect(bool shiftOn, CGraphView* view, CRect& rect);

private:
	// Data members
	bool         const m_shiftOn; /*!< Flag to indicate the shift key state */
	CGraphView*  const m_view;    /*!< View */
	const CRect&       m_rect;    /*!< Rectangle */

public:
	// Interface
	virtual kTGraph TraverseNode(CLogNode* node);
};

/// Graph command status class
class TGraphCmdStatus : public TGraph
{
public:
	/// Construction
	TGraphCmdStatus();

private:
	// Data members
	CWinLogData* m_sel1;	/*!< First selected item log data */
	CWinLogData* m_sel2;	/*!< Second selected item log data */
	int m_selCount;			/*!< Selection count */

public:
	// Interface
	virtual kTGraph TraverseNode(CLogNode* node);

	int GetSelCount() const;
	CWinLogData* GetSel1() const;
	CWinLogData* GetSel2() const;
};

/// Graph console
class CGraphConsole : public CCvsConsole
{
public:
	// Construction
	CGraphConsole();

private:
	// Data members
	CWincvsView* m_view;	/*!< View */

public:
	// Interface
	virtual long cvs_out(const char* txt, long len);
	virtual long cvs_err(const char* txt, long len);
};

/// Graph command delete revisions class
class TGraphDelCmd : public TGraph
{
public:
	// Construction
	TGraphDelCmd();

private:
	// Data members
	int m_countSel;
	bool m_onlyRev;
	std::vector<std::string> m_allRevs;

public:
	// Interface
	virtual kTGraph TraverseNode(CLogNode* node);

	int GetCountSel() const;
	bool GetOnlyRev() const;
	std::vector<std::string>& GetAllRevs();
};

/// Select non-significant revisions
class TGraphSelNonSig : public TGraph
{
public:
	// Construction
	TGraphSelNonSig(CGraphView* view);

private:
	// Data members
	CGraphView* m_view;	/*!< View */

public:
	// Interface
	virtual kTGraph TraverseNode(CLogNode* node);
};

/// Console used to retrieve a specific revision
class CRevisionConsole : public CCvsStreamConsole
{
public:
	// Construction
	CRevisionConsole();

public:
	// Interface
	bool Init(LPCTSTR path, LPCTSTR fname);
};

/// Graph rectangle tracker
class CGraphRectTracker : public CRectTracker
{
public:
	CGraphRectTracker(CGraphView* view) : m_view(view) {}

	virtual void OnChangedRect(const CRect& rectOld);
protected:
	CGraphView* m_view;	/*!< View */
};

/////////////////////////////////////////////////////////////////////////////
// CGraphView view

/// Graph view class
class CGraphView : public CScrollView
{
	DECLARE_DYNCREATE(CGraphView)
protected:
	CGraphView();           // protected constructor used by dynamic creation
	virtual ~CGraphView();

// Attributes
public:

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphView)
	public:
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CGraphView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewReload();
	afx_msg void OnUpdateViewReload(CCmdUI* pCmdUI);
	afx_msg void OnViewDiff();
	afx_msg void OnUpdateViewDiff(CCmdUI* pCmdUI);
	afx_msg void OnViewUpdate();
	afx_msg void OnUpdateViewUpdate(CCmdUI* pCmdUI);
	afx_msg void OnViewTag();
	afx_msg void OnUpdateViewTag(CCmdUI* pCmdUI);
	afx_msg void OnViewDelrev();
	afx_msg void OnUpdateViewDelrev(CCmdUI* pCmdUI);
	afx_msg void OnViewSelnonsig();
	afx_msg void OnUpdateViewSelnonsig(CCmdUI* pCmdUI);
	afx_msg void OnViewRetrieve();
	afx_msg void OnUpdateViewRetrieve(CCmdUI* pCmdUI);
	afx_msg void DisableUpdate(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAnnotate();
	afx_msg void OnGraphGraphoptions();
	afx_msg void OnGraphFont();
	afx_msg void OnUpdateGraphChangelogmessage(CCmdUI* pCmdUI);
	afx_msg void OnGraphChangelogmessage();
	afx_msg void OnUpdateGraphSetdescription(CCmdUI* pCmdUI);
	afx_msg void OnGraphSetdescription();
	afx_msg void OnGraphSetstate();
	afx_msg void OnUpdateGraphSetstate(CCmdUI* pCmdUI);
	afx_msg void OnViewUnlockf();
	afx_msg void OnUpdateViewUnlockf(CCmdUI* pCmdUI);
	afx_msg void OnViewEditsel();
	afx_msg void OnViewEditseldef();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnViewRetrieveAs();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnViewOpensel();
	afx_msg void OnViewOpenselas();
	afx_msg void OnQueryShellactionDefault();
	afx_msg void OnUpdateViewTagnew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTagdelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTagbranch(CCmdUI* pCmdUI);
	afx_msg void OnViewTagnew();
	afx_msg void OnViewTagdelete();
	afx_msg void OnViewTagbranch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	static CImageList m_imageList;	/*!< Image list */
	CFont* m_font;					/*!< Font */
	bool m_scrollToCurrentRev;		/*!< true to scroll the view to show current revision */

	// For printing
	int m_numberOfPages;	/*!< Number of pages */
	int m_pageHeight;		/*!< Page height */

	// Methods
	bool ProcessSelectionCommand(KiSelectionHandler* handler, TGraphCmdStatus* trav);

	bool DisableCommon();
	bool IsSelSingleNode(kLogNode type, bool equal = true);
	
	void FileAction();
	void SaveSel(bool queryName = false);

	bool RetrieveSel(std::string& file);

	void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	
public:
	// Interface
	void CalcImageSize(void);

	/// Get the image list
	static inline CImageList& GetImgList(void) { return m_imageList; }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHVIEW_H__7A07C311_03F9_11D2_BCB2_000000000000__INCLUDED_)
