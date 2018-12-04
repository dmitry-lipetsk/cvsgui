/*
 ------------------------------------------------------------------------------
 * CTCP2Parse.cpp  --
 *
 *	a C++ implementation of the Text attribute section of the CTCP/2
 *	draft by Robey Pointer (that eggdrop dude) and friends.  This code
 *	is based directly from Robey's Java example.  This code includes
 *	full support for decoding mIrc, ANSI, and ircII attributes as well.
 *	
 *	See --> http://www.lag.net/~robey/ctcp
 *	    --> http://www.egroups.com/group/ctcp2
 *
 *	Depends: STL string template, no OS specifics.
 *
 *	This source is under BSD license and has no restrictions the way
 *	the GPL does.  Sell it for all I care.  Just be nice and give me
 *	credit for it.  Although it's not a requirement.
 *
 *	Author: David Gravereaux <davygrvy@pobox.com>
 *		Tomahawk Software Group
 *
 *	Conversion routines donated by Keith Lea <keith@cs.oswego.edu>
 *
 * RCS: @(#) $Id: CTCP2Parse.cpp,v 1.4 2001/01/14 15:33:39 davygrvy Exp $
 ------------------------------------------------------------------------------
 */

#include "stdafx.h"
#include "CTCP2Parse.hpp"

CTCP2Parse::CTCP2Parse ()
{
	boldCnt = revCnt = overCnt = underCnt = italicCnt = blinkCnt = urlCnt = 0;
	boldLast = revLast = overLast = underLast = italicLast = blinkLast = urlLast = 0;
	fntSize = fntLast = 0;

	fg.RGB = fgLast.RGB = fgDef.RGB = Lt_Grey;
	bg.RGB = bgLast.RGB = bgDef.RGB = Black;

	cp = cpLast = cpDef = Fixed;
	enc = encLast = encDef = iso8859_1;
}

void CTCP2Parse::SetDefaults (
	unsigned char fgR, unsigned char fgG, unsigned char fgB,	// default foreground color
	unsigned char bgR, unsigned char bgG, unsigned char bgB,	// default background color
	CharSpacingType cpDef_,
	EncodingType encDef_)
{

	// reassemble the seperate RGB values into our internal rep.

	fg.R = fgLast.R = fgDef.R = fgR;
	fg.G = fgLast.G = fgDef.G = fgG;
	fg.B = fgLast.B = fgDef.B = fgB;

	bg.R = bgLast.R = bgDef.R = bgR;
	bg.G = bgLast.G = bgDef.G = bgG;
	bg.B = bgLast.B = bgDef.B = bgB;

	cp = cpLast = cpDef = cpDef_;
	enc = encLast = encDef = encDef_;
}

void CTCP2Parse::ParseIt (std::string *line)
{
	int parseFrmChar = 0, parseEndChar = 0, parseStart = 0;
	int fntVal, fntDir = 1;
	int rollBack;

	// Pre-parse and replace other attribute standards with
	// the ctcp2 equivalents for display.
	mirc2ctcp(line);
	ircii2ctcp(line);
	ansi2ctcp(line);


	// find an opening ctcp/2 format character (^F)
	parseFrmChar = line->find_first_of("\006");

	// do we have any?
	while (parseFrmChar != std::string::npos)
	{
		// find the closing ctcp/2 format character (^F)
		parseEndChar = line->find_first_of("\006", parseFrmChar+1);
		
		if (parseEndChar == std::string::npos)
		{
			// an opening without a closing is bogus
			// so remove it and fallout of the while loop
			line->replace(parseFrmChar, 1, "");
			break;
		}
		
		// check for text before this format character
		// and after the last format character (or index 0 for
		// the first iteration.
		if (parseFrmChar > parseStart)
		{
			// yes.. we got a text segment ready to push.
			segment = new std::string(line->substr(parseStart, (parseFrmChar - parseStart)));
			PushOut();
			delete segment;
		}

		// now, interpret this format code and decode this attribute
		// (and options, if any).


		parseFrmChar++;  // consume the first ^F.


		switch (line->at(parseFrmChar)) {
			
#	define CTCP2_TOGGLE(ch, var)  \
		case (ch): \
		decodeToggle(line, &parseFrmChar, &parseEndChar, &(var)); \
			break;

		// bold
		CTCP2_TOGGLE('B', boldCnt);

		// reverse
		CTCP2_TOGGLE('V', revCnt);

		// underline
		CTCP2_TOGGLE('U', underCnt);

		// over-strike
		CTCP2_TOGGLE('S', overCnt);

		// italics
		CTCP2_TOGGLE('I', italicCnt);

		// blink
		CTCP2_TOGGLE('K', blinkCnt);

		// URL
		CTCP2_TOGGLE('L', urlCnt);

#	undef CTCP2_TOGGLE

		// color
		case 'C':
			parseFrmChar++;   // consume the 'C'
			if (parseFrmChar < parseEndChar)
			{
				if (line->at(parseFrmChar) == '.')
				{
					// reset to default
					fg.RGB = fgDef.RGB;
					parseFrmChar++;  // consume the '.'
				}
				else if (line->at(parseFrmChar) == '-')
				{
					// ignore
					parseFrmChar++;   // consume the '-'
				}
				else
				{
					if (rollBack = decodeColor(line, &parseFrmChar, &parseEndChar, &fg))
					{
						// rollback the error to the 'C'
						parseFrmChar -= rollBack;
						fg.RGB = fgDef.RGB;
						break;
					}
				}

				
				// background color?
				if (parseFrmChar < parseEndChar)
				{
					if (line->at(parseFrmChar) == '.')
					{
						bg.RGB = bgDef.RGB;
						parseFrmChar++;  // consume the '.'
					}
					else if (line->at(parseFrmChar) == '-')
					{
					    // ignore
					    parseFrmChar++;   // consume the '-'
					}
					else
					{
						if (rollBack = decodeColor(line, &parseFrmChar, &parseEndChar, &bg))
						{
							// rollback the error to the 'C'
							parseFrmChar -= rollBack;
							bg.RGB = bgDef.RGB;
							break;
						}
					}
				}
				else
					parseFrmChar--;   // rollback one
			}
			else
			{
				// no colors were specified: reset to normal
				fg.RGB = fgDef.RGB;
				bg.RGB = bgDef.RGB;
			}

			// if fg/bg are too similar, reset to defaults.
			// (this is in the ctcp2 draft).  These calculations
			// are DIRECTLY from ctcp2's draft.
			if ((fg.R >= bg.R - 8) && (fg.R <= bg.R + 8) &&
			    (fg.G >= bg.G - 8) && (fg.G <= bg.G + 8) &&
			    (fg.B >= bg.B - 8) && (fg.B <= bg.B + 8))
			{
				fg.RGB = fgDef.RGB;
				bg.RGB = bgDef.RGB;
			}
			break;
			
		// font size
		case 'F':
			parseFrmChar++;
			if (parseFrmChar < parseEndChar)
			{
				if (line->at(parseFrmChar) == '+') fntDir = 1;
				else fntDir = -1;
				parseFrmChar++;
				if (parseFrmChar < parseEndChar)
				{
					fntVal = line->at(parseFrmChar) - 48;
					if (fntVal > 4) fntVal = 4;
				} else {
					// no amount -- assume 1
					fntVal = 1;
					parseFrmChar--;
				}
				fntSize += (fntDir*fntVal);
			} else {
				// no parameters -- reset to normal
				fntSize = 0;
				parseFrmChar--;
			}
			break;
			
		// character spacing
		case 'P':
			parseFrmChar++;   // consume it
			if (parseFrmChar < parseEndChar)
			{
				switch (line->at(parseFrmChar))
				{
				case 'P':
					cp = Proportional; parseFrmChar++; break;
				case 'F':
					cp = Fixed; parseFrmChar++; break;
				default:
					// rollback to show the error.
					parseFrmChar -= 2;
				}
			} else {
				// no parameters -- reset to normal
				cp = cpDef;
			}
			break;
			
		// text encoding
		case 'E':
			parseFrmChar++;   // consume it
			switch (line->at(parseFrmChar))
			{
			case '1':
				if (line->at(parseFrmChar+1) == '0') parseFrmChar++; // what's iso8859-10 ???
				enc = iso8859_1; parseFrmChar++; break;
			case '2':
				enc = iso8859_2; parseFrmChar++; break;
			case '3':
				enc = iso8859_3; parseFrmChar++; break;
			case '4':
				enc = iso8859_4; parseFrmChar++; break;
			case '5':
				enc = iso8859_5; parseFrmChar++; break;
			case '6':
				enc = iso8859_6; parseFrmChar++; break;
			case '7':
				enc = iso8859_7; parseFrmChar++; break;
			case '8':
				enc = iso8859_8; parseFrmChar++; break;
			case '9':
				enc = iso8859_9; parseFrmChar++; break;
			case 'U':
				enc = UTF_8; parseFrmChar++; break;
			default:
				// rollback to show the error.
				parseFrmChar -= 2;
			}
			break;
			
		// revert to normal
		case 'N':
			parseFrmChar++;   // consume it
			boldCnt = revCnt = overCnt = underCnt = italicCnt = blinkCnt = urlCnt = 0;
			enc = encDef;
			fg.RGB = fgDef.RGB;
			bg.RGB = bgDef.RGB;
			cp = cpDef;
			break;
				
		// client extensions
		case 'X':
			//  no extensions are defined at this time.
			/*
			parseFrmChar++;
			switch (line->at(parseFrmChar)) {
			case '':	
			default:
				 rollback to show the error.
				parseFrmChar -= 2;
			}
			*/
			parseFrmChar--;
			break;
				
		// error condition...
		default:
			// rollback one.  unrecognized format specifier.
			parseFrmChar--;
		}
    
		// extra params? include them in the text surrounded by "[ ]"
		// (This is recommended, in case someone chooses to "hide" a
		//    message by doing "^FNsell yoyodyne stocks!^F" -- this code
		//    effectively "unhides" it, and is [yet again] recommended
		//    by the CTCP2 draft.)
    
		parseFrmChar++;
		if (parseFrmChar < parseEndChar)
		{
			line->replace(parseFrmChar, parseEndChar - parseFrmChar,
				std::string("\006[") +
				line->substr(parseFrmChar, parseEndChar-parseFrmChar) +
				std::string("]"));

			parseEndChar = parseFrmChar;
		}
    
		// save old end position
		parseStart = parseEndChar+1;
    
		// got any more?
		parseFrmChar = line->find_first_of("\006", parseStart);
	};
  
  
  
	// anything left?
	if (parseStart < line->length())
	{
		// last text segment is ready to push.
		segment = new std::string(line->substr(parseStart));
		PushOut();
		delete segment;
	}
};

