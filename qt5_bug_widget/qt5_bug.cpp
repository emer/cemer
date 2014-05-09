#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QGLWidget>
#include <QLineEdit>
#include <QMainWindow>

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

#if 1
  app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
  app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
#endif

  QMainWindow* win = new QMainWindow();
  QWidget* body = new QWidget(win);
  win->setCentralWidget(body);
  QHBoxLayout* lay = new QHBoxLayout(body);
  QTreeWidget* tree = new QTreeWidget(body);
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

  QLineEdit* le = new QLineEdit(body);
  lay->addWidget(le);

#if 1
  // including this gl widget causes selection to fail 
  // on the items in the tree -- often causes a crash
  // in full-scale app
  QGLWidget* gl = new QGLWidget(body);
  lay->addWidget(gl);
  gl->setMinimumSize(200,200);
#endif

  win->show();

  app.exec();
  return 0;
}
