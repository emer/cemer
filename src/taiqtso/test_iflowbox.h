
#ifndef TEST_IFLOWBOX_H
#define TEST_IFLOWBOX_H

#include "iflowlayout.h"
#include "iflowbox.h"

#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qdialog.h>

class Test_iFlowBox: public QDialog {
  Q_OBJECT
public:
  QVBoxLayout*	layOuter;
  QHButtonGroup*	bgrHAlign;
  iFlowBox*	fbx;
  QPushButton*	buttons[20];

  Test_iFlowBox(QWidget* parent = 0, const char* name = 0);
  ~Test_iFlowBox();
public slots:
  void bgrHAlign_clicked(int bnum);

};

#endif
