/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QDirectionalLight>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QFirstPersonCameraController>

#include <QPropertyAnimation>

// #include "T3LineStrip.h"
#include "T3TriangleStrip.h"
#include "T3Frame.h"

#include <random>

class MyController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(T3TriangleStripMesh* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(float time READ time WRITE setTime NOTIFY timeChanged)

public:
    MyController(QObject *parent = 0)
        : QObject(parent)
        , m_target(Q_NULLPTR)
    {
    }

    void setTarget(T3TriangleStripMesh* target)
    {
        if (m_target != target) {
            m_target = target;
            emit targetChanged();
        }
    }

    T3TriangleStripMesh* target() const { return m_target; }

    void setTime(float time)
    {
      m_time = time;
      if((int)m_time % 60 == 0) {
        drawUpdate();
        emit timeChanged();
      }
    }

    float time() const { return m_time; }

signals:
    void targetChanged();
    void timeChanged();

protected:
    void drawUpdate()
    {
      QVector3D norm(0,0,1.0);

      float sz = (random() % 10) + 1.0f;

      m_target->restart();
      
      // basic rectangle
      m_target->addVertex(QVector3D(0,0,0),norm);
      m_target->addVertex(QVector3D(sz,0,0),norm);
      m_target->addVertex(QVector3D(0,sz,0),norm);
      m_target->addVertex(QVector3D(sz,sz,0),norm);
      m_target->addTriangle(0,1,2);
      m_target->addIndex(3);
      
      m_target->addBreak();

      m_target->addVertex(QVector3D(15.0f + 0,0,0),norm);
      m_target->addVertex(QVector3D(15.0f + sz,0,0),norm);
      m_target->addVertex(QVector3D(15.0f + 0,sz,0),norm);
      m_target->addVertex(QVector3D(15.0f + sz,sz,0),norm);
      m_target->addTriangle(4,5,6);
      m_target->addIndex(7);
      
      m_target->updateMesh();
      
      // if(m_time == 0.0f && m_target->pointCount() > 4) {
      //   m_target->restart();
      //   // NOTE: key question here: how can I get this restart of lines to show
      //   // up in the display, without brute-force clearing EVERYTHING!?
      // }
      // QVector3D pos(m_time / 100.0, 10.0 * ((float)random() / 4294967295.0), 0.0f);
      // m_target->lineTo(pos);
      // m_target->updateLines();
    }

private:
    T3TriangleStripMesh* m_target;
    float m_time;
};




int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *camera = view.camera();

    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 30.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    // Qt3DExtras::QFirstPersonCameraController *camController = new Qt3DExtras::QFirstPersonCameraController(rootEntity);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    // Qt3DCore::QEntity *sun_light_ent = new Qt3DCore::QEntity(rootEntity);
    // Qt3DRender::QDirectionalLight* sun_light = new Qt3DRender::QDirectionalLight();
    // sun_light->setWorldDirection(QVector3D(0.0f, -1.0f, 0.0f));
    // sun_light->setColor(QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f));
    // sun_light_ent->addComponent(sun_light);

    Qt3DCore::QEntity *camera_light_ent = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QDirectionalLight* camera_light = new Qt3DRender::QDirectionalLight();
    camera_light->setWorldDirection(QVector3D(0.0f, -0.5f, -1.0f));
    camera_light->setColor(QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f));
    camera_light_ent->addComponent(camera_light);
  

  // Material
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(rootEntity);

    material->setDiffuse(QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f));
    
#if 0    
    // Lines
    Qt3DCore::QEntity *linesEntity = new Qt3DCore::QEntity(rootEntity);
    T3LineStripMesh *linesMesh = new T3LineStripMesh;

    linesMesh->moveTo(QVector3D(0,0,0)); // attempt to prevent crash.
    linesMesh->lineTo(QVector3D(1,0,0));
    linesMesh->updateLines();
    
    MyController *controller = new MyController(linesMesh);
    controller->setTarget(linesMesh);

    QPropertyAnimation *lineDrawAnim = new QPropertyAnimation(linesMesh);
    lineDrawAnim->setTargetObject(controller);
    lineDrawAnim->setPropertyName("time");
    lineDrawAnim->setStartValue(QVariant::fromValue(0));
    lineDrawAnim->setEndValue(QVariant::fromValue(1000));
    lineDrawAnim->setDuration(10000);
    lineDrawAnim->setLoopCount(-1);
    lineDrawAnim->start();

    linesEntity->addComponent(linesMesh);
    linesEntity->addComponent(material);
#endif
    
    // Tris
    Qt3DCore::QEntity *trisEntity = new Qt3DCore::QEntity(rootEntity);
    T3TriangleStripMesh *trisMesh = new T3TriangleStripMesh;

    QVector3D norm(0,0,1.0);

    float sz = 10.0f;
    
    // basic rectangle
    trisMesh->addVertex(QVector3D(0,0,0),norm);
    trisMesh->addVertex(QVector3D(sz,0,0),norm);
    trisMesh->addVertex(QVector3D(0,sz,0),norm);
    trisMesh->addVertex(QVector3D(sz,sz,0),norm);

    trisMesh->addTriangle(0,1,2);
    trisMesh->addIndex(3);

    trisMesh->addBreak();

    trisMesh->addVertex(QVector3D(15.0f + 0,0,0),norm);
    trisMesh->addVertex(QVector3D(15.0f + sz,0,0),norm);
    trisMesh->addVertex(QVector3D(15.0f + 0,sz,0),norm);
    trisMesh->addVertex(QVector3D(15.0f + sz,sz,0),norm);
    trisMesh->addTriangle(4,5,6);
    trisMesh->addIndex(7);

    // NOTE: uncomment this line to get the crash, comment it out to see that frame is fine
    trisMesh->updateMesh();
    
    MyController *controller = new MyController(trisMesh);
    controller->setTarget(trisMesh);

    QPropertyAnimation *lineDrawAnim = new QPropertyAnimation(trisMesh);
    lineDrawAnim->setTargetObject(controller);
    lineDrawAnim->setPropertyName("time");
    lineDrawAnim->setStartValue(QVariant::fromValue(0));
    lineDrawAnim->setEndValue(QVariant::fromValue(1000));
    lineDrawAnim->setDuration(10000);
    lineDrawAnim->setLoopCount(-1);
    // lineDrawAnim->start();

    trisEntity->addComponent(trisMesh);
    trisEntity->addComponent(material);

    // Frame test -- works fine
    Qt3DCore::QEntity *frameEntity = new Qt3DCore::QEntity(rootEntity);
    T3FrameMesh *frameMesh = new T3FrameMesh;
    frameEntity->addComponent(frameMesh);
    frameEntity->addComponent(material);

    frameMesh->setWidth(10.0);
    frameMesh->setHeight(8.0);
    frameMesh->setDepth(2.0);
    frameMesh->setFrameWidth(2.0);

    Qt3DCore::QTransform *frameTransform = new Qt3DCore::QTransform;
    frameTransform->setTranslation(QVector3D(-10.0f, 0, 0));
    frameEntity->addComponent(frameTransform);
      
    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}

#include "main.moc"
