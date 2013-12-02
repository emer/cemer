#include "qt5_bug.h"

#include <QTreeWidgetItem>
#include <QApplication>
#include <QGLWidget>
#include <QCursor>
#include <QTimer>
#include <QBitmap>

// Wait Cursor, 16x16

#define wait_cursor_width 16
#define wait_cursor_height 16
#define wait_cursor_x_hot 7
#define wait_cursor_y_hot 8
static uchar wait_cursor_bits[] = {
   0xfc, 0x3f, 0x00, 0x00, 0xfc, 0x3f, 0x08, 0x10, 0xd0, 0x0b, 0xe0, 0x07,
   0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x40, 0x02, 0x20, 0x05, 0xd0, 0x0b,
   0xf8, 0x1f, 0xfc, 0x3f, 0x00, 0x00, 0xfc, 0x3f};

static uchar wait_mask_bits[] = {
   0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfc, 0x3f, 0xfc, 0x3f, 0xf8, 0x1f,
   0xf8, 0x1f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf8, 0x1f, 0xf8, 0x1f, 0xfc, 0x3f,
   0xfc, 0x3f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f};


WaitProc::WaitProc(QWidget* parent) : QWidget(parent) {
  iteration = 0;
  lay = new QHBoxLayout(this);
  tree = NULL;
  //  busy = new QCursor(Qt::WaitCursor);

  QBitmap waiter = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height),
                                     wait_cursor_bits, QImage::Format_MonoLSB);
  QBitmap waiter_m = QBitmap::fromData(QSize(wait_cursor_width, wait_cursor_height),
                                       wait_mask_bits, QImage::Format_MonoLSB);
  busy = new QCursor(waiter, waiter_m, wait_cursor_x_hot, wait_cursor_y_hot);
}

WaitProc::~WaitProc() {
  delete busy;
  delete tree;
}

void WaitProc::BuildTree() {
  if(tree) {
    lay->removeWidget(tree);
    delete tree;
  }
  tree = new QTreeWidget(this);
  lay->addWidget(tree);

  tree->setSelectionMode(QAbstractItemView::SingleSelection); 
  tree->setColumnCount(1);
  tree->setDragEnabled(true);
  tree->setAcceptDrops(true);
  tree->setDropIndicatorShown(true);
  tree->setDragDropMode(QAbstractItemView::InternalMove);
  tree->setAutoScroll(false);
  tree->setAutoScrollMargin(16);

  for(int i=0;i<50;i++) {
    QTreeWidgetItem* ait = new QTreeWidgetItem();
    ait->setText(0, "loop item " + QString::number(i));
    ait->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    tree->addTopLevelItem(ait);
  }
}

void WaitProc::timer_timeout() {
  if(iteration % 2 == 0) {
    ((QApplication*)QApplication::instance())->setOverrideCursor(*busy);
  }

  QApplication::processEvents();

  BuildTree();

  QApplication::processEvents();

  QTreeWidgetItemIterator it(tree);
  int cnt = 0;
  while (*it) {
    (*it)->setText(0, "iter " + QString::number(iteration) + " no: " + QString::number(cnt));
    ++cnt;
    ++it;
  }
  if(iteration % 2 == 1) {
    ((QApplication*)QApplication::instance())->restoreOverrideCursor();
  }

  QApplication::processEvents();

  ++iteration;
}

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

#if 1
  app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
#endif

  WaitProc* wait_proc = new WaitProc;

  QTimer* timer = new QTimer();
  WaitProc::connect(timer, SIGNAL(timeout()), wait_proc, SLOT(timer_timeout()));
  timer->start(2000);

#if 0
  // including this gl widget causes selection to fail 
  // on the items in the tree -- often causes a crash
  // in full-scale app
  QGLWidget* gl = new QGLWidget(wait_proc);
  wait_proc->lay->addWidget(gl);
  gl->setMinimumSize(200,200);
#endif

  wait_proc->BuildTree();

  wait_proc->show();

  app.exec();
  delete wait_proc;
  return 0;
}
