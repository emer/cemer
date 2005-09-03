
#ifndef TEST_IFLOWLAYOUT_H
#define TEST_IFLOWLAYOUT_H

#include "iflowlayout.h"

#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qdialog.h>

class Test_iFlowLayout: public QDialog {
  Q_OBJECT
public:
  QVBoxLayout*	layOuter;
  QHButtonGroup*	bgrHAlign;
  iFlowLayout*	layFlow;
  QPushButton*	buttons[20];

  Test_iFlowLayout(QWidget* parent = 0, const char* name = 0);
  ~Test_iFlowLayout();
public slots:
  void bgrHAlign_clicked(int bnum);

};

#endif
