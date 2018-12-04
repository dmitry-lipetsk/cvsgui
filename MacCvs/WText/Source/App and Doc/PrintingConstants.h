// =================================================================================
//	DocumentsConstants.h			©1995-1997 Metrowerks Inc. All rights reserved.
// =================================================================================

#ifndef __WT_PrintingConstants__
#define __WT_PrintingConstants__

#pragma once


#ifndef _H_PP_Types
#include <PP_Types.h>
#endif

const ResIDT	rPPob_TextWindow				= 1000;
const PaneIDT	kTextScroller					= 1;
const PaneIDT	kTextView						= 2;

const ResIDT	rPPob_TextPrintout				= 1100;
const PaneIDT	kTextPlaceholder				= 1;

const MessageT	cmd_Black			= FOUR_CHAR_CODE('blck');
const MessageT	cmd_Red				= FOUR_CHAR_CODE('red ');
const MessageT	cmd_Green			= FOUR_CHAR_CODE('gren');
const MessageT	cmd_Blue			= FOUR_CHAR_CODE('bloo');
const MessageT	cmd_Yellow			= FOUR_CHAR_CODE('yelo');
const MessageT	cmd_Magenta			= FOUR_CHAR_CODE('magn');
const MessageT	cmd_Cyan			= FOUR_CHAR_CODE('cyan');
const MessageT	cmd_Other			= FOUR_CHAR_CODE('OtHr');
const MessageT	cmd_ButtonFontMenu	= 9010;
const MessageT	cmd_ButtonSizeMenu	= 9011;
const MessageT	cmd_ButtonStyleMenu	= 9012;
const MessageT	cmd_ButtonColorMenu	= 9013;
const RGBColor	RGBBlack			= {0x0, 0x0, 0x0};
const RGBColor	RGBRed				= {0xFFFF, 0x0, 0x0};
const RGBColor	RGBGreen			= {0x0, 0xFFFF, 0x0};
const RGBColor	RGBBlue				= {0x0, 0x0, 0xFFFF};
const RGBColor	RGBYellow			= {0xFFFF, 0xFFFF, 0x0};
const RGBColor	RGBMagenta			= {0xFFFF, 0x0, 0xFFFF};
const RGBColor	RGBCyan				= {0x0, 0xFFFF, 0xFFFF};
#endif