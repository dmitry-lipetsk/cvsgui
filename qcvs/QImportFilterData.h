/**********************************************************************

	--- Qt Architect generated file ---

	File: QImportFilterData.h
	Last generated: Mon Nov 30 22:41:41 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#ifndef QImportFilterData_included
#define QImportFilterData_included

#include <qdialog.h>
#include <qlistview.h>
#include <qpushbt.h>

class QImportFilterData : public QDialog
{
    Q_OBJECT

public:

    QImportFilterData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~QImportFilterData();

public slots:


protected slots:

    virtual void OnEdit();
    virtual void OnEdit(QListViewItem*);
    virtual void OnSelChanged(QListViewItem*);

protected:
    QListView* fList;
    QPushButton* fEdit;

};

#endif // QImportFilterData_included
