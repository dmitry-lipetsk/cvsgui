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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- April 1998
 */

/*
 * MacBinEncoding.h --- class to store the mac binary encoding mappings
 */

#ifndef MACBINENCODING_H
#define MACBINENCODING_H

#include <LOutlineTable.h>
#include <LCommander.h>
#include <LOutlineItem.h>
#include <LString.h>

#include "MacBinMaps.h"

class CMacBinTable : public LOutlineTable, public LCommander
{
public:
	enum { class_ID = 'MBTA' };

	CMacBinTable( LStream *inStream );
	virtual ~CMacBinTable();
	
	static void SetCurrentMap(MacBinMaps *map) {gCurMacBinMap = map;}
	static MacBinMaps * GetCurrentMap(void) {return gCurMacBinMap;}
protected:
	static MacBinMaps *gCurMacBinMap;

	virtual	void FinishCreateSelf();
	
private:
	// defensive programming
	CMacBinTable();
	CMacBinTable( const CMacBinTable &inOriginal );
	CMacBinTable&		operator=( const CMacBinTable &inOriginal );
};

class CMacBinItem : public LOutlineItem
{
public:
	CMacBinItem(const MACBINMAP & map);
	virtual ~CMacBinItem();
	
	virtual bool EditCell(void);
		// return false if user canceled
	virtual void RemoveCell(void);
	
protected:
	virtual void GetDrawContentsSelf(const STableCell& inCell, SOutlineDrawContents& ioDrawContents);
	virtual void DrawRowAdornments(const Rect& inLocalRowRect);
	virtual void DoubleClick(const STableCell& inCell, const SMouseDownEvent& inMouseDown,
		const SOutlineDrawContents&	inDrawContents, Boolean inHitText);
	
	MACBINMAP fMap;
private:
	// defensive programming
	CMacBinItem();
	CMacBinItem( const CMacBinItem &inOriginal );
	CMacBinItem&	operator=( const CMacBinItem &inOriginal );
};

#endif /* MACBINENCODING_H */
