/**********************************************************************

	--- Qt Architect generated file ---

	File: QUpdate_MAINData.cpp
	Last generated: Tue Nov 24 16:25:12 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include <qpixmap.h>
#include <qlayout.h>
#include "QUpdate_MAINData.h"

#define Inherited QWidget


QUpdate_MAINData::QUpdate_MAINData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, 0 )
{
	fStdout = new QCheckBox( this, "CheckBox_1" );
	fStdout->setGeometry( 5, 5, 510, 30 );
	fStdout->setMinimumSize( 0, 0 );
	fStdout->setMaximumSize( 32767, 30 );
	fStdout->setFocusPolicy( QWidget::TabFocus );
	fStdout->setBackgroundMode( QWidget::PaletteBackground );
	fStdout->setFontPropagation( QWidget::NoChildren );
	fStdout->setPalettePropagation( QWidget::NoChildren );
	fStdout->setText( "Check-out files to standard output (avoids stickiness)" );
	fStdout->setAutoRepeat( FALSE );
	fStdout->setAutoResize( FALSE );

	fNorecurs = new QCheckBox( this, "CheckBox_5" );
	fNorecurs->setGeometry( 5, 40, 510, 30 );
	fNorecurs->setMinimumSize( 0, 0 );
	fNorecurs->setMaximumSize( 32767, 30 );
	fNorecurs->setFocusPolicy( QWidget::TabFocus );
	fNorecurs->setBackgroundMode( QWidget::PaletteBackground );
	fNorecurs->setFontPropagation( QWidget::NoChildren );
	fNorecurs->setPalettePropagation( QWidget::NoChildren );
	fNorecurs->setText( "Do not recurse into sub-folders" );
	fNorecurs->setAutoRepeat( FALSE );
	fNorecurs->setAutoResize( FALSE );

	fResetSticky = new QCheckBox( this, "CheckBox_6" );
	fResetSticky->setGeometry( 5, 75, 510, 30 );
	fResetSticky->setMinimumSize( 0, 0 );
	fResetSticky->setMaximumSize( 32767, 30 );
	fResetSticky->setFocusPolicy( QWidget::TabFocus );
	fResetSticky->setBackgroundMode( QWidget::PaletteBackground );
	fResetSticky->setFontPropagation( QWidget::NoChildren );
	fResetSticky->setPalettePropagation( QWidget::NoChildren );
	fResetSticky->setText( "Reset any sticky date/rev/""-k"" options" );
	fResetSticky->setAutoRepeat( FALSE );
	fResetSticky->setAutoResize( FALSE );

	fCreateMissing = new QCheckBox( this, "CheckBox_7" );
	fCreateMissing->setGeometry( 5, 110, 510, 30 );
	fCreateMissing->setMinimumSize( 0, 0 );
	fCreateMissing->setMaximumSize( 32767, 30 );
	fCreateMissing->setFocusPolicy( QWidget::TabFocus );
	fCreateMissing->setBackgroundMode( QWidget::PaletteBackground );
	fCreateMissing->setFontPropagation( QWidget::NoChildren );
	fCreateMissing->setPalettePropagation( QWidget::NoChildren );
	fCreateMissing->setText( "Create missing directories that exist into the repository" );
	fCreateMissing->setAutoRepeat( FALSE );
	fCreateMissing->setAutoResize( FALSE );

	QBoxLayout* qtarch_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1->addStrut( 0 );
	qtarch_layout_1->addWidget( fStdout, 1, 36 );
	qtarch_layout_1->addWidget( fNorecurs, 1, 36 );
	qtarch_layout_1->addWidget( fResetSticky, 1, 36 );
	qtarch_layout_1->addWidget( fCreateMissing, 1, 36 );
	resize( 520,260 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


QUpdate_MAINData::~QUpdate_MAINData()
{
}
