
#include "qt5_bug.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QApplication>
#include <QMainWindow>
#include <QWindow>
#include <QLineEdit>
#include <QToolBar>
#include <QProgressBar>
#include <QStackedWidget>
#include <QTabBar>
#include <QWebEngineView>
#include <QFrame>
#include <QLabel>


void MyWebBrowser::go_clicked() {
  QString urltxt = url_text->text();
  if(urltxt.isEmpty()) return;
  QUrl url(urltxt);
  web_view->setUrl(url);
}

void MyWebBrowser::stop_clicked() {
  web_view->stop();
}

void MyWebBrowser::brow_urlChanged(const QUrl& url) {
  url_text->setText(url.toString());
}

void MyWebBrowser::panelSelected(int idx) {
  panels->setCurrentIndex(idx);
}

int
main(int argc, char ** argv)
{
  QApplication app(argc, argv);

  QMainWindow* win = new QMainWindow();
  // this should work but does not:
  win->setAttribute(Qt::WA_AcceptTouchEvents, false);

  QWidget* body = new QWidget(win);
  win->setCentralWidget(body);
  QHBoxLayout* hlay = new QHBoxLayout(body);

  MyWebBrowser* brow = new MyWebBrowser(body);
  hlay->addWidget(brow);
  
  QVBoxLayout* vlay = new QVBoxLayout(brow);

  QWidget* tabs = new QWidget(brow);
  vlay->addWidget(tabs);

  brow->tab_bar = new QTabBar(tabs);
  vlay->addWidget(brow->tab_bar);
  brow->panels = new QStackedWidget(tabs);
  vlay->addWidget(brow->panels);

  MyWebBrowser::connect(brow->tab_bar, SIGNAL(currentChanged(int)),
                        brow, SLOT(panelSelected(int)) );
  
  brow->pan1 = new QFrame;
  QVBoxLayout* p1vb = new QVBoxLayout(brow->pan1);
  QLabel* lab1 = new QLabel(brow->pan1);
  lab1->setText("panel 1");
  p1vb->addWidget(lab1);

  brow->panels->addWidget(brow->pan1);
  brow->tab_bar->addTab("Panel 1");

  brow->pan2 = new QFrame;
  QVBoxLayout* p2vb = new QVBoxLayout(brow->pan2);
  
  brow->panels->addWidget(brow->pan2);
  brow->tab_bar->addTab("Panel 2");

  QToolBar* tool_bar = new QToolBar(brow->pan2);
  p2vb->addWidget(tool_bar);

  brow->actBack = tool_bar->addAction("<");
  brow->actForward = tool_bar->addAction(">" );
  tool_bar->addSeparator();

  brow->url_text = new QLineEdit();
  tool_bar->addWidget(brow->url_text);

  brow->url_text->setText("https://google.com");
  
  brow->prog_bar = new QProgressBar();
  brow->prog_bar->setRange(0, 100);
  brow->prog_bar->setMaximumWidth(30);
  tool_bar->addWidget(brow->prog_bar);

  brow->actGo = tool_bar->addAction("Go");
  brow->actStop = tool_bar->addAction("X");

  // HOW TO TEST BUG:
  // These next two lines create the web view in the usual way -- they are the problem.
  // Even just the first line, without the addWidget, is sufficient to create the bug.
  // Comment out both of them, and uncomment the following line, and you'll see that
  // the tabs work fine.
  // The bug is that you cannot switch tabs once you've selected 2nd panel with the
  // web view in it -- selecting Panel 1 doesn't work!
  brow->web_view = new QWebEngineView(brow->pan2);
  p2vb->addWidget(brow->web_view);

  // this is the line to uncomment when commenting out the above two --
  // needed just to have a non-null web_view pointer for remaining code
  // brow->web_view = new QWebEngineView();
  
  MyWebBrowser::connect(brow->actBack, SIGNAL(triggered()), brow->web_view, SLOT(back()) );
  MyWebBrowser::connect(brow->actForward, SIGNAL(triggered()), brow->web_view, SLOT(forward()) );
  MyWebBrowser::connect(brow->actGo, SIGNAL(triggered()), brow, SLOT(go_clicked()) );
  MyWebBrowser::connect(brow->actStop, SIGNAL(triggered()), brow, SLOT(stop_clicked()) );
  MyWebBrowser::connect(brow->url_text, SIGNAL(returnPressed()), brow, SLOT(go_clicked()) );
  MyWebBrowser::connect(brow->web_view, SIGNAL(urlChanged(const QUrl&)),
          brow, SLOT(brow_urlChanged(const QUrl&)) );
  MyWebBrowser::connect(brow->web_view, SIGNAL(loadProgress(int)), brow->prog_bar, SLOT(setValue(int)) );
  MyWebBrowser::connect(brow->web_view, SIGNAL(loadStarted()), brow->prog_bar, SLOT(reset()) );

  win->show();

  app.exec();
  return 0;
}


