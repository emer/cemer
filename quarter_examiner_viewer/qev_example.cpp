
// SoQtExaminerViewer written in Quarter system
// by Randall C. O'Reilly randy.oreilly@colorado.edu
// code is made available under the GPL and the parts that
// are written by O'Reilly and/or his collaborators and students are 
// Copyright (C) Regents of the University of Colorado,
// see quarter_examiner_viewer.h for full details

// this is a simple example for using the QuarterExaminerViewer widget

#include "quarter_examiner_viewer.h"

#include <QApplication>
#include <QGLWidget>
#include <QGLFormat>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>

using namespace SIM::Coin3D::Quarter;

#include <iostream>

int
main(int argc, char ** argv) {
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

  QGLWidget* qglw = (QGLWidget*)viewer->quarter; // it is this guy
  QGLFormat fmt = qglw->format();

  fmt.setSampleBuffers(true);
  fmt.setSamples(4);
  // qglw->setFormat(fmt);               // obs: this is supposedly deprecated..
  qglw->makeCurrent();
  glEnable(GL_MULTISAMPLE);

  // alternative no-multisample
  // fmt.setSampleBuffers(false);
  // qglw->setFormat(fmt);               // obs: this is supposedly deprecated..
  // qglw->makeCurrent();
  // glDisable(GL_MULTISAMPLE);

  SoSelection* sel_so = new SoSelection();
  sel_so->policy = SoSelection::SINGLE;
  // sel_so->addSelectionCallback(SoSelectionCallback, (void*)this);
  // sel_so->addDeselectionCallback(SoDeselectionCallback, (void*)this);
  sel_so->addChild(root);
  viewer->quarter->setSceneGraph(sel_so);

  SoBoxHighlightRenderAction* rend_act = new SoBoxHighlightRenderAction;
  SoRenderManager* rman = viewer->quarter->getSoRenderManager();
  rman->setGLRenderAction(rend_act);
  // following may be important for smoothing in offscreen rendering!
  //    rman->setAntialiasing(true, MAX(4, 1));
  rman->setAntialiasing(true, 1);
  viewer->quarter->setTransparencyType(QuarterWidget::BLEND);

  viewer->quarter->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));

  // Pop up the QuarterWidget
  viewer->show();
  // viewer->quarter->getSoRenderManager()->reinitialize();
  viewer->quarter->makeCurrent();
  const QGLContext* ctx = viewer->quarter->context();
  fmt = ctx->format();

  std::cerr << "is valid: " << ctx->isValid() << std::endl;
  std::cerr << " accum: " << fmt.accum() << std::endl;
  std::cerr << " alpha: " << fmt.alpha() << std::endl;
  std::cerr << " depth: " << fmt.depth() << std::endl;
  std::cerr << " dri: " << fmt.directRendering() << std::endl;
  std::cerr << " dbuf: " << fmt.doubleBuffer() << std::endl;
  std::cerr << " rgba: " << fmt.rgba() << std::endl;
  std::cerr << " samp: " << fmt.sampleBuffers() << std::endl;
  std::cerr << " sampbufs: " << fmt.samples() << std::endl;
  std::cerr << " plane: " << fmt.plane() << std::endl;
  std::cerr << " red: " << fmt.redBufferSize() << std::endl;
  std::cerr << " green: " << fmt.greenBufferSize() << std::endl;
  std::cerr << " blue: " << fmt.blueBufferSize() << std::endl;
  std::cerr << " alpha: " << fmt.alphaBufferSize() << std::endl;

  const char* glv = (const char *)glGetString(GL_VERSION);
  if(!glv) {
    std::cerr << "no version!" << std::endl;
  }
  else {
    std::cerr << "glv: " << glv << std::endl;
  }

  app.exec();
  // Loop until exit.

  // Clean up resources.
  root->unref();
  delete viewer;

  Quarter::clean();

  return 0;
}
