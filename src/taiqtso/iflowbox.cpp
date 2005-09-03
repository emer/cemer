/**********************************************************************
** $Id: qt/qgroupbox.cpp   3.3.2   edited Oct 22 13:27 $
**
** Implementation of iFlowBox widget class
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

#include "iflowbox.h"
#include "iflowlayout.h"
#include "qlayout.h"
#include "qpainter.h"
#include "qbitmap.h"
#include "qaccel.h"
#include "qfocusdata.h"
#include "qobjectlist.h"
#include "qdrawutil.h"
#include "qapplication.h"
#include "qstyle.h"
#if defined(QT_ACCESSIBILITY_SUPPORT)
#include "qaccessible.h"
#endif

/*!
    \class iFlowBox qgroupbox.h
    \brief The iFlowBox widget provides a group box frame with a title.

    \ingroup organizers
    \ingroup geomanagement
    \ingroup appearance
    \mainclass

    A group box provides a frame, a title and a keyboard shortcut, and
    displays various other widgets inside itself. The title is on top,
    the keyboard shortcut moves keyboard focus to one of the group
    box's child widgets, and the child widgets are usually laid out
    horizontally (or vertically) inside the frame.

    The simplest way to use it is to create a group box with the
    desired number of columns (or rows) and orientation, and then just
    create widgets with the group box as parent.

    It is also possible to change the orientation() and number of
    columns() after construction, or to ignore all the automatic
    layout support and manage the layout yourself. You can add 'empty'
    spaces to the group box with addSpace().

    iFlowBox also lets you set the title() (normally set in the
    constructor) and the title's alignment().

    You can change the spacing used by the group box with
    setInsideMargin() and setInsideSpacing(). To minimize space
    consumption, you can remove the right, left and bottom edges of
    the frame with setFlat().

    <img src=qgrpbox-w.png>

    \sa QButtonGroup
*/

class QCheckBox;

class QGroupBoxPrivate
{
public:
    QGroupBoxPrivate():
	spacer( 0 ) {}

    QSpacerItem *spacer;
};




/*!
    Constructs a group box widget with no title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.

    This constructor does not do automatic layout.
*/

iFlowBox::iFlowBox( QWidget *parent)
    : QFrame( parent)
{
    init();
}

/*!
    Constructs a group box with the title \a title.

    The \a parent and \a name arguments are passed to the QWidget
    constructor.

    This constructor does not do automatic layout.
*/

iFlowBox::iFlowBox( const QString &title, QWidget *parent)
    : QFrame( parent)
{
    init();
    setTitle( title );
}

/*!
    Constructs a group box with no title. Child widgets will be
    arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/

iFlowBox::iFlowBox(int flowAlignment_, int spacing_, Orientation orientation,
		    QWidget *parent)
    : QFrame( parent)
{
    init();
    flowAlign = flowAlignment_;
    if (spacing_ < 0) spacing_ = 5;
    spac = spacing_;
    dir = orientation;
    setLayout();
}

/*!
    Constructs a group box titled \a title. Child widgets will be
    arranged in \a strips rows or columns (depending on \a
    orientation).

    The \a parent and \a name arguments are passed to the QWidget
    constructor.
*/

iFlowBox::iFlowBox(int flowAlignment_, int spacing_, Orientation orientation,
		    const QString &title, QWidget *parent)
    : QFrame( parent)
{
    init();
    setTitle( title );
    flowAlign = flowAlignment_;
    if (spacing_ < 0) spacing_ = 5;
    spac = spacing_;
    dir = orientation;
    setLayout();
}

/*!
    Destroys the group box.
*/
iFlowBox::~iFlowBox()
{
    delete d;
}

void iFlowBox::init()
{
    align = AlignAuto;
    flowAlign = (AlignHCenter | AlignTop);
    setFrameStyle( QFrame::GroupBoxPanel | QFrame::Sunken );
#ifndef QT_NO_ACCEL
    accel = 0;
#endif
    vbox = 0;
    flow = 0;
    d = new QGroupBoxPrivate();
    lenvisible = 0;
    dir = Horizontal;
    marg = 11;
    spac = 5;
    bFlat = FALSE;
}

