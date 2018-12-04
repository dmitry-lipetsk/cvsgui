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

#include "stdafx.h"

#include "BrowseViewModel.h"
#include "Persistent.h"
#include "CvsEntries.h"
#include "BrowseViewColumn.h"

#if qMacPP
#	include "MacMisc.h"
#endif

using namespace std;

static CPersistentBool gViewChanged("P_ViewChanged", false, kAddSettings);
static CPersistentBool gViewAdded("P_ViewAdded", false, kAddSettings);
static CPersistentBool gViewRemoved("P_ViewRemoved", false, kAddSettings);
static CPersistentBool gViewConflicts("P_ViewConflicts", false, kAddSettings);
static CPersistentBool gViewUnknown("P_ViewUnknown", false, kAddSettings);
static CPersistentBool gViewHideUnknown("P_ViewHideUnknown", false, kAddSettings);
static CPersistentBool gViewMissing("P_ViewMissing", false, kAddSettings);
static CPersistentBool gViewHideMissing("P_ViewHideMissing", false, kAddSettings);
static CPersistentBool gDisplayRecursive("P_DisplayRecursive", false, kAddSettings);
static CPersistentBool gFileViewIgnore("P_FileViewIgnore", true, kAddSettings);
static CPersistentBool gFilterMasksEnable("P_FilterMasksEnable", true, kAddSettings);

//////////////////////////////////////////////////////////////////////////
// NotificationManager

NotificationManager::NotificationManager()
{
}

/// Default constructor just zeroes everything
NotificationManager::Node::Node()
	: observer(NULL), handler(NULL)
{
}

/// Full-blown construction
NotificationManager::Node::Node(CObject* o, FNOBSERVER h)
	: observer(o), handler(h)
{
}

NotificationManager::Node::Node(const Node& n)
{
	observer = n.observer;
	handler = n.handler;
}

NotificationManager::Node& NotificationManager::Node::operator=(const Node& n)
{
	observer = n.observer;
	handler = n.handler;
	return *this;
}

/// Register an observer
void NotificationManager::CheckIn(CObject* observer, FNOBSERVER handler)
{
	m_nodes.push_back(Node(observer, handler));
}

/// Unregister an observer
void NotificationManager::CheckOut(CObject* observer)
{
	for(std::vector<Node>::iterator i = m_nodes.begin(); i != m_nodes.end(); i++)
	{
		if( (*i).observer == observer )
		{
			m_nodes.erase(i);
			break;
		}
	}
}

/// Fire up a notification once done with changes in observable
void NotificationManager::NotifyAll()
{
	// [2018-12-04]
	//  Iteration from forward to back may produce AV. Then will scan from back to front.

	for(std::vector<Node>::iterator i = m_nodes.end(); i != m_nodes.begin();)
	{
		--i;

		// Make a local copy so that observers can freely unregister during the process
		Node& node = *i;
		node.handler(node.observer);

		assert(i <= m_nodes.end());
		assert(i >= m_nodes.begin());
	} // for i
}

//////////////////////////////////////////////////////////////////////////
// KoFilterMask

/*!
	Set new mask
	\param mask Mask to be set
	\return true if mask was changed, false otherwise
*/
bool KoFilterMask::SetMask(const char* mask)
{
	const bool changed = m_mask.empty() || strcmp(m_mask.c_str(), mask) != 0;
	if( changed )
	{
		m_mask = mask;
		m_tokens.clear();
		
		if( !m_mask.empty() )
		{
			// Build token list 
			char* buffer = strdup(mask);
			static const char separators[] = FILTER_TOKEN_SEPARATORS;
			
			for(char* token = strtok(buffer, separators); token; token = strtok(NULL, separators))
			{
				m_tokens.push_back(string(token));
			}

			free(buffer);
		}		
	}
	
	if( m_mask.empty() )
	{
		m_tokens.clear();
	}

	return changed;
}

