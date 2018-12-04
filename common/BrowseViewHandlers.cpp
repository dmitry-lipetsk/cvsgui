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
* Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> --- July 2002
*/

// BrowseViewHandlers.cpp : implementation file
//

#include "stdafx.h"

#ifdef WIN32
#include "wincvs.h"
#endif /* !WIN32 */

#include <string>

#include "BrowseViewHandlers.h"
#include "PromptFiles.h"
#include "MultiFiles.h"

using namespace std;

/*!
	Constructor
	\param strCommand Command name
	\param needSelection Need selection indicator
	\param needViewReset Need reset indicator
*/
KiSelectionHandler::KiSelectionHandler(const char* strCommand, bool needSelection /*= true*/, bool needViewReset /*= false*/)
	: m_strCommand(strCommand), m_pmf(NULL), m_selType(kSelNone), 
	  m_autoDelete(false), m_needSelection(needSelection), m_needViewReset(needViewReset)
{
}

KiSelectionHandler::~KiSelectionHandler()
{
	if( m_autoDelete && m_pmf )
	{
		delete m_pmf;
	}
}

/*!
	Get the selection type
	\return The selection type
*/
kSelType KiSelectionHandler::GetSelType() const
{
	return m_selType;
}

/*!
	Get the need selection flag
	\return	The need selection flag
*/
bool KiSelectionHandler::GetNeedSelection() const
{
	return m_needSelection;
}

/*!
	Get the need view reset
	\return	The need view reset flag
*/
bool KiSelectionHandler::GetNeedViewReset() const
{
	return m_needViewReset;
}

/*!
	Called for selected folder
	\param fullpath Directory path to add to selection
	\note Typically you should call the KiSelectionHandler::OnFolder in your override first
*/
void KiSelectionHandler::OnFolder(const char* fullpath)
{
	m_selType = kSelDir;
	if( fullpath )
	{
		m_pmf = new MultiFiles;
		if( m_pmf )
		{
			m_autoDelete = true;
			m_pmf->newdir(fullpath);
		}
	}
}

/*!
	Called for selected files
	\param pmf Files selection as MultiFiles
	\note Typically you should call the KiSelectionHandler::OnFiles in your override first
*/
void KiSelectionHandler::OnFiles(MultiFiles* pmf)
{
	m_selType = kSelFiles;
	m_pmf = pmf;
}

/*!
	Get the selection
	\return The selection as a pointer to MultiFiles object
	\note Prompts the user if necessary (controlled by KiSelectionHandler::m_needSelection flag)
*/
MultiFiles* KiSelectionHandler::GetSelection()
{
	if( !m_pmf && m_needSelection )
	{
		switch( m_selType )
		{
		case kSelFiles:
			m_pmf = new MultiFiles;
			if( m_pmf )
			{
				m_autoDelete = true;
				
				string strPrompt;
				strPrompt += _i18n("Select files for ");
				strPrompt += m_strCommand;
				
				if( !BrowserGetMultiFiles(strPrompt.c_str(), *m_pmf, false) )
				{
					delete m_pmf;
					m_pmf = NULL;
				}
			}
			break;
		case kSelDir:
			m_pmf = new MultiFiles;
			if( m_pmf )
			{
				string strPrompt;
				strPrompt += _i18n("Select directory to ");
				strPrompt += m_strCommand;
				
				string dir;
				if( BrowserGetDirectory(strPrompt.c_str(), dir) )
				{
					m_pmf->newdir(dir.c_str());
				}
				else
				{
					delete m_pmf;
					m_pmf = NULL;
				}
			}
			break;
		default:
#ifdef WIN32
			ASSERT(FALSE);	// type must be set to something if m_needSelection is true
#endif
			break;
		}
	}
	
	return m_pmf;
}

/*!
	Check whether the command needs the selection to be normalized
	\return true if commands needs normalization, false otherwise
*/
bool KiSelectionHandler::NeedsNormalize()
{
	return false;
}

/// Get the CVS console, returns NULL by default
CCvsConsole* KiSelectionHandler::GetConsole()
{
	return NULL;
}
