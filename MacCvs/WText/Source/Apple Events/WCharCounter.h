//=================================================================
// ¥ WCharCounter 
// © Timothy Paustian All Rights Reserved 1999
//	 WT TextEngine
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Count the chars in a given strech of runs. PerformAction
// takes each run and tests to see if it contains text. if it does 
// the chars in the run are counted
//
//
// Revision:
//
//=================================================================

#ifndef __W__WCharCounter
#define __W__WCharCounter
#pragma once


#include "WRunLooper.h"

#if PP_Uses_Pragma_Import
#pragma import on
#endif


class WCharCounter : public WRunLooper{

	public:
										WCharCounter(
											WTextView	* inEngine);
											
		virtual						~WCharCounter();

		virtual bool			PerformAction(							
											WERunInfo			inRun,
											SInt32			inStart,
											SInt32			inEnd);
		
		virtual void			SetModelCount(SInt32 inModelCount) {mCharCount = inModelCount;}
		virtual SInt32			GetModelCount() {return mCharCount;}
		virtual void			SetModelOffsets(
										SInt32	inStart,
										SInt32	inEnd);
										
		virtual void			GetModelOffsets(
										SInt32	& outStart,
										SInt32	& outEnd);
		
		virtual SInt32			GetIsTwoByte() {return mIsTwoByte;}
		
		protected:
			SInt32		mCharCount;
			SInt32		mCharOffset;
			bool		mIsTwoByte;
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif