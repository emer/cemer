
#include <QDialog>

// declare all other types mentioned but not required to include:
class iDialogObjDiffFilter; //
class QVBoxLayout; //
class QTreeWidget; //
class QTreeWidgetItem; //
class QPushButton; //
class QBrush; //

class iDialogObjDiffBrowser : public QDialog {
  typedef QDialog inherited;
  Q_OBJECT
public:
  enum ODBCols {
    COL_NEST,
    COL_A_FLG,
    COL_A_NM,
    COL_A_VAL,
    COL_A_VIEW,
    COL_SEP,
    COL_B_FLG,
    COL_B_NM,
    COL_B_VAL,
    COL_B_VIEW,
    COL_N,
  };

  static iDialogObjDiffBrowser* New(QWidget* par_window_ = NULL);

  QVBoxLayout*          layOuter;
  QTreeWidget*            items;        // list of items
  QPushButton*              btnAllA;
  QPushButton*              btnFiltA;
  QPushButton*              btnAllB;
  QPushButton*              btnFiltB;
  QPushButton*              btnOk;
  QPushButton*              btnCancel;
  QBrush*                   add_color;
  QBrush*                   del_color;
  QBrush*                   chg_color;
  QBrush*                   add_color_lt;
  QBrush*                   del_color_lt;
  QBrush*                   chg_color_lt;
  QBrush*                   off_color;

  virtual void  Constr();

  iDialogObjDiffBrowser(QWidget* par_window_);
  ~iDialogObjDiffBrowser();

public slots:
  virtual void         Browse();

};
