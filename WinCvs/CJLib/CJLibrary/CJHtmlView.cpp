// CJHtmlView.cpp : implementation file
//
// This file is part of the CJLibrary for Visual C++ / MFC
// Copyright C 1998-1999 COdejock Software, All Rights Reserved.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 1, or (at your option)
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Iuri Apollonio's article 'Doc / View - 
// Using HtmlView apart from Tecno Preview' http://www.codeguru.com/doc_view/htmlview.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 2005/08/09 00:22:44 $
 * $Revision: 1.2 $
 * $Archive: /CodeJock/CJLibrary/CJHtmlView.cpp $
 *
 * $History: CJHtmlView.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 10:05a
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) to help to eliminate screen flicker.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:26p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 1:29a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:27p
 * Updated in $/CodeJockey/CJLibrary
 * Added vc5 compatibility.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/CJLibrary
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJHtmlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD AFXAPI _AfxRelease(LPUNKNOWN* lplpUnknown)
{
	ASSERT(lplpUnknown != NULL);
	if (*lplpUnknown != NULL)
	{
		DWORD dwRef = (*lplpUnknown)->Release();
		*lplpUnknown = NULL;
		return dwRef;
	}
	return 0;
}

// helper for reliable and small Release calls
DWORD AFXAPI _AfxRelease(LPUNKNOWN* plpUnknown);
#ifndef _DEBUG
// generate smaller code in release build
#define RELEASE(lpUnk) _AfxRelease((LPUNKNOWN*)&lpUnk)
#else
// generate larger but typesafe code in debug build
#define RELEASE(lpUnk) do \
	{ if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = NULL; } } while (0)
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView

BEGIN_MESSAGE_MAP(CCJHtmlView, CFormView)
	//{{AFX_MSG_MAP(CCJHtmlView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CCJHtmlView, CFormView)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 102 /* StatusTextChange */, OnStatusTextChange, VTS_BSTR)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 108 /* ProgressChange */, OnProgressChange, VTS_I4 VTS_I4)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 105 /* CommandStateChange */, OnCommandStateChange, VTS_I4 VTS_BOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 106 /* DownloadBegin */, OnDownloadBegin, VTS_NONE)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 104 /* DownloadComplete */, OnDownloadComplete, VTS_NONE)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 113 /* TitleChange */, OnTitleChange, VTS_BSTR)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 252 /* NavigateComplete2 */, NavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 250 /* BeforeNavigate2 */, BeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 112 /* PropertyChange */, OnPropertyChange, VTS_BSTR)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 251 /* NewWindow2 */, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 259 /* DocumentComplete */, DocumentComplete, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 253 /* OnQuit */, OnQuit, VTS_NONE)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 254 /* OnVisible */, OnVisible, VTS_BOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 255 /* OnToolBar */, OnToolBar, VTS_BOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 256 /* OnMenuBar */, OnMenuBar, VTS_BOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 257 /* OnStatusBar */, OnStatusBar, VTS_BOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 258 /* OnFullScreen */, OnFullScreen, VTS_BOOL)
	ON_EVENT(CCJHtmlView, AFX_IDW_PANE_FIRST, 260 /* OnTheaterMode */, OnTheaterMode, VTS_BOOL)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView construction/destruction

CCJHtmlView::CCJHtmlView()
	: CFormView((LPCTSTR) NULL)
{
	m_pBrowserApp = NULL;
}

