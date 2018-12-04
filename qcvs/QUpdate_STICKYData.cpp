/**********************************************************************

	--- Qt Architect generated file ---

	File: QUpdate_STICKYData.cpp
	Last generated: Thu Nov 26 11:42:24 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include <qpixmap.h>
#include <qlayout.h>
#include "QUpdate_STICKYData.h"

#define Inherited QWidget

#include <qlabel.h>

QUpdate_STICKYData::QUpdate_STICKYData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, 0 )
{
	QLabel* qtarch_Label_1;
	qtarch_Label_1 = new QLabel( this, "Label_1" );
	qtarch_Label_1->setGeometry( 5, 5, 510, 30 );
	qtarch_Label_1->setMinimumSize( 0, 0 );
	qtarch_Label_1->setMaximumSize( 32767, 30 );
	qtarch_Label_1->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_1->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_1->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_1->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_1->setText( "Sticky (persistents) options :" );
	qtarch_Label_1->setAlignment( 289 );
	qtarch_Label_1->setMargin( -1 );

	fDateCheck = new QCheckBox( this, "CheckBox_1" );
	fDateCheck->setGeometry( 5, 69, 253, 30 );
	fDateCheck->setMinimumSize( 0, 0 );
	fDateCheck->setMaximumSize( 32767, 30 );
	connect( fDateCheck, SIGNAL(toggled(bool)), SLOT(OnDateCheck(bool)) );
	fDateCheck->setFocusPolicy( QWidget::TabFocus );
	fDateCheck->setBackgroundMode( QWidget::PaletteBackground );
	fDateCheck->setFontPropagation( QWidget::NoChildren );
	fDateCheck->setPalettePropagation( QWidget::NoChildren );
	fDateCheck->setText( "Before date :" );
	fDateCheck->setAutoRepeat( FALSE );
	fDateCheck->setAutoResize( FALSE );

	fDate = new QComboBox( TRUE, this, "ComboBox_1" );
	fDate->setGeometry( 263, 69, 252, 30 );
	fDate->setMinimumSize( 0, 0 );
	fDate->setMaximumSize( 32767, 30 );
	fDate->setFocusPolicy( QWidget::StrongFocus );
	fDate->setBackgroundMode( QWidget::PaletteBackground );
	fDate->setFontPropagation( QWidget::NoChildren );
	fDate->setPalettePropagation( QWidget::NoChildren );
	fDate->setInsertionPolicy( QComboBox::AtBottom );
	fDate->setSizeLimit( 10 );
	fDate->setAutoResize( FALSE );

	fRevCheck = new QCheckBox( this, "CheckBox_2" );
	fRevCheck->setGeometry( 5, 162, 253, 30 );
	fRevCheck->setMinimumSize( 0, 0 );
	fRevCheck->setMaximumSize( 32767, 30 );
	connect( fRevCheck, SIGNAL(toggled(bool)), SLOT(OnRevCheck(bool)) );
	fRevCheck->setFocusPolicy( QWidget::TabFocus );
	fRevCheck->setBackgroundMode( QWidget::PaletteBackground );
	fRevCheck->setFontPropagation( QWidget::NoChildren );
	fRevCheck->setPalettePropagation( QWidget::NoChildren );
	fRevCheck->setText( "Retrieve rev./tag/branch :" );
	fRevCheck->setAutoRepeat( FALSE );
	fRevCheck->setAutoResize( FALSE );

	fRev = new QComboBox( TRUE, this, "ComboBox_2" );
	fRev->setGeometry( 263, 162, 252, 30 );
	fRev->setMinimumSize( 0, 0 );
	fRev->setMaximumSize( 32767, 30 );
	fRev->setFocusPolicy( QWidget::StrongFocus );
	fRev->setBackgroundMode( QWidget::PaletteBackground );
	fRev->setFontPropagation( QWidget::NoChildren );
	fRev->setPalettePropagation( QWidget::NoChildren );
	fRev->setInsertionPolicy( QComboBox::AtBottom );
	fRev->setSizeLimit( 10 );
	fRev->setAutoResize( FALSE );

	fRecent = new QCheckBox( this, "CheckBox_3" );
	fRecent->setGeometry( 5, 225, 510, 30 );
	fRecent->setMinimumSize( 0, 0 );
	fRecent->setMaximumSize( 32767, 30 );
	fRecent->setFocusPolicy( QWidget::TabFocus );
	fRecent->setBackgroundMode( QWidget::PaletteBackground );
	fRecent->setFontPropagation( QWidget::NoChildren );
	fRecent->setPalettePropagation( QWidget::NoChildren );
	fRecent->setText( "If no matching revision is found, use the most recent one" );
	fRecent->setAutoRepeat( FALSE );
	fRecent->setAutoResize( FALSE );

	QBoxLayout* qtarch_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1->addStrut( 0 );
	qtarch_layout_1->addWidget( qtarch_Label_1, 1, 36 );
	QBoxLayout* qtarch_layout_1_2 = new QBoxLayout( QBoxLayout::LeftToRight, 5, NULL );
	qtarch_layout_1->addLayout( qtarch_layout_1_2, 1 );
	qtarch_layout_1_2->addStrut( 0 );
	qtarch_layout_1_2->addWidget( fDateCheck, 1, 36 );
	qtarch_layout_1_2->addWidget( fDate, 1, 36 );
	QBoxLayout* qtarch_layout_1_3 = new QBoxLayout( QBoxLayout::LeftToRight, 5, NULL );
	qtarch_layout_1->addLayout( qtarch_layout_1_3, 1 );
	qtarch_layout_1_3->addStrut( 0 );
	qtarch_layout_1_3->addWidget( fRevCheck, 1, 36 );
	qtarch_layout_1_3->addWidget( fRev, 1, 36 );
	qtarch_layout_1->addWidget( fRecent, 1, 36 );
	resize( 520,260 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


QUpdate_STICKYData::~QUpdate_STICKYData()
{
}
void QUpdate_STICKYData::OnDateCheck(bool)
{
}
void QUpdate_STICKYData::OnRevCheck(bool)
{
}
