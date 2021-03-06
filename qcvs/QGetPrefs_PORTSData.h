/**********************************************************************

	--- Qt Architect generated file ---

	File: QGetPrefs_PORTSData.h
	Last generated: Mon Nov 23 23:14:25 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef QGetPrefs_PORTSData_included
#define QGetPrefs_PORTSData_included

#include <qdialog.h>
#include <qchkbox.h>
#include <qlined.h>

class QGetPrefs_PORTSData : public QDialog
{
    Q_OBJECT

public:

    QGetPrefs_PORTSData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QGetPrefs_PORTSData();

public slots:


protected slots:

    virtual void ToggleRshName(bool);
    virtual void ToggleShellPort(bool);
    virtual void ToggleKerberosPort(bool);
    virtual void TogglePserverPort(bool);
    virtual void ToggleServerName(bool);

protected:
    QCheckBox* fShellPortCheck;
    QLineEdit* fShellPort;
    QCheckBox* fPserverPortCheck;
    QLineEdit* fServerPort;
    QCheckBox* fKerberosPortCheck;
    QLineEdit* fKerberosPort;
    QCheckBox* fAltServerCheck;
    QLineEdit* fServerName;
    QCheckBox* fAltRshCheck;
    QLineEdit* fRshName;

};

#endif // QGetPrefs_PORTSData_included