CCJHtmlView::~CCJHtmlView()
{
	if (m_pBrowserApp != NULL)
		m_pBrowserApp->Release();
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView drawing

void CCJHtmlView::OnDraw(CDC* /* pDC */)
{
	// this class should never do its own drawing;
	// the browser control should handle everything

	ASSERT(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView printing

void CCJHtmlView::OnFilePrint()
{
	// get the HTMLDocument

	if (m_pBrowserApp != NULL)
	{
		LPOLECOMMANDTARGET lpTarget = NULL;
		LPDISPATCH lpDisp = GetHtmlDocument();

		if (lpDisp != NULL)
		{
			// the control will handle all printing UI

			if (SUCCEEDED(lpDisp->QueryInterface(IID_IOleCommandTarget,
					(LPVOID*) &lpTarget)))
			{
				lpTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL, NULL);
				lpTarget->Release();
			}
			lpDisp->Release();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView diagnostics

#ifdef _DEBUG
void CCJHtmlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCJHtmlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView message handlers

void CCJHtmlView::OnDestroy()
{
	RELEASE(m_pBrowserApp);
}

void CCJHtmlView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndBrowser.m_hWnd))
	{
		// need to push non-client borders out of the client area
		CRect rect;
		GetClientRect(rect);
		::AdjustWindowRectEx(rect,
			m_wndBrowser.GetStyle(), FALSE, WS_EX_CLIENTEDGE);
		m_wndBrowser.SetWindowPos(NULL, rect.left, rect.top,
			rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CCJHtmlView::OnPaint()
{
	Default();
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView operations

BOOL CCJHtmlView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
						DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
						UINT nID, CCreateContext* pContext)
{
	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,
				dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

	AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView properties

CString CCJHtmlView::GetType() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_Type(&bstr);
	CString retVal(bstr);
	return retVal;
}

long CCJHtmlView::GetLeft() const
{
	ASSERT(m_pBrowserApp != NULL);

	long result;
	m_pBrowserApp->get_Left(&result);
	return result;
}


long CCJHtmlView::GetTop() const
{
	ASSERT(m_pBrowserApp != NULL);
	long result;
	m_pBrowserApp->get_Top(&result);
	return result;
}

int CCJHtmlView::GetToolBar() const
{
	ASSERT(m_pBrowserApp != NULL);
	int result;
	m_pBrowserApp->get_ToolBar(&result);
	return result;
}

long CCJHtmlView::GetHeight() const
{
	ASSERT(m_pBrowserApp != NULL);
	long result;
	m_pBrowserApp->get_Height(&result);
	return result;
}

BOOL CCJHtmlView::GetVisible() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Visible(&result);
	return result;
}

CString CCJHtmlView::GetLocationName() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_LocationName(&bstr);
	CString retVal(bstr);
	return retVal;
}

CString CCJHtmlView::GetLocationURL() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_LocationURL(&bstr);
	CString retVal(bstr);
	return retVal;
}

BOOL CCJHtmlView::GetBusy() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Busy(&result);
	return result;
}

READYSTATE CCJHtmlView::GetReadyState() const
{
	ASSERT(m_pBrowserApp != NULL);

	READYSTATE result;
	m_pBrowserApp->get_ReadyState(&result);
	return result;
}

BOOL CCJHtmlView::GetOffline() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Offline(&result);
	return result;
}

BOOL CCJHtmlView::GetSilent() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Silent(&result);
	return result;
}

LPDISPATCH CCJHtmlView::GetApplication() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Application(&result);
	return result;
}


LPDISPATCH CCJHtmlView::GetParentBrowser() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Parent(&result);
	return result;
}

LPDISPATCH CCJHtmlView::GetContainer() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Container(&result);
	return result;
}

LPDISPATCH CCJHtmlView::GetHtmlDocument() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Document(&result);
	return result;
}

BOOL CCJHtmlView::GetTopLevelContainer() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_TopLevelContainer(&result);
	return result;
}

BOOL CCJHtmlView::GetMenuBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_MenuBar(&result);
	return result;
}

BOOL CCJHtmlView::GetFullScreen() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_FullScreen(&result);
	return result;
}

BOOL CCJHtmlView::GetStatusBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_StatusBar(&result);
	return result;
}

OLECMDF CCJHtmlView::QueryStatusWB(OLECMDID cmdID) const
{
	ASSERT(m_pBrowserApp != NULL);

	OLECMDF result;
	m_pBrowserApp->QueryStatusWB(cmdID, &result);
	return result;
}

void CCJHtmlView::ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt,
	VARIANT* pvaIn, VARIANT* pvaOut)
{
	ASSERT(m_pBrowserApp != NULL);

	m_pBrowserApp->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
}

BOOL CCJHtmlView::GetRegisterAsBrowser() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_RegisterAsBrowser(&result);
	return result;
}

BOOL CCJHtmlView::GetRegisterAsDropTarget() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_RegisterAsDropTarget(&result);
	return result;
}

BOOL CCJHtmlView::GetTheaterMode() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_TheaterMode(&result);
	return result;
}

BOOL CCJHtmlView::GetAddressBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_AddressBar(&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView operations

BOOL CCJHtmlView::LoadFromResource(LPCTSTR lpszResource)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

BOOL CCJHtmlView::LoadFromResource(UINT nRes)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

void CCJHtmlView::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */ ,
	LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
	DWORD dwPostDataLen /* = 0 */)
{
	CString strURL(lpszURL);
	BSTR bstrURL = strURL.AllocSysString();

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	m_pBrowserApp->Navigate(bstrURL,
		COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		vPostData,
		COleVariant(lpszHeaders, VT_BSTR));
}

#ifdef _VC_VERSION_5
void MakeOleVariant(COleVariant &ov, LPCITEMIDLIST pidl)
{
	AfxVariantInit(&ov);

	if (pidl != NULL)
	{
		// walk through entries in the list and accumulate their size

		UINT cbTotal = 0;
		SAFEARRAY *psa = NULL;
		LPCITEMIDLIST pidlWalker = pidl;

		while (pidlWalker->mkid.cb)
		{
			cbTotal += pidlWalker->mkid.cb;
			pidlWalker = (LPCITEMIDLIST)
				(((LPBYTE)pidlWalker) + pidlWalker->mkid.cb);
		}

		// add the base structure size
		cbTotal += sizeof(ITEMIDLIST);

		// get a safe array for them
		psa = SafeArrayCreateVector(VT_UI1, 0, cbTotal);

		// copy it and set members
		if (psa != NULL)
		{
			memcpy(psa->pvData, (LPBYTE) pidl, cbTotal);
			ov.vt = VT_ARRAY | VT_UI1;
			ov.parray = psa;
		}
	}
}
#endif

void CCJHtmlView::Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */)
{
	ASSERT(m_pBrowserApp != NULL);

#ifdef _VC_VERSION_5
	COleVariant vPIDL;
	MakeOleVariant(vPIDL, pIDL);
#else
	COleVariant vPIDL(pIDL);
#endif
	COleVariant empty;

	m_pBrowserApp->Navigate2(vPIDL,
		COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		empty, empty);
}

void CCJHtmlView::Navigate2(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */,
	LPVOID lpvPostData /* = NULL */, DWORD dwPostDataLen /* = 0 */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	COleVariant vURL(lpszURL, VT_BSTR);
	COleVariant vHeaders(lpszHeaders, VT_BSTR);
	COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
	COleVariant vFlags((long) dwFlags, VT_I4);

	m_pBrowserApp->Navigate2(vURL,
		vFlags, vTargetFrameName, vPostData, vHeaders);
}

void CCJHtmlView::Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
	CByteArray& baPostData, LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleVariant vPostData = baPostData;
	COleVariant vURL(lpszURL, VT_BSTR);
	COleVariant vHeaders(lpszHeaders, VT_BSTR);
	COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
	COleVariant vFlags((long) dwFlags, VT_I4);

	ASSERT(m_pBrowserApp != NULL);

	m_pBrowserApp->Navigate2(vURL, vFlags, vTargetFrameName,
		vPostData, vHeaders);
}

void CCJHtmlView::PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue)
{
	ASSERT(m_pBrowserApp != NULL);

	CString strProp(lpszProperty);
	BSTR bstrProp = strProp.AllocSysString();
	m_pBrowserApp->PutProperty(bstrProp, vtValue);
	::SysFreeString(bstrProp);
}

BOOL CCJHtmlView::GetProperty(LPCTSTR lpszProperty, CString& strValue)
{
	ASSERT(m_pBrowserApp != NULL);

	CString strProperty(lpszProperty);
	BSTR bstrProperty = strProperty.AllocSysString();

	BOOL bResult = FALSE;
	VARIANT vReturn;
	vReturn.vt = VT_BSTR;
	vReturn.bstrVal = NULL;
	HRESULT hr = m_pBrowserApp->GetProperty(bstrProperty, &vReturn);

	if (SUCCEEDED(hr))
	{
		strValue = CString(vReturn.bstrVal);
		bResult = TRUE;
	}

	::SysFreeString(bstrProperty);
	return bResult;
}

COleVariant CCJHtmlView::GetProperty(LPCTSTR lpszProperty)
{
	COleVariant result;

	static BYTE parms[] =
		VTS_BSTR;
	m_wndBrowser.InvokeHelper(0x12f, DISPATCH_METHOD,
		VT_VARIANT, (void*)&result, parms, lpszProperty);

	return result;
}

