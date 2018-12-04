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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * CBrowserFileItem.h --- file item for the browser
 */

#pragma once

#include <LString.h>
#include "CBrowserTable.h"

class CBrowserFileItem : public CBrowserItem
{
public:
	CBrowserFileItem(EntnodeData *data);
	virtual ~CBrowserFileItem();

protected:
	virtual	void DetermineFileSize();

	virtual void GetDrawContentsSelf(const STableCell & inCell,
		SOutlineDrawContents & ioDrawContents);
	virtual void DrawRowAdornments(const Rect & inLocalRowRect);
	virtual void DrawCell(const STableCell & inCell, const Rect & inLocalCellRect);
		// from LOutlineItem

	Handle mStateH, mLockH;
	SInt32 mFileSize;
	LStr255 mSizeStr;
private:
	// defensive programming
	CBrowserFileItem();
	CBrowserFileItem(const CBrowserFileItem & inOriginal);
	CBrowserFileItem & operator=(const CBrowserFileItem & inOriginal);
};