const CTCP2Parse::RGBColor CTCP2Parse::indexColor[16] = {
	Black, Blue, Green, Cyan, Red, Magenta, Yellow, Lt_Grey, Grey,
	Lt_Blue, Lt_Green, Lt_Cyan, Lt_Red, Lt_Magenta, Lt_Yellow, White
};



#define CCOLOR (char)3  // ctr-c
#define CCTCP2 (char)6  // ctr-f
#define MIRC_TRANSPARENT    16
#define isn(ch) ((ch) >= '0' && (ch) <= '9')


const char
mircAttrCvt::tokens[2] = {
	CCOLOR, 0
};


const char *
mircAttrCvt::mirc2ctcpmap[17] = {
	"F",		// white
	"0",		// black
	"1",		// blue
	"2",		// green
	"C",		// bright red
	"#7F3F3F",	// brown   (not exactly red (7F0000))
	"5",		// purple
	"#FF7F00",	// orange  (definately not yellow (7F7F00))
	"E",		// bright yellow
	"A",		// bright green
	"3",		// cyan
	"B",		// bright cyan
	"9",		// bright blue
	"#FF7FC0",	// pink    (not really bright magenta (FF00FF))
	"8",		// grey
	"7",		// light grey
	"."		// return to default (^K99)
};


void
mircAttrCvt::mirc2ctcp(std::string *in)
{
	int col;
	char f[20];
	bool eqf, eqb;
	short oldfg, oldbg, fg = -1, bg = -1;
	int pos;
	
	pos = in->find_first_of(tokens);
	
	while (pos != std::string::npos)
	{
		oldfg = fg;
		oldbg = bg;
		col = extractcolor(in, pos+1, &fg, &bg);
		eqf = (oldfg == fg);
		eqb = (oldbg == bg);
		
		if (col && (!eqf || !eqb))
		{
			// if the color hasn't changed, we insert a - which means
			// basically "ignore me"
			sprintf(f, "C%s%s", 
				eqf ? "-" : mirc2ctcpmap[fg], 
				eqb ? (eqf ? 0 : "-") : mirc2ctcpmap[bg]);
			
			in->replace(pos, col+1,
				std::string("") + CCTCP2 + f + CCTCP2);
			pos += (2 + strlen(f));   // consume the additions
		}
		else
		{
			in->replace(pos, 1, std::string("") + CCTCP2 + 'C' + CCTCP2);
			pos += 2;   // consume the additions
		}
		pos = in->find_first_of(tokens, pos);
	}
};


