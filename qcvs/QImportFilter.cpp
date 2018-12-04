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

	File: QImportFilter.cpp
	Last generated: Mon Nov 30 18:18:11 1998

 *********************************************************************/

#include "QImportFilter.h"
#include "ImportFilter.h"
#include "QImportReport.h"
#include "QImportWarning.h"

#include <qpixmap.h>

#include "mini-stop.xpm"
#include "warning.xpm"

static QPixmap *istop = 0L, *iwarn = 0L;

QImportType::QImportType(QListView * parent) :
	QListViewItem(parent)
{
}

QImportType::~QImportType()
{
}

QImportError::QImportError(QListView * parent, ReportWarning *data) :
	QImportType(parent), warn(data)
{
}

QImportError::~QImportError()
{
}

void QImportError::setup()
{
	CStr desc;
	setExpandable(FALSE);

	bool isError = warn->Kind() != kFileIsOK && warn->Kind() != kTextEscapeChar;
	setText(0, isError ? "Error" : "Warning");
	setPixmap(0, *(isError ? istop : iwarn));

	switch(warn->Kind())
	{
	case kFileIsOK:
		desc = "File is OK"; // should not happen
		break;
	case kFileMissing:
		desc = "File is missing or unreadable";
		break;
	case kFileIsAlias:
		desc = "File is an alias";
		break;
	case kFileInvalidName:
		desc = "File has an invalid name";
		break;
	case kTextWrongLF:
		desc = "File has the wrong line feed for this machine";
		break;
	case kTextIsBinary:
		desc = "File seems to be binary, while expecting text";
		break;
	case kTextEscapeChar:
		desc = "File has some escape characters in it (0x00-0x20, 0x80-0xFF)";
		break;
	case kTextWrongSig:
		desc = "Text File has a wrong mac signature";
		break;
	case kBinWrongSig:
		desc = "Binary File has a wrong mac signature";
		break;
	default:
		desc = "File has some unknown problem";
		break;
	}
	setText(1, QString(desc));

	QListViewItem::setup();
}

QImportConflict::QImportConflict(QListView * parent, ReportConflict *data) :
	QImportType(parent), conf(data)
{
}

QImportConflict::~QImportConflict()
{
}

void QImportConflict::setup()
{
	setExpandable(FALSE);

	if(conf->HasConflict())
		setPixmap(0, *istop);

	QListViewItem::setup();
}

QString QImportConflict::text(int column) const
{
	QString desc;
	if(column == 0)
		return conf->HasConflict() ? QString("Error") : QString("OK");
	else if (column == 1)
	{
		desc = "Type ";
		desc += (conf->IsExtension() ? "*" : "");
		desc += (conf->GetPattern() != 0L ? conf->GetPattern() : "");
		return desc;
	}

	if(conf->HasForceBinary())
	{
		desc = "FORCE BINARY";
	}
	else if(conf->HasForceText())
	{
		desc = "FORCE TEXT";
	}
	else if(conf->HasIgnore())
	{
		desc = "IGNORE";
	}
	else if(conf->IsBinary())
	{
		desc = "BINARY";
	}
	else
	{
		desc = "TEXT";
	}
	return desc;
}

#define Inherited QImportFilterData

QImportFilter::QImportFilter
(
	ReportConflict *& entries,
	ReportWarning *& warnings,
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name ),
	m_entries(entries),
	m_warnings(warnings)
{
	if(iwarn == 0L)
	{
		istop = new QPixmap(i_mini_stop);
		iwarn = new QPixmap(i_warning);
	}

	setCaption( "Import filter/wrapper" );
	fEdit->setEnabled(false);
	fList->setAllColumnsShowFocus(true);

	CStr desc;
	ReportWarning * warn = warnings;
	while(warn != 0L)
	{
		// Store this information in the list view
		new QImportError(fList, warn);

		warn = warn->next;
	}

	ReportConflict * conf = entries;
	while(conf != 0L)
	{
		// Store this information in the list view
		new QImportConflict(fList, conf);
		
		conf = conf->next;
	}
	
}


QImportFilter::~QImportFilter()
{
}

void QImportFilter::OnEdit()
{
	QListViewItem * item = fList->firstChild();
	if(item != 0L) do
	{
		if(item->isSelected())
		{
			OnEdit(item);
			return;
		}
		item = item->nextSibling();
	} while(item != 0L);
}

void QImportFilter::OnEdit(QListViewItem* item)
{
	if(((QImportType *)item)->IsWarning())
	{
		QImportWarning dlg;
		dlg.DoDataExchange(true, ((QImportError *)item)->warn);
		if(dlg.exec() == QDialog::Accepted)
		{
			dlg.DoDataExchange(false, ((QImportError *)item)->warn);
			fList->triggerUpdate();
		}
	}
	else
	{
		QImportReport dlg;
		dlg.DoDataExchange(true, ((QImportConflict *)item)->conf);
		if(dlg.exec() == QDialog::Accepted)
		{
			dlg.DoDataExchange(false, ((QImportConflict *)item)->conf);
			fList->triggerUpdate();
		}
	}
}

void QImportFilter::OnSelChanged(QListViewItem*)
{
	QListViewItem * item = fList->firstChild();
	if(item != 0L) do
	{
		if(item->isSelected())
		{
			fEdit->setEnabled(true);
			return;
		}
		item = item->nextSibling();
	} while(item != 0L);

	fEdit->setEnabled(false);
}

#include "QImportFilter.moc"
#include "QImportFilterData.moc"
