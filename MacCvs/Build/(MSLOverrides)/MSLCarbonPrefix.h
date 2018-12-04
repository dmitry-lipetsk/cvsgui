/* Metrowerks Standard Library
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2003/11/18 21:38:45 $
 * $Revision: 1.1 $
 */

#ifndef _MSL_MSLCARBONPREFIX_H
#define _MSL_MSLCARBONPREFIX_H

#ifdef TARGET_API_MAC_CARBON
	#if !TARGET_API_MAC_CARBON
		#error TARGET_API_MAC_CARBON must be on to use this prefix file!
	#endif
#else
	#define TARGET_API_MAC_CARBON 1
#endif

#include <os_enum.h>

#if __MACH__
	#pragma c99 on
	
	#define _MSL_USING_MW_C_HEADERS 0 // JUM: force using BSD headers
	
	#ifndef __NOEXTENSIONS__
		#define __NOEXTENSIONS__
	#endif
	#ifndef __CF_USE_FRAMEWORK_INCLUDES__
		#define __CF_USE_FRAMEWORK_INCLUDES__
	#endif
	
	#define __dest_os __mac_os_x
#else
	#define __dest_os __mac_os
#endif

#define __MSL_LONGLONG_SUPPORT__

#endif /* _MSL_MSLCARBONPREFIX_H */

/* Change record:
 * cc  010306 made a uniform change record
 * JWW 011027 Set __dest_os to __mac_os_x when compiling for Mach-O
 * JWW 020205 Define __NOEXTENSIONS__ when compiling for Mach-O so <fp.h> will work
 * JWW 020311 Test TARGET_API_MAC_CARBON and allow it to be defined prior to this prefix file
 */