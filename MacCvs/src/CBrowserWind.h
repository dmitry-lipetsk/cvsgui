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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 2000
 */

/*
 * CBrowserWind.h --- main browser class
 */

#pragma once

#include <LWindow.h>
#include "TextBinary.h"
#include "MultiFiles.h"

class KiSelectionHandler;
class EntnodeData;

// dummy class used to carry out the browser selection
class CBrowserCell
{
public:
	CBrowserCell() { }
	CBrowserCell(const UFSSpec & spec, EntnodeData *data) : fSpec(spec), fData(data) { }
	CBrowserCell(const char *name, EntnodeData *data) : fName(name), fData(data) { }
	
	CBrowserCell(const CBrowserCell & cell)
	{
		*this = cell;
	}
	
	CBrowserCell & operator=(const CBrowserCell & cell)
	{
		fSpec = cell.fSpec;
		fName = cell.fName;
		fData = cell.fData;
		return *this;
	}
	
	operator const char *(void) const { return fName; }
	
	CStr fName;
	UFSSpec fSpec;
	EntnodeData *fData;
};

class CBrowserWind : public LWindow, public LListener, public LPeriodical
{
public:
	enum { class_ID = 'CBWI' };

	CBrowserWind( LStream *inStream );
	virtual ~CBrowserWind();

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void				*ioParam);
	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

	virtual void	ListenToMessage(
							MessageT		inMessage,
							void*			ioParam);

	virtual	bool		LocateBrowser(
								const char* dir);

	virtual	void		SpendTime(
								const EventRecord&	inMacEvent);

	void UpdateBrowserMenu();

  static void RegisterClasses();
protected:
	static UStr sLastActivatedPath;
	
	// current selection in the window
	MultiFiles  m_mf;	// list of files (sorted by folder)
	std::vector<CBrowserCell> m_allFolders; // list of folders
	int         mCurrentFilterStyle;
	bool        mHandlingFilterChange;
	
	bool ProcessSelectionCommand(KiSelectionHandler & handler);

	virtual void FindCommandStatusBrowser(CommandT inCommand, Boolean &outEnabled,
									  Boolean &outUsesMark, UInt16 &outMark,
									  Str255 outName);
	virtual Boolean ObeyCommandBrowser(CommandT inCommand, void *ioParam);
		// for the browser selection

	virtual void	FinishCreateSelf();
	virtual void	ActivateSelf();
	virtual void	DeactivateSelf();

  virtual void CalcStandardBoundsForScreen(
                    	const Rect&	inScreenBounds,
                    	Rect&		outStdBounds) const;
};
