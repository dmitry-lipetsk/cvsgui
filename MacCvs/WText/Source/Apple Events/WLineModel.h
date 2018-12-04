//=================================================================
// ¥ CLineModel
// © Timothy Paustian All Rights Reserved 1997
//	 Apple Event Text Model
// <mailto:paustian@bact.wisc.edu>
// <http://www.bact.wisc.edu/WTText/overview.html>
//=================================================================
//
// Purpose:	Handle lines objects in apple events. These you have to
// be careful of because they change depending upon the word wrap
// of the text. For non-word wrap text paragraphs would work just
// as well
//
// Revision:
//
//=================================================================

#ifndef	_H_CLineModel
#define	_H_CLineModel

#pragma once

#if PP_Uses_Pragma_Import
	#pragma import on
#endif



#include	"WTextModel.h"

class WLineModel : public WTextModel{

private:
							WLineModel();
							
		WLineModel& 	operator=(
									WLineModel & inRhs);							

							WLineModel(
								const WLineModel & inOriginal);							
public:
	

						
							WLineModel(	
								LModelObject	*inObject, 
								SInt32 			inPosition);

	
	virtual 				~WLineModel();
	
	
							
	virtual SInt32			CountSubModels(
										DescType	inModelID) const;
	
	virtual void			GetSubModelByPosition(	
										DescType		inModelID,
										SInt32			inPosition,
										AEDesc		&outToken) const;
										
private:
	
				void		InitLineModel(
									LModelObject	*inObject);
	
};

#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif
