/**********************************************************************

	--- Qt Architect generated file ---

	File: QImportFilterData.cpp
	Last generated: Mon Nov 30 22:41:41 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include <qpixmap.h>
#include <qlayout.h>
#include "QImportFilterData.h"

#define Inherited QDialog

#include <qpushbt.h>

QImportFilterData::QImportFilterData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE, 0 )
{
	fList = new QListView( this, "ListView_1" );
	fList->setGeometry( 5, 5, 510, 204 );
	fList->setMinimumSize( 0, 0 );
	fList->setMaximumSize( 32767, 32767 );
	connect( fList, SIGNAL(doubleClicked(QListViewItem*)), SLOT(OnEdit(QListViewItem*)) );
	connect( fList, SIGNAL(returnPressed(QListViewItem*)), SLOT(OnEdit(QListViewItem*)) );
	connect( fList, SIGNAL(selectionChanged(QListViewItem*)), SLOT(OnSelChanged(QListViewItem*)) );
	fList->setFocusPolicy( QWidget::TabFocus );
	fList->setBackgroundMode( QWidget::PaletteBackground );
	fList->setFontPropagation( QWidget::NoChildren );
	fList->setPalettePropagation( QWidget::NoChildren );
	fList->setResizePolicy( QScrollView::Manual );
	fList->setVScrollBarMode( QScrollView::Auto );
	fList->setHScrollBarMode( QScrollView::Auto );
	fList->setTreeStepSize( 20 );
	fList->setMultiSelection( FALSE );
	fList->setAllColumnsShowFocus( FALSE );
	fList->setItemMargin( 1 );
	fList->setRootIsDecorated( FALSE );
	fList->addColumn( "Kind", -1 );
	fList->setColumnWidthMode( 0, QListView::Maximum );
	fList->setColumnAlignment( 0, 1 );
	fList->addColumn( "Description", -1 );
	fList->setColumnWidthMode( 1, QListView::Maximum );
	fList->setColumnAlignment( 1, 1 );
	fList->addColumn( "State", -1 );
	fList->setColumnWidthMode( 2, QListView::Maximum );
	fList->setColumnAlignment( 2, 1 );

	QPushButton* qtarch_PushButton_1;
	qtarch_PushButton_1 = new QPushButton( this, "PushButton_1" );
	qtarch_PushButton_1->setGeometry( 391, 220, 124, 30 );
	qtarch_PushButton_1->setMinimumSize( 0, 0 );
	qtarch_PushButton_1->setMaximumSize( 32767, 30 );
	connect( qtarch_PushButton_1, SIGNAL(clicked()), SLOT(accept()) );
	qtarch_PushButton_1->setFocusPolicy( QWidget::TabFocus );
	qtarch_PushButton_1->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_PushButton_1->setFontPropagation( QWidget::NoChildren );
	qtarch_PushButton_1->setPalettePropagation( QWidget::NoChildren );
	qtarch_PushButton_1->setText( "OK" );
	qtarch_PushButton_1->setAutoRepeat( FALSE );
	qtarch_PushButton_1->setAutoResize( FALSE );
	qtarch_PushButton_1->setAutoDefault( TRUE );

	QPushButton* qtarch_PushButton_4;
	qtarch_PushButton_4 = new QPushButton( this, "PushButton_4" );
	qtarch_PushButton_4->setGeometry( 263, 220, 123, 30 );
	qtarch_PushButton_4->setMinimumSize( 0, 0 );
	qtarch_PushButton_4->setMaximumSize( 32767, 30 );
	connect( qtarch_PushButton_4, SIGNAL(clicked()), SLOT(reject()) );
	qtarch_PushButton_4->setFocusPolicy( QWidget::TabFocus );
	qtarch_PushButton_4->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_PushButton_4->setFontPropagation( QWidget::NoChildren );
	qtarch_PushButton_4->setPalettePropagation( QWidget::NoChildren );
	qtarch_PushButton_4->setText( "Cancel" );
	qtarch_PushButton_4->setAutoRepeat( FALSE );
	qtarch_PushButton_4->setAutoResize( FALSE );

	fEdit = new QPushButton( this, "PushButton_5" );
	fEdit->setGeometry( 5, 220, 124, 30 );
	fEdit->setMinimumSize( 0, 0 );
	fEdit->setMaximumSize( 32767, 30 );
	connect( fEdit, SIGNAL(clicked()), SLOT(OnEdit()) );
	fEdit->setFocusPolicy( QWidget::TabFocus );
	fEdit->setBackgroundMode( QWidget::PaletteBackground );
	fEdit->setFontPropagation( QWidget::NoChildren );
	fEdit->setPalettePropagation( QWidget::NoChildren );
	fEdit->setText( "Edit" );
	fEdit->setAutoRepeat( FALSE );
	fEdit->setAutoResize( FALSE );

	QBoxLayout* qtarch_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1->addStrut( 0 );
	qtarch_layout_1->addWidget( fList, 5, 36 );
	QGridLayout* qtarch_layout_1_2 = new QGridLayout( 1, 4, 5, NULL );
	qtarch_layout_1->addLayout( qtarch_layout_1_2, 1 );
	qtarch_layout_1_2->addColSpacing( 0, 5 );
	qtarch_layout_1_2->setColStretch( 0, 1 );
	qtarch_layout_1_2->addColSpacing( 1, 5 );
	qtarch_layout_1_2->setColStretch( 1, 1 );
	qtarch_layout_1_2->addColSpacing( 2, 5 );
	qtarch_layout_1_2->setColStretch( 2, 1 );
	qtarch_layout_1_2->addColSpacing( 3, 5 );
	qtarch_layout_1_2->setColStretch( 3, 1 );
	qtarch_layout_1_2->addRowSpacing( 0, 0 );
	qtarch_layout_1_2->setRowStretch( 0, 1 );
	qtarch_layout_1_2->addWidget( fEdit, 0, 0, 36 );
	qtarch_layout_1_2->addWidget( qtarch_PushButton_4, 0, 2, 36 );
	qtarch_layout_1_2->addWidget( qtarch_PushButton_1, 0, 3, 36 );
	resize( 520,260 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


QImportFilterData::~QImportFilterData()
{
}
void QImportFilterData::OnEdit(QListViewItem*)
{
}
void QImportFilterData::OnSelChanged(QListViewItem*)
{
}
void QImportFilterData::OnEdit()
{
}