//=================================================================
// ¥ WCharModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handling the character model. Just a few modifications
// to CTextModel to get this to work right. This routine mainly 
// dissallows many action since char cannot have submodels.
//
// Revision:
//
//=================================================================

#ifndef	_H_WCharModel
#define	_H_WCharModel

#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif


#include	"WTextModel.h"

class WCharModel : public WTextModel{

protected:
							WCharModel();
							
							
public:
	
							WCharModel(	
								LModelObject	*inObject, 
								SInt32 			inPosition);
							WCharModel(
								const WCharModel & inOriginal);
							
	virtual 				~WCharModel();
	
	WCharModel& 			operator=(
								WCharModel & inRhs);
						
	virtual SInt32			CountSubModels(
								DescType	/*inModelID*/) const;
	
	virtual void			GetSubModelByPosition(	DescType		/*inModelID*/,
													SInt32			/*inPosition*/,
													AEDesc			&/*outToken*/) const;
private:
	
		 	void			InitCharModel(
								LModelObject *	inObject);	
		
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif
