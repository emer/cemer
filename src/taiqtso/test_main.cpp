
#include "test_mainwindow.h"

#include <qapplication.h>

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    MainWindow w;
    w.show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
