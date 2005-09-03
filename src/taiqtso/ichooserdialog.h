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
