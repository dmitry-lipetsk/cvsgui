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

#ifndef __BROWSEVIEW_COLUMN_H__
#define __BROWSEVIEW_COLUMN_H__

class EntnodeData;

#include "ustr.h"

/// Compare callback
#ifdef WIN32
int CALLBACK _Compare(LPARAM data1, LPARAM data2, LPARAM data);
#else
int _Compare(void* data1, void* data2, void* data);
#endif

#ifndef WIN32
/// Column data
struct LV_COLUMN
{
	int width;			/*!< Column width */
	const char* name;	/*!< Column name */
};
#endif

/// Context in which a column is accessed
class KoColumnContext
{
public:
	// Construction
	KoColumnContext(const char* path);

private:
	// Data members
	std::string m_path;	/*!< Current path */

public:
	// Interface
	const std::string& GetPath() const;
};

/// Column descriptor abstract class
class KiColumn 
{
public:
	// Interface

	/*!
		Get the column index
		\return The column index
	*/
	virtual int GetColumnIndex() const = 0;

	/*!
		Get the column setup data
		\param lvc Return column setup data
	*/
	virtual void GetSetupData(LV_COLUMN* lvc) const = 0;

	/*!
		Retrieve formatted text as pertained to a node
		\param context Column context
		\param data Node data
		\param buffer Return buffer
		\param bufferSize Return buffer size
	*/
	virtual void GetText(const KoColumnContext* context, const EntnodeData* data, char* buffer, const int bufferSize) const = 0;

	/*!
		Compare two nodes
		\param context Column context
		\param d1 Data of the first node
		\param d2 Data of the second node
		\return -1,0 or +1
	*/
	virtual int Compare(const KoColumnContext* context, const EntnodeData* d1, const EntnodeData* d2) const = 0;
	
	/*!
		Check whether the column shall be sorted ascending initially
		\return true if the column should be sorted ascending, false otherwise
		\note Override if the initial sorting order is not ascending
	*/
	virtual bool IsDefaultAscending() const
	{
		return true;
	}
};

/// Columnar model of the data		
class KiColumnModel
{
public:
	// Interface

	/*!
		Get the column descriptor by its index
		\param pos Index to get the column descriptor
		\return Column descriptor
	*/
	virtual const KiColumn* GetAt(const int pos) const = 0;

	/*!
		Get the column type by its index
		\param pos Index to get the column type
		\return The column type
	*/
	virtual int GetType(const int pos) const = 0;

	/*!
		Get the column count
		\return The column count
	*/
	virtual int GetCount() const = 0;

	virtual int GetColumn(const int type) const;

	// Factory methods
	static KiColumnModel* GetRecursiveModel();
	static KiColumnModel* GetRegularModel();
};

/// Sort parameters
struct CSortParam
{
	// Construction
	CSortParam(bool fAccendant, bool fGroupSubDir, 
		const KiColumn* column, const KiColumn* columnAlt, KoColumnContext* context) 
		: m_column(column), m_columnAlt(columnAlt), m_context(context), 
		m_fAccendant(fAccendant), m_fGroupSubDir(fGroupSubDir)
	{
	}

	// Data members
	const KiColumn* m_column;		/*!< Descriptor of the column being sorted */
	const KiColumn* m_columnAlt;	/*!< Descriptor of the alternate sorting column */
	KoColumnContext* m_context;		/*!< Column context */

	bool m_fAccendant;				/*!< Ascendant sorting */
	bool m_fGroupSubDir;			/*!< Group subdirectories */
};

#endif
// __BROWSEVIEW_COLUMN_H__