int
mircAttrCvt::extractcolor (std::string *in, int start, short *fg, short *bg)
{
	int ptr = start, len = in->length(), preColor;
	
	if (ptr == len) return 0;

	if (isn(in->at(ptr)))
	{
		// there's a foreground

		if (ptr + 1 < len && isn(in->at(ptr + 1)))
		{
			// it's 2 digits
			preColor = (((in->at(ptr) - '0') * 10) + (in->at(ptr + 1) - '0'));
			if (preColor == 99) *fg = MIRC_TRANSPARENT;
			else *fg = preColor % 16;
			ptr += 2;
		}
		else
		{
			// it's 1 digit
			*fg = in->at(ptr) - '0';
			ptr++;
		}
		if (ptr + 1 < len && in->at(ptr) == ',' && isn(in->at(ptr + 1)))
		{
			// there's a background
			ptr++; // the comma

			if (ptr + 1 < len && isn(in->at(ptr + 1)))
			{
				// it's 2 digits
				preColor = (((in->at(ptr) - '0') * 10) + (in->at(ptr + 1) - '0'));
				if (preColor == 99) *bg = MIRC_TRANSPARENT;
				else *bg = preColor % 16;
				ptr += 2;
			}
			else
			{
				// it's 1 digit
				*bg = in->at(ptr) - '0';
				ptr++;
			}
		}
	}
	return (ptr - start);
};


// ------------------------------ macro definitions -------------------
#define CBOLD  (char)2  // ctr-b
#define CCTCP2 (char)6  // ctr-f
#define CSTOP  (char)15 // ctr-o
#define CREV   (char)22 // ctr-v, reverse
#define CUNDR  (char)31 // ctr-underscore


const char irciiAttrCvt::tokens[5] = {
	CBOLD, CUNDR, CREV, CSTOP, 0
};

void
irciiAttrCvt::ircii2ctcp (std::string *in)
{
	int pos, bold = 0, under = 0, rev = 0;
	
	pos = in->find_first_of(tokens);
	
	while (pos != std::string::npos)
	{
		switch (in->at(pos))
		{
		case CBOLD:
			bold = !bold;
			in->replace(pos, 1,
				std::string("") + CCTCP2 + 'B' + (bold?'+':'-') + CCTCP2);
			pos += 3;   // consume the additions
			break;
			
		case CUNDR:
			under = !under;
			in->replace(pos, 1,
				std::string("") + CCTCP2 + 'U' + (under?'+':'-') + CCTCP2);
			pos += 3;   // consume the additions
			break;
			
		case CREV:
			rev = !rev;
			in->replace(pos, 1,
				std::string("") + CCTCP2 + 'V' + (rev?'+':'-') + CCTCP2);
			pos += 3;   // consume the additions
			break;
			
		case CSTOP:
			bold = under = rev = 0;
			in->replace(pos, 1, std::string("") + CCTCP2 + 'N' + CCTCP2);
			pos += 2;   // consume the additions
		}
		pos = in->find_first_of(tokens,++pos);
	}
}


// See http://www.delorie.com/gnu/docs/screen/screen_48.html for a good list of ansi escapes.
// We're not supporting everything, though.  Just the small bit about colors and cursor move right.


#define INVALID_SEQUENCE    -1
const char *ansiAttrCvt::AnsiSeq::_notOf = "1234567890=;";

ansiAttrCvt::AnsiSeq::AnsiSeq (std::string *in, int pos)
	: _seq(0), _verb(0)
{
	int verbPos;
	int seqLen;
	int pos1, pos2;
	
	verbPos = in->find_first_not_of(_notOf, pos + 2);
	
	if (verbPos == std::string::npos || in->at(verbPos) == '\033')
	{
		_verb = INVALID_SEQUENCE;
		in->replace(pos, 2, "");
		return;
	}
	
	seqLen = verbPos - pos - 1;
	
	// get a copy of the ansi sequence without the "esc["
	_seq = new std::string(in->substr(pos + 2, seqLen));
	
	// remove it from the original, now that we have the needed info
	in->replace(pos, verbPos - pos + 1, "");
	
	// set the verb
	_verb = _seq->at(seqLen - 1);
	
	for (pos1 = 0, pos2 = 0; pos2 < seqLen; pos2++)
	{
		if (_seq->at(pos2) != ';' && pos2 < (seqLen - 1)) continue;
		_params.push_back(atoi(_seq->substr(pos1, pos2 - pos1).c_str()));
		pos1 = pos2 + 1;
	}
	
}


