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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- February 2000
 */

// CPseudoButton window

#ifndef PSEUDOBUTTON_H
#define PSEUDOBUTTON_H

#include <vector>

typedef enum
{
	DD_END,
	DD_PIXMAP,	/* X11 pixmap (color indexed) */
	DD_BMAP,	/* X11 bitmap (B&W) */
	DD_RGB		/* RGB / char */
} UTB_KIND;

HBITMAP UMakeBitmap(const void *icon, int & width, int & height, UTB_KIND kind,
					COLORREF back, COLORREF fore, bool enabled = true);

HBITMAP UMakeBitmap(const void *icon, int width, int height, UTB_KIND kind, bool enabled = true);

class CPseudoButton : public CCJFlatButton
{
	DECLARE_DYNAMIC(CPseudoButton)

// Construction
public:
	CPseudoButton();

	virtual bool LoadXPM(int rsrcid);
		// return false if it fails

	static bool loadIcon(CString & icon, int rsrcid);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Attributes
protected:
	HBITMAP m_bitmap;
	HBITMAP m_disbitmap;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPseudoButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPseudoButton();

	// Generated message map functions
protected:
	virtual void DrawButtonBitmap(CDC* pDC, UINT nState, CRect& rcItem);

	//{{AFX_MSG(CPseudoButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////////

class CPseudoRotButton : public CPseudoButton
{
	DECLARE_DYNAMIC(CPseudoRotButton)

// Construction
public:
	CPseudoRotButton();

// Attributes
protected:
	std::vector<HBITMAP> m_bitmaps;
	std::vector<HBITMAP> m_disbitmaps;
	int m_curindex;

// Operations
public:
	virtual bool LoadXPMs(const int *rsrcid, int numrsrc);

	virtual void DoRotate(int index);
	inline int GetCurrent(void) const { return m_curindex; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPseudoRotButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPseudoRotButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPseudoRotButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////////

class CPseudoToggle : public CPseudoButton
{
	DECLARE_DYNAMIC(CPseudoToggle)

// Construction
public:
	CPseudoToggle();

// Attributes
protected:
	int m_flag;
	bool m_radio;
	CString m_group;

// Operations
public:
	~CPseudoToggle();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	// visual state (pushed/unpushed)
	virtual void SetState(int state);
	inline int GetState(void) const { return m_flag;}
	inline void ToggleBtn() { SetState(GetState() == 0 ? 1:0);}
	
	// radio button behaviour
	inline void SetRadio(bool state) { m_radio = state; }
	inline bool IsRadio(void) const  { return m_radio; }
	inline void SetGroup(const char *group) { m_group = group; }
	inline const char *GetGroup(void) const { return m_group; }

	//{{AFX_MSG(COglView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
