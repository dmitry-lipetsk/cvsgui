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
 * Author : Jerzy Kaczorowski <kaczoroj@hotmail.com> (Originally: Sebastien Abras <sabdev@ibelgique.com>) --- July 2002
 */

// ItemListDlg.cpp : implementation file
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stdafx.h"

#if qUnix
#	include "UCvsDialogs.h"
#	include "memory"
#endif

#ifdef WIN32
#	include "wincvs.h"
#	include "wincvsView.h"
#	include "PromptFiles.h"
#endif /* !WIN32 */

#ifdef qMacCvsPP
#	include <Icons.h>

#	include <UModalDialogs.h>
#	include <LTableMultiGeometry.h>
#	include <LOutlineKeySelector.h>
#	include <LTableSingleSelector.h>
#	include <LOutlineRowSelector.h>
#	include <UAttachments.h>
#	include <UGAColorRamp.h>
#	include <LTextEditView.h>

#	include "MacCvsConstant.h"
#	include "LPopupFiller.h"
#endif /* qMacCvsPP */

#include "MultiString.h"
#include "ItemListDlg.h"
#include "CvsCommands.h"
#include "BrowseViewColumn.h"

#include "CvsArgs.h"
#include "cvsgui_process.h"
#include "AppConsole.h"
#include "CvsEntries.h"
#include "SelectionDetailsDlg.h"
#include "CvsAlert.h"
#include "CvsRootDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

#define STR_ITEMLIST_DIR		"<Dir>"
#define STR_ITEMLIST_UP_DIR		".."
#define STR_ITEMLIST_BRANCH		"Branch"
#define STR_ITEMLIST_REVISION	"Revision"

/// Persistent <b>recurse</b> setting for tag/branch browse
CPersistentBool gItemListTagRecurse("P_ItemListTagRecurse", false);

/// Persistent <b>list</b> setting for module/directory-file browse
CPersistentBool gItemListModuleList("P_ItemListModuleList", false, kAddSettings);

/// Persistent <b>auto refresh</b> setting for tag/branch browse
CPersistentInt gItemListAutoRefreshTag("P_ItemListAutoRefreshTag", kAutoRefreshFirstTime);

/// Persistent <b>auto refresh</b> setting for module/directory-file browse
CPersistentInt gItemListAutoRefreshModule("P_ItemListAutoRefreshModule", kAutoRefreshFirstTime);

//////////////////////////////////////////////////////////////////////////
// ItemSelectionData

ItemSelectionData::ItemSelectionData()
{
	m_forceCvsroot = false;
}

#ifdef WIN32
ItemSelectionData::ItemSelectionData(const CCvsrootEntryDlg& cvsrootEntryDlg)
{
	m_forceCvsroot = cvsrootEntryDlg.GetForceCvsrootCheck() != 0;
	
	CString strCvsroot;
	cvsrootEntryDlg.GetCvsrootWindowText(strCvsroot);
	m_cvsroot = strCvsroot;
}
#endif

/// Get force CVSROOT
bool ItemSelectionData::GetForceCvsroot() const
{
	return m_forceCvsroot;
}

/// Get CVSROOT 
const std::string& ItemSelectionData::GetCvsroot() const
{
	return m_cvsroot;
}

//////////////////////////////////////////////////////////////////////////
// CItemListConsole

CItemListConsole::CItemListConsole()
{
	m_launchRes = 0;
}

CItemListConsole::~CItemListConsole()
{
	m_itemList.clear();
}

/// Get the parsing result
const ItemList& CItemListConsole::GetItemList() const
{
	return m_itemList;
}

//////////////////////////////////////////////////////////////////////////
// CTagParserConsole

CTagParserConsole::CTagParserConsole()
{
}

CTagParserConsole::~CTagParserConsole()
{
}

void CTagParserConsole::Parse(int launchRes)
{
	bool btag = false;
	
	string line;

#ifdef HAVE_SSTREAM
	istringstream myCvsOut(GetCvsOut().GetData());
#else
	istrstream myCvsOut(GetCvsOut().GetData(), GetCvsOut().GetSize());
#endif

	while( getline(myCvsOut, line) )
	{
		if( btag )
		{
			if( line.empty() || line.find("keyword substitution:") != string::npos )
			{
				btag = false;
			}
			else
			{
				stringstream stream;
				string tag;
				
				stream << line;
				stream >> tag;
				
				{
					const string::size_type colonPos = tag.find(':');

					if( colonPos != string::npos )
						tag.erase(colonPos, tag.size());
				}//local
				
				if( !tag.empty() )
				{
					ItemRowInfo rowInfo;
					string type;
					
					stream >> type;
					
					if( type.size() > 0 )
					{
						bool isBranch = false;
						
						const int dotsCount = count(type.begin(), type.end(), '.');
						if( dotsCount > 2 && dotsCount % 2 )
						{
							// If we have even and greater than two number of separated integers
							// then check if it's a magic branch
							string magicTest = type;
							magicTest.erase(magicTest.find_last_of('.'), magicTest.size());
							magicTest.erase(0, magicTest.find_last_of('.') + 1);
							
							if( !magicTest.empty() && magicTest[0] == '0' )
							{
								isBranch = true;
							}
						}
						else if( dotsCount > 1 && !(dotsCount % 2) )
						{
							// We have an odd and greater than one number of integers, surely a branch
							isBranch = true;
						}
						
						// Set the type appropriate to findings
						type = isBranch ? STR_ITEMLIST_BRANCH : STR_ITEMLIST_REVISION;
					}
					
					ItemList::iterator i = m_itemList.find(tag);
					if( i != m_itemList.end() )
					{
						if( !type.empty() )
						{
							(*i).second.m_description.insert(type);
						}
					}
					else
					{
						if( !type.empty() )
						{
							rowInfo.m_description.insert(type);
						}
						
						m_itemList.insert(std::make_pair(tag, rowInfo));
					}
				}
			}
		}
		
		if( line.find("symbolic names:") != string::npos )
		{
			btag = true;
		}
	}
	
	CItemListConsole::Parse(launchRes);
}

//////////////////////////////////////////////////////////////////////////
// CModuleParserConsole

CModuleParserConsole::CModuleParserConsole()
{
}

CModuleParserConsole::~CModuleParserConsole()
{
}

/*!
	Check whether the modules list needs to be normalized before parsing
	\return true if normalization is needed, false otherwise
*/
bool CModuleParserConsole::NeedsNormalize() const
{
	bool res = false;
	
#ifdef HAVE_SSTREAM
	istringstream myCvsOut(GetCvsOut().GetData());
#else
	istrstream myCvsOut(GetCvsOut().GetData(), GetCvsOut().GetSize());
#endif

	string line;
	while( getline(myCvsOut, line) )
	{
		if( !line.empty() && isspace(line.c_str()[0]) )
		{
			res = true;
			break;
		}
	}
	
	return res;
}

/*!
	Normalize modules list output
	\note We need to normalize modules list because for a very long lines CVS breaks the output
*/
void CModuleParserConsole::Normalize()
{
	string cvsOut;
	
	string line;
	string partialLine;
	
#ifdef HAVE_SSTREAM
	istringstream myCvsOut(GetCvsOut().GetData());
#else
	istrstream myCvsOut(GetCvsOut().GetData(), GetCvsOut().GetSize());
#endif

	while( getline(myCvsOut, line) )
	{
		if( !line.empty() )
		{
			if( isspace(line.c_str()[0]) )
			{
				partialLine += " " + line.erase(0, line.find_first_not_of(" "));
			}
			else
			{
				if( !cvsOut.empty() )
				{
					cvsOut += "\n";
				}
				
				cvsOut += partialLine;
				partialLine = line;
			}
		}
	}
	
	if( !cvsOut.empty() )
	{
		cvsOut += "\n";
	}
	
	cvsOut += partialLine;

	this->SetCvsOut(cvsOut.c_str(), cvsOut.size());
}

void CModuleParserConsole::Parse(int launchRes)
{
	if( NeedsNormalize() )
	{
		Normalize();
	}

	string line;
	
#ifdef HAVE_SSTREAM
	istringstream myCvsOut(GetCvsOut().GetData());
#else
	istrstream myCvsOut(GetCvsOut().GetData(), GetCvsOut().GetSize());
#endif

	while( getline(myCvsOut, line) )
	{
		if( !line.empty() && !isspace(line.c_str()[0]) )
		{
			stringstream stream;
			string module;
			
			stream << line;
			stream >> module;
			
			if( !module.empty() )
			{
				ItemRowInfo rowInfo;
				
				string definition = line;
				definition.erase(definition.begin(), definition.begin() + module.size());

				while( !definition.empty() && isspace(definition[0]) )
				{
					definition.erase(definition.begin(), definition.begin() + 1);
				}
				
				if( !definition.empty() )
				{
					stringstream strDefintion;
					string options;
					string directories;
					string token;

					strDefintion << definition;
					
					bool hasProg = false;

					do{
						strDefintion >> token;
						if( !token.empty() )
						{
							if( directories.empty() )
							{
								if( options.empty() )
								{
									if( token[0] != '-' )
									{
										directories += token;
										continue;
									}
								}

								if( token[0] == '-' || hasProg )
								{
									if( !options.empty() )
									{
										options += " ";
									}

									options += token;

									if( hasProg )
									{
										hasProg = false;
									}
									else if( token == "-d" || 
										token == "-e" || 
										token == "-i" || 
										token == "-o" ||
										token == "-s" || 
										token == "-t" || 
										token == "-u" )
									{
										hasProg = true;
									}
								}
								else
								{
									directories += token;
								}
							}
							else
							{
								directories += " ";
								directories += token;
							}
						}
					}while( !strDefintion.eof());
										
					rowInfo.m_description.insert(options.c_str());
					rowInfo.m_descriptionEx.insert(directories.c_str());
				}

				m_itemList.insert(std::make_pair(module, rowInfo));
			}
		}
	}
	
	CItemListConsole::Parse(launchRes);
}

//////////////////////////////////////////////////////////////////////////
// CListParserConsole

CListParserConsole::CListParserConsole()
{
}

CListParserConsole::~CListParserConsole()
{
}

void CListParserConsole::Parse(int launchRes)
{
	string line;
	
#ifdef HAVE_SSTREAM
	istringstream myCvsOut(GetCvsOut().GetData());
#else
	istrstream myCvsOut(GetCvsOut().GetData(), GetCvsOut().GetSize());
#endif

	while( getline(myCvsOut, line) )
	{
		if( !line.empty() && !isspace(line.c_str()[0]) )
		{
			const char entrySeparator = '/';
			string entryType;
			stringstream entry(line);
			
			if( getline(entry, entryType, entrySeparator) )
			{
				if( entryType.size() > 0 && entryType.c_str()[0] == 'D' )
				{
					string directory;
					
					if( getline(entry, directory, entrySeparator) && !directory.empty() )
					{
						ItemRowInfo rowInfo;
						rowInfo.m_description.insert(STR_ITEMLIST_DIR);
						
						m_itemList.insert(std::make_pair(directory, rowInfo));
					}
				}
				else
				{
					string file;
					
					if( getline(entry, file, entrySeparator) && !file.empty() )
					{
						string revision;
						string date;
						
						if( getline(entry, revision, entrySeparator) && 
							getline(entry, date, entrySeparator) )
						{
							ItemRowInfo rowInfo;
							rowInfo.m_description.insert(revision);
							rowInfo.m_descriptionEx.insert(date);
							
							m_itemList.insert(std::make_pair(file, rowInfo));
						}
					}
				}
			}
		}
	}
	
	CItemListConsole::Parse(launchRes);
}


/*!
	Format an item's description string given a collection of descriptions
	\param itemDesc collection of descriptions
	\return Formatted item's description string 
*/
static string FormatItemDescription(const ItemDescription& itemDesc)
{
	string strRes;

	for(ItemDescription::const_iterator it = itemDesc.begin(); it != itemDesc.end(); it++)
	{
		if( !strRes.empty() )
		{
			strRes += ", ";
		}

		strRes += (*it).c_str();
	}

	return strRes;
}

/*!
	Build the path string given a list of items
	\param listPath List of items to be in the path
	\param pathDelimiter Optional path delimiter to be used
	\return The formatted path as string
	\note Path ends with path delimiter
*/
static string FormatListPath(const ListPath& listPath, const char pathDelimiter = '/')
{
	string strRes;

	for(ListPath::const_iterator it = listPath.begin(); it != listPath.end(); it++ ) 
	{
		strRes += (*it).c_str();
		strRes += pathDelimiter;
	}

	return strRes;
}

