//=================================================================
// ¥ WTWordCounter
// © Timothy Paustian All Rights Reserved 1999
//	 WT TextEngine
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Count the words in a given strech of runs. PerformAction
// takes each run and tests to see if it contains text. if it does 
// the words i
//
// Revision:
//
//=================================================================

#ifndef __W__WTWordCounter
#define __W__WTWordCounter
#pragma once

#include "WRunLooper.h"

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

//class WTextView;

class WWordCounter : public WRunLooper{

	public:
									WWordCounter(
											WTextView	* inEngine);
											
		virtual					~WWordCounter();

		virtual bool			PerformAction(							
											WERunInfo			inRun,
											SInt32			inStartt,
											SInt32			inEnd);
											
		virtual void			SetModelCount(SInt32 inModelCount) {mWordCount = inModelCount;}
		
		virtual SInt32			GetModelCount() {return mWordCount;}
		
		virtual void			SetModelOffsets(
											SInt32		inStart,
											SInt32		inEnd);
		
		virtual void			GetModelOffsets(
											SInt32	&	outStart,
											SInt32	&	outEnd);			
		
		protected:
			SInt32		mWordCount;
			SInt32		mWordStart;
			SInt32		mWordEnd;
			
		private:
		
		static bool				IsSpace(
										const char	c)
									{return (  ((c) == ' ') || ((c) == '\r') || ((c) == '\t')  );}
									
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif