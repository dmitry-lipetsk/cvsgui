/**********************************************************************

	--- Qt Architect generated file ---

	File: QCheckout_MAIN.cpp
	Last generated: Tue Dec 1 17:16:50 1998

 *********************************************************************/

#include "QCheckout_MAIN.h"
#include "qcvsapp.h"

#define Inherited QCheckout_MAINData

QCheckout_MAIN::QCheckout_MAIN
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
}


QCheckout_MAIN::~QCheckout_MAIN()
{
}

void QCheckout_MAIN::DoDataExchange(bool putvalue, CPStr & sLastModName, bool & norecurs, bool & toStdout,
									CMString & oldModules)
{
	::DoDataExchange(putvalue, oldModules, fModule);

	if(putvalue)
	{
		fNorecurs->setChecked(norecurs);
		fStdout->setChecked(toStdout);
		fModule->setEditText(QString(sLastModName));
	}
	else
	{
		norecurs = fNorecurs->isChecked();
		toStdout = fStdout->isChecked();

		sLastModName = fModule->currentText();
	}
}

#include "QCheckout_MAIN.moc"
#include "QCheckout_MAINData.moc"
