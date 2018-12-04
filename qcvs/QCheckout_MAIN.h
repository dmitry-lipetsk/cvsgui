/**********************************************************************

	--- Qt Architect generated file ---

	File: QCheckout_MAIN.h
	Last generated: Tue Dec 1 17:16:50 1998

 *********************************************************************/

#ifndef QCheckout_MAIN_included
#define QCheckout_MAIN_included

#include "QCheckout_MAINData.h"
#include "MultiString.h"

class QCheckout_MAIN : public QCheckout_MAINData
{
    Q_OBJECT

public:

    QCheckout_MAIN
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCheckout_MAIN();

	void DoDataExchange(bool putvalue, CPStr & sLastModName, bool & norecurs, bool & toStdout,
						CMString & oldModules);
};
#endif // QCheckout_MAIN_included
