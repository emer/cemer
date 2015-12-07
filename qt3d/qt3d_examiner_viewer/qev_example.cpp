
// SoQtExaminerViewer written in Quarter system
// by Randall C. O'Reilly randy.oreilly@colorado.edu
// code is made available under the GPL and the parts that
// are written by O'Reilly and/or his collaborators and students are 
// Copyright (C) Regents of the University of Colorado,
// see quarter_examiner_viewer.h for full details

// this is a simple example for using the QuarterExaminerViewer widget

#include "qt3d_examiner_viewer.h"

#include <QApplication>
#include <Qt3DInput/QInputAspect>

#include <QWindow.h>

#include <Qt3DRenderer/qcylindermesh.h>
#include <Qt3DRenderer/qmesh.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qrenderpass.h>

#include <Qt3DCore/qscaletransform.h>
#include <Qt3DCore/qrotatetransform.h>
#include <Qt3DCore/qlookattransform.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qaspectengine.h>

#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/qframegraph.h>
#include <Qt3DRenderer/qforwardrenderer.h>

#include <iostream>

int main(int argc, char ** argv)
{
  QApplication app(argc, argv);

  Qt3DExaminerViewer *viewer = new Qt3DExaminerViewer;

  Qt3D::QAspectEngine engine;
  engine.registerAspect(new Qt3D::QRenderAspect());
  Qt3D::QInputAspect *input = new Qt3D::QInputAspect;
  engine.registerAspect(input);
  engine.initialize();
  QVariantMap data;
  data.insert(QStringLiteral("surface"), QVariant::fromValue(static_cast<QSurface *>(viewer->Get_window())));
  data.insert(QStringLiteral("eventSource"), QVariant::fromValue(viewer->Get_window()));
  engine.setData(data);

  // Root entity
  Qt3D::QEntity *rootEntity = new Qt3D::QEntity();

  // Camera
  Qt3D::QCamera *cameraEntity = new Qt3D::QCamera(rootEntity);
  cameraEntity->setObjectName(QStringLiteral("cameraEntity"));
  cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
  cameraEntity->setPosition(QVector3D(0, 0, -20.0f));
  cameraEntity->setUpVector(QVector3D(0, 1, 0));
  cameraEntity->setViewCenter(QVector3D(0, 0, 0));
  input->setCamera(cameraEntity);
  viewer->setViewerCamera(cameraEntity);

  // FrameGraph
  Qt3D::QFrameGraph *frameGraph = new Qt3D::QFrameGraph();
  Qt3D::QForwardRenderer * forwardRenderer = new Qt3D::QForwardRenderer();

  // TechiqueFilter and renderPassFilter are not implement yet
  forwardRenderer->setCamera(cameraEntity);//viewer->getViewerCamera());
  forwardRenderer->setClearColor(Qt::black);

  frameGraph->setActiveFrameGraph(forwardRenderer);

  // Cylinder shape data
  Qt3D::QCylinderMesh *cylinder = new Qt3D::QCylinderMesh();
  cylinder->setRadius(1);
  cylinder->setLength(3);
  cylinder->setRings(100);
  cylinder->setSlices(20);

  // CylinderMesh Transform
  Qt3D::QScaleTransform *cylinderScale = new Qt3D::QScaleTransform();
  Qt3D::QRotateTransform *cylinderRotation = new Qt3D::QRotateTransform();
  Qt3D::QTransform *cylinderTransforms = new Qt3D::QTransform();

  const float scale = 1.5f;
  cylinderScale->setScale3D(QVector3D(scale, scale, scale));
  cylinderRotation->setAngleDeg(5.0f);
  cylinderRotation->setAxis(QVector3D(1, 0, 0));

  cylinderTransforms->addTransform(cylinderScale);
  cylinderTransforms->addTransform(cylinderRotation);

  // Cylinder
  Qt3D::QEntity *cylinderEntity = new Qt3D::QEntity(rootEntity);
  cylinderEntity->addComponent(cylinder);
  cylinderEntity->addComponent(cylinderTransforms);

  // Setting the FrameGraph
  rootEntity->addComponent(frameGraph);

  // Set root object of the scene
  engine.setRootEntity(rootEntity);

  // Pop up the QuarterWidget
  viewer->resize(800, 600);
  viewer->show();

#pragma message ("Figure out how to get the window context.")
#if 0

   viewer->quarter->makeCurrent();
   const QGLContext* ctx = viewer->quarter->context();
   QGLFormat fmt = ctx->format();

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

#endif // #if 0

  // const char* glv = (const char *)glGetString(GL_VERSION);
  // if (!glv) {
  //   std::cerr << "no version!" << std::endl;
  // }
  // else {
  //   std::cerr << "glv: " << glv << std::endl;
  // }

  app.exec();
  // Loop until exit.

  // Clean up resources.
  delete viewer;

  return 0;
}