/*!
	Launch and parse CVS command appropriate to the type and parameters
	\param type List dialog type
	\param option Option
	\param readFrom Location to read from
	\param forceCvsroot true to force using CVSROOT settings, false otherwise
	\param cvsroot CVSROOT
	\param revision Revision
	\param date Date
	\param listPath Path as a list
	\return The pointer to item list console
	\note You have to free the memory allocated for the item list console
*/
static CItemListConsole* LaunchItemListCommand(kItemListDlg type, bool option, 
											   const char* readFrom, 
											   bool forceCvsroot, const char* cvsroot, 
											   const char* revision, const char* date,
											   const ListPath* listPath = NULL)
{
	CItemListConsole* itemListConsole = NULL;

	CvsArgs args;
	MultiFiles mf;

	if( forceCvsroot )
	{
#if INTERNAL_AUTHEN
		args.addcvsroot();
#else
		args.add("-d");
		args.add(cvsroot);
#endif /* INTERNAL_AUTHEN */
	}

	switch( type )
	{
	case kItemListTag:
		itemListConsole = new CTagParserConsole;
		
		args.add(forceCvsroot ? "rlog" : "log");
		args.add("-h");
		
		if( !option )
		{
			args.add("-l");
		}

		if( strlen(revision) )
		{
			string strRevision;
			
			strRevision += "-r";
			strRevision += revision;

			args.add(strRevision.c_str());
		}
		
		if( strlen(date) )
		{
			args.add("-d");
			args.add(date);
		}

		{
			string uppath, file;
			SplitPath(readFrom, uppath, file, true);
			
			mf.newdir(uppath.c_str());
			
			if( !file.empty() )
			{
				mf.newfile(file.c_str());
			}
		}
		break;
	case kItemListModule:
		if( option )
		{
			itemListConsole = new CListParserConsole;

			args.add("ls");
			args.add("-e");

			if( strlen(revision) )
			{
				args.add("-r");
				args.add(revision);
			}

			if( strlen(date) )
			{
				args.add("-D");
				args.add(date);
			}

			if( listPath )
			{
				string strListPath = FormatListPath(*listPath);
				if( !strListPath.empty() )
				{
					args.endopt();
					args.add(strListPath.c_str());
				}
				else  
				{
					args.add("/");
				}
			}
			else  
			{
				args.add("/");
			}
		}
		else
		{
			itemListConsole = new CModuleParserConsole;

			args.add("checkout");
			args.add("-c");
		}

		mf.newdir(readFrom);
		break;
	default:
#ifdef WIN32
		ASSERT(FALSE);	// unknown option
#endif
		break;
	}

	if( itemListConsole )
	{
		const char* dir = (*mf.begin()).add(args);
		
		args.print(dir);
		const int launchRes = launchCVS(dir, args.Argc(), args.Argv(), itemListConsole);
		itemListConsole->Parse(launchRes);
	}

	return itemListConsole;
}


//////////////////////////////////////////////////////////////////////////
// ItemDisplayInfo

/*!
	Create display info
	\param item Item
	\param rowInfo Row info for the item
*/
void ItemDisplayInfo::Create(const std::string& item, const ItemRowInfo& rowInfo)
{
	m_item = item;
	m_description = FormatItemDescription(rowInfo.m_description);
	m_descriptionEx = FormatItemDescription(rowInfo.m_descriptionEx);

	m_rowInfo = rowInfo;
}

/// Get item text
const std::string& ItemDisplayInfo::GetItem() const
{
	return m_item;
}

/// Get the description text
const std::string& ItemDisplayInfo::GetDescription() const
{
	return m_description;
}

/// Get the extended description text
const std::string& ItemDisplayInfo::GetDescriptionEx() const
{
	return m_descriptionEx;
}

/// Get the row info
const ItemRowInfo& ItemDisplayInfo::GetRowInfo() const
{
	return m_rowInfo;
}

#ifdef WIN32

/*!
	Callback method used for sorting of items in the list control
	\param itemData1 ItemDisplayInfo* of the first element
	\param itemData2 ItemDisplayInfo* of the second element
	\param lParam CompareInfo* Sort settings
	\return 1, -1 or 0 depending on the sort settings
*/
int CALLBACK CItemListDlg::CompareItems(LPARAM itemData1, LPARAM itemData2, LPARAM lParam)
{
	int res = 0;

	const CompareInfo* info = (CompareInfo*)lParam;
	if( info )
	{
		ItemDisplayInfo* displayInfo1 = (ItemDisplayInfo*)itemData1;
		ItemDisplayInfo* displayInfo2 = (ItemDisplayInfo*)itemData2;
		
		if( displayInfo1 && displayInfo2 )
		{
			switch( info->m_type )
			{
			case kItemListTag:
				switch( info->m_column )
				{
				case 0:
					res = stricmp(displayInfo1->GetItem().c_str(), displayInfo2->GetItem().c_str());
					break;
				case 1:
					res = stricmp(displayInfo1->GetDescription().c_str(), displayInfo2->GetDescription().c_str());
					break;
				default:
					ASSERT(FALSE); // should not come here
					break;
				}
				break;
			case kItemListModule:
				if( info->m_option )
				{
					if( strcmp(displayInfo1->GetItem().c_str(), STR_ITEMLIST_UP_DIR) == 0 )
					{
						res = info->m_asc ? 1 : -1;
					}
					else if( strcmp(displayInfo2->GetItem().c_str(), STR_ITEMLIST_UP_DIR) == 0 )
					{
						res = info->m_asc ? -1 : 1;
					}
					else
					{
						bool isDir1 = strcmp(displayInfo1->GetDescription().c_str(), STR_ITEMLIST_DIR) == 0;
						bool isDir2 = strcmp(displayInfo2->GetDescription().c_str(), STR_ITEMLIST_DIR) == 0;

						if( isDir1 && !isDir2 )
						{
							res = info->m_asc ? 1 : -1;
						}
						else if( !isDir1 && isDir2 )
						{
							res = info->m_asc ? -1 : 1;
						}
						else if( isDir1 && isDir2 )
						{
							res = stricmp(displayInfo1->GetItem().c_str(), displayInfo2->GetItem().c_str());
							if( info->m_column != 0 )
							{
								if( info->m_asc )
								{
									res *= -1;
								}
							}
						}
						else
						{
							switch( info->m_column )
							{
							case 0:
								res = stricmp(displayInfo1->GetItem().c_str(), displayInfo2->GetItem().c_str());
								break;
							case 1:
								res = stricmp(displayInfo1->GetDescription().c_str(), displayInfo2->GetDescription().c_str());
								break;
							case 2:
								{
									const time_t time1 = GetDateTime(displayInfo1->GetDescriptionEx().c_str()); 
									const time_t time2 = GetDateTime(displayInfo2->GetDescriptionEx().c_str()); 
									
									if( time1 != (time_t)-1 && time2 != (time_t)-1 ) 
									{ 
										res = time1 < time2 ? -1 : (time1 > time2 ? 1 : 0);
									} 
									else
									{
										res = stricmp(displayInfo1->GetDescriptionEx().c_str(), displayInfo2->GetDescriptionEx().c_str());
									}
								}
								break;
							default:
								ASSERT(FALSE); // should not come here
								break;
							}
						}
					}
				}
				else
				{
					switch( info->m_column )
					{
					case 0:
						res = stricmp(displayInfo1->GetItem().c_str(), displayInfo2->GetItem().c_str());
						break;
					case 1:
						res = stricmp(displayInfo1->GetDescription().c_str(), displayInfo2->GetDescription().c_str());
						break;
					case 2:
						res = stricmp(displayInfo1->GetDescriptionEx().c_str(), displayInfo2->GetDescriptionEx().c_str());
						break;
					default:
						ASSERT(FALSE); // should not come here
						break;
					}
				}

				break;
			default:
				ASSERT(FALSE);	// unknown type
				break;
			}
		}
	}

	if( res )
	{
		if( info->m_asc )
		{
			res *= -1;
		}
	}

	return res;
}


/////////////////////////////////////////////////////////////////////////////
// CItemListDlg dialog


CItemListDlg::CItemListDlg(const MultiFiles* mf,
						   const char* item, kItemListDlg type, ItemList& itemList, const ListPath* listPath,
						   bool forceRoot, const char* cvsroot, 
						   const char* rev, const char* date, 
						   CWnd* pParent /*=NULL*/)
						   : m_mf(mf), m_itemList(itemList), 
						   m_readFromCombo(USmartCombo::AutoDropWidth), m_autoRefreshCombo(USmartCombo::AutoDropWidth),
						   CHHelpDialog(CItemListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemListDlg)
	m_readFrom = _T("");
	m_item = _T("");
	m_option = FALSE;
	m_hasRev = FALSE;
	m_rev = _T("");
	m_date = _T("");
	m_hasDate = FALSE;
	//}}AFX_DATA_INIT
	
	m_item = item;

	m_type = type;

	switch( m_type )
	{
	case kItemListTag:
		m_option = (bool)gItemListTagRecurse;
		break;
	case kItemListModule:
		m_option = (bool)gItemListModuleList;
		break;
	default:
		// Nothing to do
		break;
	}

	if( listPath )
	{
		m_listPath = *listPath;
	}

	m_cvsStopped = false;

	m_rev = rev;
	m_date = date;

	m_sortAsc = false;
	m_sortColumn = 0;
	
	m_forceRoot = forceRoot;
	m_cvsroot = cvsroot;

	m_revCombo.SetItems(&gRevNames);
	m_dateCombo.SetItems(&gDateNames);
}


void CItemListDlg::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemListDlg)
	DDX_Control(pDX, IDC_CVSROOT_ENTRY_STATIC, m_cvsrootEntryPlaceholder);
	DDX_Control(pDX, IDC_AUTO_REFRESH, m_autoRefreshCombo);
	DDX_Control(pDX, IDC_BROWSE_SELECTION, m_selectionButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
	DDX_Control(pDX, IDC_BROWSE_TAG, m_browseTagButton);
	DDX_Control(pDX, IDC_ITEM, m_itemEdit);
	DDX_Control(pDX, IDC_REFRESH_ITEMS, m_refreshButton);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDC_BROWSE_READ_FROM, m_browseReadFromButton);
	DDX_Control(pDX, IDC_ITEM_LABEL, m_itemLabelStatic);
	DDX_Control(pDX, IDC_HAS_DATE, m_hasDateCheck);
	DDX_Control(pDX, IDC_HAS_REV, m_hasRevCheck);
	DDX_Control(pDX, IDC_DATE, m_dateCombo);
	DDX_Control(pDX, IDC_REV_TAG, m_revCombo);
	DDX_Control(pDX, IDC_ITEM_LIST_OPTION, m_optionCheck);
	DDX_Control(pDX, IDC_ITEM_LIST, m_itemListCtrl);
	DDX_Control(pDX, IDC_READ_FROM, m_readFromCombo);
	DDX_CBString(pDX, IDC_READ_FROM, m_readFrom);
	DDX_Text(pDX, IDC_ITEM, m_item);
	DDX_Check(pDX, IDC_ITEM_LIST_OPTION, m_option);
	DDX_Check(pDX, IDC_HAS_REV, m_hasRev);
	DDX_CBString(pDX, IDC_REV_TAG, m_rev);
	DDX_CBString(pDX, IDC_DATE, m_date);
	DDX_Check(pDX, IDC_HAS_DATE, m_hasDate);
	//}}AFX_DATA_MAP

	DDX_ComboMString(pDX, IDC_REV_TAG, m_revCombo);
	DDX_ComboMString(pDX, IDC_DATE, m_dateCombo);
	
	if( pDX->m_bSaveAndValidate )
	{
		m_itemList.clear();
		
		for(int nIndex = 0; nIndex < m_itemListCtrl.GetItemCount(); nIndex++)
		{
			ItemDisplayInfo* displayInfo = (ItemDisplayInfo*)m_itemListCtrl.GetItemData(nIndex);
			if( displayInfo )
			{
				m_itemList.insert(std::make_pair(displayInfo->GetItem(), displayInfo->GetRowInfo()));
			}
		}
	}

	m_cvsrootEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_forceRoot, m_cvsroot);
}


BEGIN_MESSAGE_MAP(CItemListDlg, CHHelpDialog)
	//{{AFX_MSG_MAP(CItemListDlg)
	ON_BN_CLICKED(IDC_REFRESH_ITEMS, OnRefresh)
	ON_BN_CLICKED(IDC_ITEM_LIST_OPTION, OnOption)
	ON_BN_CLICKED(IDC_HAS_REV, OnCheckrev)
	ON_BN_CLICKED(IDC_HAS_DATE, OnCheckdate)
	ON_BN_CLICKED(IDC_BROWSE_TAG, OnBrowseTag)
	ON_NOTIFY(NM_DBLCLK, IDC_ITEM_LIST, OnDblclkItemlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_LIST, OnItemchangedItemlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_ITEM_LIST, OnColumnclickItemlist)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BROWSE_READ_FROM, OnBrowseReadfrom)
	ON_BN_CLICKED(IDC_BROWSE_SELECTION, OnSelection)
	ON_CBN_SELCHANGE(IDC_AUTO_REFRESH, OnSelchangeAutoRefresh)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemListDlg message handlers

