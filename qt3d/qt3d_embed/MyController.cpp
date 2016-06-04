
#include "MyController.h"

#include <QMainWindow>
#include <QWidget>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/QDirectionalLight>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>

void MyController::drawUpdate() {
  Qt3DExtras::Qt3DWindow* view = new Qt3DExtras::Qt3DWindow();
  QWidget* container = QWidget::createWindowContainer(view);

  m_target->setCentralWidget(container);

  // Root entity
  Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

  // Camera
  Qt3DRender::QCamera *camera = view->camera();

  camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
  camera->setPosition(QVector3D(0, 0, 30.0f));
  camera->setUpVector(QVector3D(0, 1, 0));
  camera->setViewCenter(QVector3D(0, 0, 0));

  // For camera controls
  Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
  camController->setLinearSpeed( 50.0f );
  camController->setLookSpeed( 180.0f );
  camController->setCamera(camera);

  Qt3DCore::QEntity *camera_light_ent = new Qt3DCore::QEntity(rootEntity);
  Qt3DRender::QDirectionalLight* camera_light = new Qt3DRender::QDirectionalLight();
  camera_light->setWorldDirection(QVector3D(0.0f, -0.5f, -1.0f));
  camera_light->setColor(QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f));
  camera_light_ent->addComponent(camera_light);
  
  // Material
  Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);
  material->setDiffuse(QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f));
    
  // Sphere
  Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(rootEntity);
  Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
  sphereMesh->setRadius(10.0f);

  sphereEntity->addComponent(sphereMesh);
  sphereEntity->addComponent(material);

  view->setRootEntity(rootEntity);
}
