
#ifndef TEST_ITREEVIEW_H
#define TEST_ITREEVIEW_H

#include "itreeview.h"

#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qdialog.h>

class Test_iTreeView: public QDialog {
  Q_OBJECT
public:
  QVBoxLayout*	layOuter;
  QLineEdit*	txtItem;
  iTreeView*	tvw;

  Test_iTreeView(QWidget* parent = 0, const char* name = 0);
  ~Test_iTreeView();

  void AddItems(iTreeViewItem* par, int n);
public slots:
  void tvw_selectionChanged(iTreeViewItem* itm);
};

#endif