void iFlowBox::setTextSpacer()
{
    if ( !d->spacer )
	return;
    int h = 0;
    int w = 0;
    if ( lenvisible ) {
	QFontMetrics fm = fontMetrics();
	int fh = fm.height();
	{
	    fh = fm.height();
	    w = fm.width( str, lenvisible ) + 2*fm.width( "xx" );
	}
	h = frameRect().y();
	if ( layout() ) {
	    int m = layout()->margin();
	    int sp = layout()->spacing();
	    // do we have a child layout?
	    for ( QLayoutIterator it = layout()->iterator(); it.current(); ++it ) {
		if ( it.current()->layout() ) {
		    m += it.current()->layout()->margin();
		    sp = QMAX( sp, it.current()->layout()->spacing() );
		    break;
		}
	    }
	    h = QMAX( fh-m, h );
	    h += QMAX( sp - (h+m - fh), 0 );
	}
    }
    d->spacer->changeSize( w, h, QSizePolicy::Minimum, QSizePolicy::Fixed );
}


void iFlowBox::setTitle( const QString &title )
{
    if ( str == title )				// no change
	return;
    str = title;
#ifndef QT_NO_ACCEL
    if ( accel )
	delete accel;
    accel = 0;
    int s = QAccel::shortcutKey( title );
    if ( s ) {
	accel = new QAccel( this, "automatic focus-change accelerator" );
	accel->connectItem( accel->insertItem( s, 0 ),
			    this, SLOT(fixFocus()) );
    }
#endif
    calculateFrame();
    setTextSpacer();

    update();
    updateGeometry();
#if defined(QT_ACCESSIBILITY_SUPPORT)
    QAccessible::updateAccessibility( this, 0, QAccessible::NameChanged );
#endif
}

/*!
    \property iFlowBox::title
    \brief the group box title text.

    The group box title text will have a focus-change keyboard
    accelerator if the title contains \&, followed by a letter.

    \code
	g->setTitle( "&User information" );
    \endcode
    This produces "<u>U</u>ser information"; Alt+U moves the keyboard
    focus to the group box.

    There is no default title text.
*/

/*!
    \property iFlowBox::alignment
    \brief the alignment of the group box title.

    The title is always placed on the upper frame line. The horizontal
    alignment can be specified by the alignment parameter.

    The alignment is one of the following flags:
    \list
    \i \c AlignAuto aligns the title according to the language,
    usually to the left.
    \i \c AlignLeft aligns the title text to the left.
    \i \c AlignRight aligns the title text to the right.
    \i \c AlignHCenter aligns the title text centered.
    \endlist

    The default alignment is \c AlignAuto.

    \sa Alignment
*/

void iFlowBox::setAlignment( int alignment )
{
    if (align == alignment) return;
    align = alignment;
    update();
}

/*! \reimp
*/
void iFlowBox::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent(e);
    calculateFrame();
}

/*! \reimp

  \internal
  overrides QFrame::paintEvent
*/

void iFlowBox::paintEvent( QPaintEvent *event )
{
    QPainter paint( this );

    if ( lenvisible  ) {	// draw title
	QFontMetrics fm = paint.fontMetrics();
	int h = fm.height();
	int tw = fm.width( str, lenvisible ) + fm.width(QChar(' '));
	int x;
	int marg = bFlat ? 0 : 8;
	if ( align & AlignHCenter )		// center alignment
	    x = frameRect().width()/2 - tw/2;
	else if ( align & AlignRight )	// right alignment
	    x = frameRect().width() - tw - marg;
	else if ( align & AlignLeft )		 // left alignment
	    x = marg;
	else { // auto align
	    if( QApplication::reverseLayout() )
		x = frameRect().width() - tw - marg;
	    else
		x = marg;
	}
	QRect r( x, 0, tw, h );
	int va = style().styleHint(QStyle::SH_GroupBox_TextLabelVerticalAlignment, this);
	if(va & AlignTop)
	    r.moveBy(0, fm.descent());
	QColor pen( (QRgb) style().styleHint(QStyle::SH_GroupBox_TextLabelColor, this )  );
	if (!style().styleHint(QStyle::SH_UnderlineAccelerator, this))
	    va |= NoAccel;
	style().drawItem( &paint, r, ShowPrefix | AlignHCenter | va, colorGroup(),
			  isEnabled(), 0, str, -1, ownPalette() ? 0 : &pen );
	paint.setClipRegion( event->region().subtract( r ) ); // clip everything but title
 }
    if ( bFlat ) {
	    QRect fr = frameRect();
	    QPoint p1( fr.x(), fr.y() + 1 );
            QPoint p2( fr.x() + fr.width(), p1.y() );
	    // ### This should probably be a style primitive.
            qDrawShadeLine( &paint, p1, p2, colorGroup(), TRUE,
                            lineWidth(), midLineWidth() );
    } else {
	drawFrame(&paint);
    }
    drawContents( &paint );			// draw the contents
}