CString CCJHtmlView::GetFullName() const
{
	ASSERT(m_pBrowserApp != NULL);

	BSTR bstr;
	m_pBrowserApp->get_FullName(&bstr);
	CString retVal(bstr);
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView event reflectors

void CCJHtmlView::NavigateComplete2(LPDISPATCH /* pDisp */, VARIANT* URL)
{
	ASSERT(V_VT(URL) == VT_BSTR);

	USES_CONVERSION;

	CString str = OLE2T(V_BSTR(URL));
	OnNavigateComplete2(str);
}

void CCJHtmlView::BeforeNavigate2(LPDISPATCH /* pDisp */, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	ASSERT(V_VT(URL) == VT_BSTR);
	ASSERT(V_VT(TargetFrameName) == VT_BSTR);
	ASSERT(V_VT(PostData) == (VT_VARIANT | VT_BYREF));
	ASSERT(V_VT(Headers) == VT_BSTR);
	ASSERT(Cancel != NULL);

	USES_CONVERSION;

	VARIANT* vtPostedData = V_VARIANTREF(PostData);
	CByteArray array;
	if (V_VT(vtPostedData) & VT_ARRAY)
	{
		// must be a vector of bytes
		ASSERT(vtPostedData->parray->cDims == 1 && vtPostedData->parray->cbElements == 1);

		vtPostedData->vt |= VT_UI1;
		COleSafeArray safe(vtPostedData);

		DWORD dwSize = safe.GetOneDimSize();
		LPVOID pVoid;
		safe.AccessData(&pVoid);

		array.SetSize(dwSize);
		LPBYTE lpByte = array.GetData();

		memcpy(lpByte, pVoid, dwSize);
		safe.UnaccessData();
	}
	// make real parameters out of the notification

	CString strTargetFrameName(V_BSTR(TargetFrameName));
	CString strURL = V_BSTR(URL);
	CString strHeaders = V_BSTR(Headers);
	DWORD nFlags = V_I4(Flags);

	// notify the user's class
	OnBeforeNavigate2(strURL, nFlags, strTargetFrameName,
		array, strHeaders, Cancel);
}

void CCJHtmlView::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	UNUSED_ALWAYS(pDisp);
	ASSERT(V_VT(URL) == VT_BSTR);

	CString str(V_BSTR(URL));
	OnDocumentComplete(str);
}

/////////////////////////////////////////////////////////////////////////////
// CCJHtmlView Events

void CCJHtmlView::OnProgressChange(long lProgress, long lProgressMax)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lProgress);
	UNUSED_ALWAYS(lProgressMax);
}

void CCJHtmlView::OnCommandStateChange(long lCommand, BOOL bEnable)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lCommand);
	UNUSED_ALWAYS(bEnable);
}

void CCJHtmlView::OnDownloadBegin()
{
	// user will override to handle this notification
}

void CCJHtmlView::OnDownloadComplete()
{
	// user will override to handle this notification
}

void CCJHtmlView::OnTitleChange(LPCTSTR lpszText)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszText);
}

void CCJHtmlView::OnPropertyChange(LPCTSTR lpszProperty)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszProperty);
}

void CCJHtmlView::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
	// default to continuing
	bCancel = FALSE;

	// user will override to handle this notification
	UNUSED_ALWAYS(ppDisp);
}

void CCJHtmlView::OnDocumentComplete(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
}

void CCJHtmlView::OnQuit()
{
	// user will override to handle this notification
}

void CCJHtmlView::OnVisible(BOOL bVisible)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bVisible);
}

void CCJHtmlView::OnToolBar(BOOL bToolBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bToolBar);
}

void CCJHtmlView::OnMenuBar(BOOL bMenuBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bMenuBar);
}

void CCJHtmlView::OnStatusBar(BOOL bStatusBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bStatusBar);
}

void CCJHtmlView::OnFullScreen(BOOL bFullScreen)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bFullScreen);
}

void CCJHtmlView::OnTheaterMode(BOOL bTheaterMode)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bTheaterMode);
}

void CCJHtmlView::OnNavigateComplete2(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
}

void CCJHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
	LPCTSTR lpszTargetFrameName, CByteArray& baPostData,
	LPCTSTR lpszHeaders, BOOL* bCancel)
{
	// default to continuing
	bCancel = FALSE;

	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(lpszTargetFrameName);
	UNUSED_ALWAYS(baPostData);
	UNUSED_ALWAYS(lpszHeaders);
}

void CCJHtmlView::OnStatusTextChange(LPCTSTR pszText)
{
	// try to set the status bar text via the frame

	CFrameWnd* pFrame = GetParentFrame();
	if (pFrame != NULL)
		pFrame->SetMessageText(pszText);
}

/////////////////////////////////////////////////////////////////////////////
// Helps to eliminate screen flicker.

BOOL CCJHtmlView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rectClient;
	GetClientRect( &rectClient );
	pDC->ExcludeClipRect( &rectClient );
	return CFormView::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// Pre-startup code

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNCREATE(CCJHtmlView, CFormView)
