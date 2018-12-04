/**********************************************************************

	--- Qt Architect generated file ---

	File: QImportReportData.cpp
	Last generated: Tue Dec 1 00:15:51 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by qtarch.  All changes will be lost.

 *********************************************************************/

#include <qpixmap.h>
#include <qlayout.h>
#include "QImportReportData.h"

#define Inherited QDialog

#include <qlabel.h>
#include <qpushbt.h>

QImportReportData::QImportReportData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE, 0 )
{
	fGroup = new QButtonGroup( this, "ButtonGroup_1" );
	fGroup->setGeometry( 5, 5, 430, 163 );
	fGroup->setMinimumSize( 0, 0 );
	fGroup->setMaximumSize( 32767, 32767 );
	fGroup->setFocusPolicy( QWidget::NoFocus );
	fGroup->setBackgroundMode( QWidget::PaletteBackground );
	fGroup->setFontPropagation( QWidget::NoChildren );
	fGroup->setPalettePropagation( QWidget::NoChildren );
	fGroup->setFrameStyle( 49 );
	fGroup->setTitle( "Import method :" );
	fGroup->setAlignment( 1 );

	fbut0 = new QRadioButton( fGroup, "RadioButton_1" );
	fbut0->setGeometry( 5, 33, 420, 27 );
	fbut0->setMinimumSize( 0, 0 );
	fbut0->setMaximumSize( 32767, 32767 );
	{
		QFont font( "helvetica", 12, 75, 0 );
		font.setStyleHint( (QFont::StyleHint)0 );
		font.setCharSet( (QFont::CharSet)0 );
		fbut0->setFont( font );
	}
	fbut0->setFocusPolicy( QWidget::TabFocus );
	fbut0->setBackgroundMode( QWidget::PaletteBackground );
	fbut0->setFontPropagation( QWidget::NoChildren );
	fbut0->setPalettePropagation( QWidget::NoChildren );
	fbut0->setText( "Leave unchanged" );
	fbut0->setAutoRepeat( FALSE );
	fbut0->setAutoResize( FALSE );

	fbut1 = new QRadioButton( fGroup, "RadioButton_2" );
	fbut1->setGeometry( 5, 65, 420, 28 );
	fbut1->setMinimumSize( 0, 0 );
	fbut1->setMaximumSize( 32767, 32767 );
	fbut1->setFocusPolicy( QWidget::TabFocus );
	fbut1->setBackgroundMode( QWidget::PaletteBackground );
	fbut1->setFontPropagation( QWidget::NoChildren );
	fbut1->setPalettePropagation( QWidget::NoChildren );
	fbut1->setText( "Force binary" );
	fbut1->setAutoRepeat( FALSE );
	fbut1->setAutoResize( FALSE );

	fbut2 = new QRadioButton( fGroup, "RadioButton_3" );
	fbut2->setGeometry( 5, 98, 420, 27 );
	fbut2->setMinimumSize( 0, 0 );
	fbut2->setMaximumSize( 32767, 32767 );
	fbut2->setFocusPolicy( QWidget::TabFocus );
	fbut2->setBackgroundMode( QWidget::PaletteBackground );
	fbut2->setFontPropagation( QWidget::NoChildren );
	fbut2->setPalettePropagation( QWidget::NoChildren );
	fbut2->setText( "Force text" );
	fbut2->setAutoRepeat( FALSE );
	fbut2->setAutoResize( FALSE );

	fbut3 = new QRadioButton( fGroup, "RadioButton_4" );
	fbut3->setGeometry( 5, 130, 420, 28 );
	fbut3->setMinimumSize( 0, 0 );
	fbut3->setMaximumSize( 32767, 32767 );
	fbut3->setFocusPolicy( QWidget::TabFocus );
	fbut3->setBackgroundMode( QWidget::PaletteBackground );
	fbut3->setFontPropagation( QWidget::NoChildren );
	fbut3->setPalettePropagation( QWidget::NoChildren );
	fbut3->setText( "Ignore" );
	fbut3->setAutoRepeat( FALSE );
	fbut3->setAutoResize( FALSE );

	fConflict = new QMultiLineEdit( this, "MultiLineEdit_1" );
	fConflict->setGeometry( 5, 208, 430, 121 );
	fConflict->setMinimumSize( 0, 0 );
	fConflict->setMaximumSize( 32767, 32767 );
	fConflict->setFocusPolicy( QWidget::StrongFocus );
	fConflict->setBackgroundMode( QWidget::PaletteBase );
	fConflict->setFontPropagation( QWidget::SameFont );
	fConflict->setPalettePropagation( QWidget::SameFont );
	fConflict->insertLine( "" );
	fConflict->setReadOnly( TRUE );
	fConflict->setOverwriteMode( FALSE );

	QLabel* qtarch_Label_1;
	qtarch_Label_1 = new QLabel( this, "Label_1" );
	qtarch_Label_1->setGeometry( 5, 173, 430, 30 );
	qtarch_Label_1->setMinimumSize( 0, 0 );
	qtarch_Label_1->setMaximumSize( 32767, 30 );
	qtarch_Label_1->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_1->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_Label_1->setFontPropagation( QWidget::NoChildren );
	qtarch_Label_1->setPalettePropagation( QWidget::NoChildren );
	qtarch_Label_1->setText( "Conflicts (if any) :" );
	qtarch_Label_1->setAlignment( 1313 );
	qtarch_Label_1->setMargin( -1 );

	QPushButton* qtarch_PushButton_1;
	qtarch_PushButton_1 = new QPushButton( this, "PushButton_1" );
	qtarch_PushButton_1->setGeometry( 329, 340, 106, 30 );
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

	QPushButton* qtarch_PushButton_2;
	qtarch_PushButton_2 = new QPushButton( this, "PushButton_2" );
	qtarch_PushButton_2->setGeometry( 217, 340, 107, 30 );
	qtarch_PushButton_2->setMinimumSize( 0, 0 );
	qtarch_PushButton_2->setMaximumSize( 32767, 30 );
	connect( qtarch_PushButton_2, SIGNAL(clicked()), SLOT(reject()) );
	qtarch_PushButton_2->setFocusPolicy( QWidget::TabFocus );
	qtarch_PushButton_2->setBackgroundMode( QWidget::PaletteBackground );
	qtarch_PushButton_2->setFontPropagation( QWidget::NoChildren );
	qtarch_PushButton_2->setPalettePropagation( QWidget::NoChildren );
	qtarch_PushButton_2->setText( "Cancel" );
	qtarch_PushButton_2->setAutoRepeat( FALSE );
	qtarch_PushButton_2->setAutoResize( FALSE );

	fGroup->insert( fbut0 );
	fGroup->insert( fbut1 );
	fGroup->insert( fbut2 );
	fGroup->insert( fbut3 );

	QBoxLayout* qtarch_layout_1 = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1->addStrut( 0 );
	qtarch_layout_1->addWidget( fGroup, 4, 34 );
	QBoxLayout* qtarch_layout_1_1 = new QBoxLayout( fGroup, QBoxLayout::TopToBottom, 5, 5, NULL );
	qtarch_layout_1_1->addStrut( 0 );
	qtarch_layout_1_1->addStretch( 1 );
	qtarch_layout_1_1->addWidget( fbut0, 1, 36 );
	qtarch_layout_1_1->addWidget( fbut1, 1, 36 );
	qtarch_layout_1_1->addWidget( fbut2, 1, 36 );
	qtarch_layout_1_1->addWidget( fbut3, 1, 36 );
	qtarch_layout_1->addWidget( qtarch_Label_1, 1, 36 );
	qtarch_layout_1->addWidget( fConflict, 3, 36 );
	QBoxLayout* qtarch_layout_1_4 = new QBoxLayout( QBoxLayout::RightToLeft, 5, NULL );
	qtarch_layout_1->addLayout( qtarch_layout_1_4, 1 );
	qtarch_layout_1_4->addStrut( 0 );
	qtarch_layout_1_4->addWidget( qtarch_PushButton_1, 1, 36 );
	qtarch_layout_1_4->addWidget( qtarch_PushButton_2, 1, 36 );
	qtarch_layout_1_4->addStretch( 1 );
	qtarch_layout_1_4->addStretch( 1 );
	resize( 440,380 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


QImportReportData::~QImportReportData()
{
}
