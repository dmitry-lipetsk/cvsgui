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

#ifndef __MODEL__H__
#define __MODEL__H__

#include "ustr.h"
#include <vector>
#include <string>
#include <map>
#include "CvsIgnore.h"

#if qMacPP
#define CObject LPane
#endif

#ifdef qUnix
#define CObject void
#endif

#define FILTER_TOKEN_SEPARATORS	" \t;"

// Forward references 
class EntnodeData;
class KoColumnContext;

/// Typedef used to cast to proper notification handler
typedef void (*FNOBSERVER)(CObject*);

/// Filter type enum
enum{
	kFilterNone			= 0x0000,	/*!< No filter */

	kFilterChanged		= 0x0001,	/*!< Only changed files are to be displayed */
	kFilterAdded		= 0x0002,	/*!< Only added files are to be displayed */
	kFilterRemoved		= 0x0004,	/*!< Only removed files are to be displayed */
	kFilterConflicts	= 0x0010,	/*!< Conflicting files shall be displayed */

	kFilterMissing		= 0x0020,	/*!< Missing files have to be shown */
	kFilterHideMissing	= 0x0040,	/*!< Missing files have to be hidden */
	kFilterUnknown		= 0x0100,	/*!< Unknown files have to be shown */
	kFilterHideUnknown	= 0x0200,	/*!< Unknown files have to be hidden, mutually exclusive with kFilterMissing */

	// Combinations
	kFilterModified		= kFilterChanged | kFilterAdded | kFilterRemoved | kFilterConflicts	/*!< Flags indicating that file is modified and can be commited */
};


/// Notification manager
class NotificationManager
{
public:
	// Construction
	NotificationManager();

private:
	/// Node describing observer registration
	struct Node
	{
		CObject* observer;	/*!< Observer */  
		FNOBSERVER handler; /*!< Notification handler */

		Node();
		Node(CObject* o, FNOBSERVER h);
		Node(const Node& n);
		Node& operator=(const Node& n);
	};

private:
	// Data members
	std::vector<Node> m_nodes;	/*!< Nodes */
	
public:
	// Interface
	void CheckIn(CObject* observer, FNOBSERVER handler);
	void CheckOut(CObject* observer);
	void NotifyAll();
};

/// Filter's base class
class KiFilterBase
{
public:
	// Interface

	/// Load the filter settings
	virtual void LoadSettings() = 0;

	/// Save the filter settings
	virtual void SaveSettings() = 0;

	/// Retrieves notification manager for observing changes
	virtual NotificationManager* GetNotificationManager() = 0;
};

/// Interface to filtering entries
class KiFilterModel : public KiFilterBase
{
public:
	// Interface

	/// Checks to see if an item satisfies current filtering criteria
	virtual bool IsMatch(const EntnodeData* data) const = 0;

	/// Get the filters
	virtual int GetFilters() const = 0;
};

/// Interface to recursive display of entries
class KiRecursionModel : public KiFilterBase
{
public:
	// Interface

	/// Returns true if displaying files recursively
	virtual bool IsShowRecursive() = 0;
};

/// Interface to recursive display of entries
class KiIgnoreModel : public KiFilterBase
{
public:
	// Interface

	/// Returns true if displaying ignored files
	virtual bool IsShowIgnored() = 0;
};

/// Filter mask item
class KoFilterMask
{
public:
	// Construction

private:
	// Data members
	std::string m_mask;			/*!< Filter mask */
	ignored_list_type m_tokens;	/*!< Tokenized filter mask */

public:
	// Interface
	bool SetMask(const char* mask);
	bool IsMatch(const int type, const EntnodeData* data, const KoColumnContext* context) const;
	bool Empty() const;
};

/// Filter mask container
typedef std::map<int, KoFilterMask> FilterMaskContainer;

/// Interface to filtering entries
class KoFilterModel : public KiFilterModel
{
public:
	// Construction
	KoFilterModel();
	virtual ~KoFilterModel();

protected:
	// Data members
	int m_filters;						/*!< Filters */
	
	bool m_filterMasksEnable;			/*!< Flag to turn filter masks on or off */
	FilterMaskContainer m_filterMask;	/*!< Filter mask collection map */
	KoColumnContext* m_context;			/*!< Column context */

	NotificationManager m_manager;		/*!< Manager for filter change notifications */

	// Methods
	void ToggleFilter(int filter);
	bool HasFilter(const int filter) const;

public:
	// Interface
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual NotificationManager* GetNotificationManager();

	virtual bool IsMatch(const EntnodeData* data) const;
	virtual int GetFilters() const;

	void ToggleFilterMasksEnable();
	bool IsFilterMasksEnable() const;
	bool HasFilterMasks() const;

	bool IsDefaultMode() const;

	void ClearAll();
	bool HasFilters() const;
	bool HasAnyModified() const;

	bool SetMask(int type, const char* mask);
	void SetContext(const KoColumnContext* context);
	
	void ToggleModified();
	bool IsModified() const;

	void ToggleConflict();
	bool IsConflict() const;

	void ToggleUnknown();
	bool IsUnknown() const;

	void ToggleHideUnknown();
	bool IsHideUnknown() const;

	void ToggleMissing();
	bool IsMissing() const;

	void ToggleHideMissing();
	bool IsHideMissing() const;

	void ToggleChanged();
	bool IsChanged() const;

	void ToggleAdded();
	bool IsAdded() const;

	void ToggleRemoved();
	bool IsRemoved() const;
};

/// Interface to recursive display of entries
class KoRecursionModel : public KiRecursionModel
{
public:
	// Construction
	KoRecursionModel();
	virtual ~KoRecursionModel();

private:
	// Date members
	NotificationManager m_manager;	/*!< Manager for change notifications */
	bool m_isRecursive;				/*!< Flag to tell if files in sub-folders shall be iterated */

public:
	// Interface
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual NotificationManager* GetNotificationManager();

	virtual bool IsShowRecursive();

	void ToggleRecursion();
};

/// Interface for ignored entries
class KoIgnoreModel : public KiIgnoreModel
{
public:
	// Construction
	KoIgnoreModel();
	virtual ~KoIgnoreModel();

private:
	// Data members
	NotificationManager m_manager;	/*!< Manager for change notifications */
	bool m_isIgnore;				/*!< Flag to tell if ignored files are displayed */

public:
	// Interface
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual NotificationManager* GetNotificationManager();

	virtual bool IsShowIgnored();

	void ToggleIgnore();
};

#endif 
// __MODEL__H__
