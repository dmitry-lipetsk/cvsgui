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
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * AppGlue.mac.cpp --- glue code to access CVS shared library
 */

/* here should be a define, to recorgnize if this file is needed in development-tools */
#ifdef TARGET_OS_MAC

#include "stdafx.h"
#include "AppGlue.mac.h"
#include "AppGlue.h"
#include "MacCvsApp.h"
#include "AppConsole.h"
#include "dll_process.h"
#include "FileTraversal.h"
#include "MacCvsConstant.h"

int glue_eventdispatcher(EventRecord *ev)
{
    if(CMacCvsApp::gApp == 0L)
        return 0;
	
    if ( CMacCvsApp::gApp->CheckForUserCancel() )
    {
        // user pressed cancel key combo -> simulate a hit in the "stop" button
        //
        CMacCvsApp::gApp->ObeyCommand(cmd_Stop, NULL);
        return 0;
    }
        
    if (!ev) {
        CMacCvsApp::gApp->ProcessNextEvent();
    } else {
        CMacCvsApp::gApp->DispatchEvent(*ev);
    }
    return 0;
}

#endif /* TARGET_OS_MAC */
