
#include <UReanimator.h>
#include <URegistrar.h>
#include <LScrollerView.h>
#include <LScrollBar.h>
#include <LCheckBox.h>
#include <LAMTrackActionImp.h>
#include <LBevelButton.h>
#include <LAMBevelButtonImp.h>
#include <LGABevelButtonImp.h>
#include <LWindowHeader.h>
#include <LAMWindowHeaderImp.h>
#include <LGAWindowHeaderImp.h>
#include <LCmdBevelButton.h>
#include <LPushButton.h>
#include <LAMPushButtonImp.h>
#include <LGAPushButtonImp.h>
#include <LEditText.h>
#include <LAMEditTextImp.h>
#include <LGAEditTextImp.h>
#include <LStaticText.h>
#include <LAMStaticTextImp.h>
#include <LGAStaticTextImp.h>
#include <LGADialog.h>
#include <LGACheckBoxImp.h>
#include <UEnvironment.h>
#include <UAttachments.h>
#include <LPrintout.h>
#include <LPlaceHolder.h>
//#include <LGAColorSwatchControl.h>
//#include <LSeparatorLine.h>
//#include <LGASeparatorImp.h>

#include "NThisAppRegistry.h"
#include "WTextView.h"
// ---------------------------------------------------------------------------
//	¥ NThisAppRegistry
// ---------------------------------------------------------------------------
//
// Purpose: This namespace registers all the classes that will be
// used in the app. 
//
// Revision:
//



void
NThisAppRegistry::RegisterClassesUsed()
{
	// Register PowerPlant class creator functions.
	RegisterClass_(LWindow);
	RegisterClass_(LWindowThemeAttachment);
	RegisterClass_(LCheckBox);
	RegisterClass_(LKeyScrollAttachment);
	RegisterClass_(LPrintout);
	RegisterClass_(LPlaceHolder);
	//RegisterClass_(LCMAttachment);
	//RegisterClass_(LDialogBox);
	
	RegisterClass_(WTextView);
	//UControlRegistry::RegisterClasses();
	UEnvironment::InitEnvironment();
	RegisterClass_(LBevelButton);
	RegisterClass_(LCmdBevelButton);
	RegisterClass_(LWindowHeader);
	RegisterClass_(LControlView);
	RegisterClass_(LEditText);
	RegisterClass_(LStaticText);
	RegisterClass_(LPushButton);
	RegisterClass_(LGADialog);
	RegisterClass_(LScrollerView);
	RegisterClass_(LScrollBar);
	//RegisterClass_(LGAColorSwatchControl);
	//RegisterClass_(LSeparatorLine);
	RegisterClass_(LCheckBox);
	if (UEnvironment::HasFeature (env_HasAppearance)) {
		RegisterClassID_(LAMBevelButtonImp,   LBevelButton::imp_class_ID);
		RegisterClassID_(LAMWindowHeaderImp,  LWindowHeader::imp_class_ID);
		RegisterClassID_(LAMEditTextImp,  LEditText::imp_class_ID);
		RegisterClassID_(LAMTrackActionImp, LScrollBar::imp_class_ID);
		RegisterClassID_(LAMStaticTextImp,  LStaticText::imp_class_ID);
		RegisterClassID_(LAMPushButtonImp,	  LPushButton::imp_class_ID);
		RegisterClassID_(LAMControlImp, LCheckBox::imp_class_ID);
		//RegisterClassID_(LAMControlImp, LSeparatorLine::imp_class_ID);
	
	} else {
		RegisterClassID_(LGABevelButtonImp, LBevelButton::imp_class_ID );
		RegisterClassID_(LGAWindowHeaderImp, LWindowHeader::imp_class_ID );
		RegisterClassID_(LGAEditTextImp,  LEditText::imp_class_ID);
		RegisterClassID_(LGAStaticTextImp,  LStaticText::imp_class_ID);
		RegisterClassID_(LGAPushButtonImp,	LPushButton::imp_class_ID);
		RegisterClassID_( LGACheckBoxImp,	LCheckBox::imp_class_ID );
		//RegisterClassID_( LGASeparatorImp,	LSeparatorLine::imp_class_ID );
	}
}
