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
 * SortList.cpp --- sorted list using binary sort
 */

#ifndef SORTLIST_H
#define SORTLIST_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <assert.h>

// Note : this templates require to have a ref. counting
// and a correct operator= if the instancied class
// has some allocations inside.

typedef enum
{
	computeNone,
	computeLR
} computeSort;

template <class T> class CSortList
{
public :
	// method to sort elements (similar to qsort, return -1, 0, 1)
	typedef int (*QsortMethod)(const T &, const T &);

	CSortList(int def_size, QsortMethod method_sort)
	{
		assert(def_size > 1 && method_sort != 0L);

		fDef = def_size;
		fMethod = method_sort;
		fMax = fNum = 0;
		fElems = 0L;
		fNewOne = false;
	}

	~CSortList()
	{
		Reset();
	}

	// - insert or ignore (if already exists) an element in the list.
	// - return the location of the element in the list.
	int Insert(const T & entry, bool replaceIfAlreadyExists = false)
	{
		int index = Location(entry), i;
		
		if(!fNewOne)
		{
			if(replaceIfAlreadyExists)
				fElems[index] = entry;
			
			return index;
		}
			
		// check memory
		if(fNum == fMax)
		{
			T *newElems = 0L;

			try
			{
				newElems = new T[fMax + fDef];

				for(i = 0; i < fNum + 1; i++)
				{
					if(i < index)
						newElems[i] = fElems[i];
					else if(i == index)
						newElems[i] = entry;
					else
						newElems[i] = fElems[i - 1];
				}
			}
			catch(...)
			{
				if(newElems != 0L)
					delete[] newElems;
				throw;
			}
			
			fMax += fDef;
			fNum++;
			if(fElems != 0L)
				delete[] fElems;
			fElems = newElems;
			return index;
		}

		for(i = fNum - 1; i >= index; i--)
		{
			fElems[i + 1] = fElems[i];
		}

		fElems[index] = entry;
		fNum++;
		return index;
	}
	
	// say if this entry is already in the list
	// and return its index in the list (does not
	// change the row list).
	bool Inside(const T & entry, int *indexptr = 0L) const
	{
		int index = ((CSortList*)this)->Location(entry);
		
		if(indexptr != 0L)
			*indexptr = fNewOne ? -1 : index;
		
		return !fNewOne;
	}
	
	// - insert or ignore (if already exists) an element in the list.
	// - return true if the information was ALREADY in the sorted list.
	bool InsideAndInsert(const T & entry, int * indexptr = 0L,
		bool replaceIfAlreadyExists = false)
	{
		int index = Insert(entry, replaceIfAlreadyExists);
		if(indexptr != 0L)
			*indexptr = index;

		return !fNewOne;
	}

	// delete the entry from the sorted list which matches
	// this entry. Returns false if no matching entry was found.
	bool Delete(const T & entry)
	{
		int index = Location(entry), i;
		
		if(fNewOne)
			return false;

		if(fNum == 1)
		{
			Reset();
			return true;
		}

		for(i = index; i < fNum - 1; i++)
		{
			fElems[i] = fElems[i + 1];
		}
		--fNum;
		return true;
	}

	// return an allocated copy of the sorted list.
	T *Result(int *num_elem)
	{
		*num_elem = 0;
		
		if(fNum == 0)
			return 0L;

		*num_elem = fNum;
		T *res = new T[fNum];
		for(int i = 0; i < fNum; i++)
		{
			res[i] = fElems[i];
		}

		return res;
	}

	// return an element of the list.
	inline const T & Get(int index) const
	{
		return Entry(index);
	}

	// get ready for a new sorted list (another "Init" call is not necessary,
	// but you can do it to start another kind of sorted list)
	void Reset(void)
	{
		if(fElems != 0L)
			delete[] fElems;
		fMax = fNum = 0;
		fElems = 0L;
	}

	// # elements in the sorted list at this time
	inline int NumOfElements(void) {return fNum;}
private :
	QsortMethod fMethod;
	int fDef;		// default num of elem (ex: 100)
	int fNum;		// number of element
	int fMax;		// number of element allocated
	T *fElems;		// elements stored
	bool fNewOne;	// set by 'Location'

	// return the element pointer by index.
	inline const T & Entry(int num_elem) const
	{
		assert(num_elem >= 0 && num_elem < fNum);
		
		return fElems[num_elem];
	}

	// - return the location of the element in the
	// sorted list (does not change the list).
	int Location(const T & entry)
	{
		int left, right, cmp_left = 0, cmp_right = 0;
		int cmp_middle, middle;
		computeSort compute;
		
		if(fNum == 0)
		{
			fNewOne = true;
			return 0;
		}
		
		fNewOne = false;
		left = 0;
		right = fNum - 1;
		compute = computeLR;
		
		while(1)
		{
			middle = (left + right) / 2;
			
			if(compute == computeLR)
			{
				// firstime only
				cmp_left = fMethod(Entry(left), entry);
				cmp_right = fMethod(Entry(right), entry);
			}
			cmp_middle = fMethod(Entry(middle), entry);
			
			// entry already exist (E == L || E == R)
			if(cmp_left == 0 || cmp_right == 0)
			{
				return cmp_left == 0 ? left : right;
			}
			
			// found an entry superior to the right (E > R && E < R + 1)
			if(cmp_right < 0)
			{
				fNewOne = true;
				return right + 1;
			}
			
			// found an entry inferior to the left (L - 1 < E < L)
			if(cmp_left > 0)
			{
				fNewOne = true;
				return left;
			}
			
			// found an entry inferior to the right (E > R - 1 && E < R)
			if(middle == left)
			{
				fNewOne = true;
				return right;
			}
			
			// entry already exist (E == M)
			if(cmp_middle == 0)
				return middle;
			// restart search (E > M && E < R)
			else if(cmp_middle < 0)
			{
				compute = computeNone;
				left = middle;
				cmp_left = cmp_middle;
				cmp_right = 1;
			}
			// restart search (E > L && E < M)
			else
			{
				compute = computeNone;
				right = middle;
				cmp_right = cmp_middle;
				cmp_left = -1;
			}
		}
		assert(0);
		return 0;
	}
};

#endif // SORTLIST_H
