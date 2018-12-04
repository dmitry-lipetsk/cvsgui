/**********************************************************************

	--- Qt Architect generated file ---

	File: QImport_MAINData.cpp
	Last generated: Tue Dec 1 14:39:25 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include <qpixmap.h>
#include <qlayout.h>
#include "QImport_MAINData.h"

#define Inherited QWidget

#include <qlabel.h>
#include <qbttngrp.h>

QImport_MAINData::QImport_MAINData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, 0 )
{
	QButtonGroup* qtarch_ButtonGroup_3;
	qtarch_ButtonGroup_3 = new QButtonGroup( this, "ButtonGroup_3" );
	qtarch_ButtonGroup_3->setGeometry( 5, 201, 510, 54 );
	qtarch_ButtonGroup_3->setMinimumSize( 25, 10 );
	qtarch_ButtonGroup_3->setMaximumSize( 32767, 32767 );
	qtarch_ButtonGroup_3->setFocusPolicy( QWidget::NoFocus );
	qtarch_ButtonGroup_3->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_ButtonGroup_3->setFontPropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_3->setPalettePropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_3->setFrameStyle( 49 );
	qtarch_ButtonGroup_3->setTitle( "Initial branch and tag :" );
	qtarch_ButtonGroup_3->setAlignment( 1 );

	QButtonGroup* qtarch_ButtonGroup_2;
	qtarch_ButtonGroup_2 = new QButtonGroup( this, "ButtonGroup_2" );
	qtarch_ButtonGroup_2->setGeometry( 5, 65, 510, 131 );
	qtarch_ButtonGroup_2->setMinimumSize( 0, 0 );
	qtarch_ButtonGroup_2->setMaximumSize( 32767, 32767 );
	qtarch_ButtonGroup_2->setFocusPolicy( QWidget::NoFocus );
	qtarch_ButtonGroup_2->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_ButtonGroup_2->setFontPropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_2->setPalettePropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_2->setFrameStyle( 49 );
	qtarch_ButtonGroup_2->setTitle( "Enter the log message :" );
	qtarch_ButtonGroup_2->setAlignment( 1 );

	QButtonGroup* qtarch_ButtonGroup_1;
	qtarch_ButtonGroup_1 = new QButtonGroup( this, "ButtonGroup_1" );
	qtarch_ButtonGroup_1->setGeometry( 5, 5, 510, 55 );
	qtarch_ButtonGroup_1->setMinimumSize( 10, 25 );
	qtarch_ButtonGroup_1->setMaximumSize( 32767, 55 );
	qtarch_ButtonGroup_1->setFocusPolicy( QWidget::NoFocus );
	qtarch_ButtonGroup_1->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_ButtonGroup_1->setFontPropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_1->setPalettePropagation( QWidget::NoChildren );
	qtarch_ButtonGroup_1->setFrameStyle( 49 );
	qtarch_ButtonGroup_1->setTitle( "Module name to import or update on the server :" );
	qtarch_ButtonGroup_1->setAlignment( 1 );

	fModule = new QLineEdit( qtarch_ButtonGroup_1, "LineEdit_1" );
	fModule->setGeometry( 5, 20, 500, 30 );
	fModule->setMinimumSize( 0, 0 );
	fModule->setMaximumSize( 32767, 30 );
	fModule->setFocusPolicy( QWidget::StrongFocus );
	fModule->setBackgroundMode( QWidget::PaletteBase );
	fModule->setFontPropagation( QWidget::NoChildren );
	fModule->setPalettePropagation( QWidget::NoChildren );
	fModule->setText( "" );
	fModule->setMaxLength( 32767 );
	fModule->setEchoMode( QLineEdit::Normal );
	fModule->setFrame( TRUE );

	fLog = new QMultiLineEdit( qtarch_ButtonGroup_2, "MultiLineEdit_1" );
	fLog->setGeometry( 5, 20, 500, 106 );
	fLog->setMinimumSize( 0, 0 );
	fLog->setMaximumSize( 32767, 32767 );
	fLog->setFocusPolicy( QWidget::StrongFocus );
	fLog->setBackgroundMode( QWidget::PaletteBase );
	fLog->setFontPropagation( QWidget::SameFont );
	fLog->setPalettePropagation( QWidget::SameFont );
	fLog->insertLine( "" );
	fLog->setReadOnly( FALSE );
	fLog->setOverwriteMode( FALSE );

	QLabel* qtarch_Label_1;
	qtarch_Label_1 = new QLabel( qtarch_ButtonGroup_3, "Label_1" );
	qtarch_Label_1->setGeometry( 5, 20, 121, 29 );
	qtarch_Label_1->setMinimumSize( 0, 0 );
	qtarch_Label_1->setMaximumSize( 32767, 30 );
	qtarch_Label_1->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_1->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_1->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_1->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_1->setText( "Vendor branch :" );
	qtarch_Label_1->setAlignment( 289 );
	qtarch_Label_1->setMargin( -1 );

	fVendor = new QLineEdit( qtarch_ButtonGroup_3, "LineEdit_2" );
	fVendor->setGeometry( 131, 20, 122, 29 );
	fVendor->setMinimumSize( 0, 0 );
	fVendor->setMaximumSize( 32767, 30 );
	fVendor->setFocusPolicy( QWidget::StrongFocus );
	fVendor->setBackgroundMode( QWidget::PaletteBase );
	fVendor->setFontPropagation( QWidget::NoChildren );
	fVendor->setPalettePropagation( QWidget::NoChildren );
	fVendor->setText( "" );
	fVendor->setMaxLength( 32767 );
	fVendor->setEchoMode( QLineEdit::Normal );
	fVendor->setFrame( TRUE );

	QLabel* qtarch_Label_2;
	qtarch_Label_2 = new QLabel( qtarch_ButtonGroup_3, "Label_2" );
	qtarch_Label_2->setGeometry( 258, 20, 121, 29 );
	qtarch_Label_2->setMinimumSize( 0, 0 );
	qtarch_Label_2->setMaximumSize( 32767, 30 );
	qtarch_Label_2->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_2->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_2->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_2->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_2->setText( "Release tag :" );
	qtarch_Label_2->setAlignment( 289 );
	qtarch_Label_2->setMargin( -1 );

	fRelease = new QLineEdit( qtarch_ButtonGroup_3, "LineEdit_3" );
	fRelease->setGeometry( 384, 20, 121, 29 );
	fRelease->setMinimumSize( 0, 0 );
	fRelease->setMaximumSize( 32767, 30 );
	fRelease->setFocusPolicy( QWidget::StrongFocus );
	fRelease->setBackgroundMode( QWidget::PaletteBase );
	fRelease->setFontPropagation( QWidget::NoChildren );
	fRelease->setPalettePropagation( QWidget::NoChildren );
	fRelease->setText( "" );
	fRelease->setMaxLength( 32767 );
	fRelease->setEchoMode( QLineEdit::Normal );
	fRelease->setFrame( TRUE );




	QBoxLayout* qtarch_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1->addStrut( 0 );
	qtarch_layout_1->addWidget( qtarch_ButtonGroup_1, 1, 36 );
	QBoxLayout* qtarch_layout_1_1 = new QBoxLayout( qtarch_ButtonGroup_1, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1_1->addStrut( 0 );
	qtarch_layout_1_1->addSpacing( 15 );
	qtarch_layout_1_1->addWidget( fModule, 1, 36 );
	qtarch_layout_1->addWidget( qtarch_ButtonGroup_2, 3, 36 );
	QBoxLayout* qtarch_layout_1_2 = new QBoxLayout( qtarch_ButtonGroup_2, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1_2->addStrut( 0 );
	qtarch_layout_1_2->addSpacing( 15 );
	qtarch_layout_1_2->addWidget( fLog, 1, 36 );
	qtarch_layout_1->addWidget( qtarch_ButtonGroup_3, 1, 36 );
	QBoxLayout* qtarch_layout_1_3 = new QBoxLayout( qtarch_ButtonGroup_3, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1_3->addStrut( 0 );
	qtarch_layout_1_3->addSpacing( 15 );
	QBoxLayout* qtarch_layout_1_3_2 = new QBoxLayout( QBoxLayout::LeftToRight, 5, NULL );
	qtarch_layout_1_3->addLayout( qtarch_layout_1_3_2, 1 );
	qtarch_layout_1_3_2->addStrut( 0 );
	qtarch_layout_1_3_2->addWidget( qtarch_Label_1, 1, 36 );
	qtarch_layout_1_3_2->addWidget( fVendor, 1, 36 );
	qtarch_layout_1_3_2->addWidget( qtarch_Label_2, 1, 36 );
	qtarch_layout_1_3_2->addWidget( fRelease, 1, 36 );
	resize( 520,260 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


QImport_MAINData::~QImport_MAINData()
{
}