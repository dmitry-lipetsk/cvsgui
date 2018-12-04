//=================================================================
// ¥ CWordModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle word objects in apple events.
//
// Revision:
//
//=================================================================

#ifndef	_H_CWordModel
#define	_H_CWordModel

#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


#include	"WTextModel.h"



class WWordModel : public WTextModel{
	protected:
							WWordModel();
							
							
public:
							WWordModel(
								const WWordModel & inOriginal);
							
							WWordModel(	
								LModelObject	*inObject, 
								SInt32 			inPosition);
							
	WWordModel&		 	operator=(
								WWordModel & inRhs);
	
	virtual 				~WWordModel();
	
														
	virtual SInt32			CountSubModels(
									DescType	inModelID) const;
	
	virtual void			GetSubModelByPosition(	
									DescType		inModelID,
									SInt32			inPosition,
									AEDesc			&outToken) const;
															
private:
				void			InitWordModel(
										LModelObject	*inObject);
	
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif
