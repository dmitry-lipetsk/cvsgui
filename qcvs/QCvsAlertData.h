/**********************************************************************

	--- Qt Architect generated file ---

	File: QCvsAlertData.h
	Last generated: Tue Nov 24 16:04:51 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef QCvsAlertData_included
#define QCvsAlertData_included

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>

class QCvsAlertData : public QDialog
{
    Q_OBJECT

public:

    QCvsAlertData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCvsAlertData();

public slots:


protected slots:


protected:
    QLabel* fText;
    QPushButton* fDef;
    QPushButton* fCancel;

};

#endif // QCvsAlertData_included