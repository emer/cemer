// Hack file for maketa because it chokes on Qt header files

#ifndef QTMAKETA_H
#define QTMAKETA_H

#ifdef __MAKETA__

// NOTE: we need the "dummy" member items because maketa chokes on empty class defs

typedef uint WFlags; // #IGNORE
typedef uint GFlags; // #IGNORE

//define any WFlags used in header files here -- (from qnamespace.h)

class Qt {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QEvent: public Qt {
public:
  enum Type {
    None = 0
  };
  int dummy; // #IGNORE
};

class QKeySequence: public Qt {
public:
  int dummy; // #IGNORE
};

class QListViewItem: public Qt {
public:
  int dummy; // #IGNORE
};

class QObject : public Qt {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QTab : public Qt {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QAction : public QObject {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QActionGroup : public QAction {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QByteArray {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QDragObject: public QAction {
//#IGNORE
public:
	int dummy; // #IGNORE
};


class QWidget : public QObject, public QPaintDevice {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class iRenderAreaWrapper: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QDialog: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QButton: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPushButton: public QButton  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QDockWindow: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QToolBar: public QDockWindow  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QMainWindow: public QWidget  {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QScrollview: public QWidget {
public:
  int dummy; // #IGNORE
};

class QSplitter: public QWidget {
public:
  int dummy; // #IGNORE
};

class QTabBar: public QWidget {
public:
  int dummy; // #IGNORE
};

class QTable: public QScrollview {
public:
  int dummy; // #IGNORE
};

class QPtrCollection {
//#IGNORE
public:
	int dummy; // #IGNORE
};

template<class T>
class QPtrList: public QPtrCollection {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QColor {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QIconSet {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QObjectList {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPaintDevice {
//#IGNORE
public:
	int dummy2; // #IGNORE
};

class QPalette {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPixmap {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QPoint {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QRect {
//#IGNORE
public:
	int dummy; // #IGNORE
};

class QSize { //#IGNORE
public:
	int dummy; // #IGNORE
};

class QSizePolicy { //#IGNORE
public:
	int dummy; // #IGNORE
};


#endif // __MAKETA__
#endif // QTMAKETA_H
