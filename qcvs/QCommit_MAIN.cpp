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
 * Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1998
 */

/**********************************************************************

	--- Qt Architect generated file ---

	File: QCommit_MAIN.cpp
	Last generated: Sat Nov 28 20:44:24 1998

 *********************************************************************/

#include "QCommit_MAIN.h"
#include "getline.h"
#include "TextBinary.h"
#include "CommitDlg.h"

#define Inherited QCommit_MAINData

QCommit_MAIN::QCommit_MAIN
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QCommit_MAIN::~QCommit_MAIN()
{
}

void QCommit_MAIN::OnCombo(int entry)
{
	CStr tmplPath(m_pathTmpl);
	if(!tmplPath.empty())
	{
		if(!tmplPath.endsWith(kPathDelimiter))
			tmplPath << kPathDelimiter;
		tmplPath << "CVS";
		tmplPath << kPathDelimiter;
		tmplPath << "Template";
	}

	bool gotIt = false;
	FILE *output;
	fLog->clear();
	int linecnt, col;
	
	if(entry == 0)
	{
		if(!tmplPath.empty() && (output = fopen(tmplPath, "r")) != 0L)
		{
			char *line = 0L;
			size_t line_chars_allocated = 0;
			int line_length;
			while ((line_length = getline (&line, &line_chars_allocated, output)) > 0)
			{
				fLog->getCursorPosition(&linecnt, &col);
				fLog->insertAt(line, linecnt, col);
			}

			gotIt = true;
			if (line_length < 0 && !feof (output))
				gotIt = false;

			if(line != 0L)
				free (line);

			fclose(output);
		}
		if(!gotIt)
		{
			fLog->getCursorPosition(&linecnt, &col);
			fLog->insertAt("*found no CVS/Template file*", linecnt, col);
		}
	}
	else
	{
		const vector<CStr> & list = m_CommitLogs->GetList();
		fLog->getCursorPosition(&linecnt, &col);
		fLog->insertAt(QString(list[entry - 1]), linecnt, col);
	}
}

void myDoDataExchange(bool putValue, CMString & mstr, QComboBox * combo, QMultiLineEdit *line)
{
	if(putValue)
	{
		combo->clear();
		combo->insertItem("Insert CVS/Template");
		const vector<CStr> & list = mstr.GetList();
		vector<CStr>::const_iterator i;
		for(i = list.begin(); i != list.end(); ++i)
		{
			CStr cstr(*i);
			combozify(cstr);
			combo->insertItem(QString(cstr));
		}
	}
	else
	{
		CStr value(line->text());
		if(!value.empty())
		{
			mstr.Insert(value);
		}
	}
}
void QCommit_MAIN::DoDataExchange(bool putvalue, CStr & commit, bool & norecurs,
						const char *pathTmpl, CMString & commitLogs)
{
	::myDoDataExchange(putvalue, commitLogs, fCombo, fLog);
	m_pathTmpl = pathTmpl;
	m_CommitLogs = &commitLogs;

	if(putvalue)
	{
		fNorecurs->setChecked(norecurs);
		fLog->setText(QString(commit));
	}
	else
	{
		norecurs = fNorecurs->isChecked();
		commit = fLog->text();
	}
}

#include "QCommit_MAIN.moc"
#include "QCommit_MAINData.moc"
