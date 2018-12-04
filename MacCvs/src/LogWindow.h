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
 * Contributed by Strata Inc. (http://www.strata3d.com)
 * Author : Alexandre Parenteau <aubonbeurre@hotmail.com> --- December 1997
 */

/*
 * LogWindow.h --- log window of tha application
 */

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include "AppConsole.h"
#include <list>
#include <string>

const ResIDT	rPPob_CVSLogWindow		= 128;
const ResIDT	rPPob_CVSToolbar		= 1554;
class WTextView;

class CLogWindow : public LWindow {
public:
	enum { class_ID = 'LOGW' };

	CLogWindow(LStream *inStream);
	virtual ~CLogWindow();
	
	virtual Boolean ObeyCommand(CommandT inCommand, void *ioParam);
	virtual void FindCommandStatus(CommandT inCommand, Boolean &outEnabled,
									  Boolean &outUsesMark, UInt16 &outMark,
									  Str255 outName);
		// part of LWindow inheritance
	
	virtual void	FinishCreateSelf();
		// part of LPane inheritance

	long WriteCharsToConsole(const char *buffer, long n, bool isStdErr = false);
		// console stub, intercepts MSL printf's

	inline WTextView *GetWE(void) {return mWE != nil ? mWE : FindWE();}
		// result is never nil
	
	void OutColor(const char *txt, long len);
	void OutColor(kConsoleColor color);
	void OutColor(/*WEStyleMode*/int mode, const TextStyle  & ts);
		// color support

  void SetAutoShow(bool inAutoShow) { mAutoShow = inAutoShow; }
  bool AutoShowEnabled() const { return mAutoShow; }
  
  void SetSelectOnError(bool inSelectOnError) { mSelectOnError = inSelectOnError; }
  bool SelectOnErrorEnabled() const { return mSelectOnError; }
  
	void FlushCache();

protected:
	void DoPrint(void);
		// callback for cmd_Print

	void FeedThisLine(const char *buffer, long n, bool isStdErr);

	Boolean AskSaveAs(void);
	void DoSave(FSSpec & outFSSpec);
		// callback for cmd_SaveAs

	WTextView *FindWE(void);
		// fetch the panes to find the Waste control
	
	void WriteCharsToConsoleSelf(const std::string& str, bool isStdErr);
	
#if 0
	THPrint mPrintRecordH;
#endif
	WTextView           *mWE;
	bool                mAutoShow;
	bool                mSelectOnError;
	EventLoopTimerRef   mFlushCacheTimer;
	bool                mFlushCacheTimerRunning;
//	std::list<std::pair<std::string, bool> >   mCachedOutput;
  std::string         mCachedOutput;
  TextStyle           mCachedStyle;
#	define MAX_CHAR_BY_LINE 1024
	static char *sbuf;
	static int snumbuf;
};

#endif /* LOGWINDOW_H */
