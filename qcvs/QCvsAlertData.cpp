/**********************************************************************

	--- Qt Architect generated file ---

	File: QCvsAlertData.cpp
	Last generated: Tue Nov 24 16:04:51 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include <qpixmap.h>
#include <qlayout.h>
#include "QCvsAlertData.h"

#define Inherited QDialog


QCvsAlertData::QCvsAlertData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE, 118784 )
{
	fText = new QLabel( this, "Label_1" );
	fText->setGeometry( 5, 5, 340, 79 );
	fText->setMinimumSize( 0, 0 );
	fText->setMaximumSize( 32767, 32767 );
	fText->setFocusPolicy( QWidget::NoFocus );
	fText->setBackgroundMode( QWidget::PaletteBackground );
	fText->setFontPropagation( QWidget::NoChildren );
	fText->setPalettePropagation( QWidget::NoChildren );
	fText->setText( "" );
	fText->setAlignment( 1313 );
	fText->setMargin( -1 );

	fDef = new QPushButton( this, "PushButton_1" );
	fDef->setGeometry( 233, 89, 112, 26 );
	fDef->setMinimumSize( 0, 0 );
	fDef->setMaximumSize( 32767, 32767 );
	connect( fDef, SIGNAL(clicked()), SLOT(accept()) );
	fDef->setFocusPolicy( QWidget::TabFocus );
	fDef->setBackgroundMode( QWidget::PaletteBackground );
	fDef->setFontPropagation( QWidget::NoChildren );
	fDef->setPalettePropagation( QWidget::NoChildren );
	fDef->setText( "" );
	fDef->setAutoRepeat( FALSE );
	fDef->setAutoResize( FALSE );
	fDef->setAutoDefault( TRUE );

	fCancel = new QPushButton( this, "PushButton_2" );
	fCancel->setGeometry( 117, 89, 111, 26 );
	fCancel->setMinimumSize( 0, 0 );
	fCancel->setMaximumSize( 32767, 32767 );
	connect( fCancel, SIGNAL(clicked()), SLOT(reject()) );
	fCancel->setFocusPolicy( QWidget::TabFocus );
	fCancel->setBackgroundMode( QWidget::PaletteBackground );
	fCancel->setFontPropagation( QWidget::NoChildren );
	fCancel->setPalettePropagation( QWidget::NoChildren );
	fCancel->setText( "" );
	fCancel->setAutoRepeat( FALSE );
	fCancel->setAutoResize( FALSE );

	QBoxLayout* qtarch_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1->addStrut( 0 );
	qtarch_layout_1->addWidget( fText, 3, 36 );
	QBoxLayout* qtarch_layout_1_2 = new QBoxLayout( QBoxLayout::LeftToRight, 5, NULL );
	qtarch_layout_1->addLayout( qtarch_layout_1_2, 1 );
	qtarch_layout_1_2->addStrut( 0 );
	qtarch_layout_1_2->addStretch( 1 );
	qtarch_layout_1_2->addWidget( fCancel, 1, 36 );
	qtarch_layout_1_2->addWidget( fDef, 1, 36 );
	resize( 350,120 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


QCvsAlertData::~QCvsAlertData()
{
}