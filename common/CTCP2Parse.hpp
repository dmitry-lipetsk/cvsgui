/*
 ------------------------------------------------------------------------------
 * CTCP2Parse.hpp --
 *
 *	a C++ implementation of the Text attribute section of the CTCP/2
 *	draft by Robey Pointer (that eggdrop dude) and friends.  This code
 *	is based directly from Robey's Java example.  This code includes
 *	full support for decoding mIrc, ANSI, and ircII attributes as well.
 *	
 *	See -+-> http://www.lag.net/~robey/ctcp
 *	     +-> http://www.egroups.com/group/ctcp2
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
 * RCS: @(#) $Id: CTCP2Parse.hpp,v 1.4 2001/02/04 10:19:57 davygrvy Exp $
 ------------------------------------------------------------------------------
 */


#ifndef Inc_CTCP2Parse_hpp__
#define Inc_CTCP2Parse_hpp__

#include <string>
#include <vector>


// mIrc to CTCP/2 attribute convertor
class mircAttrCvt
{
public:
	void mirc2ctcp(std::string *in);
private:
	static const char tokens[2];
	static const char *mirc2ctcpmap[17];
	int extractcolor (std::string *in, int start, short *fg, short *bg);
};


// ircII to CTCP/2 attribute convertor
class irciiAttrCvt
{
public:
	void ircii2ctcp(std::string *in);
private:
	static const char tokens[5];
};


// ANSI escape sequences to CTCP/2 attribute convertor
class ansiAttrCvt
{
public:
	void ansi2ctcp(std::string *in);
private:
	void do_m ();
	void do_C ();
	
	const static char *escSeq;
	const static char *fgColorMap[16];
	const static char *bgColorMap[8];
	
	int pos;
	int bold, underline, reverse, blink, italic;
	
	std::string *_in;
	
	//  A simple ansi escape sequence extractor class
	class AnsiSeq
	{
		std::string *_seq;
		std::vector<int> _params;
		char _verb;
		static const char *_notOf;
		
	public:
		AnsiSeq (std::string *in, int pos);
		~AnsiSeq ();
		
		char verb () { return _verb; }
		const char *seq () { return _seq->c_str(); }
		int paramc () { return _params.size(); }
		int param (int index) { return _params[index]; }
	};
	
	AnsiSeq *_seq;
};


// CTCP/2 embedded attribute parser and action invoker.
//
class CTCP2Parse : protected mircAttrCvt, irciiAttrCvt, ansiAttrCvt
{
protected:
	// encoding flags.
	enum EncodingType
	{
		iso8859_1, iso8859_2, iso8859_3, iso8859_4, iso8859_5, iso8859_6,
		iso8859_7, iso8859_8, iso8859_9, UTF_8
	};

	// Character spacing.
	enum CharSpacingType { Fixed, Proportional };

	// 24-bit colors (initialized here as integers for clarity)
	enum ColorType
	{
		Black      = 0x000000,
		Blue       = 0x00007F,
		Green      = 0x007F00,
		Cyan       = 0x007F7F,
		Red        = 0x7F0000,
		Magenta    = 0x7F007F,
		Yellow     = 0x7F7F00,
		Lt_Grey    = 0xC0C0C0,
		Grey       = 0x7F7F7F,
		Lt_Blue    = 0x0000FF,
		Lt_Green   = 0x00FF00,
		Lt_Cyan    = 0x00FFFF,
		Lt_Red     = 0xFF0000,
		Lt_Magenta = 0xFF00FF,
		Lt_Yellow  = 0xFFFF00,
		White      = 0xFFFFFF
	};

	CTCP2Parse();

	void SetDefaults (
		unsigned char fgR = (unsigned char) ((Lt_Grey & 0xFF0000) >> 16),
		unsigned char fgG = (unsigned char) ((Lt_Grey & 0x00FF00) >> 8),
		unsigned char fgB = (unsigned char) (Lt_Grey & 0x0000FF),		// default foreground color.

		unsigned char bgR = (unsigned char) ((Black & 0xFF0000) >> 16),
		unsigned char bgG = (unsigned char) ((Black & 0x00FF00) >> 8),
		unsigned char bgB = (unsigned char) (Black & 0x0000FF),		// default background color.

		CharSpacingType cpDef_ = Fixed,		// default spacing.
		EncodingType encDef_ = iso8859_1	// default encoding.
		);

	void ParseIt(std::string *line);

	virtual void PushStart () = 0;
	virtual void PushBold (bool) = 0;
	virtual void PushReverse (bool) = 0;
	virtual void PushUnderline (bool) = 0;
	virtual void PushOverstrike (bool) = 0;
	virtual void PushItalic (bool) = 0;
	virtual void PushBlink (bool) = 0;
	virtual void PushURL (bool) = 0;
	virtual void PushEncoding (EncodingType) = 0;
	virtual void PushSpacing (CharSpacingType) = 0;
	virtual void PushFontSize (int) = 0;
	virtual void PushForegroundColor (unsigned char R, unsigned char G, unsigned char B) = 0;
	virtual void PushBackgroundColor (unsigned char R, unsigned char G, unsigned char B) = 0;
	virtual void PushTextSegment (std::string *) = 0;

private:
	typedef struct
	{
		union
		{
			// don't assume the sizeof(int)
			__int32 RGB;
			struct
			{
#ifdef _M_IX86  /* little endien */
				unsigned char B;
				unsigned char G;
				unsigned char R;
				unsigned char nul_void_ignore;
#else           /* big endien */
				unsigned char nul_void_ignore;
				unsigned char R;
				unsigned char G;
				unsigned char B;
#endif
			};
		};
	} RGBColor;

	inline void decodeToggle (std::string *line, int *parseFrmChar, int *parseEndChar, int *varCnt)
	{
		(*parseFrmChar)++;   // consume the attribute
		if (*parseFrmChar < *parseEndChar)
		{
			if (line->at(*parseFrmChar) == '+')
			{
				(*varCnt)++;
				(*parseFrmChar)++;   // consume the '+'
			}
			else if (line->at(*parseFrmChar) == '-')
			{
				if ((*varCnt) > 0)
					(*varCnt)--;
				(*parseFrmChar)++;   // consume the '-'
			}
		}
		else
			(*varCnt)++;  // default to a '+'
	}

	inline int decodeColor
		(std::string *line, int *parseFrmChar, int *parseEndChar, RGBColor *c)
	{
		if (line->at(*parseFrmChar) == '#')
		{
			(*parseFrmChar)++;   // consume it
			if (*parseFrmChar + 6 <= *parseEndChar)
			{
				unsigned char C = 0;

				c->RGB = 0;		// start with a clean plate

				// hi-byte red
				if (!fromHex(line->at(*parseFrmChar), &C)) return 3;
				(*parseFrmChar)++;  // consume the hex character
				c->R |= (C << 4);

				// lo-byte red
				if (!fromHex(line->at(*parseFrmChar), &C)) return 4;
				(*parseFrmChar)++;  // consume the hex character
				c->R |= C;

				// hi-byte green
				if (!fromHex(line->at(*parseFrmChar), &C)) return 5;
				(*parseFrmChar)++;  // consume the hex character
				c->G |= (C << 4);

				// lo-byte green
				if (!fromHex(line->at(*parseFrmChar), &C)) return 6;
				(*parseFrmChar)++;  // consume the hex character
				c->G |= C;

				// hi-byte blue
				if (!fromHex(line->at(*parseFrmChar), &C)) return 7;
				(*parseFrmChar)++;  // consume the hex character
				c->B |= (C << 4);

				// lo-byte blue
				if (!fromHex(line->at(*parseFrmChar), &C)) return 8;
				(*parseFrmChar)++;  // consume the hex character
				c->B |= C;
			} else {
				// rollback the error...
				return 3;
			}
		} else {
			unsigned char index = 0;
			if (!fromHex(line->at(*parseFrmChar), &index)) return 3;
			(*parseFrmChar)++;  // consume the hex character
			c->RGB = indexColor[index].RGB;
		}
		return 0;
	}

	inline int fromHex(unsigned char h, unsigned char *val)
	{
		// 0 - 9
		if (h > 47 && h < 58) *val = (h - 48);
		// A - F
		else if (h > 64 && h < 71) *val = (h - 55);
		else return 0;   // throw a little conversion exception
		return 1;
	}

	inline void PushOut (void)
	{
		// notify the derived class, we are about to start a new attribute set.
		PushStart();

		// never push-out ALL the attributes -- only the CHANGED ones.

		if ((boldCnt > 0) ^ boldLast) PushBold (boldLast = !boldLast ? true : false);
		if ((revCnt > 0) ^ revLast) PushReverse (revLast = !revLast ? true : false);
		if ((underCnt > 0) ^ underLast)	PushUnderline (underLast = !underLast ? true : false);
		if ((overCnt > 0) ^ overLast) PushOverstrike (overLast = !overLast ? true : false);
		if ((italicCnt > 0) ^ italicLast) PushItalic (italicLast = !italicLast ? true : false);
		if ((blinkCnt > 0) ^ blinkLast) PushBlink (blinkLast = !blinkLast ? true : false);
		if ((urlCnt > 0) ^ urlLast) PushURL (urlLast = !urlLast ? true : false);
		if (enc ^ encLast) PushEncoding (encLast = enc);
		if (cp ^ cpLast) PushSpacing (cpLast = cp);
		if (fntSize ^ fntLast) PushFontSize (fntLast = fntSize);

		if (fg.RGB ^ fgLast.RGB)
		{
			fgLast.RGB = fg.RGB;
			PushForegroundColor(fg.R, fg.G, fg.B);
		}
		if (bg.RGB ^ bgLast.RGB)
		{
			bgLast.RGB = bg.RGB;
			PushBackgroundColor(bg.R, bg.G, bg.B);
		}

		PushTextSegment(segment);
	}

	static const RGBColor indexColor[16];	// Array for indexing the color values
	EncodingType enc, encLast, encDef;	// storage for the encoding type

	// storage for the toggle states
	int boldCnt,  revCnt,  overCnt,  underCnt,  italicCnt,  blinkCnt,  urlCnt;
	int boldLast, revLast, overLast, underLast, italicLast, blinkLast, urlLast;
	int fntSize, fntLast;

	CharSpacingType cp, cpLast, cpDef;	// storage for font spacing type

	RGBColor	fg, fgLast, fgDef, bg, bgLast, bgDef;

	std::string *segment;
};

#endif