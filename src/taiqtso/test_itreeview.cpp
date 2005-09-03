
#include "test_itreeview.h"

#include <qstring.h>


Test_iTreeView::Test_iTreeView(QWidget* parent, const char* name)
: QDialog(parent, name)
{
  setMaximumWidth(600);
  setMinimumWidth(400); //start with less than max
  layOuter = new QVBoxLayout(this);
  txtItem = new QLineEdit(this);
  layOuter->addWidget(txtItem);
  layOuter->addSpacing(4);
/*  bgrHAlign = new QHButtonGroup(this, "bgrHAlign");
  bgrHAlign->setRadioButtonExclusive(true);
  bgrHAlign->setTitle("Horizontal Alignment");
  new QRadioButton("Left", bgrHAlign);
  new QRadioButton("Center", bgrHAlign);
  new QRadioButton("Right", bgrHAlign);
  new QRadioButton("Justify", bgrHAlign);
  bgrHAlign->setButton(0);
  connect (bgrHAlign, SIGNAL(clicked(int)),
      this, SLOT(bgrHAlign_clicked(int)) );

  layOuter->addWidget(bgrHAlign); */

  tvw = new iTreeView(this, "tvw");
  layOuter->addWidget(tvw);
  connect(tvw, SIGNAL(selectionChanged(iTreeViewItem*)), this, SLOT(tvw_selectionChanged(iTreeViewItem*)));

  for (int i = 0; i < 8; ++i) {
    iTreeViewItem* itm = new iTreeViewItem(tvw, "Item " + QString::number(i));
    AddItems(itm, 4);
  }

  tvw->setSelectedData(this); // dummy obj, to test selecting

}

Test_iTreeView::~Test_iTreeView() {
  //nothing
}

void Test_iTreeView::AddItems(iTreeViewItem* par, int n) {
  for (int i = 0; i < n; ++i) {
    iTreeViewItem* itm = new iTreeViewItem(par, "Sub Item " + QString::number(i));
    n = n / 2;
    if (n > 0)
      AddItems(itm, n);
    else if (i == 0)
      itm->setData(this); // dummy object value, to test select
  }

}

void Test_iTreeView::tvw_selectionChanged(iTreeViewItem* itm) {
  if (itm == NULL)
    txtItem->setText("(NULL)");
  else {
    QString s = itm->text();
    if (itm->data() != NULL)
      s += QString(" (") + QString::number((ulong)itm->data()) + QString(")");
    txtItem->setText(s);
  }
}

/*
void Test_iTreeView::bgrHAlign_clicked(int bnum) {
  switch (bnum) {
  case 0:  fbx->setFlowAlignment(Qt::AlignLeft); break;
  case 1:  fbx->setFlowAlignment(Qt::AlignHCenter); break;
  case 2:  fbx->setFlowAlignment(Qt::AlignRight); break;
  case 3:  fbx->setFlowAlignment(Qt::AlignJustify); break;
  default: break;
  }
} */
