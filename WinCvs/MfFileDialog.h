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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- December 2002
 */

/*
 * MfFileDialog.h --- class to pick set of files
 */

#if !defined(AFX_MFFILEDIALOG_H__66684391_7914_4A0D_9848_022A6CA7573C__INCLUDED_)
#define AFX_MFFILEDIALOG_H__66684391_7914_4A0D_9848_022A6CA7573C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MfFileDialog.h : header file
//

/// Support class to handle the selection buffer
class CPathBuffer
{
private:
	typedef CPathBuffer	self_type;

	CPathBuffer(const self_type&);
	self_type& operator = (const self_type&);

public:
	// Construction
	CPathBuffer();
	virtual ~CPathBuffer();

private:
	// Data members
	int m_bufferSize;	/*!< Buffer size */
	TCHAR* m_buffer;	/*!< Buffer to hold the selection */

public:
	// Interface
	inline void Clear();
	inline TCHAR* GetBuffer(int size = 0);

	void Set(const CString& s);
};

class MultiFiles;

/////////////////////////////////////////////////////////////////////////////
// CMfFileDialog dialog

/// Multi-selection file dialog wrapper class
class CMfFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CMfFileDialog)

public:
	CMfFileDialog(MultiFiles& mf,
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CMfFileDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	// Data members
	CPathBuffer m_filesBuffer;	/*!< Files buffer */
	CPathBuffer m_folderBuffer;	/*!< Folder buffer */

	MultiFiles& m_mf;			/*!< Selection */

	// Methods
	virtual void OnFileNameChange();

	/// Hide, use the CMfFileDialog::m_mf instead
	POSITION GetStartPosition () const;
	/// Hide, use the CMfFileDialog::m_mf instead
	CString GetNextPathName (POSITION& pos) const;

public:
	// Interface
	virtual INT_PTR DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFFILEDIALOG_H__66684391_7914_4A0D_9848_022A6CA7573C__INCLUDED_)
