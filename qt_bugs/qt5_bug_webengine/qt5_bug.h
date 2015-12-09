
#include <QWidget>

class QLineEdit;
class QProgressBar;
class QWebEngineView;
class QAction;
class QAction;
class QStackedWidget;
class QTabBar;
class QFrame;

class MyWebBrowser : public QWidget {
  Q_OBJECT
public:

  QAction*              actBack;
  QAction*              actForward;
  QLineEdit*            url_text;
  QProgressBar*         prog_bar;
  QWebEngineView*       web_view;
  QAction*              actGo;
  QAction*              actStop;
  QStackedWidget*       panels;
  QTabBar*              tab_bar;
  QFrame*               pan1;
  QFrame*               pan2;
  
  MyWebBrowser(QWidget* par) : QWidget(par) { };
  ~MyWebBrowser() { }                                          
                               
public slots:
  void brow_urlChanged(const QUrl& url);
  void go_clicked(); // or return in url_text
  void stop_clicked(); // or return in url_text
  void panelSelected(int idx);
};  