ansiAttrCvt::AnsiSeq::~AnsiSeq ()
{
	if (_seq) delete _seq;
}


#define ANSI_SGM_TA_RESET	0
#define ANSI_SGM_TA_BOLD	1
#define ANSI_SGM_TA_FAINT	2
#define ANSI_SGM_TA_ITALIC	3
#define ANSI_SGM_TA_ULINE	4
#define ANSI_SGM_TA_BLINK	5
#define ANSI_SGM_TA_REV		7
#define ANSI_SGM_TA_CONSEAL	8
#define ANSI_SGM_TA_NORMAL	22
#define ANSI_SGM_TA_ITALIC_OFF	23
#define ANSI_SGM_TA_ULINE_OFF	24
#define ANSI_SGM_TA_BLINK_OFF	25
#define ANSI_SGM_TA_REV_OFF	27

#define ANSI_SGM_FC_BLK		30
#define ANSI_SGM_FC_RED		31
#define ANSI_SGM_FC_GRN		32
#define ANSI_SGM_FC_YEL		33
#define ANSI_SGM_FC_BLU		34
#define ANSI_SGM_FC_MAG		35
#define ANSI_SGM_FC_CYN		36
#define ANSI_SGM_FC_WHT		37
#define ANSI_SGM_FC_DEFAULT	39

#define ANSI_SGM_BC_BLK		40
#define ANSI_SGM_BC_RED		41
#define ANSI_SGM_BC_GRN		42
#define ANSI_SGM_BC_YEL		43
#define ANSI_SGM_BC_BLU		44
#define ANSI_SGM_BC_MAG		45
#define ANSI_SGM_BC_CYN		46
#define ANSI_SGM_BC_WHT		47
#define ANSI_SGM_BC_DEFAULT	49


const char *ansiAttrCvt::escSeq = "\033\133";

void
ansiAttrCvt::ansi2ctcp (std::string *in)
{
	bold = underline = reverse = blink = italic = 0;
	_in = in;

	pos = in->find(escSeq);
	
	while (pos != std::string::npos)
	{
		_seq = new AnsiSeq(in, pos);
		
		switch (_seq->verb())
		{
		case INVALID_SEQUENCE:
			break;
			
		case 'm':		// set graphic mode
			do_m();
			break;
		case 'C':		// move cursor right
			do_C();
			break;
		case 'f':		// move to cursor position
		case 'H':		//          "
		case 'A':		// move cursor up
		case 'B':		// move cursor down
		case 'D':		// move cursor left
		case 's':		// save cursor position
		case 'u':		// restore cursor position
		case 'J':		// erase display and goto home
		case 'K':		// erase line
		case 'h':		// set display mode
		case 'l':		// reset display mode
		default:
			std::string errmsg(std::string ("[ansi error: '") + _seq->seq() +
				"' sequence unsupported]");
			in->insert(pos, errmsg);
			pos += errmsg.length();
		}
		delete _seq;
		pos = in->find(escSeq, pos);
	}
};

const char *ansiAttrCvt::fgColorMap[16] = {
	"C0-",		// Black
	"C4-",		// Red
	"C2-",		// Green
	"C6-",		// Yellow
	"C1-",		// Blue
	"C5-",		// Magenta
	"C3-",		// Cyan
	"C7-",		// Light Grey
	"C8-",		// Grey
	"CC-",		// Light Red
	"CA-",		// Light Green
	"CE-",		// Light Yellow
	"C9-",		// Light Blue
	"CD-",		// Light Magenta
	"CB-",		// Light Cyan
	"CF-"		// White
};

const char *ansiAttrCvt::bgColorMap[8] = {
	"C-0",		// Black
	"C-4",		// Red
	"C-2",		// Green
	"C-6",		// Yellow
	"C-1",		// Blue
	"C-5",		// Magenta
	"C-3",		// Cyan
	"C-7"		// Light Grey
};

