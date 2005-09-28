// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


#ifndef IFLOWBOX_H
#define IFLOWBOX_H

#include "taiqtso_def.h"

#ifndef QT_H
#include "qframe.h"
#endif // QT_H

class QAccel;
class QGroupBoxPrivate;
class iFlowLayout;
class QSpacerItem;
class QVBoxLayout;

class TAIQTSO_API iFlowBox : public QFrame
{
    Q_OBJECT
    Q_PROPERTY( QString title READ title WRITE setTitle )
    Q_PROPERTY( Alignment alignment READ alignment WRITE setAlignment )
    Q_PROPERTY( Alignment flowAlignment READ flowAlignment WRITE setFlowAlignment )
    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation DESIGNABLE false )
    Q_PROPERTY( bool flat READ isFlat WRITE setFlat )
public:
    iFlowBox( QWidget* parent=0);
    iFlowBox( const QString &title,
	       QWidget* parent=0);
    iFlowBox( int flowAlignment_, int spacing_, Orientation o,
	       QWidget* parent=0);
    iFlowBox( int flowAlignment_, int spacing_, Orientation o, const QString &title,
	       QWidget* parent=0);
    ~iFlowBox();


    QString title() const { return str; }
    virtual void setTitle( const QString &);

    int alignment() const { return align; }
    virtual void setAlignment( int );

    int flowAlignment() const { return flowAlign; }
    virtual void setFlowAlignment( int );

    Orientation orientation() const { return dir; }
    void setOrientation( Orientation );

    int insideMargin() const;
    int insideSpacing() const;
    void setInsideMargin( int m );
    void setInsideSpacing( int s );

    void addSpace( int );
    QSize sizeHint() const;

    bool isFlat() const;
    void setFlat( bool b );
    void setEnabled(bool on);

protected:
    virtual void setLayout();
    bool event( QEvent * );
    void childEvent( QChildEvent * );
    void resizeEvent( QResizeEvent * );
    void paintEvent( QPaintEvent * );
    void focusInEvent( QFocusEvent * );
    void fontChange( const QFont & );

private slots:
    void fixFocus();
    void setChildrenEnabled( bool b );

private:
    void init();
    void calculateFrame();
    void insertWid( QWidget* );
    void setTextSpacer();
    QString str;
    int align;
    int flowAlign;
    int lenvisible;
#ifndef QT_NO_ACCEL
    QAccel * accel;
#endif
    QGroupBoxPrivate * d;

    QVBoxLayout* vbox;
    iFlowLayout* flow;
    uint bFlat : 1;
    Orientation dir;
    int spac, marg;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    iFlowBox( const iFlowBox & );
    iFlowBox &operator=( const iFlowBox & );
#endif
};


#endif // QGROUPBOX_H
