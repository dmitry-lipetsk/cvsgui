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
 * Author : Karl-Heinz Bruenen <gcvs@bruenen-net.de> --- Jan 2003
 */

#ifndef _CVSGUI_I18N_H
#define _CVSGUI_I18N_H

#ifdef WIN32
   /// Simply wrong, temporary definition until i18n is properly supported
#  define _i18n _T
#else
  /*
   * Standard gettext macros.
   */
#  ifdef ENABLE_NLS
#    include <libintl.h>
#    undef _i18n
#    define _i18n(String) dgettext (PACKAGE, String)
#    ifdef gettext_noop
#      define N_(String) gettext_noop (String)
#    else
#      define N_(String) (String)
#    endif
#  else
#    define textdomain(String) (String)
#    define gettext(String) (String)
#    define dgettext(Domain,Message) (Message)
#    define dcgettext(Domain,Message,Type) (Message)
#    define bindtextdomain(Domain,Directory) (Domain)
#    define _i18n(String) (String)
#    define N_(String) (String)
#  endif /* ENABLE_NLS */
#endif /* WIN32 */

#endif /* _CVSGUI_I18N_H */