void
ansiAttrCvt::do_m ()
{
	int i;
	for (i = 0; i < _seq->paramc(); i++)
	{
		switch (_seq->param(i))
		{
		// reset
		case ANSI_SGM_TA_RESET:
			bold = underline = reverse = blink = 0;
			_in->insert(pos, std::string("") + CCTCP2 + 'N' + CCTCP2);
			pos += 3;
			break;
			
		// bold intensity
		case ANSI_SGM_TA_BOLD:
			if (!bold) bold = 1;
			break;
			
		// bold intensity
		case ANSI_SGM_TA_FAINT:
			// TODO: what do I do for this ???
			break;
			
		// bold + faint off (normal intensity)
		case ANSI_SGM_TA_NORMAL:
			if (bold) bold = 0;
			break;
			
		// standout (italic)
		case ANSI_SGM_TA_ITALIC:
			if (!italic)
			{
				italic = 1;
				_in->insert(pos, std::string("") + CCTCP2 + "I+" + CCTCP2);
				pos += 4;		// consume the additions
			}
			break;
			
		// standout (italic) off
		case ANSI_SGM_TA_ITALIC_OFF:
			if (italic)
			{
				italic = 0;
				_in->insert(pos, std::string("") + CCTCP2 + "I-" + CCTCP2);
				pos += 4;		// consume the additions
			}
			break;
			
		// underline
		case ANSI_SGM_TA_ULINE:
			if (!underline)
			{
				underline = 1;
				_in->insert(pos, std::string("") + CCTCP2 + "U+" + CCTCP2);
				pos += 4;		// consume the additions
			}
			break;
			
		// underline off
		case ANSI_SGM_TA_ULINE_OFF:
			if (underline)
			{
				underline = 0;
				_in->insert(pos, std::string("") + CCTCP2 + "U-" + CCTCP2);
				pos += 4;		// consume the additions
			}
			break;
			
		// blink
		case ANSI_SGM_TA_BLINK:
			if (!blink)
			{
				blink = 1;
				_in->insert(pos, std::string("") + CCTCP2 + "K+" + CCTCP2);
				pos += 4;	// consume additions.
			}
			break;
			
		// blink off
		case ANSI_SGM_TA_BLINK_OFF:
			if (blink)
			{
				blink = 0;
				_in->insert(pos, std::string("") + CCTCP2 + "K-" + CCTCP2);
				pos += 4;	// consume additions.
			}
			break;
			
		// reverse
		case ANSI_SGM_TA_REV:
			if (!reverse)
			{
				reverse = 1;
				_in->insert(pos, std::string("") + CCTCP2 + "V+" + CCTCP2);
				pos += 4;	// consume the additions
			}
			break;

		// reverse off
		case ANSI_SGM_TA_REV_OFF:
			if (reverse)
			{
				reverse = 0;
				_in->insert(pos, std::string("") + CCTCP2 + "V-" + CCTCP2);
				pos += 4;	// consume the additions
			}
			break;

		// concealed
		case ANSI_SGM_TA_CONSEAL:
			// TODO: wtf is this?  make foreground same color as back?
			break;

		case ANSI_SGM_FC_BLK:
		case ANSI_SGM_FC_RED:
		case ANSI_SGM_FC_GRN:
		case ANSI_SGM_FC_YEL:
		case ANSI_SGM_FC_BLU:
		case ANSI_SGM_FC_MAG:
		case ANSI_SGM_FC_CYN:
		case ANSI_SGM_FC_WHT:
			_in->insert(pos, std::string("") + CCTCP2 +
				fgColorMap[_seq->param(i) - 30 + (bold ? 8 : 0)] +
				CCTCP2);
			pos += 5;		// consume the additions
			break;

		case ANSI_SGM_FC_DEFAULT:
			_in->insert(pos, std::string("") + CCTCP2 + "C.-" + CCTCP2);
			pos += 5;		// consume the additions
			break;

		case ANSI_SGM_BC_BLK:
		case ANSI_SGM_BC_RED:
		case ANSI_SGM_BC_GRN:
		case ANSI_SGM_BC_YEL:
		case ANSI_SGM_BC_BLU:
		case ANSI_SGM_BC_MAG:
		case ANSI_SGM_BC_CYN:
		case ANSI_SGM_BC_WHT:
			_in->insert(pos, std::string("") +
				CCTCP2 + bgColorMap[_seq->param(i) - 40] + CCTCP2);
			pos += 5;		// consume the additions
			break;

		case ANSI_SGM_BC_DEFAULT:
			_in->insert(pos, std::string("") + CCTCP2 + "C-." + CCTCP2);
			pos += 5;		// consume the additions
			break;

		}
	}
}


void
ansiAttrCvt::do_C ()
{
	for (int i = 0; i < _seq->param(_seq->paramc() - 1); i++)
		_in->insert(pos++, " ");
}
