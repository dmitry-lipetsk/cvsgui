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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- June 1998
 */

/*
 * CTreeView.h --- main tree view
 */

#pragma once

#include <LView.h>
#include "CPStr.h"

class LStaticText;
class CLogNode;
class EntnodeData;
class CMacLogData;

class CTreeView : public LView, public LCommander
{
public:
	enum { class_ID = 'TREW' };

	CTreeView( LStream *inStream );
	virtual ~CTreeView();

	void SetNode(CLogNode *node, const char *dir);
		// change the node visualized

	virtual Boolean		ObeyCommand(
								CommandT			inCommand,
								void				*ioParam = nil);
	virtual void		FindCommandStatus(
								CommandT			inCommand,
								Boolean				&outEnabled,
								Boolean				&outUsesMark,
								UInt16				&outMark,
								Str255				outName);
	virtual Boolean		HandleKeyPress( const EventRecord& inKeyEvent );
		// from LCommander
	
	virtual void DrawSelf(void);	
	void ClickSelf(const SMouseDownEvent & inMouseDown);
		// from LPane

  void ScrollNodeIntoView(CMacLogData* inNodeToDisplay);
  
protected:
	void CalcImageSize(void);
		// process the tree and find the bounds of it
	
	virtual void	ActivateSelf();
	virtual void	DeactivateSelf();
		// from LPane

	CLogNode *fNode;
	EntnodeData *fData;
	CStr fDir;
	CStr fName;
	
	void SelectNode(CMacLogData* inNode, bool inSelect);
	
private:
	// defensive programming
	CTreeView();
	CTreeView( const CTreeView &inOriginal );
	CTreeView& operator=( const CTreeView &inOriginal );
};