/*!
    Adds an empty cell at the next free position. If \a size is
    greater than 0, the empty cell takes \a size to be its fixed width
    (if orientation() is \c Horizontal) or height (if orientation() is
    \c Vertical).

    Use this method to separate the widgets in the group box or to
    skip the next free cell. For performance reasons, call this method
    after calling setColumnLayout() or by changing the \l
    iFlowBox::columns or \l iFlowBox::orientation properties. It is
    generally a good idea to call these methods first (if needed at
    all), and insert the widgets and spaces afterwards.
*/
void iFlowBox::addSpace( int size ) {
    QApplication::sendPostedEvents( this, QEvent::ChildInserted );

    if ( size > 0 ) {
	QSpacerItem *spacer
	    = new QSpacerItem( ( dir == Horizontal ) ? 0 : size,
			       ( dir == Vertical ) ? 0 : size,
			       QSizePolicy::Fixed, QSizePolicy::Fixed );
	flow->addItem(spacer);
    }
}

void iFlowBox::setFlowAlignment( int alignment )
{
    if (flowAlign == alignment) return;
    flowAlign = alignment;
    setLayout();
}

/*!
    Returns the width of the empty space between the items in the
    group and the frame of the group.

    Only applies if the group box has a defined orientation.

    The default is usually 11, by may vary depending on the platform
    and style.

    \sa setInsideMargin(), orientation
*/
int iFlowBox::insideMargin() const
{
    return marg;
}

/*!
    Returns the width of the empty space between each of the items
    in the group.

    Only applies if the group box has a defined orientation.

    The default is usually 5, by may vary depending on the platform
    and style.

    \sa setInsideSpacing(), orientation
*/
int iFlowBox::insideSpacing() const
{
    return spac;
}

/*!
    Sets the the width of the inside margin to \a m pixels.

    \sa insideMargin()
*/
void iFlowBox::setInsideMargin( int m )
{
    if (marg == m) return;
    marg = m;
    setLayout();
}

/*!
    Sets the width of the empty space between each of the items in
    the group to \a s pixels.

    \sa insideSpacing()
*/
void iFlowBox::setInsideSpacing( int s )
{
    if (spac == s) return;
    spac = s;
    setLayout();

}

/*!
    \property iFlowBox::orientation
    \brief the group box's orientation

    A horizontal group box arranges it's children in columns, while a
    vertical group box arranges them in rows.

    Usually it is not a good idea to set this property because it is
    slow (it does a complete layout). It is better to set the
    orientation directly in the constructor.
*/
void iFlowBox::setOrientation( Qt::Orientation o ) {
    if (dir == o) return;
    dir = o;
    setLayout();
}

/*!
    Changes the layout of the flow box. This function is only useful
    in combination with the default constructor that does not take any
    layout information. This function will put all existing children
    in the new layout. It is not good Qt programming style to call
    this function after children have been inserted. Sets the number
    of columns or rows to be \a strips, depending on \a direction.

    \sa orientation columns
*/
void iFlowBox::setLayout()
{
    if ( layout() )
	delete layout();

    vbox = 0; //TODO (BA): shouldn't these release any existing ones first?
    flow = 0;

    vbox = new QVBoxLayout( this, marg, 0 );

    d->spacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum,
				 QSizePolicy::Fixed );

    setTextSpacer();
    vbox->addItem( d->spacer );

    // Send all child events and ignore them. Otherwise we will end up
    // with doubled insertion. This won't do anything because nCols ==
    // nRows == 0.
    QApplication::sendPostedEvents( this, QEvent::ChildInserted );

    flow = new iFlowLayout(NULL, insideMargin(), spac, flowAlignment());
    vbox->addLayout( flow );

    // Add all children
    if ( children() ) {
	QObjectListIt it( *children() );
	QWidget *w;
	while( (w=(QWidget *)it.current()) != 0 ) {
	    ++it;
	    if ( w->isWidgetType()
		 )
		insertWid( w );
	}
    }
}


/*! \reimp  */
bool iFlowBox::event( QEvent * e )
{
    if ( e->type() == QEvent::LayoutHint && layout() )
	setTextSpacer();
    return QFrame::event( e );
}

/*!\reimp */
void iFlowBox::childEvent( QChildEvent *c )
{
    if ( !c->inserted() || !c->child()->isWidgetType() )
	return;
    QWidget *w = (QWidget*)c->child();
    if ( !flow )
	return;
    insertWid( w );
}

void iFlowBox::insertWid( QWidget* w )
{
    flow->addWidget(w );
    QApplication::postEvent( this, new QEvent( QEvent::LayoutHint ) );
}





/*!
    \internal

    This private slot finds a widget in this group box that can accept
    focus, and gives the focus to that widget.
*/

void iFlowBox::fixFocus()
{
    QFocusData * fd = focusData();
    QWidget * orig = fd->home();
    QWidget * best = 0;
    QWidget * candidate = 0;
    QWidget * w = orig;
    do {
	QWidget * p = w;
	while( p && p != this && !p->isTopLevel() )
	    p = p->parentWidget();
	if ( p == this && ( w->focusPolicy() & TabFocus ) == TabFocus
	     && w->isVisibleTo(this) ) {
	    if ( w->hasFocus())
		// we prefer a checked radio button or a widget that
		// already has focus, if there is one
		best = w;
	    else if ( !candidate )
		// but we'll accept anything that takes focus
		candidate = w;
	}
	w = fd->next();
    } while( w != orig );
    if ( best )
	best->setFocus();
    else if ( candidate )
	candidate->setFocus();
}


/*
    Sets the right frame rect depending on the title. Also calculates
    the visible part of the title.
*/
void iFlowBox::calculateFrame()
{
    lenvisible = str.length();

    if ( lenvisible ) { // do we have a label?
	QFontMetrics fm = fontMetrics();
	while ( lenvisible ) {
	    int tw = fm.width( str, lenvisible ) + 4*fm.width(QChar(' '));
	    if ( tw < width() )
		break;
	    lenvisible--;
	}
	if ( lenvisible ) { // but do we also have a visible label?
	    QRect r = rect();
	    int va = style().styleHint(QStyle::SH_GroupBox_TextLabelVerticalAlignment, this);
	    if(va & AlignVCenter)
		r.setTop( fm.height()/2 );				// frame rect should be
	    else if(va & AlignTop)
		r.setTop(fm.ascent());
	    setFrameRect( r );			//   smaller than client rect
	    return;
	}
    }

    // no visible label
    setFrameRect( QRect(0,0,0,0) );		//  then use client rect
}



/*! \reimp
 */
void iFlowBox::focusInEvent( QFocusEvent * )
{ // note no call to super
    fixFocus();
}


/*!\reimp
 */
void iFlowBox::fontChange( const QFont & oldFont )
{
    QWidget::fontChange( oldFont );
    calculateFrame();
    setTextSpacer();
}

/*!
  \reimp
*/

QSize iFlowBox::sizeHint() const
{
    QFontMetrics fm( font() );
    int tw, th;
    {
	tw = fm.width( title() ) + 2 * fm.width( "xx" );
	th = fm.height() + fm.width( QChar(' ') );
    }

    QSize s;
    if ( layout() ) {
	s = QFrame::sizeHint();
	return s.expandedTo( QSize( tw, 0 ) );
    } else {
	QRect r = childrenRect();
	QSize s( 100, 50 );
	s = s.expandedTo( QSize( tw, th ) );
	if ( r.isNull() )
	    return s;

	return s.expandedTo( QSize( r.width() + 2 * r.x(), r.height()+ 2 * r.y() ) );
    }
}

/*!
    \property iFlowBox::flat
    \brief whether the group box is painted flat or has a frame

    By default a group box has a surrounding frame, with the title
    being placed on the upper frame line. In flat mode the right, left
    and bottom frame lines are omitted, and only the thin line at the
    top is drawn.

    \sa title
*/
bool iFlowBox::isFlat() const
{
    return bFlat;
}

void iFlowBox::setFlat( bool b )
{
    if ( (bool)bFlat == b )
	return;
    bFlat = b;
    update();
}


/*
  sets all children of the group box except the qt_groupbox_checkbox
  to either disabled/enabled
*/
void iFlowBox::setChildrenEnabled( bool b )
{
    if ( !children() )
	return;
    QObjectListIt it( *children() );
    QObject *o;
    while( (o = it.current()) ) {
	++it;
	if ( o->isWidgetType()
	     ) {
	    QWidget *w = (QWidget*)o;
	    if ( b ) {
		if ( !w->testWState( WState_ForceDisabled ) )
		    w->setEnabled( TRUE );
	    } else {
		if ( w->isEnabled() ) {
		    w->setEnabled( FALSE );
		    ((iFlowBox*)w)->clearWState( WState_ForceDisabled );
		}
	    }
	}
    }
}

/*! \reimp */
void iFlowBox::setEnabled(bool on)
{
    QFrame::setEnabled(on);
}