/*!
	Check whether the given node matches the mask
	\param type Type of match to test
	\param data Node to be tested
	\param context Column context
	\return true if node matches the mask, false otherwise
	\note All column types are matched in file-specific way temporarily (which is why it has redundant repetitions of code).
		  This ought to change as we will profile the matching algorithm to the specifics of each column
*/
bool KoFilterMask::IsMatch(const int type, const EntnodeData* data, const KoColumnContext* context) const
{
	bool res = false;

	switch( type )
	{
	case ColumnIndex::kName:
		if( MatchIgnoredList(data->GetName(), m_tokens) )
		{
			res = true;
		}
		break;
	case ColumnIndex::kExtention:
		if( MatchIgnoredList(data->GetExtension(), m_tokens) )
		{
			res = true;
		}
		break;
	case ColumnIndex::kRevision:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(((*node)[EntnodeFile::kVN]).c_str(), m_tokens) )
			{
				res = true;
			}
		}
		break;
	case ColumnIndex::kOption:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(((*node)[EntnodeFile::kOption]).c_str(), m_tokens) )
			{
				res = true;
			}
		}
		break;
	case ColumnIndex::kEncoding:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(((*node)[EntnodeFile::kEncoding]).c_str(), m_tokens) )
			{
				res = true;
			}
		}
		break;
	case ColumnIndex::kState:
		if( MatchIgnoredList(((*data)[EntnodeData::kState]).c_str(), m_tokens) )
		{
			res = true;
		}
		break;
	case ColumnIndex::kStickyTag:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(((*node)[EntnodeFile::kTag]).c_str(), m_tokens) )
			{
				res = true;
			}
		}
		break;
	case ColumnIndex::kTimeStamp:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(((*node)[EntnodeFile::kTS]).c_str(), m_tokens) )
			{
				res = true;
			}
		}
		break;
	case ColumnIndex::kConflict:
		if( MatchIgnoredList(data->GetConflict(), m_tokens) )
		{
			res = true;
		}
		break;
	case ColumnIndex::kInfo:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(((*node)[EntnodeFile::kInfo]).c_str(), m_tokens) )
			{
				res = true;
			}
		}
		break;
	case ColumnIndex::kPath:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(node->GetRelativePath(context), m_tokens) )
			{
				res = true;
			}
		}
		break;
	case ColumnIndex::kModTime:
		if( const EntnodeFile* node = dynamic_cast<const EntnodeFile*>(data) )
		{
			if( MatchIgnoredList(((*node)[EntnodeFile::kModTime]).c_str(), m_tokens) )
			{
				res = true;
			}
		}
		break;
	default:
		// Nothing to do
		break;
	}

	return res;
}
	
/*!
	Check whether the filter mask is empty
	\return true if filter mask is empty, false otherwise
*/
bool KoFilterMask::Empty() const
{
	return m_tokens.empty();
}

//////////////////////////////////////////////////////////////////////////
// KoFilterModel

KoFilterModel::KoFilterModel()
	: m_filters(kFilterNone), m_context(NULL)
{
}

KoFilterModel::~KoFilterModel()
{
	delete m_context;
}

void KoFilterModel::LoadSettings()
{
	m_filters = kFilterNone;

	m_filters |= gViewChanged		? kFilterChanged : 0;
	m_filters |= gViewAdded			? kFilterAdded : 0;
	m_filters |= gViewRemoved		? kFilterRemoved : 0;
	m_filters |= gViewConflicts		? kFilterConflicts : 0;
	m_filters |= gViewMissing		? kFilterMissing : 0;
	m_filters |= gViewHideMissing	? kFilterHideMissing : 0;
	m_filters |= gViewUnknown	    ? kFilterUnknown : 0;
	m_filters |= gViewHideUnknown	? kFilterHideUnknown : 0;
	
	m_filterMasksEnable = gFilterMasksEnable;
}

void KoFilterModel::SaveSettings()
{
	gViewChanged		= HasFilter(kFilterChanged);
	gViewAdded	 	= HasFilter(kFilterAdded);
	gViewRemoved		= HasFilter(kFilterRemoved);
	gViewConflicts		= HasFilter(kFilterConflicts);
	gViewMissing		= HasFilter(kFilterMissing);
	gViewHideMissing	= HasFilter(kFilterHideMissing);
	gViewUnknown		= HasFilter(kFilterUnknown);
	gViewHideUnknown	= HasFilter(kFilterHideUnknown);
	
	gFilterMasksEnable = m_filterMasksEnable;
}

NotificationManager* KoFilterModel::GetNotificationManager()
{
	return &m_manager;
}

/// Check whether the filter is in default mode - to show flags are set
bool KoFilterModel::IsDefaultMode() const
{
	return !((kFilterConflicts | kFilterChanged | kFilterMissing | kFilterAdded | kFilterRemoved) & m_filters);
}

