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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- March 1998
 */

/*
 * DiffDlg.cpp : the cvs diff dialog
 */

#include "stdafx.h"

#ifdef WIN32
#	include "wincvs.h"
#	include "OptionalPropertySheet.h"
#endif /* WIN32 */

#if qUnix
#	include "UCvsDialogs.h"
#endif

#ifdef qMacCvsPP
#	include <UModalDialogs.h>
#	include <LRadioButton.h>
#	include <LEditText.h>
#	include <LCheckBox.h>
#	include <LMultiPanelView.h>
#	include <LPopupGroupBox.h>
#	include "LPopupFiller.h"

#	include "MacCvsConstant.h"
# include "UCommandDialogs.h"
#endif /* qMacCvsPP */

#define _DIFF_CPP_

#include "DiffDlg.h"
#include "CvsPrefs.h"
#include "MultiFiles.h"
#include "UpdateDlg.h"
#include "ItemListDlg.h"

using namespace std;

extern CMString gDateOrRevNames;

CPersistentBool gUseExtDiff("P_DiffUseExtDiff", false);
CPersistentBool gBulkUpdate("P_DiffBulkUpdate", false);
static CPersistentBool gIgnoreCase("P_IgnoreCase", false);
static CPersistentBool gNoWhiteSpace("P_NoWhiteSpace", false);
static CPersistentBool gNoBlankLines("P_NoBlankLines", false);
static CPersistentBool gUnifiedDiff("P_UnifiedDiff", false);

CMString gDiffOptions(20, "P_DiffOptions");

#define STR_DIFFDEFAULT_REV		"HEAD"
#define STR_DIFFDEFAULT_DATE	"yesterday"

// Option description format
#define STR_DIFFOPT_OPTIONDESCRIPTION_FORMAT	"[%s]: %s"

// Diff options and descriptions strings
#define STR_DIFFOPT_OPT_A		"-a"
#define STR_DIFFOPT_DESC_A			"Treat all files as text and compare them line-by-line, even if they do not seem to be text"
#define STR_DIFFOPT_OPT_B		"-b"
#define STR_DIFFOPT_DESC_B			"Ignore trailing white space and consider all other sequences of one or more white space characters to be equivalent"
#define STR_DIFFOPT_OPT_BB		"-B"
#define STR_DIFFOPT_DESC_BB			"Ignore changes that just insert or delete blank lines"
#define STR_DIFFOPT_OPT_BINARY	"--binary"
#define STR_DIFFOPT_DESC_BINARY		"Read and write data in binary mode"
#define STR_DIFFOPT_OPT_BRIEF	"--brief"
#define STR_DIFFOPT_DESC_BRIEF		"Report only whether the files differ, not the details of the differences"
#define STR_DIFFOPT_OPT_C		"-c"
#define STR_DIFFOPT_DESC_C			"Use the context output format"
#define STR_DIFFOPT_OPT_CC		"-C lines"
#define STR_DIFFOPT_DESC_CC			"Use the context output format, showing lines (an integer) lines of context, or three if lines is not given. For proper operation, patch typically needs at least two lines of context"
#define STR_DIFFOPT_OPT_C_G_F	"--changed-group-format=format"
#define STR_DIFFOPT_DESC_C_G_F		"Use format to output a line group containing differing lines from both files in if-then-else format. See CVS manual for line group formats"
#define STR_DIFFOPT_OPT_D		"-d"
#define STR_DIFFOPT_DESC_D			"Change the algorithm to perhaps find a smaller set of changes. This makes diff slower (sometimes much slower)"
#define STR_DIFFOPT_OPT_E		"-e"
#define STR_DIFFOPT_DESC_E			"Make output that is a valid ed script"
#define STR_DIFFOPT_OPT_E_T		"--expand-tabs"
#define STR_DIFFOPT_DESC_E_T		"Expand tabs to spaces in the output, to preserve the alignment of tabs in the input files"
#define STR_DIFFOPT_OPT_F		"-f"
#define STR_DIFFOPT_DESC_F			"Make output that looks vaguely like an ed script but has changes in the order they appear in the file"
#define STR_DIFFOPT_OPT_FF		"-F regexp"
#define STR_DIFFOPT_DESC_FF			"In context and unified format, for each hunk of differences, show some of the last preceding line that matches regexp"
#define STR_DIFFOPT_OPT_F_E		"--forward-ed"
#define STR_DIFFOPT_DESC_F_E		"Make output that looks vaguely like an ed script but has changes in the order they appear in the file"
#define STR_DIFFOPT_OPT_HH		"-H"
#define STR_DIFFOPT_DESC_HH			"Use heuristics to speed handling of large files that have numerous scattered small changes"
#define STR_DIFFOPT_OPT_H_L		"--horizon-lines=lines"
#define STR_DIFFOPT_DESC_H_L		"Do not discard the last lines lines of the common prefix and the first lines lines of the common suffix"
#define STR_DIFFOPT_OPT_I		"-i"
#define STR_DIFFOPT_DESC_I			"Ignore changes in case; consider upper- and lower-case letters equivalent"
#define STR_DIFFOPT_OPT_II		"-I regexp"
#define STR_DIFFOPT_DESC_II			"Ignore changes that just insert or delete lines that match regexp"
#define STR_DIFFOPT_OPT_IFDEF	"--ifdef=name"
#define STR_DIFFOPT_DESC_IFDEF		"Make merged if-then-else output using name"
#define STR_DIFFOPT_OPT_I_A_S	"--ignore-all-space"
#define STR_DIFFOPT_DESC_I_A_S		"Ignore white space when comparing lines"
#define STR_DIFFOPT_OPT_I_B_L	"--ignore-blank-lines"
#define STR_DIFFOPT_DESC_I_B_L		"Ignore changes that just insert or delete blank lines"
#define STR_DIFFOPT_OPT_I_C		"--ignore-case"
#define STR_DIFFOPT_DESC_I_C		"Ignore changes in case; consider upper- and lower-case to be the same"
#define STR_DIFFOPT_OPT_I_M_L	"--ignore-matching-lines=regexp"
#define STR_DIFFOPT_DESC_I_M_L		"Ignore changes that just insert or delete lines that match regexp"
#define STR_DIFFOPT_OPT_I_S_C	"--ignore-space-change"
#define STR_DIFFOPT_DESC_I_S_C		"Ignore trailing white space and consider all other sequences of one or more white space characters to be equivalent"
#define STR_DIFFOPT_OPT_I_T		"--initial-tab"
#define STR_DIFFOPT_DESC_I_T		"Output a tab rather than a space before the text of a line in normal or context format. This causes the alignment of tabs in the line to look normal"
#define STR_DIFFOPT_OPT_LL		"-L label"
#define STR_DIFFOPT_DESC_LL			"Use label instead of the file name in the context format and unified format headers"
#define STR_DIFFOPT_OPT_LABEL	"--label=label"
#define STR_DIFFOPT_DESC_LABEL		"Use label instead of the file name in the context format and unified format headers"
#define STR_DIFFOPT_OPT_L_C		"--left-column"
#define STR_DIFFOPT_DESC_L_C		"Print only the left column of two common lines in side by side format"
#define STR_DIFFOPT_OPT_L_F		"--line-format=format"
#define STR_DIFFOPT_DESC_L_F		"Use format to output all input lines in if-then-else format. See CVS manual for line formats"
#define STR_DIFFOPT_OPT_MINIMAL	"--minimal"
#define STR_DIFFOPT_DESC_MINIMAL	"Change the algorithm to perhaps find a smaller set of changes. This makes diff slower (sometimes much slower)"
#define STR_DIFFOPT_OPT_N		"-n"
#define STR_DIFFOPT_DESC_N			"Output RCS-format diffs; like `-f' except that each command specifies the number of lines affected"
#define STR_DIFFOPT_OPT_NN		"-N"
#define STR_DIFFOPT_DESC_NN			"In directory comparison, if a file is found in only one directory, treat it as present but empty in the other directory"
#define STR_DIFFOPT_OPT_N_G_F	"--new-group-format=format"
#define STR_DIFFOPT_DESC_N_G_F		"Use format to output a group of lines taken from just the second file in if-then-else format. See section A.9.1.1 Line group formats"
#define STR_DIFFOPT_OPT_N_L_F	"--new-line-format=format"
#define STR_DIFFOPT_DESC_N_L_F		"Use format to output a line taken from just the second file in if-then-else format. See CVS manual for line formats"
#define STR_DIFFOPT_OPT_O_G_F	"--old-group-format=format"
#define STR_DIFFOPT_DESC_O_G_F		"Use format to output a group of lines taken from just the first file in if-then-else format. See section A.9.1.1 Line group formats"
#define STR_DIFFOPT_OPT_O_L_F	"--old-line-format=format"
#define STR_DIFFOPT_DESC_O_L_F		"Use format to output a line taken from just the first file in if-then-else format. See CVS manual for line formats"
#define STR_DIFFOPT_OPT_P		"-p"
#define STR_DIFFOPT_DESC_PP			"Show which C function each change is in"
#define STR_DIFFOPT_OPT_RCS		"--rcs"
#define STR_DIFFOPT_DESC_RCS		"Output RCS-format diffs; like `-f' except that each command specifies the number of lines affected"
#define STR_DIFFOPT_OPT_S		"-s"
#define STR_DIFFOPT_DESC_S			"Report when two files are the same"
#define STR_DIFFOPT_OPT_S_C_F	"--show-c-function"
#define STR_DIFFOPT_DESC_S_C_F		"Show which C function each change is in"
#define STR_DIFFOPT_OPT_S_F_L	"--show-function-line=regexp"
#define STR_DIFFOPT_DESC_S_F_L		"In context and unified format, for each hunk of differences, show some of the last preceding line that matches regexp"
#define STR_DIFFOPT_OPT_S_B_S	"--side-by-side"
#define STR_DIFFOPT_DESC_S_B_S		"Use the side by side output format"
#define STR_DIFFOPT_OPT_S_L_F	"--speed-large-files"
#define STR_DIFFOPT_DESC_S_L_F		"Use heuristics to speed handling of large files that have numerous scattered small changes"
#define STR_DIFFOPT_OPT_S_C_L	"--suppress-common-lines"
#define STR_DIFFOPT_DESC_S_C_L		"Do not print common lines in side by side format"
#define STR_DIFFOPT_OPT_T		"-t"
#define STR_DIFFOPT_DESC_T			"Expand tabs to spaces in the output, to preserve the alignment of tabs in the input files"
#define STR_DIFFOPT_OPT_TT		"-T"
#define STR_DIFFOPT_DESC_TT			"Output a tab rather than a space before the text of a line in normal or context format. This causes the alignment of tabs in the line to look normal"
#define STR_DIFFOPT_OPT_TEXT	"--text"
#define STR_DIFFOPT_DESC_TEXT		"Treat all files as text and compare them line-by-line, even if they do not appear to be text"
#define STR_DIFFOPT_OPT_U		"-u"
#define STR_DIFFOPT_DESC_U			"Use the unified output format"
#define STR_DIFFOPT_OPT_U_G_F	"--unchanged-group-format=format"
#define STR_DIFFOPT_DESC_U_G_F		"Use format to output a group of common lines taken from both files in if-then-else format. See section A.9.1.1 Line group formats"
#define STR_DIFFOPT_OPT_U_L_F	"--unchanged-line-format=format"
#define STR_DIFFOPT_DESC_U_L_F		"Use format to output a line common to both files in if-then-else format. See CVS manual for line formats"
#define STR_DIFFOPT_OPT_UU		"-U lines"
#define STR_DIFFOPT_DESC_UU			"Use the unified output format, showing lines (an integer) lines of context, or three if lines is not given. For proper operation, patch typically needs at least two lines of context"
#define STR_DIFFOPT_OPT_W		"-w"
#define STR_DIFFOPT_DESC_W			"Ignore white space when comparing lines"
#define STR_DIFFOPT_OPT_WW		"-W columns"
#define STR_DIFFOPT_DESC_WW			"Use an output width of columns in side by side format"
#define STR_DIFFOPT_OPT_Y		"-y"
#define STR_DIFFOPT_DESC_Y			"Use the side by side output format"

/// Define the diff options list items
typedef struct _sDiffOptionsItem{
	const char* option;			//!< Option
	const char* description;	//!< Option description
}DiffOptionsItem;

/// Command line diff options and their descriptions
static DiffOptionsItem sDiffOptions[] = 
{
	{ STR_DIFFOPT_OPT_A,		STR_DIFFOPT_DESC_A			},
	{ STR_DIFFOPT_OPT_B,		STR_DIFFOPT_DESC_B			},
	{ STR_DIFFOPT_OPT_BB,		STR_DIFFOPT_DESC_BB			},
	{ STR_DIFFOPT_OPT_BINARY,	STR_DIFFOPT_DESC_BINARY		},
	{ STR_DIFFOPT_OPT_BRIEF,	STR_DIFFOPT_DESC_BRIEF		},
	{ STR_DIFFOPT_OPT_C,		STR_DIFFOPT_DESC_C			},
	{ STR_DIFFOPT_OPT_CC,		STR_DIFFOPT_DESC_CC			},
	{ STR_DIFFOPT_OPT_C_G_F,	STR_DIFFOPT_DESC_C_G_F		},
	{ STR_DIFFOPT_OPT_D,		STR_DIFFOPT_DESC_D			},
	{ STR_DIFFOPT_OPT_E,		STR_DIFFOPT_DESC_E			},
	{ STR_DIFFOPT_OPT_E_T,		STR_DIFFOPT_DESC_E_T		},
	{ STR_DIFFOPT_OPT_F,		STR_DIFFOPT_DESC_F			},
	{ STR_DIFFOPT_OPT_FF,		STR_DIFFOPT_DESC_FF			},
	{ STR_DIFFOPT_OPT_F_E,		STR_DIFFOPT_DESC_F_E		},
	{ STR_DIFFOPT_OPT_HH,		STR_DIFFOPT_DESC_HH			},
	{ STR_DIFFOPT_OPT_H_L,		STR_DIFFOPT_DESC_H_L		},
	{ STR_DIFFOPT_OPT_I,		STR_DIFFOPT_DESC_I			},
	{ STR_DIFFOPT_OPT_II,		STR_DIFFOPT_DESC_II			},
	{ STR_DIFFOPT_OPT_IFDEF,	STR_DIFFOPT_DESC_IFDEF		},
	{ STR_DIFFOPT_OPT_I_A_S,	STR_DIFFOPT_DESC_I_A_S		},
	{ STR_DIFFOPT_OPT_I_B_L,	STR_DIFFOPT_DESC_I_B_L		},
	{ STR_DIFFOPT_OPT_I_C,		STR_DIFFOPT_DESC_I_C		},
	{ STR_DIFFOPT_OPT_I_M_L,	STR_DIFFOPT_DESC_I_M_L		},
	{ STR_DIFFOPT_OPT_I_S_C,	STR_DIFFOPT_DESC_I_S_C		},
	{ STR_DIFFOPT_OPT_I_T,		STR_DIFFOPT_DESC_I_T		},
	{ STR_DIFFOPT_OPT_LL,		STR_DIFFOPT_DESC_LL			},
	{ STR_DIFFOPT_OPT_LABEL,	STR_DIFFOPT_DESC_LABEL		},
	{ STR_DIFFOPT_OPT_L_C,		STR_DIFFOPT_DESC_L_C		},
	{ STR_DIFFOPT_OPT_L_F,		STR_DIFFOPT_DESC_L_F		},
	{ STR_DIFFOPT_OPT_MINIMAL,	STR_DIFFOPT_DESC_MINIMAL	},
	{ STR_DIFFOPT_OPT_N,		STR_DIFFOPT_DESC_N			},
	{ STR_DIFFOPT_OPT_NN,		STR_DIFFOPT_DESC_NN			},
	{ STR_DIFFOPT_OPT_N_G_F,	STR_DIFFOPT_DESC_N_G_F		},
	{ STR_DIFFOPT_OPT_N_L_F,	STR_DIFFOPT_DESC_N_L_F		},
	{ STR_DIFFOPT_OPT_O_G_F,	STR_DIFFOPT_DESC_O_G_F		},
	{ STR_DIFFOPT_OPT_O_L_F,	STR_DIFFOPT_DESC_O_L_F		},
	{ STR_DIFFOPT_OPT_P,		STR_DIFFOPT_DESC_PP			},
	{ STR_DIFFOPT_OPT_RCS,		STR_DIFFOPT_DESC_RCS		},
	{ STR_DIFFOPT_OPT_S,		STR_DIFFOPT_DESC_S			},
	{ STR_DIFFOPT_OPT_S_C_F,	STR_DIFFOPT_DESC_S_C_F		},
	{ STR_DIFFOPT_OPT_S_F_L,	STR_DIFFOPT_DESC_S_F_L		},
	{ STR_DIFFOPT_OPT_S_B_S,	STR_DIFFOPT_DESC_S_B_S		},
	{ STR_DIFFOPT_OPT_S_L_F,	STR_DIFFOPT_DESC_S_L_F		},
	{ STR_DIFFOPT_OPT_S_C_L,	STR_DIFFOPT_DESC_S_C_L		},
	{ STR_DIFFOPT_OPT_T,		STR_DIFFOPT_DESC_T			},
	{ STR_DIFFOPT_OPT_TT,		STR_DIFFOPT_DESC_TT			},
	{ STR_DIFFOPT_OPT_TEXT,		STR_DIFFOPT_DESC_TEXT		},
	{ STR_DIFFOPT_OPT_U,		STR_DIFFOPT_DESC_U			},
	{ STR_DIFFOPT_OPT_U_G_F,	STR_DIFFOPT_DESC_U_G_F		},
	{ STR_DIFFOPT_OPT_U_L_F,	STR_DIFFOPT_DESC_U_L_F		},
	{ STR_DIFFOPT_OPT_UU,		STR_DIFFOPT_DESC_UU			},
	{ STR_DIFFOPT_OPT_W,		STR_DIFFOPT_DESC_W			},
	{ STR_DIFFOPT_OPT_WW,		STR_DIFFOPT_DESC_WW			},
	{ STR_DIFFOPT_OPT_Y,		STR_DIFFOPT_DESC_Y			},
};

