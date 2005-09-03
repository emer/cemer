
#ifndef TEST_SIMPLEWIDGETS_H
#define TEST_SIMPLEWIDGETS_H

#include "ilineedit.h"
#include "icheckbox.h"
#include "ispinbox.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qdialog.h>

class Test_SimpleWidgets: public QDialog {
public:
  QVBoxLayout*	layOuter;
  iLineEdit*	txt1;
  iSpinBox*	spn1;
  iCheckBox*	chk1;
  QCheckBox*	chkHilight; // for hilight control
  QCheckBox*	chkRO; // for readonly control

  Test_SimpleWidgets(QWidget* parent = 0, const char* name = 0);
};

#endif
