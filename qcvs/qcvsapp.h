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

#ifndef QCVSAPP_H
#define QCVSAPP_H

#include <qapplication.h>
#include "MultiString.h"

class QDirView;
class QFileView;
class QCvsConsole;
class QComboBox;

class QCvsApp : public QApplication
{
	Q_OBJECT
public:
	QCvsApp(int & argc, char ** argv);
	virtual ~QCvsApp();

	static QCvsApp *qApp;

	inline QDirView *GetDirView() const { return m_dirview; }
	inline void SetDirView(QDirView *view) { m_dirview = view; }

	inline QFileView *GetFileView() const { return m_fileview; }
	inline void SetFileView(QFileView *view) { m_fileview = view; }

	inline QCvsConsole *GetConsole() const { return m_console; }
	inline void SetConsole(QCvsConsole *view) { m_console = view; }

	void SetCvsLoaded(bool state);
	inline bool IsCvsLoaded() const { return cvsLoaded; }

	void SetCvsStopping(bool state);
	inline bool IsCvsStopping(void) const { return gCvsStopping; }

	void InvalidateMenus();
	inline bool isMenuInvalid() const { return menuInvalid; }

	typedef enum
	{
		kNone,
		kConsole,
		kFileView,
		kDirView
	} kSoftFocus;

	void SetSoftFocus(kSoftFocus focus);
	inline kSoftFocus GetSoftFocus() const { return m_focus; }

	const char *GetPkgDataDir() const;

protected:
	QDirView *m_dirview;
	QFileView *m_fileview;
	QCvsConsole *m_console;

	bool cvsLoaded;
	bool gCvsStopping;
	bool menuInvalid;
	kSoftFocus m_focus;

	inline void ValidateMenus() { menuInvalid = false; }

protected slots:
	void OnNullEvent();
};

void DoDataExchange(bool putValue, CMString & mstr, QComboBox * combo);

#endif
