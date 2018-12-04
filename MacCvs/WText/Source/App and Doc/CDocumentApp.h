// ===========================================================================
//	CDocumentApp.h				©1994-2000 Metrowerks Inc. All rights reserved.
// ===========================================================================

#ifndef _H_CDocumentApp
#define _H_CDocumentApp
#pragma once

#include <LDocApplication.h>

class CDocumentApp : public LDocApplication {

public:
							CDocumentApp();
	virtual					~CDocumentApp();

	virtual Boolean			ObeyCommand(
								CommandT			inCommand,
								void*				ioParam = nil);	

	virtual void			FindCommandStatus(
								CommandT			inCommand,
								Boolean&			outEnabled,
								Boolean&			outUsesMark,
								UInt16&				outMark,
								Str255				outName);

	virtual void			OpenDocument(
								FSSpec*				inMacFSSpec);
									
	virtual LModelObject*	MakeNewDocument();
	virtual void			ChooseDocument();

	virtual void			PrintDocument(
								FSSpec*				inMacFSSpec);

protected:
	virtual void			StartUp();
	virtual	void			DoReopenApp();
	
			void			RegisterClasses();
};

#endif // _H_CDocumentApp