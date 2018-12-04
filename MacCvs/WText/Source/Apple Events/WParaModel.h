//=================================================================
// ¥ CParaModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle paragraph objects in apple events.
//
// Revision:
//
//=================================================================

#ifndef	_H_CParaModel
#define	_H_CParaModel

#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


#include	"WTextModel.h"

class WParaModel : public WTextModel{

public:
	private:
							WParaModel();
							
							
public:
	
							WParaModel(
								const WParaModel & inOriginal);
							
							WParaModel(	
								LModelObject	*inObject, 
								SInt32 			inPosition);
		
		WParaModel& 	operator=(
								WParaModel & inRhs);
	
							
	virtual 				~WParaModel();
							
								
	virtual SInt32			CountSubModels(
										DescType	inModelID) const;
	
	virtual void			GetSubModelByPosition(	
										DescType		inModelID,
										SInt32			inPosition,
										AEDesc		&outToken) const;										
											
private:
				void			InitParaModel(
									LModelObject	*inObject);
	
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif
