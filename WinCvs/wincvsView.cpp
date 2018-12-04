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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

// wincvsView.cpp : implementation of the CWincvsView class
//

#include "stdafx.h"
#include <atlconv.h>
#include "wincvs.h"
#include "wincvsDoc.h"
#include "wincvsView.h"
#include "MainFrm.h"

#include "Persistent.h"
#include "CvsPrefs.h"
#include "AppGlue.h"
#include "TclGlue.h"
#include "PythonGlue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// For critical section that ought to be infinite since it can't timeout anyway but for mutex it's better to have it set to 20000
#define VIEW_LOCK_TIMEOUT	INFINITE

static CPersistentT<CCharFormat> gViewFont("P_ViewFont", CCharFormat(), kNoClass);

/////////////////////////////////////////////////////////////////////////////
// CWincvsView

IMPLEMENT_DYNCREATE(CWincvsView, CRichEditView)

BEGIN_MESSAGE_MAP(CWincvsView, CRichEditView)
	//{{AFX_MSG_MAP(CWincvsView)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_FORMAT_FONT, OnMyFormatFont)
	ON_WM_KEYDOWN()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
	ON_WM_GETDLGCODE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWincvsView construction/destruction

CWincvsView::CWincvsView()
{
	m_fRtfFormat = ::RegisterClipboardFormat(_T("Rich Text Format"));
	
	m_sbuf = 0L;
	m_snumbuf = 0;
}

CWincvsView::~CWincvsView()
{
	m_snumbuf = 0;
	if( m_sbuf != 0L)
	{
		free(m_sbuf);
		m_sbuf = 0L;
	}
	
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app )
	{
		app->StopScriptEngine();
	}
}

/// PreCreateWindow virtual override, modify the window style
BOOL CWincvsView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bRet = CRichEditView::PreCreateWindow(cs);
	cs.style |= WS_TABSTOP | ES_MULTILINE;

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CWincvsView drawing

/// OnDraw virtual override, do nothing here
void CWincvsView::OnDraw(CDC* pDC)
{
	//CWincvsDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	// TODO: also draw all OLE items in the document

	// Draw the selection at an arbitrary position.  This code should be
	//  removed once your real drawing code is implemented.  This position
	//  corresponds exactly to the rectangle returned by CWincvsCntrItem,
	//  to give the effect of in-place editing.

	// TODO: remove this code when final draw code is complete.

	//if (m_pSelection == NULL)
	//{
	//	POSITION pos = pDoc->GetStartPosition();
	//	m_pSelection = (CWincvsCntrItem*)pDoc->GetNextClientItem(pos);
	//}
	//if (m_pSelection != NULL)
	//	m_pSelection->Draw(pDC, CRect(10, 10, 210, 210));
}

/// OnInitialUpdate virtual override, set the character font
void CWincvsView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	CRichEditCtrl& edit = GetRichEditCtrl();
	edit.SetSel(0, -1);

	SetCharFormat((CCharFormat)gViewFont);
	edit.SetSel(-1, -1);
}

/////////////////////////////////////////////////////////////////////////////
// CWincvsView printing

/// OnPreparePrinting virtual override
BOOL CWincvsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
	//return CRichEditView::OnPreparePrinting(pInfo);
}

/// OnBeginPrinting virtual override
void CWincvsView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// No extra initialization before printing
	CRichEditView::OnBeginPrinting(pDC, pInfo);
}

/// OnEndPrinting virtual override
void CWincvsView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// No cleanup after printing
	CRichEditView::OnEndPrinting(pDC, pInfo);
}

/// WM_DESTROY message handler
void CWincvsView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.

	CRichEditView::OnDestroy();

	//COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	//if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
	//{
	//	pActiveItem->Deactivate();
	//	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	//}
}


/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

/// IsSelected virtual override
BOOL CWincvsView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CWincvsCntrItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced.

	// TODO: implement this function that tests for a selected OLE client item

	//return pDocItem == m_pSelection;
	return CRichEditView::IsSelected(pDocItem);
}

#if 0
// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation.
void CWincvsView::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}
#endif


///	WM_SETFOCUS message handler
///	\note Special handling of OnSetFocus and OnSize are required for a container when an object is being edited in-place
void CWincvsView::OnSetFocus(CWnd* pOldWnd)
{
//	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
//	if (pActiveItem != NULL &&
//		pActiveItem->GetItemState() == COleClientItem::activeUIState)
//	{
		// need to set focus to this item if it is in the same view
//		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
//		if (pWnd != NULL)
//		{
//			pWnd->SetFocus();   // don't call the base class
//			return;
//		}
//	}

	CRichEditView::OnSetFocus(pOldWnd);
}

/// WM_SIZE message handler
void CWincvsView::OnSize(UINT nType, int cx, int cy)
{
	CRichEditView::OnSize(nType, cx, cy);
//	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
//	if (pActiveItem != NULL)
//		pActiveItem->SetItemRects();
}

/*!
	Print a line into the console window
	\param buf Text to print
	\param numbuf Length of text to print
	\param isStderr true for <b>stderr</b>, false otherwise
*/
void CWincvsView::OutLine(const char* buf, int numbuf, bool isStderr)
{
	CRichEditCtrl& edit = GetRichEditCtrl();
	edit.SetSel(-1, -1);

	if( numbuf <= 0 )
	{
		return;
	}

	WCHARFORMAT cf;
	cf.cbSize = sizeof(WCHARFORMAT);
	cf = GetCharFormatSelection();

	bool colorChanged = false;
	COLORREF oldColor = cf.crTextColor;
	DWORD oldEffect = cf.dwEffects;
	CString strline(buf, numbuf);

	if( strline.GetLength() >= 5 && strline[1] == ' ' &&
		(strline[0] == 'M' || strline[0] == 'C' || strline[0] == 'U' ||
		strline[0] == 'P' || strline[0] == 'N' || strline[0] == 'A' ||
		strline[0] == 'R' || strline[0] == 'I' || strline[0] == '?' ||
		strline[0] == '<' || strline[0] == '>') )
	{
		colorChanged = true;

		switch(strline[0])
		{
		default:
		case '?' :
		case 'I' :
			cf.crTextColor = RGB(0x80, 0x80, 0x00);
			break;
		case 'U' :
		case 'N' :
			cf.crTextColor = RGB(0x00, 0xA0, 0x00);
			break;
		case 'M' :
		case 'A' :
		case 'R' :
			cf.crTextColor = RGB(0xFF, 0x00, 0xFF);
			break;
		case 'C' :
		case '<' :
			cf.crTextColor = RGB(0xFF, 0x00, 0x00);
			break;
		case '>' :
		case 'P' :
			cf.crTextColor = RGB(0x00, 0x00, 0xFF);
			break;
		}

		cf.dwMask = CFM_COLOR;
		cf.dwEffects = 0;
		SetCharFormat(cf);
	}
	else if( isStderr )
	{
		colorChanged = true;
		cf.crTextColor = RGB(0xFF, 0x80, 0x00);
		cf.dwMask = CFM_COLOR;
		cf.dwEffects = 0;
		SetCharFormat(cf);
	}

	edit.ReplaceSel(strline);

	if( colorChanged )
	{
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = oldColor;
		cf.dwEffects = oldEffect;
		SetCharFormat(cf);
	}
}

/*!
	Get current format of the text
	\param jumpEnd true to select all
	\return The current format of the text
	\note This call puts the cursor at the end of the text by default
*/
WCHARFORMAT CWincvsView::GetCurFormat(bool jumpEnd /*= true*/)
{
	CRichEditCtrl& edit = GetRichEditCtrl();
	
	if( jumpEnd )
		edit.SetSel(-1, -1);

	return GetCharFormatSelection();
}

