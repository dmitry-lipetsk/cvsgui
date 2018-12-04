//=================================================================
// ¥ CRangeText
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Routines to handle a range of text. This is a little 
// more complex since you can have two different text object types
// (although that is bad apple script).
//
// Revision:
//
//=================================================================

#ifndef	_H_WRangeModel
#define	_H_WRangeModel

#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


#include	"WTextModel.h"


class WRangeText : public WTextModel{

	private:
							WRangeText();
	public:
	
							WRangeText(
								const WRangeText & inOriginal);
								
							WRangeText(
								LModelObject	*inObject, 
								DescType startType, 
								SInt32 inStart, 
								DescType stopType, 
								SInt32 inStop);
								
	WRangeText&		operator=(
								const WRangeText & inRhs);
	
	
	virtual					~WRangeText();
	
	virtual void			GetModelRange(
								DescType	inModelKind,
								SInt32	&	ioPosition,
								SInt32	&	outStartOffset,
								SInt32	&	outEndOffset);
private:
	
			void			InitRangeText();	
private:
	
	SInt32	mRangeStart;
	SInt32	mRangeStop;
	DescType mModelKindStart;
	DescType mModelKindStop;

};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif