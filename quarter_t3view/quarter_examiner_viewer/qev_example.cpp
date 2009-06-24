
// SoQtExaminerViewer written in Quarter system
// by Randall C. O'Reilly randy.oreilly@colorado.edu
// code is made available under the GPL and the parts that
// are written by O'Reilly and/or his collaborators and students are 
// Copyright (C) Regents of the University of Colorado,
// see quarter_examiner_viewer.h for full details

// this is a simple example for using the QuarterExaminerViewer widget

#include "quarter_examiner_viewer.h"

#include <QtGui/QApplication>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>

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
 QuarterExaminerViewer* viewer = new QuarterExaminerViewer;
 viewer->setMinimumSize(800,600);
 viewer->quarter->setSceneGraph(root);

 viewer->quarter->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));

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
