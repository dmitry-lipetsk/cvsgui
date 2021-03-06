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
 * version_fu.h -- Macros used for version numbering
 */

#pragma once

//////////////////////////////////////////////////////////////////////////
// Documentation
// 
// The macros below generate a number of strings that describe aspects of 
// the current build.
// 
// Principal aspects include a number of MS-specified flags:
//
// Debug build:         Self-explanatory.
//
// Prerelease build:    An experimental or alpha or beta build. If either 
//                      of the symbols WINCVS_ALPHA_RELEASE or 
//                      WINCVS_BETA_RELEASE is defined, then the build is
//                      automatically tagged as prerelease.
//
//                      The build is automatically flagged as a prerelease
//                      build if the flag WINCVS_RELEASE is NOT defined.
//                      Moreover, if either WINCVS_ALPHA_RELEASE or 
//                      WINCVS_BETA_RELEASE is defined, it overrides
//                      WINCVS_RELEASE.
//
// Private Build:       A non-standard build. We use the private build 
//                      flag to indicate that the build was not an 
//                  `   official release. As checked out with CVS, the
//                      sources are set up to build as a private build.
//
//                      To build an official release, the flag
//                      WINCVS_OFFICIAL_DISTRIBUTION should be 
//                      defined at build time. 
//
// Special Build:       Some other kind of non-standard build. We do not
//                      currently define special builds, but the plumbing
//                      is in place in version_fu to handle special builds
//                      if the flag WINCVS_SPECIAL_BUILD is defined.
//
//
// Strings and other macros generated by version_fu:
//
// WINCVS_FILEVERSION,
// WINCVS_PRODUCTVERSION:   Version tokens to be used in the VERSION_INFO 
//                      resource. These tokens have the form x,x,x,x where 
//                      x is a decimal number indicating 
//                      major,minor,patch,build.
//
//                      File version refers to the specific executable file
//                      (WinCvs.exe or cvs.exe). Product version refers to 
//                      the WinCvs package as a whole.
//
// WINCVS_FILEVERSION_STRING,
// WINCVS_PRODUCTVERSION_STRING:    Double-quoted string equivalents of the 
//                      above. The format is:
//                      "N x.x.x.x r (s p u d Build x)",
//                      Where 
//                      N = name (of file or product)
//                      x.x.x.x = major.minor.patchlevel.build,
//                      r = prerelease build string,
//                      s = special build string,
//                      p = private build string,
//                      u = unicode build string,
//                      d = debug build string.
//
//                      Example:
//                      "WinCvs 1.3.2.7 Beta 2 (Special Private Unicode Debug Build 7)"
//
// WCVS_PATCHED_FLAG,
// WCVS_SPECIAL_BUILD_FLAG,
// WCVS_PRIVATE_BUILD_FLAG,
// WCVS_PRERELEASE_FLAG,
// WVVS_DEBUG_BUILD_FLAG:   These are flags (numbers, which can be OR'ed together
//                          for the FILEFLAGS member of VERSION_INFO.
//
// WCVS_UNICODE_BUILD_STRING_PREFIX,
// WCVS_DEBUG_BUILD_STRING_PREFIX,
// WCVS_SPECIAL_BUILD_STRING_PREFIX,
// WCVS_PRIVATE_BUILD_STRING_PREFIX,
// WCVS_PRERELEASE_BUILD_STRING_PREFIX:    These are strings (ending with 
//                          a space) that can be concatenated together
//                          to produce a description of the various
//                          build conditions. The _PREFIX part of the name
//                          indicates that these are in a form to be
//                          concatenated together. 
//
//                          The order these fields appear in the File 
//                          Description and Product Description fields of 
//                          VERSION_INFO are: PRERELEASE, PRIVATE, SPECIAL, 
//                          UNICODE, and DEBUG.
//
// WCVS_SPECIAL_BUILD_STRING,
// WCVS_PRIVATE_BUILD_STRING:    the _PREFIX versions with "build" 
//                          concatenated to the end.
//
// 

//////////////////////////////////////////////////////////////////////////

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
// WINCVS_BUILD:        That macro indicates parts of the version 
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

//////////////////////////////////////////////////////////////////////////
// We need two macros because token expansion occurs when PP_STRINGIZE 
// expands x when passing it to PP_PASTE.
#define PP_PASTE(x) #x
#define PP_STRINGIZE(x) PP_PASTE(x)

#define WINCVS_FILEVERSION WINCVS_FILE_MAJOR,WINCVS_FILE_MINOR,WINCVS_FILE_PATCHLEVEL,WINCVS_BUILD
#define WINCVS_PRODUCTVERSION WINCVS_PRODUCT_MAJOR,WINCVS_PRODUCT_MINOR,WINCVS_PRODUCT_PATCHLEVEL,WINCVS_BUILD

#if defined(_UNICODE)
#define WCVS_UNICODE_BUILD_STRING_PREFIX "Unicode "
#else
#define WCVS_UNICODE_BUILD_STRING_PREFIX
#endif

#if defined(WCVS_PATCH)
#define WCVS_PATCHED_FLAG VS_FF_PATCHED
#else   // defined(WCVS_PATCH)
#define WCVS_PATCHED_FLAG 0
#endif  // defined(WCVS_PATCH)