//{ Win32 - Part
#ifdef WIN32
#include "GetPrefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDiff_MAIN, CHHelpPropertyPage)

CDiff_MAIN::CDiff_MAIN(const MultiFiles* mf, kDiffCmdType diffType, 
					   const char* rev1, const char* rev2, 
					   bool allowExtDiff, bool useExtDiff, bool bulkUpdate,
					   bool unifiedDiff, bool noWhiteSpace, bool ignoreCase,
					   const char* keyword, const char* diffOptions) 
					   : m_mf(mf), m_diffType(diffType), m_compareTypeCombo(USmartCombo::AutoDropWidth), m_allowExtDiff(allowExtDiff), 
					   CHHelpPropertyPage(CDiff_MAIN::IDD)
{
	//{{AFX_DATA_INIT(CDiff_MAIN)
	m_isDate1 = FALSE;
	m_isDate2 = FALSE;
	m_rev1 = rev1;
	m_rev2 = rev2;
	m_noWhiteSpace = noWhiteSpace;
	m_ignoreCase = ignoreCase;
	m_compareType = kCompareSameRev;
	m_unifiedDiff = unifiedDiff;
	m_noRecurs = FALSE;
	m_extDiff = useExtDiff;
	m_bulkUpdate = bulkUpdate;
	m_diffOptions = diffOptions;
	//}}AFX_DATA_INIT

	m_keyword = keyword;

	if( kDiffGraph != m_diffType )
	{
		m_rev1Combo.SetItems(m_isDate1 ? &gDateNames : &gRevNames);
		m_rev2Combo.SetItems(m_isDate2 ? &gDateNames : &gRevNames);
	}
	else
	{
		m_compareType = m_mf->TotalNumFiles() == 1 ? kCompareAnotherRev : kCompareTwoRev;

		m_rev1Combo.SetFeature(USmartCombo::AutoDropWidth | USmartCombo::ReadOnly);
		m_rev2Combo.SetFeature(USmartCombo::AutoDropWidth | USmartCombo::ReadOnly);

		if( m_mf && m_mf->TotalNumFiles() )
		{
			MultiFiles::const_iterator mfi;
			for(mfi = m_mf->begin(); mfi != m_mf->end(); ++mfi)
			{
				for( int nIndex = 0; nIndex < mfi->NumFiles(); nIndex++ )
				{
					string path;
					string fileName;
					string currRev;
					
					mfi->get(nIndex, path, fileName, currRev);

					switch( nIndex )
					{
					case 0:
						m_rev1 = currRev.c_str();
						break;
					case 1:
						m_rev2 = currRev.c_str();
						break;
					default:
						ASSERT(FALSE);	// there should be no more than 2 files in the selection
						break;
					}
				}
			}
		}
	}

	m_diffOptionsCombo.SetItems(&gDiffOptions);
}

CDiff_MAIN::~CDiff_MAIN()
{
}

