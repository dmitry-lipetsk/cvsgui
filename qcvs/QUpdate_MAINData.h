/**********************************************************************

	--- Qt Architect generated file ---

	File: QUpdate_MAINData.h
	Last generated: Tue Nov 24 16:25:12 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef QUpdate_MAINData_included
#define QUpdate_MAINData_included

#include <qwidget.h>
#include <qchkbox.h>

class QUpdate_MAINData : public QWidget
{
    Q_OBJECT

public:

    QUpdate_MAINData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QUpdate_MAINData();

public slots:


protected slots:


protected:
    QCheckBox* fStdout;
    QCheckBox* fNorecurs;
    QCheckBox* fResetSticky;
    QCheckBox* fCreateMissing;

};

#endif // QUpdate_MAINData_included