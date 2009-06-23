// example examiner viewer from Coin

#include <QtGui/QApplication>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

using namespace SIM::Coin3D::Quarter;

int
main(int argc, char ** argv)
{
 QApplication app(argc, argv);
 // Initializes Quarter library (and implicitly also the Coin and Qt
 // libraries).
 Quarter::init();

 // Make a dead simple scene graph by using the Coin library, only
 // containing a single yellow cone under the scenegraph root.
 SoSeparator * root = new SoSeparator;
 root->ref();

 SoBaseColor * col = new SoBaseColor;
 col->rgb = SbColor(1, 1, 0);
 root->addChild(col);

 root->addChild(new SoCone);

 // Create a QuarterWidget for displaying a Coin scene graph
 QuarterWidget * viewer = new QuarterWidget;
 viewer->setSceneGraph(root);

 viewer->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));

 // Pop up the QuarterWidget
 viewer->show();
 // Loop until exit.
 app.exec();
 // Clean up resources.
 root->unref();
 delete viewer;

 Quarter::clean();

 return 0;
}

// linux compile:
// g++ quarterexample.cpp -o quarterexample -I/usr/include/qt4 -I$HOME/install/include -lQtGui -L$HOME/install/lib -lQuarter

// mac compile:
// g++ quarterexample.cpp -o quarterexample -I/Library/Frameworks/QtGui.framework/Headers -I/Library/Frameworks/QtCore.framework/Headers -framework QtCore -framework QtGui -framework Carbon -framework Inventor -framework Quarter
