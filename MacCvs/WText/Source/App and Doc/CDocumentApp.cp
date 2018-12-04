// ===========================================================================
//	CDocumentApp.cp 			©1994-2000 Metrowerks Inc. All rights reserved.
// ===========================================================================
//	This file contains the starter code for a Document PowerPlant project

#include "CDocumentApp.h"

#include <LGrowZone.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <UDrawingState.h>
#include <UMemoryMgr.h>
#include <URegistrar.h>
#include <UEnvironment.h>
#include <UAttachments.h>
#include <LDocument.h>
#include <LUndoer.h>
#include <LWindow.h>
#include <LPrintout.h>
#include <LPlaceHolder.h>
#include <UDesktop.h>
#include <UStandardDialogs.h>
#include <UControlRegistry.h>

#include "CTextView.h"
#include "CTextDocument.h"

#include <Appearance.h>



// ===========================================================================
//	¥ main
// ===========================================================================

int main()
{							
		// Set Debugging options
	SetDebugThrow_(debugAction_Alert);
	SetDebugSignal_(debugAction_Alert);

		// Initialize Memory Manager. Parameter is the number of
		// master pointer blocks to allocate
	InitializeHeap(3);
	
		// Initialize standard Toolbox managers
	UQDGlobals::InitializeToolbox();
	
		// Install a GrowZone to catch low-memory situations	
	new LGrowZone(20000);

		// Create the application object and run
	CDocumentApp	theApp;
	theApp.Run();
	
	return 0;
}


// ---------------------------------------------------------------------------
//	¥ CDocumentApp									[public]
// ---------------------------------------------------------------------------
//	Application object constructor

CDocumentApp::CDocumentApp()
{
		// Register ourselves with the Appearance Manager
	if (UEnvironment::HasFeature(env_HasAppearance)) {
		::RegisterAppearanceClient();
	}

	RegisterClasses();
	
		// Preload facilities for the Standard Dialogs
	PP_StandardDialogs::Load();
	
		// Require at least Navigation Services 1.1. See comments
		// above SetTryNavServices in UConditionalDialogs.cp for why
		// you might wish to do this.
#if PP_StdDialogs_Option == PP_StdDialogs_Conditional
	UConditionalDialogs::SetTryNavServices(0x01108000);
#endif
}


// ---------------------------------------------------------------------------
//	¥ ~CDocumentApp									[public, virtual]
// ---------------------------------------------------------------------------
//	Application object destructor

CDocumentApp::~CDocumentApp()
{
		// Clean up after Standard Dialogs
	PP_StandardDialogs::Unload();
}


// ---------------------------------------------------------------------------
//	¥ StartUp										[protected, virtual]
// ---------------------------------------------------------------------------
//	Perform an action in response to the Open Application AppleEvent.
//	Here, issue the New command to open a window.

void
CDocumentApp::StartUp()
{
	ObeyCommand(cmd_New, nil);
}


// ---------------------------------------------------------------------------
//	¥ DoReopenApp									[protected, virtual]
// ---------------------------------------------------------------------------
//	Support the Finder's "re-open application" ('rapp') Apple Event. From
//	Apple TechNote 1102 (on Mac OS 8):
//
//		The Finder now sends a 're-open application' Apple event ('rapp') to
//		applications when the application is already running and it is opened
//		from one of the Finder's windows (either by a double click or by
//		selecting the application and choosing the Open command). Applications
//		receiving a 'rapp' event (when they do not have any windows open) should
//		open a new untitled document just as they would when processing an 'oapp'
//		event.

void
CDocumentApp::DoReopenApp()
{
		// Given the suggested course of action by TN1102, the appropriate
		// action to take would be the equivalent of calling StartUp(). You
		// could call that here if you wish, but PowerPlant (nor this stationery)
		// will do that by default as StartUp() may contain code that one would
		// only wish to execute in response to an 'oapp' event, e.g. displaying
		// a splash screen. Additionally, 'rapp' should only open the new
		// untitled document if there are no other windows open.
		//
		// Given these circumstances, we'll create a new untitled document
		// (cmd_New) if no regular or modal windows are open (the TN didn't
		// address layers, but we'll assume this is what they mean).
	
	if ((UDesktop::FetchTopRegular() == nil)
		&& (UDesktop::FetchTopModal() == nil)) {
		ObeyCommand(cmd_New, nil);
	}
}