void CDiff_MAIN::DoDataExchange(CDataExchange* pDX)
{
	CHHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiff_MAIN)
	DDX_Control(pDX, IDC_SUBSTMODE_ENTRY_STATIC, m_substModeEntryPlaceholder);
	DDX_Control(pDX, IDC_DIFFOPTIONS, m_diffOptionsCombo);
	DDX_Control(pDX, IDC_IGNOREWHITESPACE, m_noWhiteSpaceCheck);
	DDX_Control(pDX, IDC_IGNORECASE, m_ignoreCaseCheck);
	DDX_Control(pDX, IDC_USE_EXTERNAL_DIFF, m_extDiffButton);
	DDX_Control(pDX, IDC_BULK_UPDATE, m_bulkUpdateButton);
	DDX_Control(pDX, IDC_BROWSE_DIFFOPTIONS, m_optionsMenuButton);
	DDX_Control(pDX, IDC_NORECURS, m_noRecursCheck);
	DDX_Control(pDX, IDC_UNIFIEDDIFF, m_unifiedDiffCheck);
	DDX_Control(pDX, IDC_BROWSE_TAG2, m_browseTag2Button);
	DDX_Control(pDX, IDC_BROWSE_TAG1, m_browseTag1Button);
	DDX_Control(pDX, IDC_ISDATE2, m_isDate2Check);
	DDX_Control(pDX, IDC_ISDATE1, m_isDate1Check);
	DDX_Control(pDX, IDC_COMPARE_TYPE, m_compareTypeCombo);
	DDX_Control(pDX, IDC_REV_TAG2, m_rev2Combo);
	DDX_Control(pDX, IDC_REV_TAG1, m_rev1Combo);
	DDX_Check(pDX, IDC_ISDATE1, m_isDate1);
	DDX_Check(pDX, IDC_ISDATE2, m_isDate2);
	DDX_CBString(pDX, IDC_REV_TAG1, m_rev1);
	DDX_CBString(pDX, IDC_REV_TAG2, m_rev2);
	DDX_Check(pDX, IDC_IGNOREWHITESPACE, m_noWhiteSpace);
	DDX_Check(pDX, IDC_IGNORECASE, m_ignoreCase);
	DDX_CBIndex(pDX, IDC_COMPARE_TYPE, m_compareType);
	DDX_Check(pDX, IDC_UNIFIEDDIFF, m_unifiedDiff);
	DDX_Check(pDX, IDC_NORECURS, m_noRecurs);
	DDX_Check(pDX, IDC_USE_EXTERNAL_DIFF, m_extDiff);
	DDX_Check(pDX, IDC_BULK_UPDATE, m_bulkUpdate);
	DDX_CBString(pDX, IDC_DIFFOPTIONS, m_diffOptions);
	//}}AFX_DATA_MAP

	if( kDiffGraph != m_diffType )
	{
		if( kCompareAnotherRev == m_compareType || kCompareTwoRev == m_compareType )
		{
			DDV_MinChars(pDX, m_rev1, 1);
		}
		
		if( kCompareTwoRev == m_compareType )
		{
			DDV_MinChars(pDX, m_rev2, 1);
		}
		
		if( !pDX->m_bSaveAndValidate )
		{
			EnableCompareType(m_compareType);
		}
		
		DDX_ComboMString(pDX, IDC_REV_TAG1, m_rev1Combo);
		DDX_ComboMString(pDX, IDC_REV_TAG2, m_rev2Combo);
		
		DDX_ComboMString(pDX, IDC_DIFFOPTIONS, m_diffOptionsCombo);
	}

	m_substModeEntryDlg.UpdateData(pDX->m_bSaveAndValidate, m_keyword);

	m_extDiffButton.EnableWindow(m_allowExtDiff ? true : false);

	OnExtdiff();
}

BEGIN_MESSAGE_MAP(CDiff_MAIN, CHHelpPropertyPage)
	//{{AFX_MSG_MAP(CDiff_MAIN)
	ON_BN_CLICKED(IDC_USE_EXTERNAL_DIFF, OnExtdiff)
	ON_BN_CLICKED(IDC_ISDATE1, OnCheck1)
	ON_BN_CLICKED(IDC_ISDATE2, OnCheck2)
	ON_BN_CLICKED(IDC_BROWSE_TAG1, OnBrowseTag1)
	ON_BN_CLICKED(IDC_BROWSE_TAG2, OnBrowseTag2)
	ON_CBN_SELCHANGE(IDC_COMPARE_TYPE, OnSelchangeCompareType)
	ON_BN_CLICKED(IDC_BROWSE_DIFFOPTIONS, OnOptionsmenu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiff_MAIN message handlers

/// BN_CLICKED message handler, enable controls
void CDiff_MAIN::OnExtdiff() 
{
	EnableCmdLineDiffOptions(!m_extDiffButton.GetCheck() || !m_extDiffButton.IsWindowEnabled());
}

/*!
	Enable controls for command line option
	\param enable true to enable controls, false to disable
*/
void CDiff_MAIN::EnableCmdLineDiffOptions(bool enable)
{
	m_bulkUpdateButton.EnableWindow(!enable);
	m_unifiedDiffCheck.EnableWindow(enable);
	m_ignoreCaseCheck.EnableWindow(enable);
	m_noWhiteSpaceCheck.EnableWindow(enable);
	m_diffOptionsCombo.EnableWindow(enable);
	m_optionsMenuButton.EnableWindow(enable);

	if( ::IsWindow(m_substModeEntryDlg.GetSafeHwnd()) )
	{
		m_substModeEntryDlg.EnableControls(enable);
	}
}

/// BN_CLICKED message handler, enable controls and set default input
void CDiff_MAIN::OnCheck1() 
{
	BOOL isDate = m_isDate1Check.GetCheck();

	m_rev1Combo.SetItems(isDate ? &gDateNames : &gRevNames);
	DDX_ComboMString(NULL, IDC_REV_TAG1, m_rev1Combo);

	m_browseTag1Button.EnableWindow(!isDate);

	SetDefaultInput(kDefRev1Change, m_compareTypeCombo.GetCurSel());
}

/// BN_CLICKED message handler, enable controls and set default input
void CDiff_MAIN::OnCheck2() 
{
	BOOL isDate = m_isDate2Check.GetCheck();

	m_rev2Combo.SetItems(isDate ? &gDateNames : &gRevNames);
	DDX_ComboMString(NULL, IDC_REV_TAG2, m_rev2Combo);

	m_browseTag2Button.EnableWindow(!isDate);

	SetDefaultInput(kDefRev2Change, m_compareTypeCombo.GetCurSel());
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CDiff_MAIN::OnBrowseTag1() 
{
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName) )
	{
		m_rev1 = tagName.c_str();
		m_rev1Combo.SetWindowText(m_rev1);
	}
}

/// BN_CLICKED message handler, display browse tag/branch dialog
void CDiff_MAIN::OnBrowseTag2() 
{
	string tagName;
	if( CompatGetTagListItem(m_mf, tagName) )
	{
		m_rev2 = tagName.c_str();
		m_rev2Combo.SetWindowText(m_rev2);
	}
}

/// CBN_SELCHANGE message handler, enable controls and set default input
void CDiff_MAIN::OnSelchangeCompareType() 
{
	const int compareType = m_compareTypeCombo.GetCurSel();
	
	EnableCompareType(compareType);
	SetDefaultInput(kDefTypeChange, compareType);
}

/*!
	Enable controls for given compare type
	\param compareType Compare type
*/
void CDiff_MAIN::EnableCompareType(const int compareType)
{
	bool hasRev1 = false;
	bool hasRev2 = false;

	switch( compareType )
	{
	default:
		ASSERT(FALSE);	//unknown compare type
	case kCompareSameRev:
		hasRev1 = hasRev2 = false;
		break;
	case kCompareAnotherRev:
		hasRev1 = true;
		hasRev2 = false;
		break;
	case kCompareTwoRev:
		hasRev1 = true;
		hasRev2 = true;
		break;
	}

	m_rev1Combo.EnableWindow(hasRev1);
	m_isDate1Check.EnableWindow(hasRev1);
	m_browseTag1Button.EnableWindow(hasRev1);

	m_rev2Combo.EnableWindow(hasRev2);
	m_isDate2Check.EnableWindow(hasRev2);
	m_browseTag2Button.EnableWindow(hasRev2);
}

/*!
	Set the default input for given compare type
	\param inputChange Input change
	\param compareType Compare type
*/
void CDiff_MAIN::SetDefaultInput(const kInputChange inputChange, const int compareType)
{
	CString strRev1;
	CString strRev2;

	m_rev1Combo.GetWindowText(strRev1);
	m_rev2Combo.GetWindowText(strRev2);

	switch( inputChange )
	{
	case kDefRev1Change:
		if( strRev1.IsEmpty() || IsDefaultedRev(&m_rev1Combo) )
		{
			m_rev1Combo.SetWindowText(m_isDate1Check.GetCheck() ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV);
		}
		break;
	case kDefRev2Change:
		if( strRev2.IsEmpty() || IsDefaultedRev(&m_rev2Combo) )
		{
			m_rev2Combo.SetWindowText(m_isDate2Check.GetCheck() ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV);
		}
		break;
	case kDefTypeChange:
		{
			// Set the input
			switch( compareType )
			{
			default:
				ASSERT(FALSE);	//unknown compare type
			case kCompareSameRev:
				break;
			case kCompareTwoRev:
				if( strRev2.IsEmpty() )
				{
					m_rev2Combo.SetWindowText(m_isDate2Check.GetCheck() ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV);
				}
				
				// Fall thru
				
			case kCompareAnotherRev:
				if( strRev1.IsEmpty() )
				{
					m_rev1Combo.SetWindowText(m_isDate1Check.GetCheck() ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV);
				}
				break;
			}
			
			// Reset unneeded input
			if( !m_rev1Combo.IsWindowEnabled() )
			{
				if( IsDefaultedRev(&m_rev1Combo) )
				{
					m_rev1Combo.SetWindowText("");
				}
			}
			
			if( !m_rev2Combo.IsWindowEnabled() )
			{
				if( IsDefaultedRev(&m_rev2Combo) )
				{
					m_rev2Combo.SetWindowText("");
				}
			}
		}

		break;

	default:
		ASSERT(FALSE); // unknown input change
		break;
	}
}

/*!
	Check whether the combo box text is a defaulted one
	\param comboBox Combo box to check
	\return true if the combo box text is a defaulted one, false otherwise
*/
bool CDiff_MAIN::IsDefaultedRev(const CComboBox* comboBox)
{
	bool res = false;

	if( comboBox )
	{
		CString strRev;
		comboBox->GetWindowText(strRev);
		
		if( (STR_DIFFDEFAULT_REV == strRev || STR_DIFFDEFAULT_DATE == strRev) && 
			comboBox->FindStringExact(-1, strRev) == CB_ERR )
		{
			res = true;
		}
	}
	else
	{
		ASSERT(FALSE);	// Bad pointer
	}

	return res;
}

