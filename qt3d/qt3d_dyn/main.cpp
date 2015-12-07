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
#include <Qt3DCore/QCamera>
#include <Qt3DCore/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QFrameGraph>
#include <Qt3DRender/QForwardRenderer>
#include <Qt3DRender/QPhongMaterial>
#include <Qt3DRender/QCylinderMesh>
#include <Qt3DRender/QSphereMesh>
#include <Qt3DRender/QTorusMesh>
#include <Qt3DRender/QWindow>

#include <QPropertyAnimation>

#include "T3LineStrip.h"

#include <random>

class LineDrawController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(T3LineStripMesh* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(float time READ time WRITE setTime NOTIFY timeChanged)

public:
    LineDrawController(QObject *parent = 0)
        : QObject(parent)
        , m_target(Q_NULLPTR)
    {
    }

    void setTarget(T3LineStripMesh* target)
    {
        if (m_target != target) {
            m_target = target;
            emit targetChanged();
        }
    }

    T3LineStripMesh* target() const { return m_target; }

    void setTime(float time)
    {
      m_time = time;
      if(
         (int)m_time < 100 ||   // NOTE: crashes with null in glDrawElementsBaseVertex
         // randomly if lines are empty -- so we draw in ernest here at start..
         // to replicate crash,  get rid of this first test
         (int)m_time % 60 == 0) {
        drawLines();
        emit timeChanged();
      }
    }

    float time() const { return m_time; }

signals:
    void targetChanged();
    void timeChanged();

protected:
    void drawLines()
    {
      if(m_time == 0.0f && m_target->pointCount() > 4) {
        m_target->restart();
        // NOTE: key question here: how can I get this restart of lines to show
        // up in the display, without brute-force clearing EVERYTHING!?
      }
      QVector3D pos(m_time / 100.0, 10.0 * ((float)random() / 4294967295.0), 0.0f);
      m_target->lineTo(pos);
      m_target->updateLines();
    }

private:
    T3LineStripMesh* m_target;
    float m_time;
};




int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DRender::QWindow view;
    Qt3DInput::QInputAspect *input = new Qt3DInput::QInputAspect;
    view.registerAspect(input);

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DCore::QCamera *cameraEntity = view.defaultCamera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    input->setCamera(cameraEntity);

    // Material
    Qt3DRender::QMaterial *material = new Qt3DRender::QPhongMaterial(rootEntity);

    // Lines
    Qt3DCore::QEntity *linesEntity = new Qt3DCore::QEntity(rootEntity);
    T3LineStripMesh *linesMesh = new T3LineStripMesh;

    linesMesh->moveTo(QVector3D(0,0,0));
    linesMesh->lineTo(QVector3D(1,0,0));
    linesMesh->updateLines();
    
    LineDrawController *controller = new LineDrawController(linesMesh);
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

    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}

#include "main.moc"
