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
 * Author : Jonathan M. Gilligan <jonathan.gilligan@vanderbilt.edu> --- 23 June 2001
 */

/*
 * version_no.h -- Definitions for version numbering in WinCvs, 
 *                 detailed documentation is below, see also, version_fu.h
 */

#pragma once

// WinCvs version
#define WINCVS_PRODUCT_MAJOR		2
#define WINCVS_PRODUCT_MINOR		1
#define WINCVS_PRODUCT_PATCHLEVEL	3

#define WINCVS_FILE_MAJOR			WINCVS_PRODUCT_MAJOR
#define WINCVS_FILE_MINOR			WINCVS_PRODUCT_MINOR
#define WINCVS_FILE_PATCHLEVEL		WINCVS_PRODUCT_PATCHLEVEL

// manually increment these when declaring a build.
// Ideally, tag the repository with the build number
// so a build can be reproduced.
#define WINCVS_BUILD 1

//
// Example preprocessor definitions.
//
// #define WINCVS_SPECIAL_BUILD "My special"
// note: no "build" at the end. This is added by
// version-fu.
//
//#define WINCVS_ALPHA_RELEASE
//#define WINCVS_BETA_RELEASE
//

#define WINCVS_RELEASE
#define WINCVS_PRIVATE_BUILD "Local"

//////////////////////////////////////////////////////////////////////////
// Preprocessor definitions (for version_no.h)
//
// Preprocessor flags (empty #defines):
// These should be #defined in version_no.h
//
// WINCVS_OFFICIAL_DISTRIBUTION:    #define this only when making 
//                      an official distribution package. This flag should 
//                      never be defined in anything checked into the cvs 
//                      repository.
//
// WINCVS_RELEASE:      define this if this is a release (as opposed to a
//                      prerelease) build.
//
// WINCVS_ALPHA_RELEASE:    define this if this is a alpha release. The 
//                      alpha number (e.g., alpha 2) will be taken from 
//                      the product patch level (see below).
//
// WINCVS_BETA_RELEASE: define this if this is a beta release. The beta 
//                      number (e.g., beta 2) will be taken from the 
//                      product patch level (see below).
//
// Preprocessor values (#defines with contents):
// These should be #defined in version_no.h
//
// WINCVS_FILE_MAJOR,
// WINCVS_FILE_MINOR,
// WINCVS_FILE_PATCHLEVEL,
// 
// WINCVS_PRODUCT_MAJOR,
// WINCVS_PRODUCT_MINOR,
// WINCVS_PRODUCT_PATCHLEVEL,
//
// WINCVS_BUILD:        These macros all indicate parts of the version 
//                      number. *FILE* indicates specific numbers for the 
//                      .exe files (WinCvs.exe, cvs.exe). The *PRODUCT*
//                      numbers indicate the version of the package as a 
//                      whole.
//
//                      At this time, WinCvs.exe is at version 1.3.2.x
//                      (WinCvs 1.3 beta 2, build x), cvs.exe is at
//                      1.11.1.x (cvs 1.11 patch level 1, build x). Both
//                      files are part of the WinCvs 1.3 beta 2
//                      distribution.
//
//                      The build numbers for both files should be 
//                      incremented (by hand editing version_no.h) at 
//                      least once for every build posted publicly at
//                      wincvs.org.
//
//                      EXAMPLE:
//
//                      #define WINCVS_FILE_MAJOR           1
//                      #define WINCVS_FILE_MINOR           3
//                      #define WINCVS_FILE_PATCHLEVEL      2
//                      
//                      #define WINCVS_PRODUCT_MAJOR        1
//                      #define WINCVS_PRODUCT_MINOR        3
//                      #define WINCVS_PRODUCT_PATCHLEVEL   2
//
//                      #define WINCVS_BUILD                1
//                      
//
// WINCVS_SPECIAL_BUILD,
// WINCVS_PRIVATE_BUILD:    These should be #defined to a double-quoted
//                      string that will be used to create a prefix for
//                      the build description string. E.g., the following:
//
//                      #define WINCVS_SPECIAL_BUILD "anomalous"
//                      #define WINCVS_PRIVATE_BUILD "in house"
//
//                      Will produce the text "in house anomalous build"
//                      in the file and product description strings,
//                      "anomalous build" in the Special Build Description
//                      string, and "in house build" in the Private Build
//                      string in the VERSION_INFO resource.
//
// All other necessary preprocessor definitions are synthesized by
// version_fu.h
//
//////////////////////////////////////////////////////////////////////////