bool KoFilterModel::IsMatch(const EntnodeData* data) const
{
	if( m_filterMasksEnable )
	{
		// Check to see if any filter matches
		for(FilterMaskContainer::const_iterator i = m_filterMask.begin(); i != m_filterMask.end(); ++i)
		{
			if( !(*i).second.Empty() && !(*i).second.IsMatch((*i).first, data, m_context) )
			{
				return false;
			}
		}
	}

	if( IsDefaultMode() && !HasFilter(kFilterUnknown) )
	{
		// Check if unknown files are to be hidden
		if( data->IsUnknown() && HasFilter(kFilterHideUnknown) )
		{
			// Unknown item match
			return false;
		}

		if( data->IsMissing() && HasFilter(kFilterHideMissing) )
		{
			// Hide missing item match
			return false;
		}
		
		return true;
	}
	
	if( data->IsRemoved() && HasFilter(kFilterRemoved) )
	{
		// Removed item match
		return true;
	}

	if( data->IsMissing() && !data->IsRemoved() && HasFilter(kFilterMissing) )
	{
		// Missing item match
		return true;
	}

	if( data->IsUnknown() && HasFilter(kFilterUnknown) )
	{
		// Unknown item match
		return true;
	}

	if( !IsNullOrEmpty(data->GetConflict()) && HasFilter(kFilterConflicts) )
	{
		// Conflict match
		return true;
	}

	if( !data->IsUnmodified() )
	{
		// Can be either changed or added 
		if( data->IsAdded() && HasFilter(kFilterAdded) )
		{
			// Added file match
			return true;
		}

		if( !data->IsAdded() && !data->IsRemoved() && HasFilter(kFilterChanged) && IsNullOrEmpty(data->GetConflict()) )
		{
			// Changed file match
			return true;
		}
	}

	// No match
	return false;
}

/*!
	Check whether any filters are active
	\return true if any filter is active, false otherwise
	\note File mask is not considered a filter
*/
bool KoFilterModel::HasFilters() const
{
	return kFilterNone != m_filters;
}

/*!
	Check whether any of the modified filters are active
	\return true if any modified filter is active, false otherwise
*/
bool KoFilterModel::HasAnyModified() const
{
	return kFilterNone != (kFilterModified & m_filters);
}

/*!
	Clear all filters
*/
void KoFilterModel::ClearAll()
{
	m_filters = kFilterNone;
}

/// Toggle modified flag
void KoFilterModel::ToggleModified()
{
	if( IsModified() )
	{
		m_filters &= ~kFilterModified;
	}
	else
	{
		m_filters |= kFilterModified;
	}
}

/// Get the modified flag
bool KoFilterModel::IsModified() const
{
	return (kFilterModified & m_filters) == kFilterModified;
}

int KoFilterModel::GetFilters() const
{
	return m_filters;
}

/// Toggle the filter masks enable flag
void KoFilterModel::ToggleFilterMasksEnable()
{
	m_filterMasksEnable = !m_filterMasksEnable;
}

/// Get the filter masks enable flag
bool KoFilterModel::IsFilterMasksEnable() const
{
	return m_filterMasksEnable;
}

/*!
	Check whether any file masks are present
	\return true if any file masks are present, false otherwise
*/
bool KoFilterModel::HasFilterMasks() const
{
	return m_filterMask.size() > 0;
}

/*!
	Set new file mask
	\param type Mask type
	\param mask Mask to be set
	\return true if mask has changed, false otherwise
*/
bool KoFilterModel::SetMask(int type, const char* mask)
{
	bool res = false;

	if ( mask == NULL ) mask = ""; // safeguard arguments
	
	FilterMaskContainer::iterator i = m_filterMask.find(type);
	if( i != m_filterMask.end() )
	{
		res = (*i).second.SetMask(mask);
		
		if( (*i).second.Empty() )
		{
			// Empty mask can be removed
			m_filterMask.erase(i);
		}
	}
	else
	{
		KoFilterMask filterMask;
		filterMask.SetMask(mask);
		
		if( !filterMask.Empty() )
		{
			m_filterMask.insert(std::make_pair(type, filterMask));
			res = true;
		}
	}

	return res;
}

/*!
	Set the column context for the filter
	\param context Column context
*/
void KoFilterModel::SetContext(const KoColumnContext* context)
{
	if( context )
	{
		delete m_context;
		m_context = new KoColumnContext(context->GetPath().c_str());
	}
}

/// Check whether the given filter is set
bool KoFilterModel::HasFilter(const int filter) const
{
	return (m_filters & filter) != 0;
}

