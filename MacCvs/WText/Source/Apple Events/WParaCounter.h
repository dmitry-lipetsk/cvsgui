//=================================================================
// ¥ WParaCounter
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTextEngine/overview.html>
//=================================================================
//
// Purpose:	 Count the number of paragraphs in a specified strech
// of text.
//
// Revision:
//
//=================================================================


#ifndef __W__WParaCounter
#define __W__WParaCounter
#pragma once

#include "WRunLooper.h"

#if PP_Uses_Pragma_Import
	#pragma import on
#endif

class WParaCounter : public WRunLooper{

	public:
										WParaCounter(
											WTextView	* inEngine);
											
		virtual						~WParaCounter();

					
		virtual bool		PerformAction(							
											WERunInfo			inPara,
											SInt32			inStart,
											SInt32			inEnd);
		
		virtual void			SetModelCount(SInt32 inModelCount){mParaCount = inModelCount;}
		
		virtual SInt32			GetModelCount() {return mParaCount;}
		
		virtual void			SetModelOffsets(
											SInt32	inStart,
											SInt32	inEnd);
											
		virtual void			GetModelOffsets(
												SInt32	& 	outStart,
												SInt32	& 	outEnd);
	protected:
		SInt32		mParaCount;
		SInt32		mParaStart;
		SInt32		mParaEnd;
		
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif