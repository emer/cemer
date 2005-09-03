
#ifndef TEST_ISPINBOX_H
#define TEST_ISPINBOX_H

#include "ispinbox.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qdialog.h>

class Test_iSpinBox: public QDialog {
public:
  QVBoxLayout*	layOuter;
  iSpinBox*	spn1;
  QPushButton*	btnSpinRO;

  Test_iSpinBox(QWidget* parent = 0, const char* name = 0);
};

#endif