/// Toggle specified filter
void KoFilterModel::ToggleFilter(int filter)
{
	if( HasFilter(filter) )
	{
		m_filters &= ~filter;
	}
	else
	{
		m_filters |= filter;
	}
}

/// Toggle show conflict flag
void KoFilterModel::ToggleConflict()
{
	ToggleFilter(kFilterConflicts);
}

/// Get the show conflict flag
bool KoFilterModel::IsConflict() const
{
	return HasFilter(kFilterConflicts);
}

/// Toggle show unknown flag
void KoFilterModel::ToggleUnknown()
{
	ToggleFilter(kFilterUnknown);

	if( HasFilter(kFilterUnknown) )
	{
		m_filters &= ~kFilterHideUnknown;
	}
}

/// Get the show unknown flag
bool KoFilterModel::IsUnknown() const
{
	return HasFilter(kFilterUnknown);
}

/// Toggle hide unknown flag
void KoFilterModel::ToggleHideUnknown()
{
	ToggleFilter(kFilterHideUnknown);

	if( HasFilter(kFilterHideUnknown) )
	{
		m_filters &= ~kFilterUnknown;
	}
}

/// Get the hide unknown flag
bool KoFilterModel::IsHideUnknown() const
{
	return HasFilter(kFilterHideUnknown);
}

/// Toggle show missing flag
void KoFilterModel::ToggleMissing()
{
	ToggleFilter(kFilterMissing);

	if( HasFilter(kFilterMissing) )
	{
		m_filters &= ~kFilterHideMissing;
	}
}

/// Get the show missing flag
bool KoFilterModel::IsMissing() const
{
	return HasFilter(kFilterMissing);
}

/// Toggle hide missing flag
void KoFilterModel::ToggleHideMissing()
{
	ToggleFilter(kFilterHideMissing);

	if( HasFilter(kFilterHideMissing) )
	{
		m_filters &= ~kFilterMissing;
	}
}

/// Get the hide missing flag
bool KoFilterModel::IsHideMissing() const
{
	return HasFilter(kFilterHideMissing);
}

/// Toggle changed flag
void KoFilterModel::ToggleChanged()
{
	ToggleFilter(kFilterChanged);
}

/// Get the changed flag
bool KoFilterModel::IsChanged() const
{
	return HasFilter(kFilterChanged);
}

/// Toggle added flag
void KoFilterModel::ToggleAdded()
{
	ToggleFilter(kFilterAdded);
}

/// Get the added flag
bool KoFilterModel::IsAdded() const
{
	return HasFilter(kFilterAdded);
}

/// Toggle removed flag
void KoFilterModel::ToggleRemoved()
{
	ToggleFilter(kFilterRemoved);
}

/// Get the removed flag
bool KoFilterModel::IsRemoved() const
{
	return HasFilter(kFilterRemoved);
}

//////////////////////////////////////////////////////////////////////////
// KoRecursionModel

KoRecursionModel::KoRecursionModel()
	: m_isRecursive(false)
{
}

KoRecursionModel::~KoRecursionModel()
{
}

void KoRecursionModel::LoadSettings()
{
	m_isRecursive = gDisplayRecursive;
}

void KoRecursionModel::SaveSettings()
{
	gDisplayRecursive = m_isRecursive;
}

NotificationManager* KoRecursionModel::GetNotificationManager()
{
	return &m_manager;
}

/// Get the show recursive flag
bool KoRecursionModel::IsShowRecursive()
{
	return m_isRecursive;
}

/// Toggle recursion flag
void KoRecursionModel::ToggleRecursion()
{
	m_isRecursive = !m_isRecursive;
}

//////////////////////////////////////////////////////////////////////////
// KoIgnoreModel

KoIgnoreModel::KoIgnoreModel()
	: m_isIgnore(false)
{
}

KoIgnoreModel::~KoIgnoreModel()
{
}

void KoIgnoreModel::LoadSettings()
{
	m_isIgnore = gFileViewIgnore;
}

void KoIgnoreModel::SaveSettings()
{
	gFileViewIgnore = m_isIgnore;
}

NotificationManager* KoIgnoreModel::GetNotificationManager()
{
	return &m_manager;
}

/// Get the show ignored flag
bool KoIgnoreModel::IsShowIgnored()
{
	return m_isIgnore;
}

/// Toggle show ignored flag
void KoIgnoreModel::ToggleIgnore()
{
	m_isIgnore = !m_isIgnore;
}
