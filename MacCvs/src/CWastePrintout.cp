//-------------------------------------------------------------------------
//	CWastePrintout.
//	Purpose: This class is an override of LPrintout to use with waste. Waste
//	wants the port set to the printer port just before printing. If this is
//	not done, waste thinks it must still update the port, and ugly drawing takes
//	place during printing. Is there a smarted way, beats me.
//
//	Revision:
//------------------------------------------------------------------------- 

#include "CWastePrintout.h"
#include <LPrintout.h>
#include <LCommander.h>
#include <UReanimator.h>
//-------------------------------------------------------------------------
//	CreatePrintout
//	Purpose: Create a prinout object from a PPob.
//	
//
//
//	Revision: 6/10/96
//-------------------------------------------------------------------------
CWastePrintout *
CWastePrintout::CreatePrintout(ResIDT	inPrintoutID)
{
	
	LCommander::SetDefaultCommander(nil);
	CWastePrintout	*thePrintout =
			(CWastePrintout*) UReanimator::ReadObjects('PPob', inPrintoutID);
	thePrintout->FinishCreate();
	
	return thePrintout;
}

// ---------------------------------------------------------------------------
//		¥ CreatePrintoutStream [static]
// ---------------------------------------------------------------------------
//	Return a new Printout object initialized using data from a Stream

CWastePrintout*
CWastePrintout::CreatePrintoutStream(
	LStream	*inStream)
{
	return (NEW CWastePrintout(inStream));
}

CWastePrintout::CWastePrintout(LStream * inStream)
: LPrintout(inStream), mWasteH(nil)
{
}

//-------------------------------------------------------------------------
//	SetWEHandle
//	Purpose: set the WEHandle that we use to set the printing port before
//	printing
//
//
//	Revision: 6/10/96
//-------------------------------------------------------------------------


void
CWastePrintout::SetWEHandle(WEReference hWE)
{
	mWasteH = hWE;
}


// ---------------------------------------------------------------------------
//		¥ PrintCopiesOfPages
// ---------------------------------------------------------------------------
//	Print copies of the specified range of Panels
//
//	This is the main "print loop"


void
CWastePrintout::PrintCopiesOfPages(
	const PanelSpec	&inFirstPanel,
	const PanelSpec	&inLastPanel,
	UInt16			inCopyCount)
{
	THPrint	printRecordH = GetPrintRecord();
	SInt16	printMgrResFile = CurResFile();
	Boolean firstPage = true;
	
	for (UInt16 copy = 1; copy <= inCopyCount; copy++) {

		// +++ Install pIdle proc

		UseResFile(printMgrResFile);

		mPrinterPort = ::PrOpenDoc(printRecordH, nil , nil);
		OutOfFocus(nil);			// Printer is the current port
		
		PanelSpec	thePanel = inFirstPanel;
		while (thePanel.pageNumber <= inLastPanel.pageNumber &&
				PrError() == noErr) {
				
			::PrOpenPage(mPrinterPort, nil);
			if(firstPage){
				firstPage = false;
				GrafPtr thePort = GetMacPort();
				WESetInfo(wePort, &thePort, mWasteH);
			}

			if (PrError() == noErr) {
			
					// PrOpenPage resets the portRect, but it doesn't
					// reset the visRgn. Since PP calls SetOrigin, the
					// visRgn gets out of synch unless we reset it here.
					
				::RectRgn(mPrinterPort->gPort.visRgn,
						  &mPrinterPort->gPort.portRect);
				PrintPanel(thePanel, nil);
			}

			:: PrClosePage(mPrinterPort);
			
			PageToPanel(++thePanel.pageNumber, thePanel);
		}

		::PrCloseDoc(mPrinterPort);
		mPrinterPort = nil;
	}
	OutOfFocus(nil);				// Printer port is gone

	if (((**printRecordH).prJob.bJDocLoop == bSpoolLoop) &&
		(PrError() == noErr)) {

		TPrStatus	printStatus;
		::PrPicFile(printRecordH, nil, nil, nil, &printStatus);
	}
}