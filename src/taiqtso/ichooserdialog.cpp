// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
/****************************************************************************
** Form implementation generated from reading ui file 'iChooserDialog.ui'
**
** Created: Mon Jul 26 14:44:52 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "iChooserDialog.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a iChooserDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
iChooserDialog::iChooserDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "iChooserDialog" );
    setSizeGripEnabled( TRUE );

    QWidget* privateLayoutWidget = new QWidget( this, "layOuter" );
    privateLayoutWidget->setGeometry( QRect( 0, 0, 600, 470 ) );
    layOuter = new QHBoxLayout( privateLayoutWidget, 12, 6, "layOuter");

    lvwItems = new QListView( privateLayoutWidget, "lvwItems" );
    lvwItems->addColumn( tr( "Column 1" ) );
    layOuter->addWidget( lvwItems );
    spacer3 = new QSpacerItem( 12, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layOuter->addItem( spacer3 );

    layButtons = new QVBoxLayout( 0, 0, 6, "layButtons");

    btnOk = new QPushButton( privateLayoutWidget, "btnOk" );
    btnOk->setAutoDefault( TRUE );
    btnOk->setDefault( TRUE );
    layButtons->addWidget( btnOk );

    btnCancel = new QPushButton( privateLayoutWidget, "btnCancel" );
    btnCancel->setAutoDefault( TRUE );
    layButtons->addWidget( btnCancel );
    Spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layButtons->addItem( Spacer1 );
    layOuter->addLayout( layButtons );
    languageChange();
    resize( QSize(598, 468).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( btnOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
iChooserDialog::~iChooserDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void iChooserDialog::languageChange()
{
    setCaption( tr( "Chooser" ) );
    lvwItems->header()->setLabel( 0, tr( "Column 1" ) );
    lvwItems->clear();
    QListViewItem * item = new QListViewItem( lvwItems, 0 );
    item->setText( 0, tr( "New Item" ) );

    btnOk->setText( tr( "&OK" ) );
    btnOk->setAccel( QKeySequence( QString::null ) );
    btnCancel->setText( tr( "&Cancel" ) );
    btnCancel->setAccel( QKeySequence( QString::null ) );
}

