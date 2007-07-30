#include <qapplication.h>
#include <QWidget>
#include "frm_main.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    frmMain w;
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    w.setVisible(true);
    return a.exec();
}