/*!
	Print colorized console output
	\param txt Text to print
	\param len Length of the text to print
*/
void CWincvsView::OutColor(const char* txt, long len)
{
	// Synchronize
	CSingleLock lock(&m_syncObject);
	if( !lock.Lock(VIEW_LOCK_TIMEOUT) )
	{
		ASSERT(FALSE); // Lock failed
		return;
	}
	
	CString strText;
	for(int index = 0; index < len; index++)
	{
		char chr = *txt++;
		if( chr == 0x0d || chr == 0x0a )
		{
			strText += (char)0x0d;
			strText += (char)0x0a;
			
			// special case if we got already \r\n
			if( (index + 1) < len && chr == 0x0d && txt[0] == 0x0a )
				index++;
		}
		else
		{
			strText += chr;
		}
	}

	CRichEditCtrl& edit = GetRichEditCtrl();
	edit.ReplaceSel(strText);
}

/*!
	Set the console color
	\param color Console color to be set
*/
void CWincvsView::OutColor(kConsoleColor color)
{
	// Synchronize
	CSingleLock lock(&m_syncObject);
	if( !lock.Lock(VIEW_LOCK_TIMEOUT) )
	{
		ASSERT(FALSE); // Lock failed
		return;
	}
	
	CRichEditCtrl& edit = GetRichEditCtrl();

	WCHARFORMAT cf = GetCurFormat(false);
	cf.dwEffects = 0;

	switch(color)
	{
	default:
		return;
		break;
	case kBrown :
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(0x80, 0x80, 0x00);
		break;
	case kGreen :
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(0x00, 0xA0, 0x00);
		break;
	case kMagenta :
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(0xFF, 0x00, 0xFF);
		break;
	case kRed :
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(0xFF, 0x00, 0x00);
		break;
	case kBlue :
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = RGB(0x00, 0x00, 0xFF);
		break;
	case kBold :
		cf.dwMask = CFM_BOLD;
		cf.dwEffects = CFE_BOLD;
		break;
	case kItalic :
		cf.dwMask = CFM_ITALIC;
		cf.dwEffects = CFE_ITALIC;
		break;
	case kUnderline :
		cf.dwMask = CFM_UNDERLINE;
		cf.dwEffects = CFE_UNDERLINE;
		break;
	}

	SetCharFormat(cf);
}

/*!
	Set the console character format
	\param format Format to be set
*/
void CWincvsView::OutColor(WCHARFORMAT& format)
{
	// Synchronize
	CSingleLock lock(&m_syncObject);
	if( !lock.Lock(VIEW_LOCK_TIMEOUT) )
	{
		ASSERT(FALSE); // Lock failed
		return;
	}
	
	CRichEditCtrl& edit = GetRichEditCtrl();
	SetCharFormat(format);
}

/*!
	Print the text to the console
	\param txt Text to print
	\param len Length of text to print
	\param isStderr true for <b>stderr</b>, false otherwise
*/
void CWincvsView::OutConsole(const char* txt, size_t len, bool isStderr /*= false*/)
{
	// Synchronize
	CSingleLock lock(&m_syncObject);
	if( !lock.Lock(VIEW_LOCK_TIMEOUT) )
	{
		ASSERT(FALSE); // Lock failed
		return;
	}

	if( len == 0 )
		return;

	if( m_sbuf == 0L )
	{
		m_sbuf = (char*)malloc((MAX_CHAR_BY_LINE + 3) * sizeof(char));
		if( m_sbuf == 0L )
			return;
	}

	size_t index = 0;
	char chr;

	do{
		for(; index < len && m_snumbuf < MAX_CHAR_BY_LINE; m_snumbuf++)
		{
			chr = txt[index++];
			
			if( chr == 0x0d || chr == 0x0a )
			{
				// Newline
				break;
			}
			
			m_sbuf[m_snumbuf] = chr;
		}
		
		// Special case if we got already \r\n
		if( index < len && chr == 0x0d && txt[index] == 0x0a )
		{
			index++;
		}
		
		if( chr == 0x0d || chr == 0x0a || m_snumbuf == MAX_CHAR_BY_LINE )
		{
			m_sbuf[m_snumbuf++] = 0x0d;
			m_sbuf[m_snumbuf++] = 0x0a;
			m_sbuf[m_snumbuf++] = 0x00;
			
			OutLine(m_sbuf, m_snumbuf, isStderr);
			m_snumbuf = 0;
		}
	}while( index < len );
}

