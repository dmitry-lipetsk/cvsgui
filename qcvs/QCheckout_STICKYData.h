/**********************************************************************

	--- Qt Architect generated file ---

	File: QCheckout_STICKYData.h
	Last generated: Thu Nov 26 11:42:24 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef QCheckout_STICKYData_included
#define QCheckout_STICKYData_included

#include <qwidget.h>
#include <qchkbox.h>
#include <qcombo.h>

class QCheckout_STICKYData : public QWidget
{
    Q_OBJECT

public:

    QCheckout_STICKYData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCheckout_STICKYData();

public slots:


protected slots:

    virtual void OnDateCheck(bool);
    virtual void OnRevCheck(bool);

protected:
    QCheckBox* fDateCheck;
    QComboBox* fDate;
    QCheckBox* fRevCheck;
    QComboBox* fRev;
    QCheckBox* fRecent;

};

#endif // QCheckout_STICKYData_included
