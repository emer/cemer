// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef QtThumbWheel_h
#define QtThumbWheel_h 1

#include <QPixmap>
#include <QAbstractSlider>

#if defined(Q_WS_WIN)
#  if !defined(QT_QTTHUMBWHEEL_EXPORT) && !defined(QT_QTTHUMBWHEEL_IMPORT)
#    define QT_QTTHUMBWHEEL_EXPORT
#  elif defined(QT_QTTHUMBWHEEL_IMPORT)
#    if defined(QT_QTTHUMBWHEEL_EXPORT)
#      undef QT_QTTHUMBWHEEL_EXPORT
#    endif
#    define QT_QTTHUMBWHEEL_EXPORT __declspec(dllimport)
#  elif defined(QT_QTTHUMBWHEEL_EXPORT)
#    undef QT_QTTHUMBWHEEL_EXPORT
#    define QT_QTTHUMBWHEEL_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTTHUMBWHEEL_EXPORT
#endif

class QtThumbWheelPrivate;
class QT_QTTHUMBWHEEL_EXPORT QtThumbWheel : public QAbstractSlider
{
    Q_OBJECT
    Q_PROPERTY(double transmissionRatio READ transmissionRatio WRITE setTransmissionRatio)
    Q_PROPERTY(bool wrapsAround READ wrapsAround WRITE setWrapsAround)
    Q_PROPERTY(int cogCount READ cogCount WRITE setCogCount)
    Q_PROPERTY(bool limitedDrag READ limitedDrag WRITE setLimitedDrag)

public:
    QtThumbWheel(QWidget *parent = 0);
    QtThumbWheel(int minimum, int maximum, int pageStep,
		 int value, Qt::Orientation orientation,
		 QWidget *parent = 0);

    ~QtThumbWheel();

    bool wrapsAround() const;
    void setWrapsAround(bool enabled);

    double transmissionRatio() const;
    virtual void setTransmissionRatio(double r);

    int cogCount() const;
    void setCogCount(int c);

    bool limitedDrag() const;
    void setLimitedDrag(bool enabled);

    QSize sizeHint() const;

protected:
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void resizeEvent(QResizeEvent *e);

    void paintEvent(QPaintEvent *);
    bool event(QEvent *e);

private:
    Q_DECLARE_PRIVATE(QtThumbWheel)
    Q_DISABLE_COPY(QtThumbWheel)
    QtThumbWheelPrivate *d_ptr;
};

#endif // QtThumbWheel_h
