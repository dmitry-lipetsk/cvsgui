/**********************************************************************

	--- Qt Architect generated file ---

	File: QUpdate_STICKYData.h
	Last generated: Thu Nov 26 11:42:24 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef QUpdate_STICKYData_included
#define QUpdate_STICKYData_included

#include <qwidget.h>
#include <qchkbox.h>
#include <qcombo.h>

class QUpdate_STICKYData : public QWidget
{
    Q_OBJECT

public:

    QUpdate_STICKYData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QUpdate_STICKYData();

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

#endif // QUpdate_STICKYData_included
