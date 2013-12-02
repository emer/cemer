#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QHBoxLayout>
#include <QGLWidget>
#include <QCursor>

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

#if 1
  app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
#endif

  QWidget* win = new QWidget;
 
  QHBoxLayout* lay = new QHBoxLayout(win);
  QTreeWidget* tree = new QTreeWidget(win);
  lay->addWidget(tree);

#if 1
  // including this gl widget causes selection to fail 
  // on the items in the tree -- often causes a crash
  // in full-scale app
  QGLWidget* gl = new QGLWidget(win);
  lay->addWidget(gl);
  gl->setMinimumSize(200,200);
#endif

  tree->setSelectionMode(QAbstractItemView::SingleSelection); 
  tree->setColumnCount(1);
  tree->setDragEnabled(true);
  tree->setAcceptDrops(true);
  tree->setDropIndicatorShown(true);
  tree->setDragDropMode(QAbstractItemView::InternalMove);
  tree->setAutoScroll(false);
  tree->setAutoScrollMargin(16);

  QTreeWidgetItem* it1 = new QTreeWidgetItem();
  it1->setText(0, "item 1");
  it1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
  tree->addTopLevelItem(it1);

  QTreeWidgetItem* it2 = new QTreeWidgetItem();
  it2->setText(0, "item 2");
  it2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
  tree->addTopLevelItem(it2);

  tree->setCurrentItem(it1);

  for(int i=0;i<50;i++) {
    QTreeWidgetItem* ait = new QTreeWidgetItem();
    ait->setText(0, "loop item " + QString::number(i));
    ait->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    tree->addTopLevelItem(ait);
  }

  win->show();

  QCursor busy(Qt::WaitCursor);
  app.setOverrideCursor(busy);

  app.exec();
  delete win;
  return 0;
}
