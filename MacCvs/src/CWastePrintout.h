#include "waste.h"
#include 	<LPrintout.h>

class CWastePrintout : public LPrintout {
	
public:
	enum {class_ID = 'WSPO'};
	
	static CWastePrintout*	CreatePrintout(ResIDT	inPrintoutID);
	
	static CWastePrintout*	CreatePrintoutStream(LStream *inStream);
	
			CWastePrintout(LStream *inStream);
	
	virtual void	PrintCopiesOfPages(
							const PanelSpec	&inFirstPanel,
							const PanelSpec	&inLastPanel,
							UInt16			inCopyCount);

	void			SetWEHandle(WEReference hWE);

protected:
	WEReference		mWasteH;
};