/// OnInitDialog virtual override, initializes controls and triggers data reading if necessary
BOOL CItemListDlg::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	SetListCtrlExtendedStyle(&m_itemListCtrl, LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);

	// Setup controls
	CHeaderCtrl* headerCtrl = m_itemListCtrl.GetHeaderCtrl();
	if( headerCtrl )
	{
		m_headerCtrl.SubclassWindow(headerCtrl->m_hWnd);
	}

	InitListCtrl(kItemDisplayData);
	InitOptionCtrls();

	m_itemsImageList.Create(IDR_ITEMLIST, 16, 1, RGB(255, 0, 255));
	m_itemListCtrl.SetImageList(&m_itemsImageList, LVSIL_SMALL);

	int nReadFromSelection = 0;

	if( m_mf )
	{
		// Add selected directories
		for(int nIndex = 0; nIndex < m_mf->NumDirs(); nIndex++)
		{
			string path;
			if( m_mf->getdir(nIndex, path) )
			{
				m_readFromCombo.AddString(path.c_str());
			}
		}
	}
	
	switch( m_type )
	{
	case kItemListTag:
		// Set windows titles
		SetWindowText("Select tag/branch");
		m_itemLabelStatic.SetWindowText("&Tag/branch:");
		m_optionCheck.SetWindowText("&Recurse");

		// The browser selection
		if( m_mf && m_mf->TotalNumFiles() )
		{
			MultiFiles::const_iterator mfi;
			for(mfi = m_mf->begin(); mfi != m_mf->end(); ++mfi)
			{
				for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
				{
					string fullPath;
					mfi->GetFullPath(nIndex, fullPath);

					const int nComboIndex = m_readFromCombo.AddString(fullPath.c_str());
					
					// It's better to have a single file selected initially for the performance reasons
					if( 0 == nReadFromSelection )
					{
						nReadFromSelection = nComboIndex;
					}
				}
			}
		}
		
		break;
	case kItemListModule:
		// Set windows titles
		SetWindowText("Select module");
		m_itemLabelStatic.SetWindowText("&Module:");
		m_optionCheck.SetWindowText("&List");
		break;
	default:
		ASSERT(FALSE);	// unknown type
		EndDialog(IDCANCEL);
		return FALSE;
	}
	
	// Set the "read from"
	if( m_readFromCombo.GetCount() )
	{
		m_readFromCombo.SetCurSel(nReadFromSelection);
	}

	// Trigger refresh
	kItemListAutoRefreshType autoRefresh = kAutoRefreshFirstTime;

	if( kItemListModule == m_type )
	{
		autoRefresh = (kItemListAutoRefreshType)(int)gItemListAutoRefreshModule;
	}
	else if( kItemListTag == m_type )
	{
		autoRefresh = (kItemListAutoRefreshType)(int)gItemListAutoRefreshTag;
	}
	else
	{
		ASSERT(FALSE); // unknown type
	}

	m_autoRefreshCombo.SetCurSel(autoRefresh);

	if( (kAutoRefreshAlways == autoRefresh) || (kAutoRefreshFirstTime == autoRefresh && m_itemList.empty()) )
	{
		// Force the window to show itself
		ShowWindow(SW_SHOW);
		PostClickRefresh();
	}
	else
	{
		DisplayItems(m_itemList, 0, NULL, false);
		if( m_itemListCtrl.IsWindowEnabled() )
		{
			SortItems(m_sortColumn, m_sortAsc, m_type, m_optionCheck.GetCheck() != 0);
		}

		if( !m_item.IsEmpty() )
		{
			SelectItem(m_item);
		}
	}

	m_cvsrootEntryDlg.Create(&m_cvsrootEntryPlaceholder, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemListDlg::InitHelpID()
{
	SetCtrlHelpID(IDC_ITEM_LABEL,		IDH_IDC_ITEM_LABEL);
	SetCtrlHelpID(IDC_ITEM,				IDH_IDC_ITEM);
	SetCtrlHelpID(IDC_BROWSE_SELECTION,	IDH_IDC_BROWSE_SELECTION);
	SetCtrlHelpID(IDC_ITEM_LIST,		IDH_IDC_ITEM_LIST);
	SetCtrlHelpID(IDC_GROUP_OPTIONS,	IDH_IDC_GROUP_OPTIONS);
	SetCtrlHelpID(IDC_READ_FROM,		IDH_IDC_READ_FROM);
	SetCtrlHelpID(IDC_BROWSE_READ_FROM,	IDH_IDC_BROWSE_READ_FROM);
	SetCtrlHelpID(IDC_REFRESH_ITEMS,	IDH_IDC_REFRESH_ITEMS);
	SetCtrlHelpID(IDC_HAS_REV,			IDH_IDC_HAS_REV);
	SetCtrlHelpID(IDC_REV_TAG,			IDH_IDC_REV_TAG);
	SetCtrlHelpID(IDC_BROWSE_TAG,		IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_HAS_DATE,			IDH_IDC_HAS_DATE);
	SetCtrlHelpID(IDC_DATE,				IDH_IDC_DATE);
	SetCtrlHelpID(IDC_AUTO_REFRESH,		IDH_IDC_AUTO_REFRESH);

	switch( m_type )
	{
	case kItemListTag:
		SetCtrlHelpID(IDC_ITEM_LIST_OPTION, IDH_IDC_FORCE_RECURSE);
		break;
	case kItemListModule:
		SetCtrlHelpID(IDC_ITEM_LIST_OPTION, IDH_IDC_ITEM_LIST_OPTION);
		break;
	default:
		ASSERT(FALSE); // unknown option
		break;
	}
}

/// BN_CLICKED message handler, display dialog help
void CItemListDlg::OnHelp() 
{
	ShowHelp();
}

/*!
	"Click" the refresh button by posting the BN_CLICKED message for the refresh button
	\return TRUE on success, FALSE otherwise
*/
BOOL CItemListDlg::PostClickRefresh()
{
	return PostMessage(WM_COMMAND, MAKEWPARAM(IDC_REFRESH_ITEMS, BN_CLICKED), (LPARAM)NULL);
}

/*!
	Set the list control columns appropriate to the dialog type, show prompt message eventually
	\param displayType Display type as specified by kItemDisplayType enum
	\note Prompt message will be shown while data are being read
*/
void CItemListDlg::InitListCtrl(const kItemDisplayType displayType)
{
	CWndRedrawMngr redrawMngr(&m_itemListCtrl);

	// Remove all items and columns
	DeleteAllItems();

	int columnCount = m_itemListCtrl.GetHeaderCtrl()->GetItemCount();
	while( columnCount-- )
	{
		m_itemListCtrl.DeleteColumn(0);
	}

	// Get the client area width to properly size columns
	CRect rect;
	m_itemListCtrl.GetClientRect(rect);

	const int listCtrlWidth = rect.Width();

	// Set the appropriate columns
	switch( displayType )
	{
	case kItemDisplayWait:
		m_itemListCtrl.InsertColumn(0, _i18n("Action"), LVCFMT_LEFT, listCtrlWidth);

		m_itemListCtrl.InsertItem(0, _i18n("Please wait while retrieving data"), kItemImgBlank);
		m_itemListCtrl.InsertItem(1, _i18n("You can press Stop button to cancel the operation"), kItemImgBlank);
		break;
	case kItemDisplayData:
		{
			const int listCtrlWidthNoScroll = listCtrlWidth - GetSystemMetrics(SM_CXVSCROLL);
			
			switch( m_type )
			{
			case kItemListTag:
				m_itemListCtrl.InsertColumn(0, _i18n("Tag/branch"), LVCFMT_LEFT, 200);
				m_itemListCtrl.InsertColumn(1, _i18n("Description"), LVCFMT_LEFT, listCtrlWidthNoScroll - 200);
				break;
			case kItemListModule:
				if( m_optionCheck.GetCheck() )
				{
					m_itemListCtrl.InsertColumn(0, _i18n("Module"), LVCFMT_LEFT, 200);
					m_itemListCtrl.InsertColumn(1, _i18n("Description"), LVCFMT_LEFT, 120);
					m_itemListCtrl.InsertColumn(2, _i18n("Timestamp"), LVCFMT_LEFT, listCtrlWidthNoScroll - 120 - 200);
				}
				else
				{
					m_itemListCtrl.InsertColumn(0, _i18n("Module"), LVCFMT_LEFT, 200);
					m_itemListCtrl.InsertColumn(1, _i18n("Options"), LVCFMT_LEFT, 60);
					m_itemListCtrl.InsertColumn(2, _i18n("Directories / Files"), LVCFMT_LEFT, listCtrlWidthNoScroll - 60 - 200);
				}
				break;
			default:
				ASSERT(FALSE);	// unknown type
				break;
			}
		}
		break;
	case kItemDisplayInfo:
		m_itemListCtrl.InsertColumn(0, _i18n("Information"), LVCFMT_LEFT, listCtrlWidth);
		break;
	default:
		ASSERT(FALSE);	// unknown display type
		break;
	}
}

/*!
	Set the controls dependent on the Option state
*/
void CItemListDlg::InitOptionCtrls()
{
	BOOL enableDateRev = (kItemListModule == m_type && !m_optionCheck.GetCheck()) ? false : true;
	
	m_hasRevCheck.EnableWindow(enableDateRev);
	m_hasDateCheck.EnableWindow(enableDateRev);
	
	OnCheckrev();
	OnCheckdate();

	switch( m_type )
	{
	case kItemListTag:
		gItemListTagRecurse = m_optionCheck.GetCheck() != 0;
		break;
	case kItemListModule:
		gItemListModuleList = m_optionCheck.GetCheck() != 0;
		break;
	default:
		// Nothing to do
		break;
	}
}

/*!
	Enable or disable controls
	\param enable Set to TRUE to enable controls, FALSE to disable controls
*/
void CItemListDlg::EnableControls(BOOL enable)
{
	m_okButton.EnableWindow(enable);
	m_refreshButton.EnableWindow(enable);
	m_readFromCombo.EnableWindow(enable);
	m_browseReadFromButton.EnableWindow(enable);
	m_itemListCtrl.EnableWindow(enable);
	m_itemEdit.EnableWindow(enable);
	m_selectionButton.EnableWindow(enable);

	m_optionCheck.EnableWindow(enable);
	m_autoRefreshCombo.EnableWindow(enable);

	m_cvsrootEntryDlg.EnableControls(enable);
	
	if( !enable )
	{
		m_hasRevCheck.EnableWindow(FALSE);
		m_revCombo.EnableWindow(FALSE);
		
		m_browseTagButton.EnableWindow(FALSE);
		
		m_hasDateCheck.EnableWindow(FALSE);
		m_dateCombo.EnableWindow(FALSE);
	}
	else
	{
		InitOptionCtrls();
	}
}

/// BN_CLICKED message handler, re-reads the data
void CItemListDlg::OnRefresh() 
{
	m_cvsStopped = false;

	// Verify the settings
	if( !CheckSettings() )
	{
		return;
	}

	// Prepare controls
	EnableControls(FALSE);

	// Change the Cancel button's name to "Stop"
	CString strPrevCancelText;
	m_cancelButton.GetWindowText(strPrevCancelText);
	m_cancelButton.SetWindowText(_i18n("Stop"));

	// Store current item text
	CString strItem;
	m_itemEdit.GetWindowText(strItem);

	// Display the reading data prompt
	InitListCtrl(kItemDisplayWait);

	// Select and launch command
	CString strReadFrom;
	CString revision;
	CString date;
	CString cvsroot;

	m_readFromCombo.GetWindowText(strReadFrom);

	if( m_hasRevCheck.GetCheck() )
	{
		m_revCombo.GetWindowText(revision);
	}
		
	if( m_hasDateCheck.GetCheck() )
	{
		m_dateCombo.GetWindowText(date);
	}

	if( m_cvsrootEntryDlg.GetForceCvsrootCheck() )
	{
		m_cvsrootEntryDlg.GetCvsrootWindowText(cvsroot);
	}

	const char* readFrom = kItemListModule == m_type && 
		m_cvsrootEntryDlg.GetForceCvsrootCheck() && !m_optionCheck.GetCheck() ? "" : (LPCTSTR)strReadFrom;

	std::auto_ptr<CItemListConsole> itemListConsole(LaunchItemListCommand(m_type, m_optionCheck.GetCheck() != 0, 
		readFrom, 
		m_cvsrootEntryDlg.GetForceCvsrootCheck() != 0, cvsroot, 
		revision, date, &m_listPath));

	if( !itemListConsole.get() )
	{
		EndDialog(IDCANCEL);
		return;
	}

	// Initialize list control
	InitListCtrl(kItemDisplayData);

	// Restore control's state
	EnableControls(TRUE);

	// Display data
#ifdef HAVE_SSTREAM
	istringstream myCvsErr(itemListConsole->GetCvsErr().GetData());
#else
	istrstream myCvsErr(itemListConsole->GetCvsErr().GetData(), itemListConsole->GetCvsErr().GetSize());
#endif

	DisplayItems(itemListConsole->GetItemList(), itemListConsole->GetLaunchRes(), &myCvsErr);

	// Restore Cancel's button name
	m_cancelButton.SetWindowText(strPrevCancelText);
	
	if( m_itemListCtrl.IsWindowEnabled() )
	{
		// Sort items
		SortItems(m_sortColumn, m_sortAsc, m_type, m_optionCheck.GetCheck() != 0);
		
		// Try to select the item
		if( strItem.IsEmpty() && m_itemListCtrl.GetItemCount() )
		{
			strItem = m_itemListCtrl.GetItemText(0, 0);
		}
		
		if( !strItem.IsEmpty() )
		{
			SelectItem(strItem);
			GotoDlgCtrl(&m_itemListCtrl);
		}
	}
}

/*!
	Verify that the settings are correct to retrieve items
	\return true if the settings are correct, false otherwise
*/
bool CItemListDlg::CheckSettings()
{
	if( m_hasRevCheck.IsWindowEnabled() && m_hasRevCheck.GetCheck() && !m_revCombo.GetWindowTextLength() )
	{
		CvsAlert(kCvsAlertWarningIcon, 
			"Invalid revision.", "Please enter more than 1 character.", 
			BUTTONTITLE_OK, NULL).ShowAlert();

		GotoDlgCtrl(&m_revCombo);
		return false;
	}

	if( m_hasDateCheck.IsWindowEnabled() && m_hasDateCheck.GetCheck() && !m_dateCombo.GetWindowTextLength() )
	{
		CvsAlert(kCvsAlertWarningIcon, 
			"Invalid date.", "Please enter more than 1 character.", 
			BUTTONTITLE_OK, NULL).ShowAlert();

		GotoDlgCtrl(&m_dateCombo);
		return false;
	}

	return true;
}

/*!
	Display items in the list
	\param itemList List of items to display
	\param launchRes Command launch result
	\param cvsErr Command error stream, can be NULL
	\param showUpDir Set to true to show the up-dir(..) in the list if appropriate
*/
#ifdef HAVE_SSTREAM
void CItemListDlg::DisplayItems(const ItemList& itemList, const int launchRes, std::istringstream* cvsErr /*= NULL*/, const bool showUpDir /*= true*/)
#else
void CItemListDlg::DisplayItems(const ItemList& itemList, const int launchRes, std::istrstream* cvsErr /*= NULL*/, const bool showUpDir /*= true*/)
#endif
{
	if( itemList.size() == 0 )
	{
		if( !m_cvsStopped )
		{
			if( launchRes == 0 )
			{
				// Show the info if the modules file doesn't contain any modules
				if( kItemListModule == m_type && !m_optionCheck.GetCheck() )
				{
					InitListCtrl(kItemDisplayInfo);
					m_itemListCtrl.InsertItem(0, _i18n("Modules file doesn't contain any modules definitions"), kItemImgBlank);
					m_itemListCtrl.EnableWindow(FALSE);
				}
			}
			else if( cvsErr )
			{
				InitListCtrl(kItemDisplayInfo);
				
				// Show the error in the list control
				m_itemListCtrl.InsertItem(0, _i18n("Command failed:"), kItemImgBlank);
				
				string line;
				while( getline(*cvsErr, line) )
				{
					m_itemListCtrl.InsertItem(m_itemListCtrl.GetItemCount(), line.c_str(), kItemImgBlank);
				}
				
				m_itemListCtrl.EnableWindow(FALSE);
			}
		}
	}
	else
	{
		// Insert the items
		ItemList::const_iterator it = itemList.begin();
		while( it != itemList.end() )
		{
			ItemDisplayInfo* displayInfo = new ItemDisplayInfo;
			if( displayInfo )
			{
				displayInfo->Create((*it).first, (*it).second);
				
				const int itemIndex = m_itemListCtrl.InsertItem(m_itemListCtrl.GetItemCount(), displayInfo->GetItem().c_str());
				if( itemIndex > -1 )
				{
					if( kItemListTag == m_type || kItemListModule == m_type )
					{
						m_itemListCtrl.SetItemText(itemIndex, 1, displayInfo->GetDescription().c_str());
					}
					
					if( kItemListModule == m_type )
					{
						m_itemListCtrl.SetItemText(itemIndex, 2, displayInfo->GetDescriptionEx().c_str());
					}
					
					if( !m_itemListCtrl.SetItemData(itemIndex, (DWORD)displayInfo) )
					{
						delete displayInfo;
					}
					
					const int image = GetImageForItem(m_type, m_optionCheck.GetCheck() != 0, displayInfo);
					if( image > -1 )
					{
						m_itemListCtrl.SetItem(itemIndex, -1, LVIF_IMAGE, NULL, image, 0, 0, 0);
					}
				}
				else
				{
					delete displayInfo;
				}
			}
			
			it++;
		}
	}
	
	if( showUpDir && kItemListModule == m_type && m_optionCheck.GetCheck() && !m_listPath.empty() )
	{
		// Insert the ".." to move one level up in the directories tree
		ItemDisplayInfo* displayInfo = new ItemDisplayInfo;
		if( displayInfo )
		{
			ItemRowInfo rowInfo;
			rowInfo.m_description.insert(STR_ITEMLIST_DIR);
				
			displayInfo->Create(string(STR_ITEMLIST_UP_DIR), rowInfo);
				
			const int itemIndex = m_itemListCtrl.InsertItem(0, displayInfo->GetItem().c_str());
			if( itemIndex > -1 )
			{
				m_itemListCtrl.SetItemText(itemIndex, 1, displayInfo->GetDescription().c_str());
				
				if( !m_itemListCtrl.SetItemData(itemIndex, (DWORD)displayInfo) )
				{
					delete displayInfo;
				}
				
				const int image = GetImageForItem(m_type, m_optionCheck.GetCheck() != 0, displayInfo);
				if( image > -1 )
				{
					m_itemListCtrl.SetItem(itemIndex, -1, LVIF_IMAGE, NULL, image, 0, 0, 0);
				}
			}
			else
			{
				delete displayInfo;
			}
		}
	}
}

/*!
	Get the image corresponding to the given item
	\param type Item list type
	\param option Item list option
	\param displayInfo Item info to find the image for
	\return The image index in the image list to be used for item or -1 if not image was selected
*/
int CItemListDlg::GetImageForItem(kItemListDlg type, bool option, const ItemDisplayInfo* displayInfo)
{
	int res = -1;

	switch( type )
	{
	case kItemListTag:
		{
			bool hasRevision = displayInfo->GetDescription().find(STR_ITEMLIST_REVISION) != string::npos;
			bool hasBranch = displayInfo->GetDescription().find(STR_ITEMLIST_BRANCH) != string::npos;
			
			if( hasRevision && hasBranch )
			{
				res = kItemImgBranchAndRevision;
			}
			else if( hasBranch )
			{
				res = kItemImgBranch;
			}
			else
			{
				res = kItemImgRevision;
			}
		}
		break;
	case kItemListModule:
		if( option )
		{
			if( strcmp(displayInfo->GetItem().c_str(), STR_ITEMLIST_UP_DIR) == 0 )
			{
				res = kItemImgUpDir;
			}
			else
			{
				bool isDir = displayInfo->GetDescription().find(STR_ITEMLIST_DIR) != string::npos;
				res = isDir ? kItemImgDir : kItemImgFile;
			}
		}
		else
		{
			res	= kItemImgModule;
		}
		break;
	default:
		ASSERT(FALSE);	// unknown type
		break;
	}

	return res;
}

/*!
	Remove all items and release associated memory held in Item Data of the list control
*/
void CItemListDlg::DeleteAllItems()
{
	for(int nIndex = 0; nIndex < m_itemListCtrl.GetItemCount(); nIndex++)
	{
		ItemDisplayInfo* displayInfo = (ItemDisplayInfo*)m_itemListCtrl.GetItemData(nIndex);
		delete displayInfo;
	}

	m_itemListCtrl.DeleteAllItems();
}

/*!
	Find and select the item in the list control
	\param item Item to be found
	\return true if the item was found, false otherwise
	\note Even thought the item state is set to selected and focused it does not set the focus to the list control itself
*/
bool CItemListDlg::SelectItem(const char* item)
{
	bool res = FALSE;

	LVFINDINFO findInfo = { 0 };
	findInfo.flags = LVFI_STRING | LVFI_PARTIAL;
	findInfo.psz = item;

	const int index = m_itemListCtrl.FindItem(&findInfo);
	if( index > -1 )
	{
		m_itemListCtrl.EnsureVisible(index, FALSE);
		m_itemListCtrl.SetItemState(index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

		res = true;
	}

	return res;
}

/// OnOK virtual override, store the persistent data
void CItemListDlg::OnOK() 
{
	switch( m_type )
	{
	case kItemListTag:
		gItemListTagRecurse = m_optionCheck.GetCheck() != 0;
		break;
	case kItemListModule:
		gItemListModuleList = m_optionCheck.GetCheck() != 0;
		break;
	default:
		// Nothing to do
		break;
	}
	
	CHHelpDialog::OnOK();
}

/// OnCancel virtual override, stop CVS command or cancel dialog
void CItemListDlg::OnCancel() 
{
	CWincvsApp* app = (CWincvsApp*)AfxGetApp();
	if( app && app->IsCvsRunning() && !app->IsCvsStopping() )
	{
		m_itemListCtrl.DeleteAllItems();
		m_itemListCtrl.InsertItem(0, _i18n("Stopping..."), kItemImgBlank);

		m_cvsStopped = true;
		stopCVS();
		return;
	}
	
	CHHelpDialog::OnCancel();
}

/// BN_CLICKED message handler, initialize the related controls and sort settings
void CItemListDlg::OnOption() 
{
	InitOptionCtrls();

	if( m_type == kItemListModule )
	{
		m_listPath.clear();

		m_sortAsc = false;
		m_sortColumn = 0;
	}
}

/// BN_CLICKED message handler, enable revision combo box
void CItemListDlg::OnCheckrev() 
{
	bool enable = m_hasRevCheck.GetCheck() && !(kItemListModule == m_type && !m_optionCheck.GetCheck());

	m_revCombo.EnableWindow(enable);
	m_browseTagButton.EnableWindow(enable);
}

/// BN_CLICKED message handler, enable date combo box
void CItemListDlg::OnCheckdate() 
{
	bool enable = m_hasDateCheck.GetCheck() && !(kItemListModule == m_type && !m_optionCheck.GetCheck());
	
	m_dateCombo.EnableWindow(enable);
}

/// BN_CLICKED message handler, browse for tag/branch
void CItemListDlg::OnBrowseTag() 
{
	ItemSelectionData itemSelectionData(m_cvsrootEntryDlg);
	
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName, &itemSelectionData) )
	{
		m_rev = tagName.c_str();
		m_revCombo.SetWindowText(m_rev);
	}
}

