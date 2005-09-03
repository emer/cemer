
#include "test_iflowbox.h"

#include <qstring.h>


Test_iFlowBox::Test_iFlowBox(QWidget* parent, const char* name)
: QDialog(parent, name)
{
  setMaximumWidth(600);
  setMinimumWidth(400); //start with less than max
  layOuter = new QVBoxLayout(this);
  bgrHAlign = new QHButtonGroup(this, "bgrHAlign");
  bgrHAlign->setRadioButtonExclusive(true);
  bgrHAlign->setTitle("Horizontal Alignment");
  new QRadioButton("Left", bgrHAlign);
  new QRadioButton("Center", bgrHAlign);
  new QRadioButton("Right", bgrHAlign);
  new QRadioButton("Justify", bgrHAlign);
  bgrHAlign->setButton(0);
  connect (bgrHAlign, SIGNAL(clicked(int)),
      this, SLOT(bgrHAlign_clicked(int)) );

  layOuter->addWidget(bgrHAlign);

  fbx = new iFlowBox(Qt::AlignLeft, 4, Qt::Horizontal, "", this, "layFlow");
  layOuter->addWidget(fbx);
  for (int i = 0; i < 20; ++i) {
    QPushButton* new_but = new QPushButton(QString("button_") + QString::number(i), fbx);
    new_but->setMinimumSize(new_but->width() + (i * 3), new_but->height()); // make them different sizes
    new_but->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    buttons[i] = new_but;
  }
}

Test_iFlowBox::~Test_iFlowBox() {
  //nothing
}

void Test_iFlowBox::bgrHAlign_clicked(int bnum) {
  switch (bnum) {
  case 0:  fbx->setFlowAlignment(Qt::AlignLeft); break;
  case 1:  fbx->setFlowAlignment(Qt::AlignHCenter); break;
  case 2:  fbx->setFlowAlignment(Qt::AlignRight); break;
  case 3:  fbx->setFlowAlignment(Qt::AlignJustify); break;
  default: break;
  }
}
