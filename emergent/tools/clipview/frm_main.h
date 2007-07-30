#ifndef FRM_MAIN_H
#define FRM_MAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <Qt3Support/Q3MimeSourceFactory>
#include "Qt3Support/Q3TextEdit"

class frmMain: public QFrame {
  Q_OBJECT
public:
    QPushButton *btnFormats;
    QComboBox *cmbFormats;
    QPushButton *btnShow;
    Q3TextEdit *txtResults;
    QLabel *lblDropZone;
    
    void languageChange();
    
    frmMain(QWidget* parent = NULL);
    ~frmMain();

public slots:
  void btnShow_clicked();
  void btnFormats_clicked();

};

#endif // FRM_MAIN_H
