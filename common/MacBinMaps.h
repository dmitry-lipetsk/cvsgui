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
 * MacBinMaps.h --- class to store the mac binary encoding mappings
 */

#ifndef MACBINMAPS_H
#define MACBINMAPS_H

#include "CvsPrefs.h"

typedef struct
{
#	define MAX_MACBIN_DESC 63
#	define MAX_MACBIN_EXTENSION 31
	OSType sig;
	char ext[MAX_MACBIN_EXTENSION + 1];
	char desc[MAX_MACBIN_DESC + 1];
	MACENCODING encod;
	bool useDefault;
} MACBINMAP;

typedef struct
{
# define  MACBINMAP_COOKIEVALUE  0xc001cafe
  unsigned long   cookie;
	int             numMaps;
	MACBINMAP maps[1];
} MACBINMAPENTRIES;

class MacBinMaps
{
public:
	MacBinMaps();
	~MacBinMaps();

	void LoadDefaults(void);
		// fill with the persistent defaults
	
	void SaveDefaults(void);
		// store as defaults
	
	const MACBINMAP *AddEntry(OSType sig, const char* extension, const char *desc, MACENCODING encod, bool useDefault);
		// an an entry
	
	void RemoveEntry(OSType sig, const char* extension);
		// remove an entry

	MacBinMaps & operator=(const MacBinMaps & otherMap);
		// replace by this map
	
	inline const MACBINMAPENTRIES * Entries(void) const {return entries;}
		// return the entries
	
	static const char *GetEnvPlainTypes(void);
		// return all the plain binaries types with a semi-colon
		// between each ones.
	
	static const char *GetEnvTypes(MACENCODING type);
		// return all the plain binaries types with a semi-colon
		// between each ones.
protected:
	void Reset(void);
	
	MACBINMAPENTRIES *entries;
};

#endif /* MACBINMAPS_H */
