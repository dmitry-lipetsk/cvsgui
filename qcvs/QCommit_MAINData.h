/**********************************************************************

	--- Qt Architect generated file ---

	File: QCommit_MAINData.h
	Last generated: Sat Nov 28 20:44:24 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef QCommit_MAINData_included
#define QCommit_MAINData_included

#include <qwidget.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qmlined.h>

class QCommit_MAINData : public QWidget
{
    Q_OBJECT

public:

    QCommit_MAINData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QCommit_MAINData();

public slots:


protected slots:

    virtual void OnCombo(int);

protected:
    QMultiLineEdit* fLog;
    QComboBox* fCombo;
    QCheckBox* fNorecurs;

};

#endif // QCommit_MAINData_included