/////////////////////////////////////////////////////////////////////////////
// CWincvsView diagnostics

#ifdef _DEBUG
void CWincvsView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CWincvsView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CWincvsDoc* CWincvsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWincvsDoc)));
	return (CWincvsDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWincvsView message handlers

/// WM_COMMAND(ID_FORMAT_FONT) message handler
void CWincvsView::OnMyFormatFont() 
{
	CRichEditCtrl& edit = GetRichEditCtrl();
	edit.SetSel(0, -1);
	
	CRichEditView::OnFormatFont();
	edit.SetSel(-1, -1);
	
	gViewFont = CCharFormat(GetCharFormatSelection());
	gCvsPrefs.save();
}

/// WM_KEYDOWN message handler, process the Enter key
void CWincvsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);

	if( nChar != VK_RETURN )
	{
		return;
	}

	long len = GetTextLength();
	if( len == 0 )
		return;

	TCHAR* buf = (char*)malloc((len + 1) * sizeof(TCHAR));
	if( buf == 0L )
		return;

	TEXTRANGE textRange;
	textRange.chrg.cpMin = 0;
	textRange.chrg.cpMax = len;
	textRange.lpstrText = buf;
	GetRichEditCtrl().SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange);

	long nStartChar, nEndChar;
	GetRichEditCtrl().GetSel(nStartChar, nEndChar);

	if( nStartChar == nEndChar ) // i.e. there is no selection
	{
		// Get the end and the beginning of the line
		if( nStartChar > 0 && buf[nStartChar] == '\r' )
			nStartChar--;

		while( nStartChar > 0 && buf[nStartChar] != '\n' && buf[nStartChar] != '\r' )
			nStartChar--;

		if( buf[nStartChar] == '\n' || buf[nStartChar] == '\r' )
			nStartChar++;

		while( nEndChar < len && buf[nEndChar] != '\n' && buf[nEndChar] != '\r' )
			nEndChar++;
	}

	long length = nEndChar - nStartChar;
	if( length > 0 && nStartChar < len )
	{
		OutConsole("\n", 1);
		buf[nEndChar] = '\0';
		
		CWincvsApp* app = (CWincvsApp*)AfxGetApp();

		if( gCvsPrefs.GetUseShell() == SHELL_PYTHON && app->HasPython() )
		{
			CPython_Interp interp;
			interp.DoScript(buf + nStartChar);
		}
		else if( gCvsPrefs.GetUseShell() == SHELL_TCL && app->HasTCL() )
		{
			CTcl_Interp interp;
			interp.DoScript(buf + nStartChar);
		}
	}

	free(buf);
}

/// QueryAcceptData virtual override
HRESULT CWincvsView::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT* lpcfFormat, DWORD dwReco, BOOL bReally, HGLOBAL hMetaPict)
{
	ASSERT(lpcfFormat != NULL);
	if( !bReally ) // not actually pasting
		return S_OK;

	// If direct pasting a particular native format allow it
	if( IsRichEditFormat(*lpcfFormat) )
		return S_OK;

	COleDataObject dataobj;
	dataobj.Attach(lpdataobj, FALSE);

	// If format is 0, then force particular formats if available
	if( *lpcfFormat == 0 && (m_nPasteType == 0) )
	{
		if( m_fRtfFormat != 0 && dataobj.IsDataAvailable(m_fRtfFormat)) // native avail, let rich edit do as it wants
		{
			*lpcfFormat = m_fRtfFormat;
			return S_OK;
		}
		else if (dataobj.IsDataAvailable(CF_TEXT))
		{
			*lpcfFormat = CF_TEXT;
			return S_OK;
		}
	}

	// *DO NOT* paste OLE formats
	// Crash reported by Gerhard Möller
	//DoPaste(dataobj, *lpcfFormat, hMetaPict);

	return S_FALSE;
}

