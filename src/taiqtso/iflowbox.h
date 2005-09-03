/**********************************************************************
** $Id: qt/qgroupbox.h   3.3.2   edited Jul 4 2003 $
**
** Definition of iFlowBox widget class
**
** Created : 950203
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

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
