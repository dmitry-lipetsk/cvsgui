//=================================================================
// ¥ WRunLooper 
// © Timothy Paustian All Rights Reserved 1999
//	 WText
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WText/overview.html>
//=================================================================
//
// Purpose:	An abstract class to encapsulate running a number of runs and
// performing some action on each run. subclasses 
//	of this class must override the abstract functions to implement the class
//
// when you create an instance of this class you need to know what runs
// you want work with.
// 
// Revision:
//
//=================================================================

#ifndef __W__WRunLooper
#define __W__WRunLooper
#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

#ifndef __climits__
#include <climits>
#endif

#ifndef __WASTE__
#include "waste.h"
#endif

class	WTextView;

class WRunLooper{

	private:
										WRunLooper(){}
										WRunLooper(
												WRunLooper &	/*inOriginal*/){}
				WRunLooper&			operator = (
													WRunLooper & /*inRhs*/) {return *this;}
	public:
										WRunLooper(
											WTextView	* inEngine);
											
		virtual						~WRunLooper();

		virtual void				RunLoopByOffset(
												SInt32				inStart,
												SInt32				inEnd,
												SInt32				inFindIndex = LONG_MAX);
												
		virtual void				RunLoop(
											SInt32		inFirstRun,
											SInt32		inStartOffset,
											SInt32		inLastRun,
											SInt32		inEndOffset,
											SInt32		inFindIndex);
									
		virtual bool		PerformAction(							
											WERunInfo		inRun,
											SInt32			inStart,
											SInt32			inEnd) = 0;
		
		virtual SInt32		GetFindIndex() {return mFindIndex;}
	
		virtual void		SetFindIndex(SInt32 inFindIndex) {mFindIndex = inFindIndex;}
	
		virtual SInt32		GetModelCount() = 0;
	
		virtual void		SetModelCount(SInt32	inModelCount) = 0;
	
		virtual void		SetModelOffsets(
										SInt32	inStart,
										SInt32	inEnd) = 0;
										
		virtual void		GetModelOffsets(
										SInt32	& outStart,
										SInt32	& outEnd) = 0;	

	protected:
	
		WTextView * 		mTE;
		SInt32				mFindIndex;
		
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif