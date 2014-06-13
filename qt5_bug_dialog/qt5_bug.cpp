#include "qt5_bug.h"

#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QMainWindow>
#include <QWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QGLWidget>


// this demo shows dialog crashing in Qt5.3.0


iDialogObjDiffBrowser* iDialogObjDiffBrowser::New(QWidget* par_window_) {
  iDialogObjDiffBrowser* rval = new iDialogObjDiffBrowser(par_window_);
  rval->Constr();
  return rval;
}

iDialogObjDiffBrowser::iDialogObjDiffBrowser(QWidget* par_window_)
:inherited(par_window_)
{
  setModal(false);
  setWindowTitle("this is a test..");
  add_color = new QBrush(QColor("pale green"));
  del_color = new QBrush(QColor("pink"));
  chg_color = new QBrush(Qt::yellow);

  int darkness = 125;
  add_color_lt = new QBrush(add_color->color().darker(darkness));
  del_color_lt = new QBrush(del_color->color().darker(darkness));
  chg_color_lt = new QBrush(chg_color->color().darker(darkness));

  off_color = new QBrush(Qt::lightGray);

  resize(1200, 600);
}

iDialogObjDiffBrowser::~iDialogObjDiffBrowser() {
  delete add_color;
  delete del_color;
  delete chg_color;
  delete add_color_lt;
  delete del_color_lt;
  delete chg_color_lt;
  delete off_color;
}

void iDialogObjDiffBrowser::Browse() {
  // modal:
  // exec();
  // non-modal:
  show();
}

void iDialogObjDiffBrowser::Constr() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(2);
  layOuter->setSpacing(2);

  QString lb_txt = "Differences between object A and object B, shown as changes needed to make A into B\nA is: \nB is: \nClick actions to actually perform edits on objects, which will take place when Ok is pressed";

  QLabel* lbl = new QLabel(lb_txt);
  layOuter->addWidget(lbl);
  //  layOuter->addSpacing(taiM->vsep_c);

  QHBoxLayout* lay = new QHBoxLayout();
  btnAllA = new QPushButton("Toggle All &A's", this);
  btnAllA->setToolTip("Toggle selection of all the selectable actions for the A item -- i.e., make the A equivalent to the B");
  btnAllA->setDefault(false);
  btnAllA->setAutoDefault(false);
  lay->addWidget(btnAllA);

  btnFiltA = new QPushButton("Set Filtered A's", this);
  btnFiltA->setToolTip("Set actions on or off for filtered subset of the selectable actions for the A item");
  btnFiltA->setDefault(false);
  btnFiltA->setAutoDefault(false);
  lay->addWidget(btnFiltA);
  lay->addStretch();

  btnAllB = new QPushButton("Toggle All &B's", this);
  btnAllB->setToolTip("Toggle selection all the selectable actions for the B item -- i.e., make the B equivalent to the A");
  btnAllB->setDefault(false);
  btnAllB->setAutoDefault(false);
  lay->addWidget(btnAllB);

  btnFiltB = new QPushButton("Set Filtered B's", this);
  btnFiltB->setToolTip("Set actions on or off for filtered subset of the selectable actions for the B item");
  btnFiltB->setDefault(false);
  btnFiltB->setAutoDefault(false);
  lay->addWidget(btnFiltB);
  lay->addStretch();

  layOuter->addLayout(lay);

  items = new QTreeWidget(this);
  layOuter->addWidget(items, 1); // list is item to expand in host

  items->setColumnCount(COL_N);
  items->setSortingEnabled(false);// only 1 order possible
  items->setEditTriggers(QAbstractItemView::DoubleClicked);
  items->headerItem()->setText(COL_NEST, "Nesting");
  items->headerItem()->setToolTip(COL_NEST, "Nesting level below the original A or B object");
  items->headerItem()->setText(COL_A_FLG, "A Action");
  items->headerItem()->setToolTip(COL_A_FLG, "Edit action to perform on the A object -- these actions, if selected, will transform A into B");
  items->headerItem()->setText(COL_A_NM, "A Name");
  items->headerItem()->setToolTip(COL_A_NM, "Name of the item in A -- member name or sub-object name");
  items->headerItem()->setText(COL_A_VAL, "A Value");
  items->headerItem()->setToolTip(COL_A_VAL, "Value of the item in B");
  items->headerItem()->setText(COL_A_VIEW, "View");
  items->headerItem()->setToolTip(COL_A_VIEW, "View A item in project -- selects this object in the appropriate browser in the main project window");

  items->headerItem()->setText(COL_SEP, " | ");

  items->headerItem()->setText(COL_B_FLG, "B Action");
  items->headerItem()->setToolTip(COL_B_FLG, "Edit action to perform on the B object -- these actions, if selected, will transform B into A (i.e., the opposite of the default 'diff' direction)");
  items->headerItem()->setText(COL_B_NM, "B Name");
  items->headerItem()->setToolTip(COL_B_NM, "Name of the item in B -- member name or sub-object name");
  items->headerItem()->setText(COL_B_VAL, "B Value");
  items->headerItem()->setToolTip(COL_B_VAL, "Value of the item in B");
  items->headerItem()->setText(COL_B_VIEW, "View");
  items->headerItem()->setToolTip(COL_B_VIEW, "View B item in project -- selects this object in the appropriate browser in the main project window");
  items->setUniformRowHeights(true);
  items->setIndentation(4);

  lay = new QHBoxLayout();
  lay->addStretch();
  btnOk = new QPushButton("&Ok", this);
  btnOk->setDefault(true);
  lay->addWidget(btnOk);
  lay->addSpacing(2);
  btnCancel = new QPushButton("&Cancel", this);
  lay->addWidget(btnCancel);
  layOuter->addLayout(lay);

  // connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  // connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );

  // connect(btnAllA, SIGNAL(clicked()), this, SLOT(toggleAllA()) );
  // connect(btnAllB, SIGNAL(clicked()), this, SLOT(toggleAllB()) );

  // connect(btnFiltA, SIGNAL(clicked()), this, SLOT(setFilteredA()) );
  // connect(btnFiltB, SIGNAL(clicked()), this, SLOT(setFilteredB()) );

  // connect(items, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
  //         SLOT(itemClicked(QTreeWidgetItem*,int)));
}

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

#if 1
  // this has no effect..
  app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
  app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
#endif

  QMainWindow* win = new QMainWindow();
  // this should work but does not:
  win->setAttribute(Qt::WA_AcceptTouchEvents, false);

  QWidget* body = new QWidget(win);
  win->setCentralWidget(body);
  QHBoxLayout* lay = new QHBoxLayout(body);

  QPushButton* openbut = new QPushButton("Open Dialog", body);
  lay->addWidget(openbut);
  lay->addStretch();

  QGLWidget* glwidg = new QGLWidget(body);
  glwidg->setMinimumSize(200,200);
  lay->addWidget(glwidg);
  lay->addStretch();

  iDialogObjDiffBrowser* dib = iDialogObjDiffBrowser::New();
  QObject::connect(openbut, SIGNAL(clicked()), dib, SLOT(Browse()) );

  win->show();

  app.exec();
  return 0;
}
