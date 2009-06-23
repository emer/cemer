// new t3viewer (with full netview capabilities) 

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
// g++ quarter_t3view.cpp -o quarter_t3view -I/usr/include/qt4 -I$HOME/install/include -lQtGui -L$HOME/install/lib -lQuarter

// mac compile:
// g++ quarter_t3view.cpp -o quarter_t3view -I/Library/Frameworks/QtGui.framework/Headers -I/Library/Frameworks/QtCore.framework/Headers -framework QtCore -framework QtGui -framework Carbon -framework Inventor -framework Quarter

/* 

Full emergent make stuff

mac compile: /usr/bin/c++ -DQT_DLL -DQT_OPENGL_LIB -DQT_WEBKIT_LIB
-DQT_GUI_LIB -DQT_XML_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DDEBUG -DQT_DEBUG
-Demergentlib_EXPORTS -g -DDEBUG -fPIC
-I/Library/Frameworks/QtOpenGL.framework/Headers
-I/Library/Frameworks/QtWebKit.framework/Headers
-I/Library/Frameworks/QtGui.framework/Headers
-I/Library/Frameworks/QtXml.framework/Headers
-I/Library/Frameworks/QtNetwork.framework/Headers
-I/Library/Frameworks/QtCore.framework/Headers
-I/Library/Frameworks/phonon.framework/Headers
-I/Library/Frameworks/QtXmlPatterns.framework/Headers
-I/Library/Frameworks/QtHelp.framework/Headers
-I/Library/Frameworks/QtAssistant.framework/Headers
-I/Library/Frameworks/QtDBus.framework/Headers
-I/Library/Frameworks/QtTest.framework/Headers -I/usr/include/QtUiTools
-I/Library/Frameworks/QtScript.framework/Headers
-I/Library/Frameworks/QtSvg.framework/Headers
-I/Library/Frameworks/QtSql.framework/Headers
-I/Library/Frameworks/QtDesigner.framework/Headers
-I/Library/Frameworks/Qt3Support.framework/Headers
-I/usr/local/Qt4.4/mkspecs/default
-I/Library/Frameworks/Inventor.framework/Headers
-I/Library/Frameworks/SoQt.framework/Headers -I/Users/oreilly/emergent
-I/Users/oreilly/emergent/include
-I/Users/oreilly/emergent/build_dbg/src/temt/lib -Woverloaded-virtual
-Wreturn-type -F/Library/Frameworks -o
CMakeFiles/emergentlib.dir/__/network/netstru_qtso.o -c
/Users/oreilly/emergent/src/emergent/network/netstru_qtso.cpp


mac link: /usr/bin/c++ -g -DDEBUG -Wl,-search_paths_first
-headerpad_max_install_names -fPIC CMakeFiles/emergent.dir/main.o -o
../../../bin/emergent_dbg ../../../lib/libemergentlib_dbg.4.0.21.dylib
-framework Inventor -Framework SoQt -framework QtOpenGL -framework OpenGL
-framework AGL -framework QtWebKit -framework QtGui -framework Carbon
-Framework AppKit -framework QtXml -framework QtNetwork /usr/lib/libssl.dylib
-framework QtCore /Usr/lib/libz.dylib -framework ApplicationServices
/usr/local/lib/libode.dylib /usr/lib/libcblas.dylib
/opt/local/lib/libgsl.dylib /usr/lib/libreadline.dylib
../../../lib/libtemt_dbg.4.0.21.dylib -framework Inventor -framework SoQt
-framework QtOpenGL -framework OpenGL -framework AGL -framework QtWebKit
-framework QtGui -framework Carbon -framework AppKit -framework QtXml
-framework QtNetwork /usr/lib/libssl.dylib -framework QtCore
/usr/lib/libz.dylib -framework ApplicationServices /usr/local/lib/libode.dylib
/usr/lib/libcblas.dylib /opt/local/lib/libgsl.dylib /usr/lib/libreadline.dylib

 */