/*!
	Check whether the clipboard contains a supported data format for paste
	\return If clipboard contains supported data format, false otherwise
*/
BOOL CWincvsView::CanPaste() const
{
	return (CountClipboardFormats() != 0) &&
		(IsClipboardFormatAvailable(CF_TEXT) ||
		IsClipboardFormatAvailable(m_fRtfFormat) ||
		//IsClipboardFormatAvailable(_oleData.cfEmbedSource) ||
		//IsClipboardFormatAvailable(_oleData.cfEmbeddedObject) ||
		//IsClipboardFormatAvailable(_oleData.cfFileName) ||
		//IsClipboardFormatAvailable(_oleData.cfFileNameW) ||
		//IsClipboardFormatAvailable(CF_METAFILEPICT) ||
		//IsClipboardFormatAvailable(CF_DIB) ||
		//IsClipboardFormatAvailable(CF_BITMAP) ||
		GetRichEditCtrl().CanPaste());
}

/// WM_DROPFILES message handler
void CWincvsView::OnDropFiles(HDROP hDropInfo) 
{	
	CWnd* pWnd = AfxGetMainWnd();	//look in InitInstance m_pMainWnd = m_mainFrame;

	if( pWnd )
	{
		CMainFrame* pMainFrame = (CMainFrame*)pWnd;
		
		if( pMainFrame->GetSafeHwnd() != NULL )
		{
			pMainFrame->DropFiles( hDropInfo );
		}
	}

	//this cause the crash, remove
	//CRichEditView::OnDropFiles(hDropInfo);
}

/// WM_COMMAND(ID_EDIT_CLEAR_ALL) message handler, clear the selection
void CWincvsView::OnEditClearAll() 
{
	CRichEditCtrl& edit = GetRichEditCtrl();
	edit.SetSel(0, -1);
	SetCharFormat((CCharFormat)gViewFont);
	edit.ReplaceSel("", TRUE);
}

/// WM_RBUTTONDOWN message handler, call CWincvsView::OnContextMenu
void CWincvsView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ClientToScreen(&point);
	OnContextMenu(this, point);

	//CRichEditView::OnRButtonDown(nFlags, point);
}

/// WM_CONTEXTMENU message handler, display the custom menu
void CWincvsView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu* pSubMenu = ::GetSubCMenu(ID_EDIT_UNDO, this);
	if( pSubMenu )
	{
		// Set the track point in case it's not the mouse click
		if( -1 == point.x && -1 == point.y )
		{
			// Set the top left corner
			point.x = 0;
			point.y = 0;
			ClientToScreen(&point);
		}
		
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

/// WM_GETDLGCODE message handler
UINT CWincvsView::OnGetDlgCode() 
{
	LRESULT DlgCode = CRichEditView::OnGetDlgCode();

	if( 0x8000 & GetKeyState(VK_TAB) )
		return DlgCode & ~(DLGC_WANTTAB | DLGC_WANTALLKEYS | DLGC_WANTCHARS);
	else
		return DlgCode;
}

/// OnActivateView virtual override, make accelerators work
void CWincvsView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// Trick the frame into believing there is no active view
	// Need this for MDI messages (Ctrl+Tab, Ctrl+F4, ...) to be translated correctly
	if( bActivate )
	{
		if( CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd() )
		{
			pMainFrame->SetActiveView(NULL);
		}
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}