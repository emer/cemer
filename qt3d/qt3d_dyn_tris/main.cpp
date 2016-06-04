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

#include <Qt3DRender/QDirectionalLight>

#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>

#include "T3TriangleStrip.h"

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
    
    // Tris
    Qt3DCore::QEntity *trisEntity = new Qt3DCore::QEntity(rootEntity);
    T3TriangleStripMesh *trisMesh = new T3TriangleStripMesh;

    QVector3D norm(0,0,1.0);

    float sz = 10.0f;

    // this should be two separate side-by-side rectangles.  instead the two
    // are connected, showing that the primitive reset (break) is not working.
    
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

    trisMesh->updateMesh();
    
    trisEntity->addComponent(trisMesh);
    trisEntity->addComponent(material);

    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
