// RCS: @(#) $Id: Win32wstring.hpp,v 1.1 2001/01/13 14:42:51 davygrvy Exp $

#include <string>
#include <windows.h>


// Wide (unicode) strings, windows style.
class Win32wstring : public std::wstring
{
public:
	Win32wstring(unsigned int codepage, std::string *s)
	{
		convert(codepage, s->c_str(), s->length());
	}

	Win32wstring(unsigned int codepage, std::string &s)
	{
		convert(codepage, s.c_str(), s.length());
	}

private:
	void convert(unsigned int codepage, const char *s, int inLen)
	{
		register int outLen;
		register wchar_t *buff = NULL;

		// I read this in the msdn docs:
		// "Windows NT 4.0 and Windows 2000: Translate using UTF-8.
		//  When this is set, dwFlags must be zero."
		//
		// Does this mean UTF-8 conversions don't work on win 95/98 ???
		//
		DWORD flags = (codepage == CP_UTF8 ? 0 : MB_USEGLYPHCHARS);

		// calc size needed
		outLen = ::MultiByteToWideChar(codepage, flags, s, inLen, buff, 0);

		buff = new wchar_t [outLen];

		// do it
		::MultiByteToWideChar(codepage, flags, s, inLen, buff, outLen);

		// set the string into our base class.
		assign(buff, outLen);

		// discard the scratch space.
		delete [] buff;
	}
};