#if defined(WINCVS_SPECIAL_BUILD)
#define WCVS_SPECIAL_BUILD_STRING_PREFIX WINCVS_SPECIAL_BUILD " "
#define WCVS_SPECIAL_BUILD_STRING WCVS_SPECIAL_BUILD_STRING_PREFIX "build"
#define WCVS_SPECIAL_BUILD_FLAG VS_FF_SPECIALBUILD
#else   // defined(WINCVS_SPECIAL_BUILD)
#define WCVS_SPECIAL_BUILD_STRING_PREFIX
#define WCVS_SPECIAL_BUILD_STRING
#define WCVS_SPECIAL_BUILD_FLAG 0
#endif  // defined(WINCVS_SPECIAL_BUILD)

#if defined(WINCVS_OFFICIAL_DISTRIBUTION)
#define WCVS_PRIVATE_BUILD_STRING_PREFIX
#define WCVS_PRIVATE_BUILD_STRING
#define WCVS_PRIVATE_BUILD_FLAG 0
#else   // defined(WINCVS_OFFICIAL_DISTRIBUTION)
#   if defined(WINCVS_PRIVATE_BUILD)
#       define WCVS_PRIVATE_BUILD_STRING_PREFIX WINCVS_PRIVATE_BUILD " "
#   else    // defined(WINCVS_PRIVATE_BUILD)
#       define WCVS_PRIVATE_BUILD_STRING_PREFIX "Local "
#   endif   // defined(WINCVS_PRIVATE_BUILD)
#define WCVS_PRIVATE_BUILD_STRING WCVS_PRIVATE_BUILD_STRING_PREFIX "build"
#define WCVS_PRIVATE_BUILD_FLAG VS_FF_PRIVATEBUILD
#endif  // defined(WINCVS_OFFICIAL_DISTRIBUTION)

#if defined(WINCVS_ALPHA_RELEASE)
#   if defined(WINCVS_RELEASE)
#       undef WINCVS_RELEASE
#   endif   // defined(WINCVS_RELEASE)
#   if defined(WINCVS_BETA_RELEASE)
#       undef WINCVS_BETA_RELEASE
#   endif   // defined(WINCVS_BETA_RELEASE)
#elif defined(WINCVS_BETA_RELEASE)      // defined(WINCVS_ALPHA_RELEASE)
#   if defined(WINCVS_RELEASE)
#       undef WINCVS_RELEASE
#   endif   // defined(WINCVS_RELEASE)
#endif  // defined(WINCVS_ALPHA_RELEASE)

#if defined(WINCVS_RELEASE)
#define WCVS_PRERELEASE_BUILD_STRING_PREFIX
#define WCVS_PRERELEASE_FLAG 0
#else   // defined(WINCVS_RELEASE)
#   if defined(WINCVS_ALPHA_RELEASE)
#       define WCVS_PRERELEASE_BUILD_STRING_PREFIX PP_STRINGIZE(Alpha WINCVS_PRODUCT_PATCHLEVEL) " "
#   elif defined(WINCVS_BETA_RELEASE)   // defined(WINCVS_ALPHA_RELEASE)
#       define WCVS_PRERELEASE_BUILD_STRING_PREFIX PP_STRINGIZE(Beta WINCVS_PRODUCT_PATCHLEVEL) " "
#   else    // defined(WINCVS_ALPHA_RELEASE)
#       define WCVS_PRERELEASE_BUILD_STRING_PREFIX "Prerelease "
#   endif   // defined(WINCVS_ALPHA_VERSION) ...
#define WCVS_PRERELEASE_FLAG VS_FF_PRERELEASE
#endif  // defined(WINCVS_RELEASE)

#if defined(_DEBUG)
#define WCVS_DEBUG_FLAG VS_FF_DEBUG
#define WCVS_DEBUG_BUILD_STRING_PREFIX "Debug "
#else   // defined(_DEBUG)
#define WCVS_DEBUG_BUILD_STRING_PREFIX
#define WCVS_DEBUG_FLAG 0
#endif  // defined(_DEBUG)

//
//  Resource Compiler chokes if we break these long lines with line continuation characters '\'
//
#define WINCVS_FILEVERSION_STRING PP_STRINGIZE(WINCVS_FILE_MAJOR.WINCVS_FILE_MINOR.WINCVS_FILE_PATCHLEVEL.WINCVS_BUILD) " " WCVS_PRERELEASE_BUILD_STRING_PREFIX "(" WCVS_PRIVATE_BUILD_STRING_PREFIX WCVS_SPECIAL_BUILD_STRING_PREFIX WCVS_UNICODE_BUILD_STRING_PREFIX WCVS_DEBUG_BUILD_STRING_PREFIX PP_STRINGIZE(Build WINCVS_BUILD) ")"

#define WINCVS_PRODUCTVERSION_STRING PP_STRINGIZE(WINCVS_FILE_MAJOR.WINCVS_FILE_MINOR.WINCVS_FILE_PATCHLEVEL.WINCVS_BUILD) " " WCVS_PRERELEASE_BUILD_STRING_PREFIX "(" WCVS_PRIVATE_BUILD_STRING_PREFIX WCVS_SPECIAL_BUILD_STRING_PREFIX WCVS_UNICODE_BUILD_STRING_PREFIX WCVS_DEBUG_BUILD_STRING_PREFIX PP_STRINGIZE(Build WINCVS_BUILD) ")"