// ---------------------------------------------------------------------------
//	¥ ObeyCommand									[public, virtual]
// ---------------------------------------------------------------------------
//	Respond to Commands. Returns true if the Command was handled, false if not.

Boolean
CDocumentApp::ObeyCommand(
	CommandT	inCommand,
	void*		ioParam)
{
	Boolean		cmdHandled = true;	// Assume we'll handle the command

	switch (inCommand) {

		default: {
			cmdHandled = LDocApplication::ObeyCommand(inCommand, ioParam);
			break;
		}
	}
	
	return cmdHandled;
}


// ---------------------------------------------------------------------------
//	¥ FindCommandStatus								[public, virtual]
// ---------------------------------------------------------------------------
//	Determine the status of a Command for the purposes of menu updating.

void
CDocumentApp::FindCommandStatus(
	CommandT	inCommand,
	Boolean&	outEnabled,
	Boolean&	outUsesMark,
	UInt16&		outMark,
	Str255		outName)
{
	switch (inCommand) {

		default: {
			LDocApplication::FindCommandStatus(inCommand, outEnabled,
											outUsesMark, outMark, outName);
			break;
		}
	}
}


// ---------------------------------------------------------------------------
//	¥ OpenDocument									[protected, virtual]
// ---------------------------------------------------------------------------
// This method is called when a file is chosen from the Open File dialog.

void
CDocumentApp::OpenDocument(
	FSSpec*		inMacFSSpec)
{
	LDocument*	theDoc = LDocument::FindByFileSpec(*inMacFSSpec);
	
		// If the document is already open make it the current document,
		// else create a new document.
	if (theDoc != nil) {
		theDoc->MakeCurrent();
	} else {							
		theDoc = new CTextDocument(this, inMacFSSpec);
	}
}


// ---------------------------------------------------------------------------
//	¥ MakeNewDocument								[protected, virtual]
// ---------------------------------------------------------------------------
// This method creates a new document and installs it into the application's
// Apple Event Object Model hierarchy.

LModelObject*
CDocumentApp::MakeNewDocument()
{
		// Make a new empty document.
	return new CTextDocument(this, nil);
}


// ---------------------------------------------------------------------------
//	¥ ChooseDocument								[protected, virtual]
// ---------------------------------------------------------------------------
// This method uses the PowerPlant Standard Dialogs to let the user choose a
// document to open.

void
CDocumentApp::ChooseDocument()
{
	PP_StandardDialogs::LFileChooser	chooser;
	
		// Open any/all TEXT files
	
	NavDialogOptions*	options = chooser.GetDialogOptions();
	if (options != nil) {
		options->dialogOptionFlags =	kNavDefaultNavDlogOptions
										+ kNavSelectAllReadableItem;
	}

	if (chooser.AskOpenFile(LFileTypeList(ResType_Text))) {
		AEDescList		docList;
		chooser.GetFileDescList(docList);
		SendAEOpenDocList(docList);
	}
}


// ---------------------------------------------------------------------------
//	¥ PrintDocument									[protected, virtual]
// ---------------------------------------------------------------------------
// This method is called in response to a Print request

void
CDocumentApp::PrintDocument(
	FSSpec*		inMacFSSpec)
{
		// Create a new document using the file spec.
	CTextDocument*	theDocument = new CTextDocument(this, inMacFSSpec);

		// Tell the document to print.
	theDocument->DoPrint();
}

// ---------------------------------------------------------------------------
//	¥ RegisterClasses								[protected]
// ---------------------------------------------------------------------------
//	To reduce clutter within the Application object's constructor, class
//	registrations appear here in this seperate function for ease of use.

void
CDocumentApp::RegisterClasses()
{
		// Register core PowerPlant classes.
	RegisterClass_(LWindow);
	RegisterClass_(LPrintout);
	RegisterClass_(LPlaceHolder);
	RegisterClass_(LKeyScrollAttachment);
	RegisterClass_(LColorEraseAttachment);
	RegisterClass_(LUndoer);

		// Register the Appearance Manager/GA classes. You may want
		// to remove this use of UControlRegistry and instead perform
		// a "manual" registration of the classes. This cuts down on
		// extra code being linked in and streamlines your app and
		// project. However, use UControlRegistry as a reference/index
		// for your work, and ensure to check UControlRegistry against
		// your registrations each PowerPlant release in case
		// any mappings might have changed.
		
	UControlRegistry::RegisterClasses();

		// Register custom classes
	RegisterClass_(CTextView);
}