/// BN_CLICKED message handler, browse for file/directory
void CItemListDlg::OnBrowseReadfrom() 
{
	if( m_cvsrootEntryDlg.GetForceCvsrootCheck() )
	{
		ItemSelectionData itemSelectionData(m_cvsrootEntryDlg);
		
		string moduleName;
		if( CompatGetModuleListItem(m_mf, moduleName, &itemSelectionData) )
		{
			m_readFromCombo.SetWindowText(moduleName.c_str());
			GotoDlgCtrl(&m_browseReadFromButton);
		}
	}
	else
	{
		CString strPath;
		m_readFromCombo.GetWindowText(strPath);
		
		string path(strPath);
		if( BrowserGetDirectory("Select a directory to checkout to", path, IsWindow(m_hWnd) ? this : NULL) )
		{
			m_readFromCombo.SetWindowText(path.c_str());
			GotoDlgCtrl(&m_browseReadFromButton);
		}
	}
}

/// BN_CLICKED message handler, browse selection info
void CItemListDlg::OnSelection() 
{
	kSelDetType selDetType = kItemListModule == m_type ? kSelDetPath : kSelDetRevision;
	string selection;

	CString strSelection;
	m_itemEdit.GetWindowText(strSelection);
	selection = (const char*)strSelection;

	if( CompatGetSelectionDetails(m_mf, selDetType, selection) )
	{
		m_itemEdit.SetWindowText(selection.c_str());
		OnOK();
	}
}

/// NM_DBLCLK message handler, select the item and close dialog for tag/modules browser or navigate directories for directory and files listing
void CItemListDlg::OnDblclkItemlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 )
	{
		if( kItemListModule == m_type && m_optionCheck.GetCheck() )
		{
			const CString description = m_itemListCtrl.GetItemText(pNMListView->iItem, 1);
			if( description == STR_ITEMLIST_DIR )
			{
				const CString item = m_itemListCtrl.GetItemText(pNMListView->iItem, 0);
				if( item == STR_ITEMLIST_UP_DIR )
				{
					if( !m_listPath.empty() )
					{
						// Set the item for selection in up-level
						const string backItem = m_listPath.back();
						m_itemEdit.SetWindowText(backItem.c_str());

						m_listPath.pop_back();
					}
				}
				else
				{
					// Make it so it's easy to "return" where we came from
					m_itemEdit.SetWindowText(STR_ITEMLIST_UP_DIR);
					m_listPath.push_back((LPCTSTR)item);
				}

				PostClickRefresh();
			}
		}
		else
		{
			OnOK();
		}
	}
	
	*pResult = 0;
}

/// LVN_ITEMCHANGED message handler, update the item edit box value as selection is changing
void CItemListDlg::OnItemchangedItemlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 && 
		pNMListView->uChanged & LVIF_STATE && pNMListView->uNewState & (LVIS_SELECTED | LVIS_FOCUSED) )
	{
		CString item;

		if( kItemListModule == m_type && m_optionCheck.GetCheck() )
		{
			item = FormatListPath(m_listPath).c_str();
			
			CString selItem = m_itemListCtrl.GetItemText(pNMListView->iItem, 0);
			if( selItem  != STR_ITEMLIST_UP_DIR )
			{
				item += selItem;
			}
			else
			{
				item.TrimRight('/');
			}
		}
		else
		{
			item = m_itemListCtrl.GetItemText(pNMListView->iItem, 0);
		}

		m_itemEdit.SetWindowText(item);
	}
	
	*pResult = 0;
}

/// LVN_COLUMNCLICK message handler, sort items in response to column click
void CItemListDlg::OnColumnclickItemlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if( m_sortColumn != pNMListView->iSubItem )
	{
		m_sortAsc = false;
	}
	else
	{
		m_sortAsc = !m_sortAsc;
	}

	m_sortColumn = pNMListView->iSubItem;

	SortItems(m_sortColumn, m_sortAsc, m_type, m_optionCheck.GetCheck() != 0);

	*pResult = 0;
}

/*!
	Sort items listed in the list control
	\param column Column to sort items by
	\param asc Sort ascending or descending
	\param type List dialog type
	\param option Option state (recurse or list checkbox)
	\note Using list control callback CompareItems
*/
void CItemListDlg::SortItems(int column, bool asc, kItemListDlg type, bool option)
{
	m_headerCtrl.SetSortImage(column, asc);

	CompareInfo info = { 0 };
	
	info.m_column = column;
	info.m_asc = asc;
	info.m_type = type;
	info.m_option = option;

	m_itemListCtrl.SortItems(CompareItems, (DWORD)&info);
}

/// WM_DESTROY message handler, release any memory allocated
void CItemListDlg::OnDestroy() 
{
	DeleteAllItems();

	CHHelpDialog::OnDestroy();
}

/// PreTranslateMessage virtual override, detects the F5 key pressed and refreshes the display
BOOL CItemListDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( WM_KEYUP == pMsg->message )
	{
		if( VK_F5 == pMsg->wParam )
		{
			PostClickRefresh();
		}
	}

	return CHHelpDialog::PreTranslateMessage(pMsg);
}

/// CBN_SELCHANGE message handler, update auto-refresh settings
void CItemListDlg::OnSelchangeAutoRefresh() 
{
	const int autoRefresh = m_autoRefreshCombo.GetCurSel();
	if( autoRefresh > -1 )
	{
		if( kItemListModule == m_type )
		{
			 gItemListAutoRefreshModule = autoRefresh;
		}
		else if( kItemListTag == m_type )
		{
			gItemListAutoRefreshTag = autoRefresh;
		}
		else
		{
			ASSERT(FALSE); // unknown type
		}
	}
}

#endif	/* WIN32 */

#ifdef qMacCvsPP
LSelectTable::LSelectTable(
	LStream *inStream )
		: LOutlineTable( inStream )
{
	// set the table geometry
	LTableMultiGeometry *geom = NEW LTableMultiGeometry(this, 50, 20);
	ThrowIfNil_(geom);
	SetTableGeometry(geom);
	
	// set the table selector
	SetTableSelector(NEW LTableSingleSelector( this )/*LOutlineRowSelector*/ );
	
	// and note that we don't set the table storage....
	
	// most of the table classes not only maintain the graphical
	// representation of your data but also the data itself. But
	// LOutlineTable doesn't really do this...it mostly handles
	// graphical representation... you need to handle your data
	// maintenance elsewhere by yourself.
	
	// insert a couple columns (name and size)
	InsertCols( 1, 0, nil, nil, false );
	geom->SetColWidth(310, 1, 1);

	// Set up keyboard selection and scrolling.
	AddAttachment(NEW LOutlineKeySelector(this, msg_AnyMessage));
	AddAttachment(NEW LKeyScrollAttachment(this));

	// Try to become default commander in the window.
	if (mSuperCommander != nil)
		mSuperCommander->SetLatentSub(this);

	mIconModule = nil;
	mIconTag = nil;
	mIconBranch = nil;

	OSErr err = ::GetIconSuite( &mIconModule, 152, kSelectorAllAvailableData );
	ThrowIfOSErr_(err);	
	ThrowIfResFail_(mIconModule);
	err = ::GetIconSuite( &mIconTag, 153, kSelectorAllAvailableData );
	ThrowIfOSErr_(err);	
	ThrowIfResFail_(mIconTag);
	err = ::GetIconSuite( &mIconBranch, 154, kSelectorAllAvailableData );
	ThrowIfOSErr_(err);	
	ThrowIfResFail_(mIconBranch);
}

LSelectTable::~LSelectTable()
{
	if (mIconModule != 0L)
		::DisposeIconSuite(mIconModule, true);
	if (mIconTag != 0L)
		::DisposeIconSuite(mIconTag, true);
	if (mIconBranch != 0L)
		::DisposeIconSuite(mIconBranch, true);
}

void LSelectTable::FinishCreateSelf()
{
	CSelectItem *theItem = NEW CSelectItem();
	ThrowIfNil_(theItem);
	
	// and insert it at the end of the table
	InsertItem( theItem, nil, nil );
}

CSelectItem::CSelectItem(const string& item, const ItemRowInfo& rowInfo) : mIcon(0L)
{
	m_item = item;
	m_description = FormatItemDescription(rowInfo.m_description);
	m_descriptionEx = FormatItemDescription(rowInfo.m_descriptionEx);

	m_rowInfo = rowInfo;
}

CSelectItem::CSelectItem() : mIcon(0L)
{
}

CSelectItem::~CSelectItem()
{
}

// this is the routine called to know what to draw within the
// table cell. See the comments in LOutlineItem.cp for more info.
void
CSelectItem::GetDrawContentsSelf(
	const STableCell&		inCell,
	SOutlineDrawContents&	ioDrawContents)
{
	switch (inCell.col)
	{
	case 1:
		ioDrawContents.outShowSelection = true;
		ioDrawContents.outHasIcon = mIcon != NULL;
		ioDrawContents.outIconSuite = mIcon;
		ioDrawContents.outTextTraits.style = mIcon == NULL ? italic : normal;
		
		LStr255 pstr(m_item.c_str());
		if(mIcon != NULL)
			LString::CopyPStr(pstr, ioDrawContents.outTextString);
		else
			LString::CopyPStr("\pNo selection", ioDrawContents.outTextString);
		break;
	}
}

// just to be cute, we'll draw an adornment (again, see the LOutlineItem.cp
// comments for more information). We'll draw a groovy gray background
void
CSelectItem::DrawRowAdornments(
	const Rect&		inLocalRowRect )
{
	ShadeRow(UGAColorRamp::GetColor(0), inLocalRowRect);
}

void
CSelectItem::DoubleClick(
	const STableCell&			/* inCell */,
	const SMouseDownEvent&		/* inMouseDown */,
	const SOutlineDrawContents&	/* inDrawContents */,
	Boolean						/* inHitText */)
{
	if(mIcon != NULL)
	{
		LPane * topWind = mOutlineTable;
		while(topWind != 0L && topWind->GetSuperView() != 0L)
			topWind = topWind->GetSuperView();
		LControl*	keyButton = dynamic_cast<LControl*>(topWind->FindPaneByID('ok  '));
		ThrowIfNil_(keyButton);
		keyButton->SimulateHotSpotClick(kControlButtonPart);
	}
}

static void OnRefresh_Select(LWindow *theDialog, kItemListDlg select) 
{
	bool forceCvsroot = theDialog->GetValueForPaneID(item_CheckForceRoot) == Button_On;
	bool recurse = theDialog->GetValueForPaneID(item_ForceRecurse) == Button_On;
	LStr255 pstr;
	theDialog->GetDescriptorForPaneID(item_ModuleName, pstr);
	std::string cstr(pstr);
	
	const char* readFrom = kItemListModule == select && forceCvsroot ? "" : cstr.c_str();
	ListPath listPath;
	
	std::auto_ptr<CItemListConsole> itemListConsole(LaunchItemListCommand(select, recurse, 
		readFrom, forceCvsroot, "" /* %%% cvsroot */, "", "", &listPath));

	LSelectTable *table = dynamic_cast<LSelectTable*>(theDialog->FindPaneByID(item_SelectTagList));
	ThrowIfNil_(table);

	LArrayIterator iter(table->GetFirstLevelItems());
	LOutlineItem* item;
	while (iter.Next(&item))
	{
		table->RemoveItem(item);
	}
	
	const ItemList& itemList = itemListConsole->GetItemList();	
	ItemList::const_iterator it = itemList.begin();
	LOutlineItem *lastItem = 0L;
	while( it != itemList.end() )
	{
		CSelectItem *theItem = NEW CSelectItem((*it).first, (*it).second);
		ThrowIfNil_(theItem);
		
		// and insert it at the end of the table
		table->InsertItem( theItem, nil, lastItem );

		Handle icn = 0L;

		switch( select )
		{
		case kItemListTag:
		{
			bool hasRevision = theItem->m_description.find(STR_ITEMLIST_REVISION) != string::npos;
			bool hasBranch = theItem->m_description.find(STR_ITEMLIST_BRANCH) != string::npos;
			
			if( hasBranch )
				icn = table->mIconBranch;
			else
				icn = table->mIconTag;
		}
		break;
		case kItemListModule:
			icn = table->mIconModule;
			break;
		}

		theItem->mIcon = icn;
		
		it++;
		lastItem = theItem;
	}
}

static void DoDataExchange_Select(LWindow *theDialog, kItemListDlg select, std::string & name, bool & forceCvsroot, const MultiFiles* mf, bool putValue)
{
	DoDataExchange (theDialog, item_CheckForceRoot, forceCvsroot, putValue);
	
	bool recurse = select == kItemListTag ? (bool)gItemListTagRecurse : (bool)gItemListModuleList;
	DoDataExchange (theDialog, item_ForceRecurse, recurse, putValue);
	if(select == kItemListTag)
		gItemListTagRecurse = recurse;
	else
	{
		LPane *pane = theDialog->FindPaneByID(item_ForceRecurse);
		pane->Hide();
		gItemListModuleList = recurse;
	}
	
	CMString files(mf->TotalNumFiles() + mf->NumDirs() + 1, "SelectTagsModules");
	CPStr file;
	if( mf && putValue)
	{
		// Add selected directories
		for( int nIndex = 0; nIndex < mf->NumDirs(); nIndex++ )
		{
			std::string path;
			if( mf->getdir(nIndex, path) )
			{
				files.Insert(path);
				if(file.empty())
					file = path;
			}
		}

		if(select == kItemListTag && mf->TotalNumFiles())
		{
			file = "";
			MultiFiles::const_iterator mfi;
			for(mfi = mf->begin(); mfi != mf->end(); ++mfi)
			{
				for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
				{
					string fullPath;
					mfi->GetFullPath(nIndex, fullPath);

					files.Insert(fullPath);
					if(file.empty())
						file = fullPath;
				}
			}
		}
	}
	
	LPopupFiller *filler = dynamic_cast<LPopupFiller*>(theDialog->FindPaneByID(item_PopModName));
	filler->DoDataExchange(files, putValue);

	DoDataExchange (theDialog, item_ModuleName, file, putValue);

	LSelectTable *table = dynamic_cast<LSelectTable*>(theDialog->FindPaneByID(item_SelectTagList));
	ThrowIfNil_(table);

	if(!putValue)
	{
		CSelectItem *item;
		STableCell cell;
		cell = table->GetFirstSelectedCell();
		if(!cell.IsNullCell())
		{
			item = dynamic_cast<CSelectItem*>
				(table->FindItemForRow(cell.row));
			ThrowIfNil_(item != 0L);

			if(item->mIcon != 0L)
				name = item->m_item.c_str();
		}
	}
}
#endif /* qMacCvsPP */

#if qUnix

#if qGTK
static void* _ImgBranchAndRevision;
static void* _ImgBranch;
static void* _ImgRevision;
static void* _ImgUpDir;
static void* _ImgDir;
static void* _ImgFile;
static void* _ImgModule;
#endif

class UCvsItemListDlg : public UWidget
{
	UDECLARE_DYNAMIC(UCvsItemListDlg)
public:
	UCvsItemListDlg(const MultiFiles* mf,
		const char* item, kItemListDlg type, ItemList& itemList, const ListPath* listPath,
		bool forceCvsroot, const char* rev, const char* date);   // standard constructor
	virtual ~UCvsItemListDlg() {}

	enum
	{
		kOK = EV_COMMAND_START,	// 0
		kCancel,				// 1
		kSelTagBranch,          // 2
		kSelectionDetailsKnob,  // 3
		kTagBranchList,         // 4
		kFilePathList,          // 5
		kFilePathBrowse,        // 6
		kRefresh,               // 7
		kRecurse,               // 8
		kForceRoot,             // 9
		kRevTag,                // 10
		kRevTagCombo,           // 11
		kRevTagBrowse,          // 12
		kDate,                  // 13
		kDateCombo,             // 14
		kAutoRefresh,           // 15
		kItemDesc,              // 16		
	};

	virtual void DoDataExchange(bool fill);
public:
	ListPath m_listPath;	/*!< Path to the current list location */
	std::string 	m_item;
	bool	m_forceCvsroot;
	bool	m_hasRev;
	std::string 	m_rev;
	std::string 	m_date;
	bool	m_hasDate;
	bool	m_recurse;

	int m_sortColumn; // column to sort by
	bool m_ascendant; // sort in ascending order
	kItemListDlg m_type;	/*!< Dialog type */

protected:
	void ShowColumns(void);
		// show all columns of the actual model

	ev_msg void OnDestroy();
	ev_msg void OnCreate();
	ev_msg int OnOK(void);
	ev_msg int OnCancel(void);
	ev_msg int OnDate(void);
	ev_msg int OnRev(void);
	ev_msg int OnRefresh(void);
	ev_msg int OnTagRevBrowse(void);
	ev_msg int OnSelectionDetails(void);
	ev_msg int OnForceRoot(void);
	ev_msg int OnOptionChk(void);
	ev_msg int OnReadFromBrowse(void);
	ev_msg void OnItemDblClick(void);
	ev_msg void OnItemSelect(int entry, const char *txt);
	ev_msg void OnSelColumn(int column);

	UDECLARE_MESSAGE_MAP()

private:
	const MultiFiles* m_mf;	/*!< Browser selection */
	
	ItemList& m_itemList;	/*!< Item's container */

	KiColumnModel* m_modelColumns;
		// columnar model in use

	bool CheckSettings( void);
	void InitOptionCtrls( void);
	void InitListCtrl(const kItemDisplayType displayType);
	void* GetImageForItem( kItemListDlg type, bool option, const ItemDisplayInfo* displayInfo);
#ifdef HAVE_SSTREAM
	void DisplayItems( const ItemList& itemList, const int launchRes, 
		istringstream* cvsErr = NULL, const bool showUpDir = true);
#else
	void DisplayItems( const ItemList& itemList, const int launchRes, 
		istrstream* cvsErr = NULL, const bool showUpDir = true);
#endif
	void PostClickRefresh( void);
	void EnableControls(bool enable);

	void Resort( void);
};

UIMPLEMENT_DYNAMIC(UCvsItemListDlg, UWidget)

UBEGIN_MESSAGE_MAP(UCvsItemListDlg, UWidget)
	ON_UCOMMAND(UCvsItemListDlg::kOK, UCvsItemListDlg::OnOK)
	ON_UCOMMAND(UCvsItemListDlg::kCancel, UCvsItemListDlg::OnCancel)
	ON_UCOMMAND(UCvsItemListDlg::kDate, UCvsItemListDlg::OnDate)
	ON_UCOMMAND(UCvsItemListDlg::kRevTag, UCvsItemListDlg::OnRev)
	ON_UCOMMAND(UCvsItemListDlg::kRefresh, UCvsItemListDlg::OnRefresh)
	ON_UCOMMAND(UCvsItemListDlg::kRevTagBrowse, UCvsItemListDlg::OnTagRevBrowse)
	ON_UCOMMAND(UCvsItemListDlg::kSelectionDetailsKnob, UCvsItemListDlg::OnSelectionDetails)
	ON_UCOMMAND(UCvsItemListDlg::kForceRoot, UCvsItemListDlg::OnForceRoot)
	ON_UCOMMAND(UCvsItemListDlg::kRecurse, UCvsItemListDlg::OnOptionChk)
	ON_UCOMMAND(UCvsItemListDlg::kFilePathBrowse, UCvsItemListDlg::OnReadFromBrowse)
	ON_LIST_SELECTING(UCvsItemListDlg::kTagBranchList, UCvsItemListDlg::OnItemSelect)
	ON_LIST_DBLCLICK(UCvsItemListDlg::kTagBranchList, UCvsItemListDlg::OnItemDblClick)
	ON_LIST_SELCOLUMN(UCvsItemListDlg::kTagBranchList, UCvsItemListDlg::OnSelColumn)
	ON_UDESTROY(UCvsItemListDlg)
	ON_UCREATE(UCvsItemListDlg)
UEND_MESSAGE_MAP()


UCvsItemListDlg::UCvsItemListDlg(const MultiFiles* mf,
						   const char* item, kItemListDlg type, ItemList& itemList, const ListPath* listPath,
						   bool forceCvsroot, const char* rev, const char* date) 
						   : UWidget(::UEventGetWidID()), m_forceCvsroot( forceCvsroot), m_type( type), m_mf(mf), m_itemList(itemList)
						   
{
	m_ascendant = true; // default ascending order
	m_sortColumn = 0; // default sort by first column
	m_hasRev = false;
	m_hasDate = false;
	switch( m_type )
	{
	case kItemListTag:
		m_recurse = (bool)gItemListTagRecurse;
		break;
	case kItemListModule:
		m_recurse = (bool)gItemListModuleList;
		break;
	default:
		m_recurse = false;
		break;
	}
	if( listPath )
	{
		m_listPath = *listPath;
	}
	m_rev = rev;
	m_date = date;

}

void UCvsItemListDlg::OnCreate(void)
{
	int columnCount = 2;//GetCount();
	
	if( _ImgBranchAndRevision == 0L)
	{
// FIX ME missing icons!
		_ImgBranchAndRevision = UCreate_pixmap(this, "branch.xpm");
		_ImgBranch = UCreate_pixmap(this, "branch.xpm");
		_ImgRevision = UCreate_pixmap(this, "tag.xpm");
		_ImgUpDir = UCreate_pixmap(this, "folder.xpm");
		_ImgDir = UCreate_pixmap(this, "folder.xpm");
		_ImgFile = UCreate_pixmap(this, "cvsfile.xpm");
		_ImgModule = UCreate_pixmap(this, "explore.xpm");
	}

	UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kUMainWidget, 0), 0L);

	UEventSendMessage(GetWidID(), EV_LIST_SHOWCOLUMNS, UMAKEINT(kUMainWidget, columnCount), 0L);

	for(int i = 0; i<columnCount; i++)
	{
		LV_COLUMN lv;

		m_modelColumns->GetAt(i)->GetSetupData(&lv);
		UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kUMainWidget, i), (char*)lv.name);
		UEventSendMessage(GetWidID(), EV_LIST_SETCOLWIDTH, UMAKEINT(kUMainWidget, i), (void *)lv.width);
	}
		
	UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kUMainWidget, 1), 0L);
}

void UCvsItemListDlg::OnDestroy(void)
{
	delete this;
}


void UCvsItemListDlg::PostClickRefresh( void)
{
	UEventPostMessage(GetWidID(), EV_CMD, kRefresh, 0L);
}

int UCvsItemListDlg::OnOK(void)
{
	switch( m_type )
	{
	case kItemListTag:
		gItemListTagRecurse = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) != 0;
		break;
	case kItemListModule:
		gItemListModuleList = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) != 0;
		break;
	default:
		// Nothing to do
		break;
	}
	EndModal(true);
	return 0;
}

int UCvsItemListDlg::OnCancel(void)
{
	EndModal(false);
	return 0;
}

int UCvsItemListDlg::OnDate()
{
	bool enable = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kDate, 0L) 
		&& !(kItemListModule == m_type && !UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L));
//	m_hasDate = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kDate, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kDateCombo, enable), 0L);
	return 0;
}

int UCvsItemListDlg::OnRev()
{
	bool enable = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRevTag, 0L) 
		&& !(kItemListModule == m_type && !UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L));
//	m_hasRev = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRevTag, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevTagCombo, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevTagBrowse, enable), 0L);
	return 0;
}

void UCvsItemListDlg::DoDataExchange(bool fill)
{
	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kDate, m_hasDate), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kRevTag, m_hasRev), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kRecurse, m_recurse), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kForceRoot, m_forceCvsroot), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kFilePathBrowse, m_forceCvsroot), 0L);

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kRevTagCombo, 0L);
		UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kRevTagCombo, (void *)(const char *)(m_rev.c_str()));
		const std::vector<std::string > & rlist = gRevNames.GetList();
		std::vector<std::string >::const_iterator i;
		for(i = rlist.begin(); i != rlist.end(); ++i)
		{
			UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kRevTagCombo, (void *)(const char *)(*i).c_str());
		}
		OnRev();

		UEventSendMessage(GetWidID(), EV_COMBO_RESETALL, kDateCombo, 0L);
		UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kDateCombo, (void *)(const char *)(m_date.c_str()));
		const std::vector<std::string > & dlist = gDateNames.GetList();
		for(i = dlist.begin(); i != dlist.end(); ++i)
		{
			UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kDateCombo, (void *)(const char *)(*i).c_str());
		}
		OnDate();

		// init dialog style...
		if( m_mf )
		{
			// Add selected directories
			for( int nIndex = 0; nIndex < m_mf->NumDirs(); nIndex++ )
			{
				std::string path;
				if( m_mf->getdir(nIndex, path) )
				{
					UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kFilePathList, (void*)(const char*)path.c_str());
				}
			}
		}

		int nReadFromSelection = UEventSendMessage(GetWidID(), EV_COMBO_GETCOUNT, kFilePathList, 0L);

		switch( m_type )
		{
		case kItemListTag:
			// Set windows titles
	 		UEventSendMessage(GetWidID(), EV_SETTEXT, kUMainWidget, (void*)_i18n("Select tag/branch"));
	 		UEventSendMessage(GetWidID(), EV_SETTEXT, kItemDesc, (void*)"Tag/branch:");
	 		UEventSendMessage(GetWidID(), EV_SETTEXT, kRecurse, (void*)_i18n("Recurse"));

			// The browser selection
			if( m_mf && m_mf->TotalNumFiles() )
			{
				MultiFiles::const_iterator mfi;
				for(mfi = m_mf->begin(); mfi != m_mf->end(); ++mfi)
				{
					for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
					{
						string fullPath;
						mfi->GetFullPath(nIndex, fullPath);

						UEventSendMessage(GetWidID(), EV_COMBO_APPEND, kFilePathList, (void*)(const char*)fullPath.c_str());
					}
				}
				// It's better to have a single file selected initially for the performance reasons
				if( nReadFromSelection)
					UEventSendMessage(GetWidID(), EV_COMBO_SETSEL, UMAKEINT(kFilePathList, nReadFromSelection+1), 0L);
			}
			
			break;
		case kItemListModule:
			// Set windows titles
	 		UEventSendMessage(GetWidID(), EV_SETTEXT, kUMainWidget, (void*)_i18n("Select module"));
	 		UEventSendMessage(GetWidID(), EV_SETTEXT, kItemDesc, (void*)"Module:");
	 		UEventSendMessage(GetWidID(), EV_SETTEXT, kRecurse, (void*)_i18n("List"));

			if( nReadFromSelection)
				UEventSendMessage(GetWidID(), EV_COMBO_SETSEL, UMAKEINT(kFilePathList, 0), 0L);

	 		UEventSendMessage(GetWidID(), EV_SHOW_WIDGET, kFilePathBrowse, FALSE);
			break;
		default:
	//		ASSERT(FALSE);	// unknown type
			EndModal(false);
			return;
		}
		
		// Trigger refresh
		kItemListAutoRefreshType autoRefresh = kAutoRefreshFirstTime;

		if( kItemListModule == m_type )
		{
			autoRefresh = (kItemListAutoRefreshType)(int)gItemListAutoRefreshModule;
		}
		else if( kItemListTag == m_type )
		{
			autoRefresh = (kItemListAutoRefreshType)(int)gItemListAutoRefreshTag;
		}
		else
		{
	//		ASSERT(FALSE); // unknown type
		}

		UEventSendMessage(GetWidID(), EV_COMBO_SETSEL, UMAKEINT(kAutoRefresh, autoRefresh), 0L);
		if( (kAutoRefreshAlways == autoRefresh) || (kAutoRefreshFirstTime == autoRefresh && m_itemList.empty()) )
		{
			// Force the window to show itself
//			gdk_flush();
			PostClickRefresh();
		}
		else
		{
			DisplayItems( m_itemList, 0, NULL, false);
			Resort();

			std::string item;
			UEventSendMessage(GetWidID(), EV_GETTEXT, kSelTagBranch, &item);
			if( !item.empty() )
			{
				UEventSendMessage(GetWidID(), EV_CHGFOCUS, kSelTagBranch, 0L);
			}

		}
	}
	else
	{
		m_forceCvsroot = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRoot, 0L);
		m_recurse = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kSelTagBranch, &m_item);
		if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRevTag, 0L) )
		{
			UEventSendMessage(GetWidID(), EV_GETTEXT, kRevTagCombo, &m_rev);
		}
			
		if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kDate, 0L) )
		{
			UEventSendMessage(GetWidID(), EV_GETTEXT, kDateCombo, &m_date);
		}
	}
}

/*!
	Enable or disable controls
	\param enable Set to TRUE to enable controls, FALSE to disable controls
*/
void UCvsItemListDlg::EnableControls(bool enable)
{
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kOK, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRefresh, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kFilePathList, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kTagBranchList, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kSelTagBranch, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kSelectionDetailsKnob, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRecurse, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kForceRoot, enable), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kAutoRefresh, enable), 0L);

	if( !enable )
	{
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevTag, FALSE), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevTagCombo, FALSE), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevTagBrowse, FALSE), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kDate, FALSE), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kDateCombo, FALSE), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kFilePathBrowse, FALSE), 0L);
	}
	else
	{
		InitOptionCtrls();
		OnForceRoot();
	}
}

int UCvsItemListDlg::OnRefresh(void)
{

	// Verify the settings
	if( !CheckSettings() )
	{
		return 0;
	}

	// Prepare controls
	EnableControls(FALSE);

	// Change the Cancel button's name to "Stop"
	std::string strPrevCancelText;
 	UEventSendMessage(GetWidID(), EV_GETTEXT, kCancel, &strPrevCancelText);
 	UEventSendMessage(GetWidID(), EV_SETTEXT, kCancel, (void*)_i18n("Stop"));

	// Store current item text
	std::string strItem;
 	UEventSendMessage(GetWidID(), EV_GETTEXT, kSelTagBranch, &strItem);

	// Display the reading data prompt
	InitListCtrl(kItemDisplayWait);

	// Select and launch command
	std::string strReadFrom;
	std::string revision;
	std::string date;

	UEventSendMessage(GetWidID(), EV_GETTEXT, kFilePathList, &strReadFrom);

	if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRevTag, 0L) )
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kRevTagCombo, &revision);
	}
		
	if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kDate, 0L) )
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kDateCombo, &date);
	}

	const char* readFrom = kItemListModule == m_type && 
		UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRoot, 0L) && 
		!UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) ? "" : (const char*)strReadFrom.c_str();

	std::auto_ptr<CItemListConsole> itemListConsole(
		LaunchItemListCommand(m_type, UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) != 0, 
			readFrom, UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRoot, 0L) != 0, "" /* cvsroot */,
			revision.c_str(), date.c_str(), &m_listPath));

	if( !itemListConsole.get() )
	{
		EndModal(false);
		return 0;
	}

	// Initialize list control
	InitListCtrl(kItemDisplayData);

	// Restore control's state
	EnableControls(TRUE);

	// Display data
