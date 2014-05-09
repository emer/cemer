#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QGLWidget>
#include <QLineEdit>
#include <QMainWindow>
#include <QWindow>

extern void TurnOnTouchEventsForWindow(QWindow* qtWindow);

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

#if 1
  app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
  app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, true);
#endif

  for(int w=0; w<2; w++) {
    QMainWindow* win = new QMainWindow(NULL, Qt::Window
                                       | Qt:: WindowSystemMenuHint
                                       | Qt::WindowMinMaxButtonsHint
                                       | Qt::WindowCloseButtonHint);
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->setAttribute(Qt::WA_AcceptTouchEvents, true);
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
      ait->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled |
                    Qt::ItemIsDropEnabled);
      tree->addTopLevelItem(ait);
    }
  
    for(int j=0;j<10;j++) {
      QWidget* sub = new QWidget(body);
      lay->addWidget(sub);
      QVBoxLayout* vlay = new QVBoxLayout(sub);
      for(int i=0; i<10; i++) {
        QLineEdit* le = new QLineEdit(sub);
        vlay->addWidget(le);
      }
    }

#if 1
    // including this gl widget causes selection to fail 
    // on the items in the tree -- often causes a crash
    // in full-scale app
    QGLWidget* gl = new QGLWidget(body);
    lay->addWidget(gl);
    gl->setMinimumSize(200,200);
#endif

    win->show();
    TurnOnTouchEventsForWindow(win->windowHandle());
  }

  app.exec();
  return 0;
}
