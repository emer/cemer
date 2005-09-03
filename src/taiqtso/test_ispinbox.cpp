
#include "test_ispinbox.h"


Test_iSpinBox::Test_iSpinBox(QWidget* parent, const char* name)
: QDialog(parent, name)
{
  layOuter = new QVBoxLayout(this);
  QHBoxLayout* lay = new QHBoxLayout(layOuter);
  //layOuter->addLayout(lay);

  // iSpinBox
  spn1 = new iSpinBox(this, "spn1");
  lay->addWidget(spn1);
  lay->addSpacing(2);
  btnSpinRO = new QPushButton("Read Only", this, "btnSpinRO");
  btnSpinRO->setToggleButton(true);
  connect(btnSpinRO, SIGNAL(toggled(bool)),
      spn1, SLOT(setReadOnly(bool)) );

  lay->addWidget(btnSpinRO);
  lay->addStretch();
}

