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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- May 1999
 */

#ifndef UMENU_H
#define UMENU_H

#include "uwidget.h"

#ifdef __cplusplus

class UMenu : public UWidget
{
	UDECLARE_DYNAMIC(UMenu)
public:
	UMenu(int widid);
	virtual ~UMenu();

#if qGTK
	inline void *GetAccelGroup(void) const { return fAccelGroup; }
		// maintain an accelerator group (GtkAccelGroup)
		// that you can add to your window.
	
	inline void *GetRadioGroup(void) const { return fRadioGroup; }
		// maintain an radio group (GList of GtkRadioMenuItem)
		// that you can add to your window.
	
	inline void SetRadioGroup(void* group) { fRadioGroup = group; }
		// set new radio group (GList of GtkRadioMenuItem)
		// that you can add to your window.
#endif

protected:
#if qGTK
	void *fAccelGroup;
	void *fRadioGroup;
#endif

	ev_msg void OnUpdateUI(void);
	ev_msg int OnCommands(int cmd);

	UDECLARE_MESSAGE_MAP()
};
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

UMENU UMenuAddPulldown(int widid, const char* name, UMENU top, int cmd = 0);
UMENU UMenuAddRadio(int widid, const char* name, UMENU top, const void* pixmap, int cmd);
UMENU UMenuAddToggle(int widid, const char* name, UMENU top, const void* pixmap, int cmd);
UMENU UMenuAddButton(int widid, const char* name, UMENU top, const void* pixmap, int cmd);
UMENU UMenuAddSeparator(int widid, UMENU top);
void UMenuDelete(UMENU top, int afterIndex);
void UMenuDeleteByCmd(UMENU top, int cmd);
int UMenuCount(UMENU top);

#ifdef __cplusplus
}
#endif

#endif /* UMenu_H */
