
#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qmainwindow.h>

class MainWindow: public QMainWindow {
  Q_OBJECT
public:
  QFrame*	frmMain;
  QGridLayout*	layOuter;
  QLabel*	lblClasses;
  QListBox*	lbxClasses;
  QPushButton*	btnCreate;

  MainWindow(QWidget* parent = 0, const char* name = 0);
  void		createTestDialog(int index); // order of items in list box

private:
  void		fillList();

private slots:
  void btnCreate_clicked();
};

#endif
