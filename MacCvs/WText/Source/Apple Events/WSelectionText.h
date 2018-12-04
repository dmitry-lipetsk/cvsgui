//=================================================================
// ¥ CSelectionText
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle the selection object in Apple script. Unlike the
// other text objects. This one sticks around. There is only one
// selection per document and we can afford to keep it around.
//
// Revision:
//
//=================================================================

#ifndef	_H_WSelectionText
#define	_H_WSelectionText

#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


#include "WTextModel.h"

const	SInt32	kAENotRelative = 'NoRe';

class WSelectionText : public WTextModel{

	private:
							WSelectionText();
							
							WSelectionText(
								const WSelectionText & inOriginal); 

		WSelectionText&		operator=(
								const WSelectionText& inRhs);								
					
	public:
	
							WSelectionText(LModelObject	*inObject);
	


	virtual					~WSelectionText();
							
	
	virtual SInt32			CountSubModels(
								DescType	inModelID) const;
											
	virtual void			SetAEProperty(
								DescType inProperty,
								const AEDesc &inValue,
								AEDesc& outAEReply);
										  							
	virtual void			MakeSelfSpecifier(	
								AEDesc	&inSuperSpecifier,
								AEDesc	&outSelfSpecifier) const;
};

#ifdef PP_Uses_Pragma_Import
#pragma import reset
#endif
#endif