/// OnInitDialog virtual override, initialize the controls
BOOL CDiff_MAIN::OnInitDialog() 
{
	CHHelpPropertyPage::OnInitDialog();
	
	// Extra initialization
	m_substModeEntryDlg.Create(&m_substModeEntryPlaceholder, this);

	// Special setup for the diff invoked using the graph view
	if( kDiffGraph == m_diffType )
	{
		m_compareTypeCombo.SetCurSel(m_mf->TotalNumFiles() == 1 ? kCompareAnotherRev : kCompareTwoRev);
		m_compareTypeCombo.EnableWindow(FALSE);

		m_browseTag1Button.EnableWindow(FALSE);
		m_browseTag2Button.EnableWindow(FALSE);

		m_isDate1Check.EnableWindow(FALSE);
		m_isDate2Check.EnableWindow(FALSE);

		m_rev1Combo.SetReadOnly(TRUE);
		m_rev1Combo.ResetContent();
		
		if( !m_rev1.IsEmpty() )
		{
			m_rev1Combo.AddString(m_rev1);
			m_rev1Combo.SetWindowText(m_rev1);
		}

		if( m_mf && m_mf->TotalNumFiles() == 2 )
		{
			m_rev2Combo.SetReadOnly(TRUE);
			m_rev2Combo.ResetContent();
			
			if( !m_rev2.IsEmpty() )
			{
				m_rev2Combo.AddString(m_rev2);
				m_rev2Combo.SetWindowText(m_rev2);
			}
		}
		else
		{
			m_rev2Combo.EnableWindow(FALSE);
		}
	}
	
	m_noRecursCheck.EnableWindow(m_mf->TotalNumFiles() == 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDiff_MAIN::InitHelpID()
{
	SetCtrlHelpID(IDC_COMPARE_TYPE,			IDH_IDC_COMPARE_TYPE);
	SetCtrlHelpID(IDC_REV_TAG1,				IDH_IDC_REV_TAG_DATE);
	SetCtrlHelpID(IDC_BROWSE_TAG1,			IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_ISDATE1,				IDH_IDC_ISDATE);
	SetCtrlHelpID(IDC_REV_TAG2,				IDH_IDC_REV_TAG_DATE);
	SetCtrlHelpID(IDC_BROWSE_TAG2,			IDH_IDC_BROWSE_TAG);
	SetCtrlHelpID(IDC_ISDATE2,				IDH_IDC_ISDATE);
	SetCtrlHelpID(IDC_USE_EXTERNAL_DIFF,	IDH_IDC_USE_EXTERNAL_DIFF);
	SetCtrlHelpID(IDC_BULK_UPDATE,			IDH_IDC_BULK_UPDATE);
	SetCtrlHelpID(IDC_UNIFIEDDIFF,			IDH_IDC_UNIFIEDDIFF);
	SetCtrlHelpID(IDC_IGNORECASE,			IDH_IDC_IGNORECASE);
	SetCtrlHelpID(IDC_IGNOREWHITESPACE,		IDH_IDC_IGNOREWHITESPACE);
	SetCtrlHelpID(IDC_NORECURS,				IDH_IDC_NORECURS);
	SetCtrlHelpID(IDC_DIFFOPTIONS,			IDH_IDC_DIFFOPTIONS);
	SetCtrlHelpID(IDC_BROWSE_DIFFOPTIONS,	IDH_IDC_BROWSE_DIFFOPTIONS);
}

/// BN_CLICKED message handler, display the diff options dialog
void CDiff_MAIN::OnOptionsmenu() 
{
	CDiff_Options diffOptionsDlg;

	CString currentOptions;
	m_diffOptionsCombo.GetWindowText(currentOptions);
	diffOptionsDlg.m_currentOptions = currentOptions;

	if( diffOptionsDlg.DoModal() == IDOK )
	{
		CString diffOptions = diffOptionsDlg.GetDiffOptions();
		if( !diffOptions.IsEmpty() )
		{
			currentOptions.TrimRight();
			currentOptions += currentOptions.IsEmpty() ? "" : " ";
			m_diffOptionsCombo.SetWindowText(currentOptions + diffOptions);
			GotoDlgCtrl(&m_diffOptionsCombo);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDiff_Options dialog

CDiff_Options::CDiff_Options(CWnd* pParent /*=NULL*/)
	: CHHelpDialog(CDiff_Options::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiff_Options)
	m_currentOptions = _T("");
	//}}AFX_DATA_INIT

	m_sortAsc = false;
	m_sortColumn = 0;
}


void CDiff_Options::DoDataExchange(CDataExchange* pDX)
{
	CHHelpDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiff_Options)
	DDX_Control(pDX, IDC_DIFF_OPTION_LIST, m_optionsListCtrl);
	DDX_Text(pDX, IDC_CURRENT_DIFF_OPTIONS, m_currentOptions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiff_Options, CHHelpDialog)
	//{{AFX_MSG_MAP(CDiff_Options)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DIFF_OPTION_LIST, OnItemchangedOptionslist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_DIFF_OPTION_LIST, OnColumnclickOptionslist)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiff_Options message handlers

/*!
	Get the diff options
	\return The diff options as CString
*/
CString CDiff_Options::GetDiffOptions()
{
	return m_diffOptions;
}

/// OnOK virtual override, sets the string containing all selected options
void CDiff_Options::OnOK() 
{
	for(int nIndex = 0; nIndex < m_optionsListCtrl.GetItemCount(); nIndex++)
	{
		if( m_optionsListCtrl.GetCheck(nIndex) )
		{
			m_diffOptions += m_optionsListCtrl.GetItemText(nIndex, 0);
			m_diffOptions += " ";
		}
	}

	m_diffOptions.TrimRight();
	
	CHHelpDialog::OnOK();
}

/// LVN_ITEMCHANGED message handler, update the option's description edit box value as selection is changing
void CDiff_Options::OnItemchangedOptionslist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem > -1 && 
		pNMListView->uChanged & LVIF_STATE && pNMListView->uNewState & (LVIS_SELECTED | LVIS_FOCUSED) )
	{
		CString item;
		item.Format(STR_DIFFOPT_OPTIONDESCRIPTION_FORMAT, 
			m_optionsListCtrl.GetItemText(pNMListView->iItem, 0), 
			m_optionsListCtrl.GetItemText(pNMListView->iItem, 1));
		
		SetDlgItemText(IDC_DIFF_OPTION_DESCRIPTION, item);
	}
	
	*pResult = 0;
}

/// LVN_COLUMNCLICK message handler, sort items in response to column click
void CDiff_Options::OnColumnclickOptionslist(NMHDR* pNMHDR, LRESULT* pResult) 
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

	SortItems(m_sortColumn, m_sortAsc);
	
	*pResult = 0;
}

/// OnInitDialog virtual override, initialize diff options list
BOOL CDiff_Options::OnInitDialog() 
{
	CHHelpDialog::OnInitDialog();
	
	// Extra initialization
	
	// Set the control's style
	SetListCtrlExtendedStyle(&m_optionsListCtrl, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_LABELTIP);

	// Setup controls
	CHeaderCtrl* headerCtrl = m_optionsListCtrl.GetHeaderCtrl();
	if( headerCtrl )
	{
		m_headerCtrl.SubclassWindow(headerCtrl->m_hWnd);
	}

	// Get the client area width to properly size columns
	CRect rect;
	m_optionsListCtrl.GetClientRect(rect);

	const int listCtrlWidth = rect.Width();
	const int listCtrlWidthNoScroll = listCtrlWidth - GetSystemMetrics(SM_CXVSCROLL);

	// Insert columns
	m_optionsListCtrl.InsertColumn(0, "Option", LVCFMT_LEFT, 65);
	m_optionsListCtrl.InsertColumn(1, "Description", LVCFMT_LEFT, listCtrlWidthNoScroll - 65);

	// Insert options
	const int itemsCount = (sizeof(sDiffOptions) / sizeof(DiffOptionsItem));
	for(int nIndex = 0; nIndex < itemsCount; nIndex++)
	{
		const int item = m_optionsListCtrl.InsertItem(m_optionsListCtrl.GetItemCount(), sDiffOptions[nIndex].option);
		if( item > -1 )
		{
			m_optionsListCtrl.SetItemText(item, 1, sDiffOptions[nIndex].description);
			m_optionsListCtrl.SetItemData(item, (DWORD)&sDiffOptions[nIndex]);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDiff_Options::InitHelpID()
{
	SetCtrlHelpID(IDC_DIFF_OPTION_LIST,			IDH_IDC_DIFF_OPTION_LIST);
	SetCtrlHelpID(IDC_DIFF_OPTION_DESCRIPTION,	IDH_IDC_DIFF_OPTION_DESCRIPTION);
	SetCtrlHelpID(IDC_CURRENT_DIFF_OPTIONS,		IDH_IDC_CURRENT_DIFF_OPTIONS);
}

/// BN_CLICKED message handler, display dialog help
void CDiff_Options::OnHelp() 
{
	ShowHelp();
}

/*!
	Sort items listed in the list control
	\param column Column to sort items by
	\param asc Sort ascending or descending
	\note Using list control callback CompareItems
*/
void CDiff_Options::SortItems(int column, bool asc)
{
	m_headerCtrl.SetSortImage(column, asc);

	CompareInfo info = { 0 };
	
	info.m_column = column;
	info.m_asc = asc;

	m_optionsListCtrl.SortItems(CompareItems, (DWORD)&info);
}

/*!
	Callback method used for sorting of items in the list control
	\param itemData1 DiffOptionsItem* of the first element
	\param itemData2 DiffOptionsItem* of the second element
	\param lParam CompareInfo* Sort settings
	\return 1, -1 or 0 depending on the sort settings
*/
int CALLBACK CDiff_Options::CompareItems(LPARAM itemData1, LPARAM itemData2, LPARAM lParam)
{
	int res = 0;

	const CompareInfo* info = (CompareInfo*)lParam;
	if( info )
	{
		DiffOptionsItem* diffOptionsItem1 = (DiffOptionsItem*)itemData1;
		DiffOptionsItem* diffOptionsItem2 = (DiffOptionsItem*)itemData2;
		
		switch( info->m_column )
		{
		case 0:
			res = stricmp(diffOptionsItem1->option, diffOptionsItem2->option);
			break;
		case 1:
			res = stricmp(diffOptionsItem1->description, diffOptionsItem2->description);
			break;
		default:
			ASSERT(FALSE); // should not come here
			break;
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

#endif /* WIN32 */
//} Win32 - Part

//{ Mac - Part
#ifdef qMacCvsPP
static void DoDataExchange_DiffMain(LWindow *theDialog, const MultiFiles* mf, bool & noRecurs, bool & isDate1, bool & isDate2,
	CStr & sLastRev1, CStr & sLastRev2, bool & hasRev1, bool & hasRev2, 
  bool& unifiedDiff, bool& noWhiteSpace, bool& noBlankLines, bool& ignoreCase,
	bool & extDiff, bool allowExtDiff, CStr& diffTool, bool putValue)
{
	LRadioButton *chkHasRev = dynamic_cast<LRadioButton*>
		(theDialog->FindPaneByID(item_CheckRevTag1));
	LEditText *editRev = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditRevTag1));
	LRadioButton *chkHasRev2 = dynamic_cast<LRadioButton*>
		(theDialog->FindPaneByID(item_CheckRevTag2));
	LEditText *editRev2 = dynamic_cast<LEditText*>
		(theDialog->FindPaneByID(item_EditRevTag2));
	LCheckBox *chkHasDate1 = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckDate1));
	LCheckBox *chkHasDate2 = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_CheckDate2));
	LPopupFiller *fill1 = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerRev1));
	LPopupFiller *fill2 = dynamic_cast<LPopupFiller*>
		(theDialog->FindPaneByID(item_FillerRev2));
	LCheckBox *chkUnifiedDiff = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_UnifiedDiff));
	LCheckBox *chkIgnoreCase = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_IgnoreCaseInDiff));
	LCheckBox *chkIgnoreWhiteSpace = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_IgnoreWhiteSpaceInDiff));
	LCheckBox *chkIgnoreEmptyLines = dynamic_cast<LCheckBox*>
		(theDialog->FindPaneByID(item_IgnoreEmptyLinesInDiff));
	
	if(putValue)
	{
		chkHasDate1->SetValue(isDate1 ? Button_On : Button_Off);
		chkHasDate2->SetValue(isDate2 ? Button_On : Button_Off);
		if(hasRev1 && hasRev2)
			theDialog->SetValueForPaneID (item_CheckRevTag2, Button_On);
		else if(hasRev1)
			theDialog->SetValueForPaneID (item_CheckRevTag1, Button_On);
		else
			theDialog->SetValueForPaneID (item_MergeNone, Button_On);

		if( sLastRev1.empty() )
		  editRev->SetText((Ptr)(isDate1 ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV), strlen(isDate1 ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV));
		else
		  editRev->SetText((Ptr)(const char *)sLastRev1, sLastRev1.length());

		if( sLastRev2.empty() )
		  editRev2->SetText((Ptr)(isDate2 ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV), strlen(isDate2 ? STR_DIFFDEFAULT_DATE : STR_DIFFDEFAULT_REV));
    else
  		editRev2->SetText((Ptr)(const char *)sLastRev2, sLastRev2.length());
  
		LTagWrapper *wrapper1 = putValue ? NEW LTagWrapper(fill1, mf) : 0L;
		fill1->DoDataExchange(gRevNames, putValue, wrapper1);
		LTagWrapper *wrapper2 = putValue ? NEW LTagWrapper(fill2, mf) : 0L;
		fill2->DoDataExchange(gRevNames, putValue, wrapper2);
		
		chkUnifiedDiff->SetValue(unifiedDiff ? Button_On : Button_Off);
		chkIgnoreCase->SetValue(ignoreCase ? Button_On : Button_Off);
		chkIgnoreWhiteSpace->SetValue(noWhiteSpace ? Button_On : Button_Off);
		chkIgnoreEmptyLines->SetValue(noBlankLines ? Button_On : Button_Off);
		
		DoDataExchange_DiffTool(theDialog, diffTool, extDiff, allowExtDiff, true);
	}
	else
	{
		isDate1 = chkHasDate1->GetValue() == Button_On;
		isDate2 = chkHasDate2->GetValue() == Button_On;

		if(theDialog->GetValueForPaneID (item_CheckRevTag2) == Button_On)
		{
			hasRev1 = hasRev2 = true;
		}
		else if (theDialog->GetValueForPaneID (item_CheckRevTag1) == Button_On)
		{
			hasRev1 = true;
			hasRev2 = false;
		}
		else
		{
			hasRev1 = hasRev2 = false;
		}

		fill1->DoDataExchange(isDate1 ? gDateNames : gRevNames, putValue);
		fill2->DoDataExchange(isDate2 ? gDateNames : gRevNames, putValue);

		char str[255];
		Size len;
		
		if ( hasRev1 ) {
		  editRev->GetText(str, 254, &len);
  		sLastRev1.set(str, len);
  	}
  	if ( hasRev2 ) {
  		editRev2->GetText(str, 254, &len);
  		sLastRev2.set(str, len);
    }
    		
		unifiedDiff = chkUnifiedDiff->GetValue() == Button_On;
		ignoreCase = chkIgnoreCase->GetValue() == Button_On;
		noWhiteSpace = chkIgnoreWhiteSpace->GetValue() == Button_On;
		noBlankLines = chkIgnoreEmptyLines->GetValue() == Button_On;

		DoDataExchange_DiffTool(theDialog, diffTool, extDiff, allowExtDiff, false);
	}
}
#endif /* qMacCvsPP */
//}
//{ Unix - Part
#ifdef qUnix
int UCvsDiff::OnOK(void)
{
	EndModal(true);
	return 0;
}

int UCvsDiff::OnCancel(void)
{
	EndModal(false);
	return 0;
}

void UCvsDiff::DoDataExchange(bool fill)
{
	if(fill)
	{
	}
	else
	{
	}
}

UCvsDiff_MAIN::UCvsDiff_MAIN(const char *rev1, const char *rev2, bool allowExtDiff, bool unifiedDiff, 
							 bool noWhiteSpace, bool noBlankLines, bool ignoreCase) : UWidget(::UEventGetWidID())
{
	m_check1 = false;
	m_check2 = false;
	m_rev1 = rev1;
	m_rev2 = rev2;
	m_diff = 0;
	m_noWhiteSpace = gNoWhiteSpace;
	m_noBlankLines = noBlankLines;	
	m_ignoreCase = gIgnoreCase;
	m_norecurs = false;
	m_extdiff = gUseExtDiff;
	m_unifiedDiff = unifiedDiff;
	m_allowExtDiff = allowExtDiff;
	
}

int UCvsDiff_MAIN::OnDiff(int cmd)
{
	int state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDiff1, 0L);
	if(state)
	{
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kCheckDate1, 0), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev1, 0), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kCheckDate2, 0), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev2, 0), 0L);
	}
	else
	{
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kCheckDate1, 1), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev1, 1), 0L);
		state = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDiff2, 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kCheckDate2, state == 0), 0L);
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kComboRev2, state == 0), 0L);
	}
	return 0;
}

void UCvsDiff_MAIN::DoDataExchange(bool fill)
{
	::DoDataExchange(fill, GetWidID(), kComboRev1, gDateOrRevNames);
	::DoDataExchange(fill, GetWidID(), kComboRev2, gDateOrRevNames);

	if(fill)
	{
		UEventSendMessage(GetWidID(), EV_ENABLECMD, UMAKEINT(kCheckExtDiff, m_allowExtDiff), 0L);

		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckDate1, m_check1), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckDate2, m_check2), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckRecurs, m_norecurs), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckExtDiff, m_extdiff), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckUnifiedDiff, m_unifiedDiff), 0L);		
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckDiff1 + m_diff, 1), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckNoWhiteSpace, m_noWhiteSpace), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckNoBlankLines, m_noBlankLines), 0L);
		UEventSendMessage(GetWidID(), EV_SETSTATE, UMAKEINT(kCheckIgnoreCase,   m_ignoreCase), 0L);		
	
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboRev1, (void *)(const char *)m_rev1.c_str());
		UEventSendMessage(GetWidID(), EV_SETTEXT, kComboRev2, (void *)(const char *)m_rev2.c_str());

		OnDiff(kCheckDiff1 + m_diff);
	}
	else
	{
		m_check1       = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate1, 0L);
		m_check2       = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDate2, 0L);
		m_norecurs     = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckRecurs, 0L);
		m_extdiff      = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckExtDiff, 0L);
		m_unifiedDiff  = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckUnifiedDiff, 0L);
		m_noWhiteSpace = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckNoWhiteSpace, 0L);
		m_noBlankLines = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckNoBlankLines, 0L);
		m_ignoreCase   = UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckIgnoreCase, 0L);

		if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDiff1, 0L))
			m_diff = 0;
		else if(UEventSendMessage(GetWidID(), EV_QUERYSTATE, kCheckDiff2, 0L))
			m_diff = 1;
		else
			m_diff = 2;

		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboRev1, &m_rev1);
		UEventSendMessage(GetWidID(), EV_GETTEXT, kComboRev2, &m_rev2);
	}
}
#endif // qUnix
//} Unix - Part

/// Get the diff options
bool CompatGetDiff(const MultiFiles* mf, kDiffCmdType diffType, 
				   bool& noRecurs, bool& isDate1, bool& isDate2,
				   std::string& rev1, std::string& rev2, bool& useExtDiff, std::string& extDiffTool,
				   bool& bulkUpdate, bool& unifiedDiff, bool& noWhiteSpace, bool& noBlankLines, bool& ignoreCase,
				   std::string& keyword, std::string& diffOptions, 
				   bool allowExtDiff /*= false*/)
{
	bool userHitOK = false;

	static string sLastRev1;
	static string sLastRev2;
	static string sLastKeyword;
	static string sLastDiffOptions;

	bool hasRev1;
	bool hasRev2;
	string initialRev1, initialRev2;
	
	// Graph view diff supplies initial revisions, so don't override them
	if( diffType == kDiffGraph )
	{
		initialRev1 = rev1;
		initialRev2 = rev2;
		hasRev1 = !rev1.empty();
		hasRev2 = !rev2.empty();
	}
	else
	{
		rev1 = "";
		rev2 = "";
		hasRev1 = false;
		hasRev2 = false;
		initialRev1 = sLastRev1;
		initialRev2 = sLastRev2;
	}

	noRecurs = false;
	isDate1 = false;
	isDate2 = false;
	useExtDiff = (bool)gUseExtDiff;
	bulkUpdate = (bool)gBulkUpdate;
	noWhiteSpace = (bool)gNoWhiteSpace;
	noBlankLines = (bool)gNoBlankLines;
	ignoreCase = (bool)gIgnoreCase;
	unifiedDiff = (bool)gUnifiedDiff;
	extDiffTool = gCvsPrefs.ExtDiff();
	
//{ Win32 - Part
#ifdef WIN32
	COptionalPropertySheet pages(gCvsPrefs.HideCommandDlgDiff(), "Diff settings");
	pages.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CDiff_MAIN page1(mf, diffType, initialRev1.c_str(), initialRev2.c_str(), allowExtDiff, useExtDiff, bulkUpdate, unifiedDiff, noWhiteSpace, ignoreCase, sLastKeyword.c_str(), sLastDiffOptions.c_str());
	CGetPrefs_GLOBALS page2;
	
	pages.AddPage(&page1);
	pages.AddPage(&page2);

	if( pages.DoModal() == IDOK )
	{
		noRecurs = page1.m_noRecurs ? true : false;

		sLastRev1 = page1.m_rev1;
		sLastRev2 = page1.m_rev2;
		isDate1 = page1.m_isDate1 ? true : false;
		isDate2 = page1.m_isDate2 ? true : false;

		gUseExtDiff = page1.m_extDiff ? true : false;
		gBulkUpdate = page1.m_bulkUpdate ? true : false;
		
		switch( page1.m_compareType )
		{
		case kCompareSameRev:
			hasRev1 = hasRev2 = false;
			break;
		case kCompareAnotherRev:
			hasRev1 = true;
			hasRev2 = false;
			break;
		case kCompareTwoRev:
			hasRev1 = true;
			hasRev2 = true;
			break;
		default:
			ASSERT(FALSE);	//unknown compare type
		}

		gUnifiedDiff = page1.m_unifiedDiff ? true : false;
		gNoWhiteSpace = page1.m_noWhiteSpace ? true : false;
		gIgnoreCase = page1.m_ignoreCase ? true : false;

		sLastKeyword = page1.m_keyword;
		sLastDiffOptions = page1.m_diffOptions;

		page2.StoreValues();
		gCvsPrefs.SetHideCommandDlgDiff(pages.GetHideCommandDlg());
		userHitOK = true;
	}
#endif /* WIN32 */
//} Win32 - Part
//{ Mac - Part
#ifdef qMacCvsPP
	StOptionalDialogHandler	theHandler(dlg_Diff, gCvsPrefs.HideCommandDlgDiff(), LCommander::GetTopCommander());
	LWindow *theDialog = theHandler.GetDialog();
	ThrowIfNil_(theDialog);
	static UInt16 sRuntimePanel = 1;
	
	LMultiPanelView *multiView = dynamic_cast<LMultiPanelView*>
		(theDialog->FindPaneByID(item_MultiViewPrefs));
	LPane *groupView = theDialog->FindPaneByID(item_GroupViewPrefs);
	
	multiView->SwitchToPanel(2);
	DoDataExchange_Globals(theDialog, true);
	multiView->SwitchToPanel(1);
	DoDataExchange_DiffMain(theDialog, mf, noRecurs, isDate1, isDate2, initialRev1, initialRev2,
		hasRev1, hasRev2, unifiedDiff, noWhiteSpace, noBlankLines, ignoreCase, useExtDiff, allowExtDiff, extDiffTool, true);
	if ( diffType == kDiffGraph )
	{
	  // disable label chooser view
	  LPane   *labelOptionsGroup = theDialog->FindPaneByID(item_DiffLabelOptions);
	  if ( labelOptionsGroup ) labelOptionsGroup->Disable();
	}
	groupView->SetValue(sRuntimePanel);
	// skip dialog when option key is pressed
  MessageT hitMessage;
	theHandler.ShowDialog();
	while (true)
	{		// Let DialogHandler process events
		hitMessage = theHandler.DoDialog();
		
		if (hitMessage == msg_OK || hitMessage == msg_Cancel)
			break;
	
	  HandleHitMessage_DiffTool(theDialog, hitMessage);
	}
	theDialog->Hide();
	sRuntimePanel = groupView->GetValue();
	
	if(hitMessage == msg_OK)
	{
		multiView->SwitchToPanel(1);
		DoDataExchange_DiffMain(theDialog, mf, noRecurs, isDate1, isDate2, sLastRev1, sLastRev2,
		  hasRev1, hasRev2, unifiedDiff, noWhiteSpace, noBlankLines, ignoreCase, useExtDiff, allowExtDiff, extDiffTool, false);
		gNoWhiteSpace   = noWhiteSpace;
		gNoBlankLines   = noBlankLines;
		gIgnoreCase     = ignoreCase;
		gUnifiedDiff    = unifiedDiff;
		gUseExtDiff     = useExtDiff;
		multiView->SwitchToPanel(2);
		DoDataExchange_Globals(theDialog, false);
		gCvsPrefs.SetHideCommandDlgDiff(theHandler.GetHideCommandDlg());
		userHitOK = true;
	}
#endif /* qMacCvsPP */
//} Mac - Part
//{ Unix - Part
#if qUnix
	void *wid = UCreate_DiffDlg();

	UCvsDiff *dlg = new UCvsDiff();
	UCvsDiff_MAIN *tab1 = new UCvsDiff_MAIN(sLastRev1.c_str(), sLastRev2.c_str(), allowExtDiff, unifiedDiff, noWhiteSpace, noBlankLines, ignoreCase);
	UEventSendMessage(dlg->GetWidID(), EV_INIT_WIDGET, kUMainWidget, wid);	
	dlg->AddPage(tab1, UCvsDiff::kTabGeneral, 0);

	if(dlg->DoModal())
	{
		noRecurs = tab1->m_norecurs;
		
		gNoWhiteSpace   = tab1->m_noWhiteSpace != 0;
		gNoBlankLines   = tab1->m_noBlankLines != 0;
		gIgnoreCase     = tab1->m_ignoreCase != 0;
		gUnifiedDiff    = tab1->m_unifiedDiff != 0;
		gUseExtDiff     = tab1->m_extdiff;
		
		sLastRev1 = tab1->m_rev1;
		sLastRev2 = tab1->m_rev2;
		isDate1 = tab1->m_check1;
		isDate2 = tab1->m_check2;

		if(tab1->m_diff == 0)
		{
			hasRev1 = hasRev2 = false;
		}
		else if(tab1->m_diff == 1)
		{
			hasRev1 = true;
			hasRev2 = false;
		}
		else
		{
			hasRev1 = true;
			hasRev2 = true;
		}

		userHitOK = true;
	}

	delete dlg;
#endif // qUnix
//} Unix - Part

	if( userHitOK )
	{
		if( hasRev1 )
			rev1 = sLastRev1;

		if( hasRev2 )
			rev2 = sLastRev2;
		
		useExtDiff     = (bool)gUseExtDiff;
		bulkUpdate     = (bool)gBulkUpdate;
		noWhiteSpace   = (bool)gNoWhiteSpace;
		noBlankLines   = (bool)gNoBlankLines;
		unifiedDiff    = (bool)gUnifiedDiff;
		ignoreCase     = (bool)gIgnoreCase;

		if( !sLastDiffOptions.empty() )
		{
			diffOptions = sLastDiffOptions;
		}

		if( !sLastKeyword.empty() )
		{
			keyword = sLastKeyword;
		}

		gCvsPrefs.save();
	}

	return userHitOK;
}
