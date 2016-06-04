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
#include <QApplication>

#include <QPropertyAnimation>

#include <QMainWindow>

#include "MyController.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QMainWindow win;

    MyController *controller = new MyController(&win);
    controller->setTarget(&win);

    QPropertyAnimation *propAnim = new QPropertyAnimation(&win);
    propAnim->setTargetObject(controller);
    propAnim->setPropertyName("time");
    propAnim->setStartValue(QVariant::fromValue(0));
    propAnim->setEndValue(QVariant::fromValue(1000));
    propAnim->setDuration(10000);
    propAnim->setLoopCount(-1);

    // comment this start line out and uncomment the drawUpdate line to get it to
    // render once, but then when you close the window, it will crash in the same place:
    // Qt3DCore`Qt3DCore::(anonymous namespace)::InternalEventListener::eventFilter(QObject*, QEvent*) + 99
    
    propAnim->start();
    //    controller->drawUpdate();
    
    win.resize(800, 600);
    win.show();

    return app.exec();
}

