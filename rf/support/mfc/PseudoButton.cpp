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

///////////////////////////////////////////////////////////////

/*
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

#include "stdafx.h"
#include "umain.h"
#include "PseudoButton.h"
#include "dib.h"
#include <map>
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPseudoButton

IMPLEMENT_DYNAMIC(CPseudoButton, CCJFlatButton)

CPseudoButton::CPseudoButton() : m_bitmap(0L), m_disbitmap(0L)
{
}

CPseudoButton::~CPseudoButton()
{
	if(m_bitmap != 0L)
		AfxDeleteObject((HGDIOBJ*)&m_bitmap);
	if(m_disbitmap != 0L)
		AfxDeleteObject((HGDIOBJ*)&m_disbitmap);
}

void CPseudoButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	BOOL oldFlag = m_bFlatLook;
 	if(lpDIS->itemState & ODS_DISABLED)
		m_bFlatLook = true;
	CCJFlatButton::DrawItem(lpDIS);
	m_bFlatLook = oldFlag;
}

bool CPseudoButton::loadIcon(CString & icon, int rsrcid)
{
	icon = "";
	bool res = false;

	HINSTANCE inst = ::AfxGetResourceHandle();
	HRSRC hRsrc = FindResource(inst, MAKEINTRESOURCE(rsrcid), "CICN");
	if (hRsrc != 0L)
	{
		HGLOBAL hg = LoadResource(inst, hRsrc);
		if (hg != 0L)
		{
			DWORD size = SizeofResource(inst, hRsrc);
			char *rsrcptr = (char *)LockResource(hg);
			icon = CString(rsrcptr, size);
			FreeResource(hg);
			res = true;
		}
	}

	return res;
}

bool CPseudoButton::LoadXPM(int rsrcid)
{
	CString sicon;

	if(!CPseudoButton::loadIcon(sicon, rsrcid))
		return false;

	COLORREF back = ::GetSysColor(COLOR_BTNFACE);
	COLORREF fore = ::GetSysColor(COLOR_GRAYTEXT);

	int w, h;
	HBITMAP bmap = UMakeBitmap((const char *)sicon, w, h, DD_PIXMAP, back, fore);
	if(bmap == 0L)
		return false;

	SetBitmap(bmap);
	if(m_bitmap != 0L)
		AfxDeleteObject((HGDIOBJ*)&m_bitmap);
	m_bitmap = bmap;

	bmap = UMakeBitmap((const char *)sicon, w, h, DD_PIXMAP, back, fore, false);
	if(bmap == 0L)
		return false;
	if(m_disbitmap != 0L)
		AfxDeleteObject((HGDIOBJ*)&m_disbitmap);
	m_disbitmap = bmap;
	return true;
}

void CPseudoButton::DrawButtonBitmap(CDC* pDC, UINT nState, CRect& rcItem)
{
	// get the handle to the bitmap (if any) associated
	// with this button.
	HBITMAP hBitmap = GetBitmap();
	if( hBitmap == NULL ) {
		return;
	}

	CRect rc = rcItem;

	// if the button is selected.
	if( nState & ODS_SELECTED ) {
		rcItem.OffsetRect(1,1);
	}

	// draw the bitmap associated with this button.
	if((nState & ODS_DISABLED) != 0 && m_disbitmap != 0L)
		pDC->DrawState( rc.TopLeft(), rc.Size(), m_disbitmap, 
			DSS_NORMAL, (HBRUSH)NULL );
	else if(m_bitmap != 0L)
		pDC->DrawState( rc.TopLeft(), rc.Size(), m_bitmap, 
			DSS_NORMAL, (HBRUSH)NULL );
	else
		pDC->DrawState( rc.TopLeft(), rc.Size(), hBitmap, 
			( nState & ODS_DISABLED )?DSS_DISABLED:DSS_NORMAL, (HBRUSH)NULL );
}

BEGIN_MESSAGE_MAP(CPseudoButton, CCJFlatButton)
	//{{AFX_MSG_MAP(CPseudoButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPseudoRotButton

IMPLEMENT_DYNAMIC(CPseudoRotButton, CPseudoButton)

CPseudoRotButton::CPseudoRotButton() : m_curindex(-1)
{
}

CPseudoRotButton::~CPseudoRotButton()
{
	m_bitmap = 0L;
	m_disbitmap = 0L;
	std::vector<HBITMAP>::iterator i;
	for(i = m_bitmaps.begin(); i != m_bitmaps.end(); ++i)
	{
		HBITMAP bmap = *i;
		AfxDeleteObject((HGDIOBJ*)&bmap);
	}
	for(i = m_disbitmaps.begin(); i != m_disbitmaps.end(); ++i)
	{
		HBITMAP bmap = *i;
		AfxDeleteObject((HGDIOBJ*)&bmap);
	}
}

bool CPseudoRotButton::LoadXPMs(const int *rsrcid, int numrsrc)
{
	CString sicon;
	COLORREF back = ::GetSysColor(COLOR_BTNFACE);
	COLORREF fore = ::GetSysColor(COLOR_GRAYTEXT);

	for(int i = 0; i < numrsrc; i++)
	{
		if(!CPseudoButton::loadIcon(sicon, rsrcid[i]))
			return false;

		int w, h;
		HBITMAP bmap = UMakeBitmap((const char *)sicon, w, h, DD_PIXMAP, back, fore);
		if(bmap == 0L)
			return false;

		HBITMAP disbmap = UMakeBitmap((const char *)sicon, w, h, DD_PIXMAP, back, fore, false);
		if(disbmap == 0L)
			return false;

		m_bitmaps.push_back(bmap);
		m_disbitmaps.push_back(disbmap);
	}

	DoRotate(0);
	return true;
}

void CPseudoRotButton::DoRotate(int index)
{
	if(index < 0 || index > (int)m_bitmaps.size() || index > (int)m_disbitmaps.size() ||
		m_curindex == index)
		return;

	SetBitmap(m_bitmaps[index]);
	Invalidate(TRUE);
	m_curindex = index;
	m_bitmap = m_bitmaps[index];
	m_disbitmap = m_disbitmaps[index];
}

BEGIN_MESSAGE_MAP(CPseudoRotButton, CPseudoButton)
	//{{AFX_MSG_MAP(CPseudoRotButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CPseudoToggle, CPseudoButton)

CPseudoToggle::CPseudoToggle()
{	
	m_flag = 0;
	m_radio = false;
}

CPseudoToggle::~CPseudoToggle()
{	
}

void CPseudoToggle::SetState(int state) 
{
	int newstate = state ? 1 : 0;
	if(m_flag == newstate)
		return;

	m_flag = newstate;
	Invalidate(TRUE);

	if(state && IsRadio() && !m_group.IsEmpty())
	{
		CWnd *hwndChild, *parent = GetParent();
		for (hwndChild = parent->GetTopWindow(); hwndChild != 0L; hwndChild = hwndChild->GetNextWindow())
		{
			if(hwndChild != this && hwndChild->IsWindowEnabled() && hwndChild->IsKindOf(RUNTIME_CLASS(CPseudoToggle)))
			{
				CPseudoToggle *btn = (CPseudoToggle *)hwndChild;
				if(btn->IsRadio() && m_group == btn->GetGroup())
				{
					btn->SetState(0);
					break;
				}
			}
		}
	}
}

void CPseudoToggle::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{	
 	if(m_flag)
		lpDIS->itemState |= ODS_SELECTED;
	CPseudoButton::DrawItem(lpDIS);
}

void CPseudoToggle::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	GetClientRect(&rect);

	//check you are still on the button
	if(PtInRect(rect, point))
		ToggleBtn();

	CPseudoButton::OnLButtonUp(nFlags, point);
}

BEGIN_MESSAGE_MAP(CPseudoToggle, CPseudoButton)
    //{{AFX_MSG_MAP(CPseudoToggle)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static const BYTE *nextLine(const BYTE *src, CString & line)
{
	const char *tmp = strchr((const char *)src, '\n');
	if(tmp == 0L)
		return 0L;

	line = CString((const char *)src, tmp - (const char *)src - 1);
	src = (const BYTE *)tmp + 1;
	return src;
}

HBITMAP UMakeBitmap(const void *icon, int & width, int & height, UTB_KIND kind,
					COLORREF back, COLORREF fore, bool enabled)
{
	if( kind == DD_BMAP)
	{
		HBITMAP hbitmap;
		LPBITMAPINFO header;
		BYTE *image, *src = (BYTE *)icon, c, k;
		RGBQUAD *pRgb;

		hbitmap = CreateDIB(width, height, 1, 0L);
		header = (LPBITMAPINFO)GlobalLock(hbitmap);
		image = (LPBYTE)header + header->bmiHeader.biSize + PaletteSize(header);
		pRgb = (RGBQUAD *)((LPBYTE)header + header->bmiHeader.biSize);

		pRgb->rgbRed = GetRValue(back);
		pRgb->rgbGreen = GetGValue(back);
		pRgb->rgbBlue = GetBValue(back);
		pRgb->rgbReserved = 0;
		pRgb++;
		pRgb->rgbRed = GetRValue(fore);
		pRgb->rgbGreen = GetGValue(fore);
		pRgb->rgbBlue = GetBValue(fore);
		pRgb->rgbReserved = 0;

		int row = WIDTHBYTES(width * 1);
		int row2 = WIDTHBYTES8(width * 1);
		for(int y = 0; y < height; y++)
		{
			BYTE *ptr = image + row * (height - y - 1);
			for(int x = 0; x < row2; x++)
			{
				c = *src++;
				k = 0;
				int n = 8;
				while(n--)
				{
					if(c & 1)
						k |= 1 << n;
					c = c >> 1;
				}
				*ptr++ = k;
			}
		}

		GlobalUnlock(hbitmap);

		HBITMAP ddbbmap = DIBToBitmap(hbitmap);
		GlobalFree((HGLOBAL)hbitmap);

		return ddbbmap;
	}
	else if(kind == DD_PIXMAP)
	{
		const BYTE *src = (BYTE *)icon;
		CString aline;

		src = nextLine(src, aline);
		if(src == 0L)
			return 0L;

		// read the header
		int w, h, nc, cpp, i, x, y;
		if(sscanf(aline, "%d%d%d%d", &w, &h, &nc, &cpp) != 4)
			return 0L;

		width = w;
		height = h;

		HBITMAP hbitmap = CreateDIB(w, h, 8, 0L);
		LPBITMAPINFO header = (LPBITMAPINFO)GlobalLock(hbitmap);
		BYTE *image = (LPBYTE)header + header->bmiHeader.biSize + PaletteSize(header);
		RGBQUAD *pRgb = (RGBQUAD *)((LPBYTE)header + header->bmiHeader.biSize);

		// read the colormap
		std::map<short, BYTE> colors;
		for(i = 0; i < nc; i++)
		{
			src = nextLine(src, aline);
			if(src == 0L)
				return 0L;

			const BYTE *line = (const BYTE *)(const char *)aline;
			short aref;
			if(cpp == 1)
			{
				aref = line[0];
				line += 1;
			}
			else if(cpp == 2)
			{
				aref = ((short)line[1] << 8)  | line[0];
				line += 2;
			}
			else
			{
				ASSERT(0);
				return 0L;
			}

			while(isspace(*line) && *line)
				line++;
			if(*line++ != 'c')
				return 0L;

			while(isspace(*line) && *line)
				line++;
			if(*line == '\0')
				return 0L;

			if(*line == '#')
			{
				line++;

				if(strlen((char *)line) < 6)
					return 0L;

				int comp[6], c;
				int j;
				for(j = 0; j < 6; j++)
				{
					c = *line++;
					if(c >= '0' && c <= '9')
						comp[j] = c - '0';
					else if(c >= 'a' && c <= 'f')
						comp[j] = c - 'a' + 10;
					else if(c >= 'A' && c <= 'F')
						comp[j] = c - 'A' + 10;
					else
						return 0L;
				}

				colors.insert(std::make_pair(aref, i));

				if(i == 0)
				{
					COLORREF bk = back;
					pRgb->rgbRed = GetRValue(bk);
					pRgb->rgbGreen = GetGValue(bk);
					pRgb->rgbBlue = GetBValue(bk);
					pRgb->rgbReserved = 0;
				}
				else
				{
					COLORREF color = RGB(comp[0]*16+comp[1],comp[2]*16+comp[3],comp[4]*16+comp[5]);
					if(!enabled)
						color = fore;
					pRgb->rgbRed = GetRValue(color);
					pRgb->rgbGreen = GetGValue(color);
					pRgb->rgbBlue = GetBValue(color);
					pRgb->rgbReserved = 0;
				}
			}
			else if(strcmp((char *)line, "None") == 0)
			{
				colors.insert(std::make_pair(aref, i));
				COLORREF bk = back;
				pRgb->rgbRed = GetRValue(bk);
				pRgb->rgbGreen = GetGValue(bk);
				pRgb->rgbBlue = GetBValue(bk);
				pRgb->rgbReserved = 0;
			}
			else
				return 0L;
			pRgb++;
		}

		if(nc != colors.size() || nc > 256)
			return 0L;

		// make the bitmap
		for(y = 0; y < h; y++)
		{
			src = nextLine(src, aline);
			if(src == 0L)
				return 0L;

			const BYTE *line = (const BYTE *)(const char *)aline;
			BYTE *ptr = image + WIDTHBYTES(w * 8) * (h - y - 1);
			for(x = 0; x < w; x++)
			{
				short aref;
				if(cpp == 1)
				{
					aref = line[0];
					line++;
				}
				else if(cpp == 2)
				{
					aref = ((short)line[1] << 8)  | line[0];
					line += 2;
				}
				std::map<short, BYTE>::iterator pos = colors.find(aref);
				if(pos == colors.end())
					return 0L;
				*ptr++ = (*pos).second;
			}
		}

		GlobalUnlock(hbitmap);

		HBITMAP ddbbmap = DIBToBitmap(hbitmap);
		GlobalFree((HGLOBAL)hbitmap);

		return ddbbmap;
	}
	else if(kind == DD_RGB)
	{
		BYTE *src = (BYTE *)icon;

		// read the header
		int w = width, h = height, x, y;

		HBITMAP hbitmap = CreateDIB(w, h, 24, 0L);
		LPBITMAPINFO header = (LPBITMAPINFO)GlobalLock(hbitmap);
		BYTE *image = (LPBYTE)header + header->bmiHeader.biSize + PaletteSize(header);

		// make the bitmap
		for(y = 0; y < h; y++)
		{
#if 0
			BYTE *ptr = image + WIDTHBYTES(w * 24) * (h - y - 1);
#else
			BYTE *ptr = image + WIDTHBYTES(w * 24) * y;
#endif
			for(x = 0; x < w; x++)
			{
				ptr[2] = *src++;
				ptr[1] = *src++;
				ptr[0] = *src++;
				ptr += 3;
				src++;
			}
		}

		GlobalUnlock(hbitmap);

		HBITMAP ddbbmap = DIBToBitmap(hbitmap);
		GlobalFree((HGLOBAL)hbitmap);

		return ddbbmap;
	}

	return 0L;
}

HBITMAP UMakeBitmap(const void *icon, int width, int height, UTB_KIND kind, bool enabled)
{
	return UMakeBitmap(icon, width, height, kind,
		::GetSysColor(COLOR_BTNFACE), ::GetSysColor(COLOR_BTNTEXT), enabled);
}