#ifdef HAVE_SSTREAM
	istringstream myCvsErr(itemListConsole->GetCvsErr().GetData());
#else
	istrstream myCvsErr(itemListConsole->GetCvsErr().GetData(), itemListConsole->GetCvsErr().GetSize());
#endif

	DisplayItems( itemListConsole->GetItemList(), itemListConsole->GetLaunchRes(), &myCvsErr);

	// Restore Cancel's button name
	UEventSendMessage(GetWidID(), EV_SETTEXT, kCancel, (void*)(const char*)strPrevCancelText.c_str());
	
	// Sort items
	Resort();
	
	// Try to select the item
	UEventSendMessage(GetWidID(), EV_GETTEXT, kSelTagBranch, &strItem);
	if( strItem.empty())
	{
		int row = -1;
		row = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL,
									UMAKEINT(kTagBranchList, row), 0L);
		if( row != -1)
		{
			ULIST_INFO info1;
			info1.col = 1;
			info1.row = row;
			UEventSendMessage(GetWidID(), EV_LIST_GETINFO, kTagBranchList, &info1);
			strItem = ((ItemDisplayInfo*)(info1.data))->GetItem().c_str();
			UEventSendMessage(GetWidID(), EV_SETTEXT, kSelTagBranch, (void*)strItem.c_str());
		}
	}
	
	if( !strItem.empty() )
	{
		UEventSendMessage(GetWidID(), EV_CHGFOCUS, kSelTagBranch, 0L);
	}
	
	return 0;
}

/*!
	Verify that the settings are correct to retrieve items
	\return true if the settings are correct, false otherwise
*/
bool UCvsItemListDlg::CheckSettings(void)
{
	std::string date;
	std::string rev;
	if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRevTag, 0L) )
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kRevTagCombo, &rev);
		if ( rev.empty()) 
		{
			CvsAlert al( kCvsAlertStopIcon, _i18n("Please enter more than 1 character for revision/tag"));
			al.ShowAlert();
			UEventSendMessage(GetWidID(), EV_CHGFOCUS, kRevTagCombo, 0L);
			return false;
		}
	}
		
	if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kDate, 0L) )
	{
		UEventSendMessage(GetWidID(), EV_GETTEXT, kDateCombo, &date);
		if ( date.empty()) 
		{
			CvsAlert al( kCvsAlertStopIcon, _i18n("Please enter more than 1 character for date"));
			al.ShowAlert();
			UEventSendMessage(GetWidID(), EV_CHGFOCUS, kDateCombo, 0L);
			return false;
		}
	}
	return true;
}

int UCvsItemListDlg::OnTagRevBrowse(void)
{
	std::string tagName;
	if( CompatGetTagListItem( m_mf, tagName) )
	{
		m_rev = (const char*)tagName.c_str();
		UEventSendMessage(GetWidID(), EV_SETTEXT, kRevTagCombo, (void*)(const char*)tagName.c_str());
	}
	return 0;
}

int UCvsItemListDlg::OnSelectionDetails(void)
{
	kSelDetType selDetType = kItemListModule == m_type ? kSelDetPath : kSelDetRevision;
	std::string selection;

 	UEventSendMessage(GetWidID(), EV_GETTEXT, kSelTagBranch, &selection);

	if( CompatGetSelectionDetails( m_mf, selDetType, selection) )
	{
 		UEventSendMessage(GetWidID(), EV_SETTEXT, kSelTagBranch, (void*)(const char*)selection.c_str());
		OnOK();
	}
	return 0;
}

int UCvsItemListDlg::OnForceRoot(void)
{
	bool enableReadFrom = !(kItemListModule == m_type 
		&& UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRoot, 0L) 
		&& !UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L));
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT( kFilePathList, enableReadFrom), 0L);

	bool enableBrowseReadFrom = kItemListTag == m_type 
		&& UEventSendMessage(GetWidID(), EV_QUERYSTATE, kForceRoot, 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT( kFilePathBrowse, enableBrowseReadFrom), 0L);
	return 0;
}

int UCvsItemListDlg::OnOptionChk(void)
{
	m_recurse = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L);

	InitOptionCtrls();

	if( m_type == kItemListModule )
	{
		m_listPath.clear();

		m_ascendant = false;
		m_sortColumn = 0;
	}

	return 0;
}
 
int UCvsItemListDlg::OnReadFromBrowse(void)
{
	std::string moduleName;
	if( CompatGetModuleListItem( m_mf, moduleName) )
	{
		UEventSendMessage(GetWidID(), EV_SETTEXT, kFilePathList, (void*)(const char*)moduleName.c_str());
		UEventSendMessage(GetWidID(), EV_CHGFOCUS, kFilePathBrowse, 0L);
	}
	return 0;
}

/*!
	Set the controls dependent on the Option state
*/
void UCvsItemListDlg::InitOptionCtrls( void)
{
	bool enableDateRev = (kItemListModule == m_type 
		&& !UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L)) ? false : true;
	
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kDate, enableDateRev), 0L);
	UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kRevTag, enableDateRev), 0L);
	
	OnRev();
	OnDate();

	switch( m_type )
	{
	case kItemListTag:
		gItemListTagRecurse = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) != 0;
		break;
	case kItemListModule:
		gItemListModuleList = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) != 0;
		break;
	default:
		// Nothing to do
		break;
	}
}

void UCvsItemListDlg::InitListCtrl(const kItemDisplayType displayType)
{
	// Remove all items and columns
	UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kTagBranchList, 0), 0L);
	// FIX ME remove all ROWSETDATA items here!
	UEventSendMessage(GetWidID(), EV_LIST_RESETALL, kTagBranchList, 0L);
	UEventSendMessage(GetWidID(), EV_LIST_HIDECOLUMNS, UMAKEINT(kTagBranchList,3), 0L);

	// Set the appropriate columns
	switch( displayType )
	{
	case kItemDisplayWait:
		UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,0), (void*)_i18n("Action"));
		UEventSendMessage(GetWidID(), EV_LIST_SHOWCOLUMNS, UMAKEINT(kTagBranchList,1), 0L);
		{
			int rownum;
			ULIST_INSERT entry;
			UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kTagBranchList, &rownum);
			UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kTagBranchList, rownum), NULL);
			entry.row = rownum;
			entry.col = 0;
			entry.title = _i18n("Please wait while retrieving data");
			entry.icon = 0L;
			UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
			entry.col = 0;
			entry.title = _i18n("You can press Stop button to cancel the operation");
			entry.icon = 0L;
			UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
		}
		break;
	case kItemDisplayData:
		{
			switch( m_type )
			{
			case kItemListTag:
				UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,0), (void*)"Tag/branch");
				UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,1), (void*)_i18n("Description"));
				UEventSendMessage(GetWidID(), EV_LIST_SHOWCOLUMNS, UMAKEINT(kTagBranchList,2), 0L);
				break;
			case kItemListModule:
				if( UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L))
				{
					UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,0), (void*)"Module");
					UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,1), (void*)_i18n("Description"));
					UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,2), (void*)_i18n("Timestamp"));
				}
				else
				{
					UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,0), (void*)"Module");
					UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,1), (void*)_i18n("Options"));
					UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,2), (void*)_i18n("Directories / Files"));
				}
				UEventSendMessage(GetWidID(), EV_LIST_SHOWCOLUMNS, UMAKEINT(kTagBranchList,3), 0L);
				break;
			default:
//				ASSERT(FALSE);	// unknown type
				break;
			}
		}
		break;
	case kItemDisplayInfo:
		UEventSendMessage(GetWidID(), EV_LIST_SETCOLTITLE, UMAKEINT(kTagBranchList,0), (void*)_i18n("Information"));
		UEventSendMessage(GetWidID(), EV_LIST_SHOWCOLUMNS, UMAKEINT(kTagBranchList,1), 0L);
		break;
	default:
//		ASSERT(FALSE);	// unknown display type
		break;
	}
	UEventSendMessage(GetWidID(), EV_LIST_SETFEEDBACK, UMAKEINT(kTagBranchList, 1), 0L);
}

void UCvsItemListDlg::DisplayItems(const ItemList& itemList, 
	const int launchRes, 
#ifdef HAVE_SSTREAM
	istringstream* cvsErr /*= NULL*/, 
#else
	istrstream* cvsErr /*= NULL*/, 
#endif
	const bool showUpDir /*= true*/)
{

	if( itemList.size() == 0 )
	{
//		if( !m_cvsStopped )
		{
			if( launchRes == 0 )
			{
				// Show the info if the modules file doesn't contain any modules
				if( kItemListModule == m_type && !UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L))
				{
					InitListCtrl(kItemDisplayInfo);
					int rownum;
					ULIST_INSERT entry;
					UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kTagBranchList, &rownum);
					UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kTagBranchList, rownum), NULL);
					entry.row = rownum;
					entry.col = 0;
					entry.title = _i18n("Modules file doesn't contain any modules definitions");
					entry.icon = 0L;
					UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
					UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kTagBranchList, 0), 0L);
				}
			}
			else if( cvsErr )
			{
				InitListCtrl(kItemDisplayInfo);
				
				// Show the error in the list control
				int rownum;
				ULIST_INSERT entry;
				UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kTagBranchList, &rownum);
				UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kTagBranchList, rownum), NULL);
				entry.row = rownum;
				entry.col = 0;
				entry.title = _i18n("Command failed:");
				entry.icon = 0L;
				UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
				
				string line;
				while( getline(*cvsErr, line) )
				{
					UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kTagBranchList, &rownum);
					UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kTagBranchList, rownum), NULL);
					entry.row = rownum;
					entry.col = 0;
					entry.title = line.c_str();
					entry.icon = 0L;
					UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
				}
 				UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kTagBranchList, 0), 0L);
 			}
		}
	}
	else
	{
		// Insert the items
		for( ItemList::const_iterator it = itemList.begin(); it != itemList.end(); ++it)
		{
			ItemDisplayInfo* displayInfo = new ItemDisplayInfo;
			if( displayInfo )
			{
				displayInfo->Create((*it).first, (*it).second);
				int rownum;
				ULIST_INSERT entry;
				UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kTagBranchList, &rownum);
				if ( rownum > -1) 
				{
					void* image = GetImageForItem( 
						m_type, UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L), displayInfo);
					entry.row = rownum;
					entry.col = 0;
					entry.title = displayInfo->GetItem().c_str();
					entry.icon = image;
					UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
					if( kItemListTag == m_type || kItemListModule == m_type )
					{
						entry.col = 1;
						entry.title = displayInfo->GetDescription().c_str();
						entry.icon = 0L;
						UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
					}
					if( kItemListModule == m_type )
					{
						entry.col = 2;
						entry.title = displayInfo->GetDescriptionEx().c_str();
						entry.icon = 0L;
						UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
					}
					UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kTagBranchList, rownum), displayInfo);
				}
				else
				{
					delete displayInfo;
				}
			}
		}
	}
	
	if( showUpDir 
		&& kItemListModule == m_type 
		&& UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) 
		&& !m_listPath.empty() )
	{
		// Insert the ".." to move one level up in the directories tree
		ItemDisplayInfo* displayInfo = new ItemDisplayInfo;
		if( displayInfo )
		{
			ItemRowInfo rowInfo;
			rowInfo.m_description.insert(STR_ITEMLIST_DIR);
				
			displayInfo->Create(string(STR_ITEMLIST_UP_DIR), rowInfo);
			int rownum;
			ULIST_INSERT entry;
			UEventSendMessage(GetWidID(), EV_LIST_NEWROW, kTagBranchList, &rownum);
			if ( rownum > -1) 
			{
				void* image = GetImageForItem( 
					m_type, UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L), displayInfo);
				entry.row = rownum;
				entry.col = 0;
				entry.title = displayInfo->GetItem().c_str();
				entry.icon = image;
				UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
				entry.col = 1;
				entry.title = displayInfo->GetDescription().c_str();
				entry.icon = 0L;
				UEventSendMessage(GetWidID(), EV_LIST_INSERT, kTagBranchList, &entry);
				UEventSendMessage(GetWidID(), EV_LIST_ROWSETDATA, UMAKEINT(kTagBranchList, rownum), displayInfo);
			}
			else
			{
				delete displayInfo;
			}
		}
	}
}

