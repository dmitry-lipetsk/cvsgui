// =================================================================================
//	WasteDemoApp.h	 					© 1999 Timothy Paustian. All rights reserved.
// =================================================================================

#ifndef	_H_WasteDemoApp
#define	_H_WasteDemoApp

#pragma once


#include <LDocApplication.h>


#if PP_Uses_Pragma_Import
	#pragma import on
#endif


class WasteDemoApp : public LDocApplication {
public:
	
							WasteDemoApp();
	virtual 				~WasteDemoApp();

	//Apple events
	virtual void		HandleAppleEvent(	const AppleEvent	&inAppleEvent,
											AppleEvent			&outAEReply,
											AEDesc				&outResult,
											long				inAENumber);
	
	virtual void		GetSubModelByName(	DescType		inModelID,
											Str255			inName,
											AEDesc			&outToken) const;
											
	virtual void		GetSubModelByPosition(	DescType		inModelID,
												SInt32			inPosition,
												AEDesc			&outToken) const;
protected:
	virtual void			StartUp();
	virtual void			Initialize();
	virtual void			OpenDocument( FSSpec *inMacFSSpec );
	virtual LModelObject *	MakeNewDocument();
	virtual void			ChooseDocument();
	virtual void			PrintDocument( FSSpec *inMacFSSpec );
	
static FSSpec			sFileSpec;
static bool				sUsingTSM;

#if PP_DEBUG == 1
	LDebugMenuAttachment	* mDebugAttachment;
#endif
	
	
};


#if PP_Uses_Pragma_Import
	#pragma import reset
#endif
#endif