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
 * CBrowserDirItem.h --- directory item for the browser
 */

#pragma once

#include <time.h>
#include "CBrowserTable.h"

class CBrowserDirItem : public CBrowserItem
{
public:
	CBrowserDirItem( EntnodeData *data );
	virtual ~CBrowserDirItem();

	virtual Boolean CanExpand() const;

	void GetEntriesModTime(time_t & newEntriesMod, time_t & newEntriesLogMod);
	inline time_t EntriesModTime(void) {return fEntriesMod;}
	inline time_t EntriesLogModTime(void) {return fEntriesLogMod;}
		// we cache to mod. times of some CVS administration files
		// to see if we have to refresh the view.
protected:

	virtual void GetDrawContentsSelf(const STableCell & inCell,
		SOutlineDrawContents & ioDrawContents);
	virtual void DrawRowAdornments(const Rect & inLocalRowRect);
	virtual void DrawCell(const STableCell & inCell, const Rect & inLocalCellRect);
	virtual	void ExpandSelf();
		// from LOutlineItem
	
	time_t fEntriesMod;
	time_t fEntriesLogMod;
	std::vector<CStr> fIgnList;
	CStr fTag;
	
private:
	CBrowserDirItem();
	CBrowserDirItem(const CBrowserDirItem & inOriginal);
	CBrowserDirItem & operator=(const CBrowserDirItem & inOriginal);
};
