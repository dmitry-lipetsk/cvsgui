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

// GraphDoc.h : interface of the CGraphDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHDOC_H__09F456C1_0018_11D2_BCAA_000000000000__INCLUDED_)
#define AFX_GRAPHDOC_H__09F456C1_0018_11D2_BCAA_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CLogNode;
class CWincvsView;
class CGraphView;
class EntnodeData;

/// Graph document class
class CGraphDoc : public CDocument
{
protected: // create from serialization only
	// Construction
	CGraphDoc();
	DECLARE_DYNCREATE(CGraphDoc)
public:
	virtual ~CGraphDoc();

// Attributes
public:

	// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGraphDoc)
	afx_msg void OnWindowCloseall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CGraphDoc)
	afx_msg long NumNodes();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
private:
	// Data members
	CLogNode* m_fNode;		/*!< Node data */
	CString m_fDir;			/*!< Directory */
	CString m_fName;		/*!< File name */
	EntnodeData* m_fData;	/*!< Node data */

	volatile bool m_reloading;	/*!< Flag to indicate that reloading is in progress */

	// Methods
	void Clear(void);

public:
	// Interface
	void SetNode(CLogNode* node, const char* dir);
	void SetReloading(const bool reloading);

	inline CLogNode* GetNode(void);
	inline EntnodeData* GetEntryInfo(void);
	inline const char* GetDir(void) const;
	inline const char* GetName(void) const;

	CGraphView* GetTreeView(void);
};

/// Get the node data
CLogNode* CGraphDoc::GetNode(void)
{
	return m_fNode;
}

/// Get the entry info
EntnodeData* CGraphDoc::GetEntryInfo(void)
{
	return m_fData;
}

/// Get the directory
const char* CGraphDoc::GetDir(void) const
{
	return m_fDir;
}

/// Get the file name
const char* CGraphDoc::GetName(void) const
{
	return m_fName;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHDOC_H__09F456C1_0018_11D2_BCAA_000000000000__INCLUDED_)
