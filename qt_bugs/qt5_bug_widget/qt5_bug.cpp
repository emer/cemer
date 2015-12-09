#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QMainWindow>
#include <QWindow>

// this demo shows touch events affecting focus of delegate editor
// for tree view -- your palms wresting near the trackpad will start
// to create touch -> focus events that move away from the delegate editor
// seems to happen most quickly if you just start typing to trigger editing

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

#if 0
  // this has no effect..
  app.setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
#endif

  QMainWindow* win = new QMainWindow();
  // this should work but does not:
  win->setAttribute(Qt::WA_AcceptTouchEvents, false);

  QWidget* body = new QWidget(win);
  win->setCentralWidget(body);
  QHBoxLayout* lay = new QHBoxLayout(body);
  QTreeWidget* tree = new QTreeWidget(body);
  lay->addWidget(tree);

  tree->setSelectionMode(QAbstractItemView::SingleSelection); 
  tree->setColumnCount(1);
  // these triggers are not critical for getting the bug, but make it easier to 
  // start editing to see it -- seems to happen more quickly if you just
  // start typing to trigger the editing
  tree->setEditTriggers(QAbstractItemView::DoubleClicked |
                        QAbstractItemView::SelectedClicked |
                        QAbstractItemView::EditKeyPressed |
                        QAbstractItemView::AnyKeyPressed
                        );

  for(int i=0;i<50;i++) {
    QTreeWidgetItem* ait = new QTreeWidgetItem();
    ait->setText(0, "loop item " + QString::number(i));
    ait->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled |
                  Qt::ItemIsDropEnabled | Qt::ItemIsEditable); // key is editable
    tree->addTopLevelItem(ait);
  }

  win->show();

  app.exec();
  return 0;
}
