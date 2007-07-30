// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

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
** Form interface generated from reading ui file 'iChooserDialog.ui'
**
** Created: Mon Jul 26 14:44:52 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CHOOSERDIALOG_H
#define CHOOSERDIALOG_H

#include "tai_qtdefs.h"

#include <qvariant.h>
#include <qdialog.h>
#include <qlistview.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QListView;
class QListViewItem;
class QPushButton;


class iChooserDialog : public QDialog
{
    Q_OBJECT

public:
    iChooserDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~iChooserDialog();

    QListView* lvwItems;
    QPushButton* btnOk;
    QPushButton* btnCancel;

protected:
    QHBoxLayout* layOuter;
    QSpacerItem* spacer3;
    QVBoxLayout* layButtons;
    QSpacerItem* Spacer1;

protected slots:
    virtual void languageChange();

};

#endif // CHOOSERDIALOG_H
