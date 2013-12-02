#include <QWidget>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QCursor>

class WaitProc: public QWidget {
  Q_OBJECT
public:
  QTreeWidget*  tree;
  QHBoxLayout*  lay;
  QCursor*      busy;
  int           iteration;

  WaitProc(QWidget* parent = NULL);
  virtual ~WaitProc();

  void BuildTree();

protected slots:
  virtual void          timer_timeout(); // called when timer times out, for waitproc 
};

