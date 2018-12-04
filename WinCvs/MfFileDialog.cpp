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
 * MfFileDialog.cpp --- class to pick set of files
 */

// MfFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "wincvs.h"
#include "MfFileDialog.h"
#include "MultiFiles.h"
#include "CvsCommands.h"

#include <cderr.h>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////////
// CPathBuffer

CPathBuffer::CPathBuffer()
{
	m_bufferSize = 0;
	m_buffer = NULL;
}

CPathBuffer::~CPathBuffer()
{
	Clear();
}

/// Release allocated buffer memory
void CPathBuffer::Clear()
{
	m_bufferSize = 0;
	delete [] m_buffer;
	m_buffer = NULL;
}

/*!
	Get the buffer of the specified size
	\param size Size of the required buffer, specify zero to get the current buffer
	\return The pointer to the buffer, NULL on error
*/
TCHAR* CPathBuffer::GetBuffer(int size /*= 0*/)
{
	if( size > m_bufferSize )
	{
		Clear();

		m_bufferSize = size;
		m_buffer = new TCHAR[size];
	}

	return m_buffer;
}

//------------------------------------------------------------------------
void CPathBuffer::Set(const CString& s)
{
 this->Clear();

 int const cc=s.GetLength();

 if(cc==0)
  return;

 m_buffer=new TCHAR[cc+1]; //throw, overflow?

 for(int i=0;i!=cc;++i)
  m_buffer[i]=s[i];

 m_buffer[cc]=0;

 m_bufferSize=cc+1;
}//Set

/////////////////////////////////////////////////////////////////////////////
// CMfFileDialog

IMPLEMENT_DYNAMIC(CMfFileDialog, CFileDialog)

CMfFileDialog::CMfFileDialog(MultiFiles& mf, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) : m_mf(mf), 
		CFileDialog(TRUE, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

BEGIN_MESSAGE_MAP(CMfFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CMfFileDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/// DoModal virtual override, overwrite FNERR_BUFFERTOOSMALL error and retrieve the selection
INT_PTR CMfFileDialog::DoModal()
{
	CWincvsApp::CModalGuard modalGuard;
	
	m_filesBuffer.Clear();
	m_folderBuffer.Clear();

	INT_PTR modalResult = CFileDialog::DoModal();
	if( IDCANCEL == modalResult )
	{
		// Override the "buffer too small" error since we provide our own buffer
		if( CommDlgExtendedError() == FNERR_BUFFERTOOSMALL )
		{
			modalResult = IDOK;
		}
	}

	if( IDOK == modalResult && m_filesBuffer.GetBuffer() && m_folderBuffer.GetBuffer() )
	{
		if( m_ofn.Flags & OFN_ALLOWMULTISELECT )
		{
			//! \todo
            //!  [2012-11-08] sf_animal: Seem, this code will be work incorrectly.

			m_mf.newdir(m_folderBuffer.GetBuffer());
			
			// Fill the selection
			string selFile;
			stringstream filesStream;
			filesStream << m_filesBuffer.GetBuffer();
			
			while( getline(filesStream, selFile, '\"') )
			{
				if( selFile.size() > 0 && !isspace(selFile.c_str()[0]) )
				{
					m_mf.newfile(selFile.c_str());
				}
			}
		}
		else
		{
			const CString strPathName = GetPathName();
			bool fileExists = true;

			if( !(m_ofn.Flags & OFN_PATHMUSTEXIST) )
			{
				struct stat st;
				if( stat(strPathName, &st) != 0 && errno == ENOENT )
				{
					fileExists = false;
				}
			}

			if( fileExists )
			{
				string uppath, file;
				if( SplitPath(strPathName, uppath, file) )
				{
					m_mf.newdir(uppath.c_str());
					m_mf.newfile(file.c_str());
				}
			}
			else
			{
				m_mf.newdir("");
				m_mf.newfile(GetFileName());
			}
		}
	}

	return modalResult;
}

/// OnFileNameChange virtual override, fill in the buffers with new selection
void CMfFileDialog::OnFileNameChange()
{
#if 1
	//[2012-11-08] sf_animal:
	// 1. Dirty solution the unexpected problem (this->GetParent() always returns NULL)
	//    with [VS2010, Win7 x64].

	m_filesBuffer.Set(this->GetFileName());
	m_folderBuffer.Set(this->GetFolderPath());

#else
	const CWnd* pParent = GetParent();
	if( pParent )
	{
		const HWND hParent = pParent->m_hWnd;
		
		// Get the buffer size required
		const int filesBufferSize = CommDlg_OpenSave_GetSpec(hParent, NULL, 0);
		const int folderBufferSize = CommDlg_OpenSave_GetFolderPath(hParent, NULL, 0);

		CommDlg_OpenSave_GetSpec(hParent, m_filesBuffer.GetBuffer(filesBufferSize), filesBufferSize);
		CommDlg_OpenSave_GetFolderPath(hParent, m_folderBuffer.GetBuffer(folderBufferSize), folderBufferSize);
	}
#endif
}//OnFileNameChange