void* UCvsItemListDlg::GetImageForItem( kItemListDlg type, bool option, const ItemDisplayInfo* displayInfo)
{
	void* res = NULL;
	
	switch( type )
	{
	case kItemListTag:
		{
			bool hasRevision = displayInfo->GetDescription().find(STR_ITEMLIST_REVISION) != string::npos;
			bool hasBranch = displayInfo->GetDescription().find(STR_ITEMLIST_BRANCH) != string::npos;
			
			if( hasRevision && hasBranch )
			{
				res = _ImgBranchAndRevision;
			}
			else if( hasBranch )
			{
				res = _ImgBranch;
			}
			else
			{
				res = _ImgRevision;
			}
		}
		break;
	case kItemListModule:
		if( option )
		{
			if( strcmp(displayInfo->GetItem().c_str(), STR_ITEMLIST_UP_DIR) == 0 )
			{
				res = _ImgUpDir;
			}
			else
			{
				bool isDir = displayInfo->GetDescription().find(STR_ITEMLIST_DIR) != string::npos;
				res = isDir ? _ImgDir : _ImgFile;
			}
		}
		else
		{
			res	= _ImgModule;
		}
		break;
	default:
//		ASSERT(FALSE);	// unknown type
		break;
	}

	return res;
}


void UCvsItemListDlg::OnItemSelect(int entry, const char *txt)
{
	ULIST_INFO info;
	info.col = 0;
	info.row = entry;
	UEventSendMessage(GetWidID(), EV_LIST_GETINFO, kTagBranchList, &info);

	std::string item;

	if( kItemListModule == m_type && UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) )
	{
		item = FormatListPath(m_listPath).c_str();
		
		std::string selItem = ((ItemDisplayInfo*)(info.data))->GetItem().c_str();
		if( selItem  != STR_ITEMLIST_UP_DIR )
		{
			item += selItem;
		}
		else
		{
			TrimRight(item, '/');
		}
	}
	else
	{
		item = ((ItemDisplayInfo*)(info.data))->GetItem().c_str();
	}

	UEventSendMessage(GetWidID(), EV_SETTEXT, kSelTagBranch, (void*)(const char*)item.c_str());
}

void UCvsItemListDlg::OnItemDblClick(void)
{
	int row = -1;
	if((row = UEventSendMessage(GetWidID(), EV_LIST_GETNEXTSEL,
								UMAKEINT(kTagBranchList, row), 0L)) == -1)
		return;

	if( kItemListModule == m_type && UEventSendMessage(GetWidID(), EV_QUERYSTATE, kRecurse, 0L) )
	{
		ULIST_INFO info1;
		info1.col = 1;
		info1.row = row;
		UEventSendMessage(GetWidID(), EV_LIST_GETINFO, kTagBranchList, &info1);

		const std::string description = ((ItemDisplayInfo*)(info1.data))->GetDescription().c_str();
		if( description == STR_ITEMLIST_DIR )
		{
			const std::string item = ((ItemDisplayInfo*)(info1.data))->GetItem().c_str();
			if( item == STR_ITEMLIST_UP_DIR )
			{
				if( !m_listPath.empty() )
				{
					// Set the item for selection in up-level
					const string backItem = m_listPath.back();
					UEventSendMessage(GetWidID(), EV_SETTEXT, kSelTagBranch, (void*)backItem.c_str());

					m_listPath.pop_back();
				}
			}
			else
			{
				// Make it so it's easy to "return" where we came from
				UEventSendMessage(GetWidID(), EV_SETTEXT, kSelTagBranch, (void*)STR_ITEMLIST_UP_DIR);
				m_listPath.push_back(item.c_str());
			}

			PostClickRefresh();
		}
	}
	else
	{
		OnOK();
	}
}

void UCvsItemListDlg::OnSelColumn(int column)
{
	if(column == m_sortColumn)
		m_ascendant = !m_ascendant;
	else
	{
		m_ascendant = true;
		m_sortColumn = column;
	}
	Resort();
}

static int ItemListSorter(UWidget *w, void *data1, void *data2)
{
	ItemDisplayInfo *i1 = (ItemDisplayInfo*)data1;
	ItemDisplayInfo *i2 = (ItemDisplayInfo*)data2;
	UCvsItemListDlg *dlg = (UCvsItemListDlg*)w;
	int res = 0;

	switch ( dlg->m_type) {
	case kItemListModule:
		if( dlg->m_recurse )
		{
			if( strcmp(i1->GetItem().c_str(), STR_ITEMLIST_UP_DIR) == 0 )
			{
				res = dlg->m_ascendant ? 1 : -1;
			}
			else if( strcmp(i2->GetItem().c_str(), STR_ITEMLIST_UP_DIR) == 0 )
			{
				res = dlg->m_ascendant ? -1 : 1;
			}
			else
			{
				bool isDir1 = strcmp(i1->GetDescription().c_str(), STR_ITEMLIST_DIR) == 0;
				bool isDir2 = strcmp(i2->GetDescription().c_str(), STR_ITEMLIST_DIR) == 0;

				if( isDir1 && !isDir2 )
				{
					res = dlg->m_ascendant ? 1 : -1;
				}
				else if( !isDir1 && isDir2 )
				{
					res = dlg->m_ascendant ? -1 : 1;
				}
				else if( isDir1 && isDir2 )
				{
					res = stricmp(i1->GetItem().c_str(), i2->GetItem().c_str());
					if( dlg->m_sortColumn != 0 )
					{
						if( dlg->m_ascendant )
						{
							res *= -1;
						}
					}
				}
				else
				{
					switch( dlg->m_sortColumn )
					{
					case 0:
						res = stricmp(i1->GetItem().c_str(), i2->GetItem().c_str());
						break;
					case 1:
						res = stricmp(i1->GetDescription().c_str(), i2->GetDescription().c_str());
						break;
					case 2:
						{
							const time_t time1 = GetDateTime(i1->GetDescriptionEx().c_str()); 
							const time_t time2 = GetDateTime(i2->GetDescriptionEx().c_str()); 
							
							if( time1 != (time_t)-1 && time2 != (time_t)-1 ) 
							{ 
								res = time1 < time2 ? -1 : (time1 > time2 ? 1 : 0);
							} 
							else
							{
								res = stricmp(i1->GetDescriptionEx().c_str(), i2->GetDescriptionEx().c_str());
							}
						}
						break;
					default:
//						g_assert(0); // should not come here
						break;
					}
				}
			}
		}
		else
		{
			switch( dlg->m_sortColumn )
			{
			case 0:
				res = stricmp(i1->GetItem().c_str(), i2->GetItem().c_str());
				break;
			case 1:
				res = stricmp(i1->GetDescription().c_str(), i2->GetDescription().c_str());
				break;
			case 2:
				res = stricmp(i1->GetDescriptionEx().c_str(), i2->GetDescriptionEx().c_str());
				break;
			default:
//				g_assert(0); // should not come here
				break;
			}
		}
		break; 
	case kItemListTag:
		switch( dlg->m_sortColumn )
		{
		case 0:
			res = stricmp(i1->GetItem().c_str(), i2->GetItem().c_str());
			break;
		case 1:
			res = stricmp(i1->GetDescription().c_str(), i2->GetDescription().c_str());
			break;
		default:
//			g_assert(0); // should not come here
			break;
		}
		break;
	default:
//		g_assert(0);
		break;
	} 

	if( res )
	{
		if( dlg->m_ascendant)
		{
			res *= -1;
		}
	}

	return res;
}

void UCvsItemListDlg::Resort( void)
{
	UEventSendMessage(GetWidID(), EV_LIST_RESORT, UMAKEINT(kTagBranchList, m_sortColumn), (void *)ItemListSorter);
	UEventSendMessage(GetWidID(), EV_LIST_SETSORTDIR, UMAKEINT(kTagBranchList, m_sortColumn), (void *)m_ascendant);
}

#endif /* qUnix */

/*!
	Get Tag item list
	\param mf Files selection
	\param tagName Return tag name
	\param itemSelectionData Item selection data
	\return true on success, false otherwise
*/
bool CompatGetTagListItem(const MultiFiles* mf, 
						  std::string& tagName, 
						  const ItemSelectionData* itemSelectionData /*= NULL*/)
{
	bool userHitOK = false;

	static string sLastTag;
	static ItemList sTagList;
	static bool lastForceCvsroot = false;
	static string sLastCvsroot;
	static string sLastRev;
	static string sLastDate;

	bool forceCvsroot = !itemSelectionData ? lastForceCvsroot : itemSelectionData->GetForceCvsroot();
	string cvsroot = !itemSelectionData ? sLastCvsroot : itemSelectionData->GetCvsroot();

#ifndef qMacCvsPP
#ifdef WIN32
	CItemListDlg itemListDlg(mf, 
		sLastTag.c_str(), kItemListTag, sTagList, NULL, 
		forceCvsroot, cvsroot.c_str(), 
		sLastRev.c_str(), sLastDate.c_str());

	if( itemListDlg.DoModal() == IDOK )
	{
		sLastTag = itemListDlg.m_item;
		sLastRev = itemListDlg.m_rev;
		sLastDate = itemListDlg.m_date;
		lastForceCvsroot = itemListDlg.m_forceRoot != 0;

		userHitOK = true;
	}
#endif /* WIN32 */
#if qUnix
	void *wid;
#	if qGTK
	wid = UCreate_SelTagBranchDlg();
#	endif

	UCvsItemListDlg *dlg = new UCvsItemListDlg(mf, 
		sLastTag.c_str(), kItemListTag, sTagList, NULL, 
		lastForceCvsroot, sLastRev.c_str(), sLastDate.c_str());
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	if(dlg->DoModal())
	{
		sLastTag = dlg->m_item;
		sLastRev = dlg->m_rev;
		sLastDate = dlg->m_date;
		lastForceCvsroot = dlg->m_forceCvsroot;

		userHitOK = true;
	}
	delete dlg;

#endif // qUnix

#else
	StDialogHandler	theHandler(dlg_Select, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	
	theDialog->Show();
	theDialog->SetDescriptor(LStr255("Select tag/branch"));
	
	DoDataExchange_Select(theDialog, kItemListTag, sLastTag, lastForceCvsroot, mf, true);
	
	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if(hitMessage == 'ENDO')
			hitMessage = msg_OK;
			
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
		
		if(hitMessage == item_Refresh)
		{
			OnRefresh_Select(theDialog, kItemListTag);
		}
	}
	theDialog->Hide();

	if(hitMessage == msg_OK)
	{
		DoDataExchange_Select(theDialog, kItemListTag, sLastTag, lastForceCvsroot, mf, false);
	
		userHitOK = true;
	}
#endif /* qMacCvsPP */

	if( userHitOK )
	{
		tagName = sLastTag;
	}

	return userHitOK;
}

/*!
	Get Module item list
	\param mf Files selection
	\param moduleName Return module name
	\param itemSelectionData Item selection data
	\return true on success, false otherwise
*/
bool CompatGetModuleListItem(const MultiFiles* mf, 
							 std::string& moduleName, 
							 const ItemSelectionData* itemSelectionData /*= NULL*/)
{
	bool userHitOK = false;

	static string sLastModule;
	static ItemList sLastModuleList;
	static ListPath sLastListPath;
	static bool lastForceCvsroot = false;
	static string sLastCvsroot;
	static string sLastRev;
	static string sLastDate;
	
	bool forceCvsroot = !itemSelectionData ? lastForceCvsroot : itemSelectionData->GetForceCvsroot();
	string cvsroot = !itemSelectionData ? sLastCvsroot : itemSelectionData->GetCvsroot();

#ifdef WIN32
	CItemListDlg itemListDlg(mf, 
		sLastModule.c_str(), kItemListModule, sLastModuleList, &sLastListPath, 
		forceCvsroot, cvsroot.c_str(), 
		sLastRev.c_str(), sLastDate.c_str());

	if( itemListDlg.DoModal() == IDOK )
	{
		sLastModule = itemListDlg.m_item;
		sLastListPath = itemListDlg.m_listPath;
		sLastRev = itemListDlg.m_rev;
		sLastDate = itemListDlg.m_date;
		lastForceCvsroot = itemListDlg.m_forceRoot != 0;
		sLastCvsroot = itemListDlg.m_cvsroot;

		userHitOK = true;
	}
#endif /* WIN32 */

#if qUnix
	void *wid = NULL;
#	if qGTK
	wid = UCreate_SelTagBranchDlg();
#	endif

	UCvsItemListDlg *dlg = new UCvsItemListDlg(mf, 
		sLastModule.c_str(), kItemListModule, sLastModuleList, &sLastListPath, 
		lastForceCvsroot, sLastRev.c_str(), sLastDate.c_str());
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	if(dlg->DoModal())
	{
		sLastModule = dlg->m_item;
		sLastListPath = dlg->m_listPath;
		sLastRev = dlg->m_rev;
		sLastDate = dlg->m_date;
		lastForceCvsroot = dlg->m_forceCvsroot;

		userHitOK = true;
	}
	delete dlg;

#endif // qUnix

#ifdef qMacCvsPP
	StDialogHandler	theHandler(dlg_Select, LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	
	theDialog->Show();
	theDialog->SetDescriptor(LStr255("Select module"));

	DoDataExchange_Select(theDialog, kItemListModule, sLastModule, lastForceCvsroot, mf, true);

	MessageT hitMessage;
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if(hitMessage == 'ENDO')
			hitMessage = msg_OK;
			
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
		
		if(hitMessage == item_Refresh)
		{
			OnRefresh_Select(theDialog, kItemListModule);
		}
	}
	theDialog->Hide();
	
	if(hitMessage == msg_OK)
	{
		DoDataExchange_Select(theDialog, kItemListModule, sLastModule, lastForceCvsroot, mf, false);
		
		userHitOK = true;
	}
#endif /* qMacCvsPP */

	if( userHitOK )
	{
		moduleName = sLastModule;
	}

	return userHitOK;
}
