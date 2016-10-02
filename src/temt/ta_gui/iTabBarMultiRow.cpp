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

#include "iTabBarMultiRow.h"

#include <QAbstractItemDelegate>
#include <QApplication>
#include <QBitmap>
#include <QCursor>
#include <QEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>
#include <QTabwidget>
#include <QTooltip>
#include <QWhatsThis>
#include <QToolButton>
#ifndef QT_NO_ACCESSIBILITY
#include <QAccessible>
#endif

// can't seem to include this..
// #ifdef Q_OS_OSX
// #include <qpa/qplatformnativeinterface.h>
// #endif

// from qtbase/src/widgets/kernel/qlayoutengine_p.h:

#include <limits.h>
static const Q_DECL_UNUSED int QLAYOUTSIZE_MAX = INT_MAX/256/16;

struct QLayoutStruct
{
    inline void init(int stretchFactor = 0, int minSize = 0) {
        stretch = stretchFactor;
        minimumSize = sizeHint = minSize;
        maximumSize = QLAYOUTSIZE_MAX;
        expansive = false;
        empty = true;
        spacing = 0;
    }

    int smartSizeHint() {
        return (stretch > 0) ? minimumSize : sizeHint;
    }
    int effectiveSpacer(int uniformSpacer) const {
        Q_ASSERT(uniformSpacer >= 0 || spacing >= 0);
        return (uniformSpacer >= 0) ? uniformSpacer : spacing;
    }

    // parameters
    int stretch;
    int sizeHint;
    int maximumSize;
    int minimumSize;
    int spacing;
    bool expansive;
    bool empty;

    // temporary storage
    bool done;

    // result
    int pos;
    int size;
};

Q_WIDGETS_EXPORT void qGeomCalc(QVector<QLayoutStruct> &chain, int start, int count,
                                int pos, int space, int spacer = -1);


QMovableTabWidget::QMovableTabWidget(QWidget *parent)
  : QWidget(parent)
{
}

void QMovableTabWidget::setPixmap(const QPixmap &pixmap)
{
  m_pixmap = pixmap;
  update();
}

void QMovableTabWidget::paintEvent(QPaintEvent *e)
{
  Q_UNUSED(e);
  QPainter p(this);
  p.drawPixmap(0, 0, m_pixmap);
}

inline static bool verticalTabs(iTabBarMultiRow::Shape shape)
{
  return shape == iTabBarMultiRow::RoundedWest
    || shape == iTabBarMultiRow::RoundedEast
    || shape == iTabBarMultiRow::TriangularWest
    || shape == iTabBarMultiRow::TriangularEast;
}

void iTabBarMultiRowPrivate::updateMacBorderMetrics()
{
// #if defined(Q_OS_OSX)
//   Q_Q(iTabBarMultiRow);
//   // Extend the unified title and toolbar area to cover the tab bar iff
//   // 1) the tab bar is in document mode
//   // 2) the tab bar is directly below an "unified" area.
//   // The extending itself is done in the Cocoa platform plugin and Mac style,
//   // this function registers geometry and visibility state for the tab bar.

//   // Calculate geometry
//   int upper, lower;
//   if (documentMode) {
//     QPoint windowPos = q->mapTo(q->window(), QPoint(0,0));
//     upper = windowPos.y();
//     int tabStripHeight = q->tabSizeHint(0).height();
//     int pixelTweak = -3;
//     lower = upper + tabStripHeight + pixelTweak;
//   } else {
//     upper = 0;
//     lower = 0;
//   }

//   QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
//   quintptr identifier = reinterpret_cast<quintptr>(q);

//   // Set geometry
//   QPlatformNativeInterface::NativeResourceForIntegrationFunction function =
//     nativeInterface->nativeResourceFunctionForIntegration("registerContentBorderArea");
//   if (!function)
//     return; // Not Cocoa platform plugin.
//   typedef void (*RegisterContentBorderAreaFunction)(QWindow *window, quintptr identifier, int upper, int lower);
//   (reinterpret_cast<RegisterContentBorderAreaFunction>(function))(q->window()->windowHandle(), identifier, upper, lower);

//   // Set visibility state
//   function = nativeInterface->nativeResourceFunctionForIntegration("setContentBorderAreaEnabled");
//   if (!function)
//     return;
//   typedef void (*SetContentBorderAreaEnabledFunction)(QWindow *window, quintptr identifier, bool enable);
//   (reinterpret_cast<SetContentBorderAreaEnabledFunction>(function))(q->window()->windowHandle(), identifier, q->isVisible());
// #endif
}

/*!
  Initialize \a option with the values from the tab at \a tabIndex. This method
  is useful for subclasses when they need a QStyleOptionTab,
  but don't want to fill in all the information themselves.

  \sa QStyleOption::initFrom(), QTabWidget::initStyleOption()
*/
void iTabBarMultiRow::initStyleOption(QStyleOptionTab *option, int tabIndex) const
{
  int totalTabs = dpriv->tabList.size();

  if (!option || (tabIndex < 0 || tabIndex >= totalTabs))
    return;

  const iTabBarMultiRowPrivate::Tab &tab = dpriv->tabList.at(tabIndex);
  option->initFrom(this);
  option->state &= ~(QStyle::State_HasFocus | QStyle::State_MouseOver);
  option->rect = tabRect(tabIndex);
  bool isCurrent = tabIndex == dpriv->currentIndex;
  option->row = 0;
  if (tabIndex == dpriv->pressedIndex)
    option->state |= QStyle::State_Sunken;
  if (isCurrent)
    option->state |= QStyle::State_Selected;
  if (isCurrent && hasFocus())
    option->state |= QStyle::State_HasFocus;
  if (!tab.enabled)
    option->state &= ~QStyle::State_Enabled;
  if (isActiveWindow())
    option->state |= QStyle::State_Active;
  if (!dpriv->dragInProgress && option->rect == dpriv->hoverRect)
    option->state |= QStyle::State_MouseOver;
  option->shape = (QTabBar::Shape)dpriv->shape;
  option->text = tab.text;

  if (tab.textColor.isValid())
    option->palette.setColor(foregroundRole(), tab.textColor);

  option->icon = tab.icon;
  option->iconSize = iconSize();  // Will get the default value then.

  option->leftButtonSize = tab.leftWidget ? tab.leftWidget->size() : QSize();
  option->rightButtonSize = tab.rightWidget ? tab.rightWidget->size() : QSize();
  option->documentMode = dpriv->documentMode;

  if (tabIndex > 0 && tabIndex - 1 == dpriv->currentIndex)
    option->selectedPosition = QStyleOptionTab::PreviousIsSelected;
  else if (tabIndex + 1 < totalTabs && tabIndex + 1 == dpriv->currentIndex)
    option->selectedPosition = QStyleOptionTab::NextIsSelected;
  else
    option->selectedPosition = QStyleOptionTab::NotAdjacent;

  bool paintBeginning = (tabIndex == 0) || (dpriv->dragInProgress && tabIndex == dpriv->pressedIndex + 1);
  bool paintEnd = (tabIndex == totalTabs - 1) || (dpriv->dragInProgress && tabIndex == dpriv->pressedIndex - 1);
  if (paintBeginning) {
    if (paintEnd)
      option->position = QStyleOptionTab::OnlyOneTab;
    else
      option->position = QStyleOptionTab::Beginning;
  } else if (paintEnd) {
    option->position = QStyleOptionTab::End;
  } else {
    option->position = QStyleOptionTab::Middle;
  }

#ifndef QT_NO_TABWIDGET
  if (const QTabWidget *tw = qobject_cast<const QTabWidget *>(parentWidget())) {
    option->features |= QStyleOptionTab::HasFrame;
    if (tw->cornerWidget(Qt::TopLeftCorner) || tw->cornerWidget(Qt::BottomLeftCorner))
      option->cornerWidgets |= QStyleOptionTab::LeftCornerWidget;
    if (tw->cornerWidget(Qt::TopRightCorner) || tw->cornerWidget(Qt::BottomRightCorner))
      option->cornerWidgets |= QStyleOptionTab::RightCornerWidget;
  }
#endif

  QRect textRect = style()->subElementRect(QStyle::SE_TabBarTabText, option, this);
  option->text = fontMetrics().elidedText(option->text, dpriv->elideMode, textRect.width(),
                                          Qt::TextShowMnemonic);
}

int iTabBarMultiRowPrivate::extraWidth() const
{
  Q_Q(const iTabBarMultiRow);
  return 2 * qMax(q->style()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, q),
                  QApplication::globalStrut().width());
}

void iTabBarMultiRowPrivate::init()
{
  Q_Q(iTabBarMultiRow);
  leftB = new QToolButton(q);
  leftB->setAutoRepeat(true);
  QObject::connect(leftB, SIGNAL(clicked()), q, SLOT(_q_scrollTabs()));
  leftB->hide();
  rightB = new QToolButton(q);
  rightB->setAutoRepeat(true);
  QObject::connect(rightB, SIGNAL(clicked()), q, SLOT(_q_scrollTabs()));
  rightB->hide();
#ifdef QT_KEYPAD_NAVIGATION
  if (QApplication::keypadNavigationEnabled()) {
    leftB->setFocusPolicy(Qt::NoFocus);
    rightB->setFocusPolicy(Qt::NoFocus);
    q->setFocusPolicy(Qt::NoFocus);
  } else
#endif
    q->setFocusPolicy(Qt::TabFocus);

#ifndef QT_NO_ACCESSIBILITY
  leftB->setAccessibleName(iTabBarMultiRow::tr("Scroll Left"));
  rightB->setAccessibleName(iTabBarMultiRow::tr("Scroll Right"));
#endif
  q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  elideMode = Qt::TextElideMode(q->style()->styleHint(QStyle::SH_TabBar_ElideMode, 0, q));
  useScrollButtons = !q->style()->styleHint(QStyle::SH_TabBar_PreferNoArrows, 0, q);
}

iTabBarMultiRowPrivate::Tab *iTabBarMultiRowPrivate::at(int index)
{
  return validIndex(index)?&tabList[index]:0;
}

const iTabBarMultiRowPrivate::Tab *iTabBarMultiRowPrivate::at(int index) const
{
  return validIndex(index)?&tabList[index]:0;
}

int iTabBarMultiRowPrivate::indexAtPos(const QPoint &p) const
{
  Q_Q(const iTabBarMultiRow);
  if (q->tabRect(currentIndex).contains(p))
    return currentIndex;
  for (int i = 0; i < tabList.count(); ++i)
    if (tabList.at(i).enabled && q->tabRect(i).contains(p))
      return i;
  return -1;
}

void iTabBarMultiRowPrivate::layoutTabs()
{
  Q_Q(iTabBarMultiRow);
  scrollOffset = 0;
  layoutDirty = false;
  QSize size = q->size();
  int last, available;
  int maxExtent;
  int i;
  bool vertTabs = verticalTabs(shape);
  int tabChainIndex = 0;

  Qt::Alignment tabAlignment = Qt::Alignment(q->style()->styleHint(QStyle::SH_TabBar_Alignment, 0, q));
  QVector<QLayoutStruct> tabChain(tabList.count() + 2);

  // We put an empty item at the front and back and set its expansive attribute
  // depending on tabAlignment.
  tabChain[tabChainIndex].init();
  tabChain[tabChainIndex].expansive = (tabAlignment != Qt::AlignLeft)
    && (tabAlignment != Qt::AlignJustify);
  tabChain[tabChainIndex].empty = true;
  ++tabChainIndex;

  // We now go through our list of tabs and set the minimum size and the size hint
  // This will allow us to elide text if necessary. Since we don't set
  // a maximum size, tabs will EXPAND to fill up the empty space.
  // Since tab widget is rather *ahem* strict about keeping the geometry of the
  // tab bar to its absolute minimum, this won't bleed through, but will show up
  // if you use tab bar on its own (a.k.a. not a bug, but a feature).
  // Update: if expanding is false, we DO set a maximum size to prevent the tabs
  // being wider than necessary.
  if (!vertTabs) {
    int minx = 0;
    int x = 0;
    int maxHeight = 0;
    for (i = 0; i < tabList.count(); ++i, ++tabChainIndex) {
      QSize sz = q->tabSizeHint(i);
      tabList[i].maxRect = QRect(x, 0, sz.width(), sz.height());
      x += sz.width();
      maxHeight = qMax(maxHeight, sz.height());
      sz = q->minimumTabSizeHint(i);
      tabList[i].minRect = QRect(minx, 0, sz.width(), sz.height());
      minx += sz.width();
      tabChain[tabChainIndex].init();
      tabChain[tabChainIndex].sizeHint = tabList.at(i).maxRect.width();
      tabChain[tabChainIndex].minimumSize = sz.width();
      tabChain[tabChainIndex].empty = false;
      tabChain[tabChainIndex].expansive = true;

      if (!expanding)
        tabChain[tabChainIndex].maximumSize = tabChain[tabChainIndex].sizeHint;
    }

    last = minx;
    available = size.width();
    maxExtent = maxHeight;
  } else {
    int miny = 0;
    int y = 0;
    int maxWidth = 0;
    for (i = 0; i < tabList.count(); ++i, ++tabChainIndex) {
      QSize sz = q->tabSizeHint(i);
      tabList[i].maxRect = QRect(0, y, sz.width(), sz.height());
      y += sz.height();
      maxWidth = qMax(maxWidth, sz.width());
      sz = q->minimumTabSizeHint(i);
      tabList[i].minRect = QRect(0, miny, sz.width(), sz.height());
      miny += sz.height();
      tabChain[tabChainIndex].init();
      tabChain[tabChainIndex].sizeHint = tabList.at(i).maxRect.height();
      tabChain[tabChainIndex].minimumSize = sz.height();
      tabChain[tabChainIndex].empty = false;
      tabChain[tabChainIndex].expansive = true;

      if (!expanding)
        tabChain[tabChainIndex].maximumSize = tabChain[tabChainIndex].sizeHint;
    }

    last = miny;
    available = size.height();
    maxExtent = maxWidth;
  }

  Q_ASSERT(tabChainIndex == tabChain.count() - 1); // add an assert just to make sure.
  // Mirror our front item.
  tabChain[tabChainIndex].init();
  tabChain[tabChainIndex].expansive = (tabAlignment != Qt::AlignRight)
    && (tabAlignment != Qt::AlignJustify);
  tabChain[tabChainIndex].empty = true;

  // Do the calculation
  qGeomCalc(tabChain, 0, tabChain.count(), 0, qMax(available, last), 0);

  // Use the results
  for (i = 0; i < tabList.count(); ++i) {
    const QLayoutStruct &lstruct = tabChain.at(i + 1);
    if (!vertTabs)
      tabList[i].rect.setRect(lstruct.pos, 0, lstruct.size, maxExtent);
    else
      tabList[i].rect.setRect(0, lstruct.pos, maxExtent, lstruct.size);
  }

  if (useScrollButtons && tabList.count() && last > available) {
    int extra = extraWidth();
    if (!vertTabs) {
      Qt::LayoutDirection ld = q->layoutDirection();
      QRect arrows = QStyle::visualRect(ld, q->rect(),
                                        QRect(available - extra, 0, extra, size.height()));
      int buttonOverlap = q->style()->pixelMetric(QStyle::PM_TabBar_ScrollButtonOverlap, 0, q);

      if (ld == Qt::LeftToRight) {
        leftB->setGeometry(arrows.left(), arrows.top(), extra/2, arrows.height());
        rightB->setGeometry(arrows.right() - extra/2 + buttonOverlap, arrows.top(),
                            extra/2, arrows.height());
        leftB->setArrowType(Qt::LeftArrow);
        rightB->setArrowType(Qt::RightArrow);
      } else {
        rightB->setGeometry(arrows.left(), arrows.top(), extra/2, arrows.height());
        leftB->setGeometry(arrows.right() - extra/2 + buttonOverlap, arrows.top(),
                           extra/2, arrows.height());
        rightB->setArrowType(Qt::LeftArrow);
        leftB->setArrowType(Qt::RightArrow);
      }
    } else {
      QRect arrows = QRect(0, available - extra, size.width(), extra );
      leftB->setGeometry(arrows.left(), arrows.top(), arrows.width(), extra/2);
      leftB->setArrowType(Qt::UpArrow);
      rightB->setGeometry(arrows.left(), arrows.bottom() - extra/2 + 1,
                          arrows.width(), extra/2);
      rightB->setArrowType(Qt::DownArrow);
    }
    leftB->setEnabled(scrollOffset > 0);
    rightB->setEnabled(last - scrollOffset >= available - extra);
    leftB->show();
    rightB->show();
  } else {
    rightB->hide();
    leftB->hide();
  }

  layoutWidgets();
  q->tabLayoutChange();
}

void iTabBarMultiRowPrivate::makeVisible(int index)
{
  Q_Q(iTabBarMultiRow);
  if (!validIndex(index) || leftB->isHidden())
    return;

  const QRect tabRect = tabList.at(index).rect;
  const int oldScrollOffset = scrollOffset;
  const bool horiz = !verticalTabs(shape);
  const int available = (horiz ? q->width() : q->height()) - extraWidth();
  const int start = horiz ? tabRect.left() : tabRect.top();
  const int end = horiz ? tabRect.right() : tabRect.bottom();
  if (start < scrollOffset) // too far left
    scrollOffset = start - (index ? 8 : 0);
  else if (end > scrollOffset + available) // too far right
    scrollOffset = end - available + 1;

  leftB->setEnabled(scrollOffset > 0);
  const int last = horiz ? tabList.last().rect.right() : tabList.last().rect.bottom();
  rightB->setEnabled(last - scrollOffset >= available);
  if (oldScrollOffset != scrollOffset) {
    q->update();
    layoutWidgets();
  }
}

void iTabBarMultiRowPrivate::killSwitchTabTimer()
{
  Q_Q(iTabBarMultiRow);
  if (switchTabTimerId) {
    q->killTimer(switchTabTimerId);
    switchTabTimerId = 0;
  }
  switchTabCurrentIndex = -1;
}

void iTabBarMultiRowPrivate::layoutTab(int index)
{
  Q_Q(iTabBarMultiRow);
  Q_ASSERT(index >= 0);

  Tab &tab = tabList[index];
  bool vertical = verticalTabs(shape);
  if (!(tab.leftWidget || tab.rightWidget))
    return;

  QStyleOptionTab opt;
  q->initStyleOption(&opt, index);
  if (tab.leftWidget) {
    QRect rect = q->style()->subElementRect(QStyle::SE_TabBarTabLeftButton, &opt, q);
    QPoint p = rect.topLeft();
    if ((index == pressedIndex) || paintWithOffsets) {
      if (vertical)
        p.setY(p.y() + tabList[index].dragOffset);
      else
        p.setX(p.x() + tabList[index].dragOffset);
    }
    tab.leftWidget->move(p);
  }
  if (tab.rightWidget) {
    QRect rect = q->style()->subElementRect(QStyle::SE_TabBarTabRightButton, &opt, q);
    QPoint p = rect.topLeft();
    if ((index == pressedIndex) || paintWithOffsets) {
      if (vertical)
        p.setY(p.y() + tab.dragOffset);
      else
        p.setX(p.x() + tab.dragOffset);
    }
    tab.rightWidget->move(p);
  }
}

void iTabBarMultiRowPrivate::layoutWidgets(int start)
{
  Q_Q(iTabBarMultiRow);
  for (int i = start; i < q->count(); ++i) {
    layoutTab(i);
  }
}

void iTabBarMultiRowPrivate::autoHideTabs()
{
  Q_Q(iTabBarMultiRow);

  if (autoHide)
    q->setVisible(q->count() > 1);
}

void iTabBarMultiRow::_q_closeTab()
{
  dpriv->_q_closeTab();
}

void iTabBarMultiRowPrivate::_q_closeTab()
{
  Q_Q(iTabBarMultiRow);
  QObject *object = q->sender();
  int tabToClose = -1;
  iTabBarMultiRow::ButtonPosition closeSide = (iTabBarMultiRow::ButtonPosition)q->style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, q);
  for (int i = 0; i < tabList.count(); ++i) {
    if (closeSide == iTabBarMultiRow::LeftSide) {
      if (tabList.at(i).leftWidget == object) {
        tabToClose = i;
        break;
      }
    } else {
      if (tabList.at(i).rightWidget == object) {
        tabToClose = i;
        break;
      }
    }
  }
  if (tabToClose != -1)
    emit q->tabCloseRequested(tabToClose);
}

void iTabBarMultiRow::_q_scrollTabs()
{
  dpriv->_q_scrollTabs();
}

void iTabBarMultiRowPrivate::_q_scrollTabs()
{
  Q_Q(iTabBarMultiRow);
  const QObject *sender = q->sender();
  int i = -1;
  if (!verticalTabs(shape)) {
    if (sender == leftB) {
      for (i = tabList.count() - 1; i >= 0; --i) {
        if (tabList.at(i).rect.left() - scrollOffset < 0) {
          makeVisible(i);
          return;
        }
      }
    } else if (sender == rightB) {
      int availableWidth = q->width() - extraWidth();
      for (i = 0; i < tabList.count(); ++i) {
        if (tabList.at(i).rect.right() - scrollOffset > availableWidth) {
          makeVisible(i);
          return;
        }
      }
    }
  } else { // vertical
    if (sender == leftB) {
      for (i = tabList.count() - 1; i >= 0; --i) {
        if (tabList.at(i).rect.top() - scrollOffset < 0) {
          makeVisible(i);
          return;
        }
      }
    } else if (sender == rightB) {
      int available = q->height() - extraWidth();
      for (i = 0; i < tabList.count(); ++i) {
        if (tabList.at(i).rect.bottom() - scrollOffset > available) {
          makeVisible(i);
          return;
        }
      }
    }
  }
}

void iTabBarMultiRowPrivate::refresh()
{
  Q_Q(iTabBarMultiRow);

  // be safe in case a subclass is also handling move with the tabs
  if (pressedIndex != -1
      && movable
      && QApplication::mouseButtons() == Qt::NoButton) {
    moveTabFinished(pressedIndex);
    if (!validIndex(pressedIndex))
      pressedIndex = -1;
  }

  if (!q->isVisible()) {
    layoutDirty = true;
  } else {
    layoutTabs();
    makeVisible(currentIndex);
    q->update();
    q->updateGeometry();
  }
}

/*!
  Creates a new tab bar with the given \a parent.
*/
iTabBarMultiRow::iTabBarMultiRow(QWidget* parent)
  :QTabBar(parent)
{
  dpriv = new iTabBarMultiRowPrivate;
  dpriv->q_obj = this;
  dpriv->init();
}


/*!
  Destroys the tab bar.
*/
iTabBarMultiRow::~iTabBarMultiRow()
{
}

/*!
  \property iTabBarMultiRow::shape
  \brief the shape of the tabs in the tab bar

  Possible values for this property are described by the Shape enum.
*/


iTabBarMultiRow::Shape iTabBarMultiRow::shape() const
{
  return dpriv->shape;
}

void iTabBarMultiRow::setShape(Shape shape)
{
  if (dpriv->shape == shape)
    return;
  dpriv->shape = shape;
  QTabBar::setShape(shape);
}

/*!
  \property iTabBarMultiRow::drawBase
  \brief defines whether or not tab bar should draw its base.

  If true then iTabBarMultiRow draws a base in relation to the styles overlab.
  Otherwise only the tabs are drawn.

  \sa QStyle::pixelMetric(), QStyle::PM_TabBarBaseOverlap, QStyleOptionTabBarBase
*/

void iTabBarMultiRow::setDrawBase(bool drawBase)
{
  if (dpriv->drawBase == drawBase)
    return;
  dpriv->drawBase = drawBase;
  QTabBar::setDrawBase(drawBase);
}

bool iTabBarMultiRow::drawBase() const
{
  return dpriv->drawBase;
}

/*!
  Adds a new tab with text \a text. Returns the new
  tab's index.
*/
int iTabBarMultiRow::addTab(const QString &text)
{
  return insertTab(-1, text);
}

/*!
  \overload

  Adds a new tab with icon \a icon and text \a
  text. Returns the new tab's index.
*/
int iTabBarMultiRow::addTab(const QIcon& icon, const QString &text)
{
  return insertTab(-1, icon, text);
}

/*!
  Inserts a new tab with text \a text at position \a index. If \a
  index is out of range, the new tab is appened. Returns the new
  tab's index.
*/
int iTabBarMultiRow::insertTab(int index, const QString &text)
{
  return insertTab(index, QIcon(), text);
}

/*!\overload

  Inserts a new tab with icon \a icon and text \a text at position
  \a index. If \a index is out of range, the new tab is
  appended. Returns the new tab's index.

  If the iTabBarMultiRow was empty before this function is called, the inserted tab
  becomes the current tab.

  Inserting a new tab at an index less than or equal to the current index
  will increment the current index, but keep the current tab.
*/
int iTabBarMultiRow::insertTab(int index, const QIcon& icon, const QString &text)
{
  if (!dpriv->validIndex(index)) {
    index = dpriv->tabList.count();
    dpriv->tabList.append(iTabBarMultiRowPrivate::Tab(icon, text));
  } else {
    dpriv->tabList.insert(index, iTabBarMultiRowPrivate::Tab(icon, text));
  }
#ifndef QT_NO_SHORTCUT
  dpriv->tabList[index].shortcutId = grabShortcut(QKeySequence::mnemonic(text));
#endif
  dpriv->refresh();
  if (dpriv->tabList.count() == 1)
    setCurrentIndex(index);
  else if (index <= dpriv->currentIndex)
    ++dpriv->currentIndex;

  if (dpriv->closeButtonOnTabs) {
    QStyleOptionTab opt;
    initStyleOption(&opt, index);
    ButtonPosition closeSide = (ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
    QAbstractButton *closeButton = new iTabBarCloseButton(this);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(_q_closeTab()));
    setTabButton(index, closeSide, closeButton);
  }

  for (int i = 0; i < dpriv->tabList.count(); ++i) {
    if (dpriv->tabList[i].lastTab >= index)
      ++dpriv->tabList[i].lastTab;
  }

  tabInserted(index);
  dpriv->autoHideTabs();
  return index;
}


/*!
  Removes the tab at position \a index.

  \sa SelectionBehavior
*/
void iTabBarMultiRow::removeTab(int index)
{
  if (dpriv->validIndex(index)) {
    if (dpriv->dragInProgress)
      dpriv->moveTabFinished(dpriv->pressedIndex);

#ifndef QT_NO_SHORTCUT
    releaseShortcut(dpriv->tabList.at(index).shortcutId);
#endif
    if (dpriv->tabList[index].leftWidget) {
      dpriv->tabList[index].leftWidget->hide();
      dpriv->tabList[index].leftWidget->deleteLater();
      dpriv->tabList[index].leftWidget = 0;
    }
    if (dpriv->tabList[index].rightWidget) {
      dpriv->tabList[index].rightWidget->hide();
      dpriv->tabList[index].rightWidget->deleteLater();
      dpriv->tabList[index].rightWidget = 0;
    }

    int newIndex = dpriv->tabList[index].lastTab;
    dpriv->tabList.removeAt(index);
    for (int i = 0; i < dpriv->tabList.count(); ++i) {
      if (dpriv->tabList[i].lastTab == index)
        dpriv->tabList[i].lastTab = -1;
      if (dpriv->tabList[i].lastTab > index)
        --dpriv->tabList[i].lastTab;
    }
    if (index == dpriv->currentIndex) {
      // The current tab is going away, in order to make sure
      // we emit that "current has changed", we need to reset this
      // around.
      dpriv->currentIndex = -1;
      if (dpriv->tabList.size() > 0) {
        switch(dpriv->selectionBehaviorOnRemove) {
        case SelectPreviousTab:
          if (newIndex > index)
            newIndex--;
          if (dpriv->validIndex(newIndex))
            break;
          // else fallthrough
        case SelectRightTab:
          newIndex = index;
          if (newIndex >= dpriv->tabList.size())
            newIndex = dpriv->tabList.size() - 1;
          break;
        case SelectLeftTab:
          newIndex = index - 1;
          if (newIndex < 0)
            newIndex = 0;
          break;
        default:
          break;
        }

        if (dpriv->validIndex(newIndex)) {
          // don't loose newIndex's old through setCurrentIndex
          int bump = dpriv->tabList[newIndex].lastTab;
          setCurrentIndex(newIndex);
          dpriv->tabList[newIndex].lastTab = bump;
        }
      } else {
        emit currentChanged(-1);
      }
    } else if (index < dpriv->currentIndex) {
      setCurrentIndex(dpriv->currentIndex - 1);
    }
    dpriv->refresh();
    dpriv->autoHideTabs();
    tabRemoved(index);
  }
}


/*!
  Returns \c true if the tab at position \a index is enabled; otherwise
  returns \c false.
*/
bool iTabBarMultiRow::isTabEnabled(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    return tab->enabled;
  return false;
}

/*!
  If \a enabled is true then the tab at position \a index is
  enabled; otherwise the item at position \a index is disabled.
*/
void iTabBarMultiRow::setTabEnabled(int index, bool enabled)
{
  if (iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index)) {
    tab->enabled = enabled;
#ifndef QT_NO_SHORTCUT
    setShortcutEnabled(tab->shortcutId, enabled);
#endif
    update();
    if (!enabled && index == dpriv->currentIndex)
      setCurrentIndex(dpriv->validIndex(index+1)?index+1:0);
    else if (enabled && !dpriv->validIndex(dpriv->currentIndex))
      setCurrentIndex(index);
  }
}


/*!
  Returns the text of the tab at position \a index, or an empty
  string if \a index is out of range.
*/
QString iTabBarMultiRow::tabText(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    return tab->text;
  return QString();
}

/*!
  Sets the text of the tab at position \a index to \a text.
*/
void iTabBarMultiRow::setTabText(int index, const QString &text)
{
  if (iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index)) {
    tab->text = text;
#ifndef QT_NO_SHORTCUT
    releaseShortcut(tab->shortcutId);
    tab->shortcutId = grabShortcut(QKeySequence::mnemonic(text));
    setShortcutEnabled(tab->shortcutId, tab->enabled);
#endif
    dpriv->refresh();
  }
}

/*!
  Returns the text color of the tab with the given \a index, or a invalid
  color if \a index is out of range.

  \sa setTabTextColor()
*/
QColor iTabBarMultiRow::tabTextColor(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    return tab->textColor;
  return QColor();
}

/*!
  Sets the color of the text in the tab with the given \a index to the specified \a color.

  If an invalid color is specified, the tab will use the iTabBarMultiRow foreground role instead.

  \sa tabTextColor()
*/
void iTabBarMultiRow::setTabTextColor(int index, const QColor &color)
{
  if (iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index)) {
    tab->textColor = color;
    update(tabRect(index));
  }
}

/*!
  Returns the icon of the tab at position \a index, or a null icon
  if \a index is out of range.
*/
QIcon iTabBarMultiRow::tabIcon(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    return tab->icon;
  return QIcon();
}

/*!
  Sets the icon of the tab at position \a index to \a icon.
*/
void iTabBarMultiRow::setTabIcon(int index, const QIcon & icon)
{
  if (iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index)) {
    bool simpleIconChange = (!icon.isNull() && !tab->icon.isNull());
    tab->icon = icon;
    if (simpleIconChange)
      update(tabRect(index));
    else
      dpriv->refresh();
  }
}

#ifndef QT_NO_TOOLTIP
/*!
  Sets the tool tip of the tab at position \a index to \a tip.
*/
void iTabBarMultiRow::setTabToolTip(int index, const QString & tip)
{
  if (iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    tab->toolTip = tip;
}

/*!
  Returns the tool tip of the tab at position \a index, or an empty
  string if \a index is out of range.
*/
QString iTabBarMultiRow::tabToolTip(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    return tab->toolTip;
  return QString();
}
#endif // QT_NO_TOOLTIP

#ifndef QT_NO_WHATSTHIS
/*!
  \since 4.1

  Sets the What's This help text of the tab at position \a index
  to \a text.
*/
void iTabBarMultiRow::setTabWhatsThis(int index, const QString &text)
{
  if (iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    tab->whatsThis = text;
}

/*!
  \since 4.1

  Returns the What's This help text of the tab at position \a index,
  or an empty string if \a index is out of range.
*/
QString iTabBarMultiRow::tabWhatsThis(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    return tab->whatsThis;
  return QString();
}

#endif // QT_NO_WHATSTHIS

/*!
  Sets the data of the tab at position \a index to \a data.
*/
void iTabBarMultiRow::setTabData(int index, const QVariant & data)
{
  if (iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    tab->data = data;
}

/*!
  Returns the data of the tab at position \a index, or a null
  variant if \a index is out of range.
*/
QVariant iTabBarMultiRow::tabData(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index))
    return tab->data;
  return QVariant();
}

/*!
  Returns the visual rectangle of the tab at position \a
  index, or a null rectangle if \a index is out of range.
*/
QRect iTabBarMultiRow::tabRect(int index) const
{
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index)) {
    if (dpriv->layoutDirty)
      const_cast<iTabBarMultiRowPrivate*>(dpriv)->layoutTabs();
    QRect r = tab->rect;
    if (verticalTabs(dpriv->shape))
      r.translate(0, -dpriv->scrollOffset);
    else
      r.translate(-dpriv->scrollOffset, 0);
    if (!verticalTabs(dpriv->shape))
      r = QStyle::visualRect(layoutDirection(), rect(), r);
    return r;
  }
  return QRect();
}

/*!
  \since 4.3
  Returns the index of the tab that covers \a position or -1 if no
  tab covers \a position;
*/

int iTabBarMultiRow::tabAt(const QPoint &position) const
{
  if (dpriv->validIndex(dpriv->currentIndex)
      && tabRect(dpriv->currentIndex).contains(position)) {
    return dpriv->currentIndex;
  }
  const int max = dpriv->tabList.size();
  for (int i = 0; i < max; ++i) {
    if (tabRect(i).contains(position)) {
      return i;
    }
  }
  return -1;
}

/*!
  \property iTabBarMultiRow::currentIndex
  \brief the index of the tab bar's visible tab

  The current index is -1 if there is no current tab.
*/

int iTabBarMultiRow::currentIndex() const
{
  if (dpriv->validIndex(dpriv->currentIndex))
    return dpriv->currentIndex;
  return -1;
}


void iTabBarMultiRow::setCurrentIndex(int index)
{
  if (dpriv->dragInProgress && dpriv->pressedIndex != -1)
    return;

  int oldIndex = dpriv->currentIndex;
  if (dpriv->validIndex(index) && dpriv->currentIndex != index) {
    dpriv->currentIndex = index;
    update();
    dpriv->makeVisible(index);
    dpriv->tabList[index].lastTab = oldIndex;
    if (oldIndex >= 0 && oldIndex < count())
      dpriv->layoutTab(oldIndex);
    dpriv->layoutTab(index);
#ifndef QT_NO_ACCESSIBILITY
    if (QAccessible::isActive()) {
      if (hasFocus()) {
        QAccessibleEvent focusEvent(this, QAccessible::Focus);
        focusEvent.setChild(index);
        QAccessible::updateAccessibility(&focusEvent);
      }
      QAccessibleEvent selectionEvent(this, QAccessible::Selection);
      selectionEvent.setChild(index);
      QAccessible::updateAccessibility(&selectionEvent);
    }
#endif
    emit currentChanged(index);
  }
}

/*!
  \property iTabBarMultiRow::iconSize
  \brief The size for icons in the tab bar
  \since 4.1

  The default value is style-dependent. \c iconSize is a maximum
  size; icons that are smaller are not scaled up.

  \sa QTabWidget::iconSize
*/
QSize iTabBarMultiRow::iconSize() const
{
  if (dpriv->iconSize.isValid())
    return dpriv->iconSize;
  int iconExtent = style()->pixelMetric(QStyle::PM_TabBarIconSize, 0, this);
  return QSize(iconExtent, iconExtent);

}

void iTabBarMultiRow::setIconSize(const QSize &size)
{
  dpriv->iconSize = size;
  dpriv->layoutDirty = true;
  update();
  updateGeometry();
}

/*!
  \property iTabBarMultiRow::count
  \brief the number of tabs in the tab bar
*/

int iTabBarMultiRow::count() const
{
  return dpriv->tabList.count();
}


/*!\reimp
 */
QSize iTabBarMultiRow::sizeHint() const
{
  if (dpriv->layoutDirty)
    const_cast<iTabBarMultiRowPrivate*>(dpriv)->layoutTabs();
  QRect r;
  for (int i = 0; i < dpriv->tabList.count(); ++i)
    r = r.united(dpriv->tabList.at(i).maxRect);
  QSize sz = QApplication::globalStrut();
  return r.size().expandedTo(sz);
}

/*!\reimp
 */
QSize iTabBarMultiRow::minimumSizeHint() const
{
  if (dpriv->layoutDirty)
    const_cast<iTabBarMultiRowPrivate*>(dpriv)->layoutTabs();
  if (!dpriv->useScrollButtons) {
    QRect r;
    for (int i = 0; i < dpriv->tabList.count(); ++i)
      r = r.united(dpriv->tabList.at(i).minRect);
    return r.size().expandedTo(QApplication::globalStrut());
  }
  if (verticalTabs(dpriv->shape))
    return QSize(sizeHint().width(), dpriv->rightB->sizeHint().height() * 2 + 75);
  else
    return QSize(dpriv->rightB->sizeHint().width() * 2 + 75, sizeHint().height());
}

// Compute the most-elided possible text, for minimumSizeHint
static QString computeElidedText(Qt::TextElideMode mode, const QString &text)
{
  if (text.length() <= 3)
    return text;

  static const QLatin1String Ellipses("...");
  QString ret;
  switch (mode) {
  case Qt::ElideRight:
    ret = text.left(2) + Ellipses;
    break;
  case Qt::ElideMiddle:
    ret = text.left(1) + Ellipses + text.right(1);
    break;
  case Qt::ElideLeft:
    ret = Ellipses + text.right(2);
    break;
  case Qt::ElideNone:
    ret = text;
    break;
  }
  return ret;
}

/*!
  Returns the minimum tab size hint for the tab at position \a index.
  \since Qt 5.0
*/

QSize iTabBarMultiRow::minimumTabSizeHint(int index) const
{
  iTabBarMultiRowPrivate::Tab &tab = const_cast<iTabBarMultiRowPrivate::Tab&>(dpriv->tabList[index]);
  QString oldText = tab.text;
  tab.text = computeElidedText(dpriv->elideMode, oldText);
  QSize size = tabSizeHint(index);
  tab.text = oldText;
  return size;
}

/*!
  Returns the size hint for the tab at position \a index.
*/
QSize iTabBarMultiRow::tabSizeHint(int index) const
{
  //Note: this must match with the computations in QCommonStylePrivate::tabLayout
  if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(index)) {
    QStyleOptionTab opt;
    initStyleOption(&opt, index);
    opt.text = dpriv->tabList.at(index).text;
    QSize iconSize = tab->icon.isNull() ? QSize(0, 0) : opt.iconSize;
    int hframe = style()->pixelMetric(QStyle::PM_TabBarTabHSpace, &opt, this);
    int vframe = style()->pixelMetric(QStyle::PM_TabBarTabVSpace, &opt, this);
    const QFontMetrics fm = fontMetrics();

    int maxWidgetHeight = qMax(opt.leftButtonSize.height(), opt.rightButtonSize.height());
    int maxWidgetWidth = qMax(opt.leftButtonSize.width(), opt.rightButtonSize.width());

    int widgetWidth = 0;
    int widgetHeight = 0;
    int padding = 0;
    if (!opt.leftButtonSize.isEmpty()) {
      padding += 4;
      widgetWidth += opt.leftButtonSize.width();
      widgetHeight += opt.leftButtonSize.height();
    }
    if (!opt.rightButtonSize.isEmpty()) {
      padding += 4;
      widgetWidth += opt.rightButtonSize.width();
      widgetHeight += opt.rightButtonSize.height();
    }
    if (!opt.icon.isNull())
      padding += 4;

    QSize csz;
    if (verticalTabs(dpriv->shape)) {
      csz = QSize( qMax(maxWidgetWidth, qMax(fm.height(), iconSize.height())) + vframe,
                   fm.size(Qt::TextShowMnemonic, tab->text).width() + iconSize.width() + hframe + widgetHeight + padding);
    } else {
      csz = QSize(fm.size(Qt::TextShowMnemonic, tab->text).width() + iconSize.width() + hframe
                  + widgetWidth + padding,
                  qMax(maxWidgetHeight, qMax(fm.height(), iconSize.height())) + vframe);
    }

    QSize retSize = style()->sizeFromContents(QStyle::CT_TabBarTab, &opt, csz, this);
    return retSize;
  }
  return QSize();
}

/*!
  This virtual handler is called after a new tab was added or
  inserted at position \a index.

  \sa tabRemoved()
*/
void iTabBarMultiRow::tabInserted(int index)
{
  Q_UNUSED(index)
    }

/*!
  This virtual handler is called after a tab was removed from
  position \a index.

  \sa tabInserted()
*/
void iTabBarMultiRow::tabRemoved(int index)
{
  Q_UNUSED(index)
    }

/*!
  This virtual handler is called whenever the tab layout changes.

  \sa tabRect()
*/
void iTabBarMultiRow::tabLayoutChange()
{
}


/*!\reimp
 */
void iTabBarMultiRow::showEvent(QShowEvent *)
{
  if (dpriv->layoutDirty)
    dpriv->refresh();
  if (!dpriv->validIndex(dpriv->currentIndex))
    setCurrentIndex(0);
  dpriv->updateMacBorderMetrics();
}

/*!\reimp
 */
void iTabBarMultiRow::hideEvent(QHideEvent *)
{
  dpriv->updateMacBorderMetrics();
}

/*!\reimp
 */
bool iTabBarMultiRow::event(QEvent *event)
{
  if (event->type() == QEvent::HoverMove
      || event->type() == QEvent::HoverEnter) {
    QHoverEvent *he = static_cast<QHoverEvent *>(event);
    if (!dpriv->hoverRect.contains(he->pos())) {
      QRect oldHoverRect = dpriv->hoverRect;
      for (int i = 0; i < dpriv->tabList.count(); ++i) {
        QRect area = tabRect(i);
        if (area.contains(he->pos())) {
          dpriv->hoverRect = area;
          break;
        }
      }
      if (he->oldPos() != QPoint(-1, -1))
        update(oldHoverRect);
      update(dpriv->hoverRect);
    }
    return true;
  } else if (event->type() == QEvent::HoverLeave ) {
    QRect oldHoverRect = dpriv->hoverRect;
    dpriv->hoverRect = QRect();
    update(oldHoverRect);
    return true;
#ifndef QT_NO_TOOLTIP
  } else if (event->type() == QEvent::ToolTip) {
    if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(tabAt(static_cast<QHelpEvent*>(event)->pos()))) {
      if (!tab->toolTip.isEmpty()) {
        QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tab->toolTip, this);
        return true;
      }
    }
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
  } else if (event->type() == QEvent::QueryWhatsThis) {
    const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(dpriv->indexAtPos(static_cast<QHelpEvent*>(event)->pos()));
    if (!tab || tab->whatsThis.isEmpty())
      event->ignore();
    return true;
  } else if (event->type() == QEvent::WhatsThis) {
    if (const iTabBarMultiRowPrivate::Tab *tab = dpriv->at(dpriv->indexAtPos(static_cast<QHelpEvent*>(event)->pos()))) {
      if (!tab->whatsThis.isEmpty()) {
        QWhatsThis::showText(static_cast<QHelpEvent*>(event)->globalPos(),
                             tab->whatsThis, this);
        return true;
      }
    }
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_SHORTCUT
  } else if (event->type() == QEvent::Shortcut) {
    QShortcutEvent *se = static_cast<QShortcutEvent *>(event);
    for (int i = 0; i < dpriv->tabList.count(); ++i) {
      const iTabBarMultiRowPrivate::Tab *tab = &dpriv->tabList.at(i);
      if (tab->shortcutId == se->shortcutId()) {
        setCurrentIndex(i);
        return true;
      }
    }
#endif
  } else if (event->type() == QEvent::MouseButtonDblClick) { // ### fixme Qt 6: move to mouseDoubleClickEvent(), here for BC reasons.
    const QPoint pos = static_cast<const QMouseEvent *>(event)->pos();
    const bool isEventInCornerButtons = (!dpriv->leftB->isHidden() && dpriv->leftB->geometry().contains(pos))
      || (!dpriv->rightB->isHidden() && dpriv->rightB->geometry().contains(pos));
    if (!isEventInCornerButtons)
      emit tabBarDoubleClicked(tabAt(pos));
  } else if (event->type() == QEvent::Move) {
    dpriv->updateMacBorderMetrics();
    return QWidget::event(event);

#ifndef QT_NO_DRAGANDDROP
  } else if (event->type() == QEvent::DragEnter) {
    if (dpriv->changeCurrentOnDrag)
      event->accept();
  } else if (event->type() == QEvent::DragMove) {
    if (dpriv->changeCurrentOnDrag) {
      const int tabIndex = tabAt(static_cast<QDragMoveEvent *>(event)->pos());
      if (isTabEnabled(tabIndex) && dpriv->switchTabCurrentIndex != tabIndex) {
        dpriv->switchTabCurrentIndex = tabIndex;
        if (dpriv->switchTabTimerId)
          killTimer(dpriv->switchTabTimerId);
        dpriv->switchTabTimerId = startTimer(style()->styleHint(QStyle::SH_TabBar_ChangeCurrentDelay));
      }
      event->ignore();
    }
  } else if (event->type() == QEvent::DragLeave || event->type() == QEvent::Drop) {
    dpriv->killSwitchTabTimer();
    event->ignore();
#endif
  }
  return QWidget::event(event);
}

/*!\reimp
 */
void iTabBarMultiRow::resizeEvent(QResizeEvent *)
{
  if (dpriv->layoutDirty)
    updateGeometry();
  dpriv->layoutTabs();

  dpriv->makeVisible(dpriv->currentIndex);
}

/*!\reimp
 */
void iTabBarMultiRow::paintEvent(QPaintEvent *)
{

  QStyleOptionTabBarBase optTabBase;
  iTabBarMultiRowPrivate::initStyleBaseOption(&optTabBase, this, size());

  QStylePainter p(this);
  int selected = -1;
  int cut = -1;
  bool rtl = optTabBase.direction == Qt::RightToLeft;
  bool vertical = verticalTabs(dpriv->shape);
  QStyleOptionTab cutTab;
  selected = dpriv->currentIndex;
  if (dpriv->dragInProgress)
    selected = dpriv->pressedIndex;

  for (int i = 0; i < dpriv->tabList.count(); ++i)
    optTabBase.tabBarRect |= tabRect(i);

  optTabBase.selectedTabRect = tabRect(selected);

  if (dpriv->drawBase)
    p.drawPrimitive(QStyle::PE_FrameTabBarBase, optTabBase);

  for (int i = 0; i < dpriv->tabList.count(); ++i) {
    QStyleOptionTab tab;
    initStyleOption(&tab, i);
    if (dpriv->paintWithOffsets && dpriv->tabList[i].dragOffset != 0) {
      if (vertical) {
        tab.rect.moveTop(tab.rect.y() + dpriv->tabList[i].dragOffset);
      } else {
        tab.rect.moveLeft(tab.rect.x() + dpriv->tabList[i].dragOffset);
      }
    }
    if (!(tab.state & QStyle::State_Enabled)) {
      tab.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    // If this tab is partially obscured, make a note of it so that we can pass the information
    // along when we draw the tear.
    if (((!vertical && (!rtl && tab.rect.left() < 0)) || (rtl && tab.rect.right() > width()))
        || (vertical && tab.rect.top() < 0)) {
      cut = i;
      cutTab = tab;
    }
    // Don't bother drawing a tab if the entire tab is outside of the visible tab bar.
    if ((!vertical && (tab.rect.right() < 0 || tab.rect.left() > width()))
        || (vertical && (tab.rect.bottom() < 0 || tab.rect.top() > height())))
      continue;

    optTabBase.tabBarRect |= tab.rect;
    if (i == selected)
      continue;

    p.drawControl(QStyle::CE_TabBarTab, tab);
  }

  // Draw the selected tab last to get it "on top"
  if (selected >= 0) {
    QStyleOptionTab tab;
    initStyleOption(&tab, selected);
    if (dpriv->paintWithOffsets && dpriv->tabList[selected].dragOffset != 0) {
      if (vertical)
        tab.rect.moveTop(tab.rect.y() + dpriv->tabList[selected].dragOffset);
      else
        tab.rect.moveLeft(tab.rect.x() + dpriv->tabList[selected].dragOffset);
    }
    if (!dpriv->dragInProgress)
      p.drawControl(QStyle::CE_TabBarTab, tab);
    else {
      int taboverlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, 0, this);
      dpriv->movingTab->setGeometry(tab.rect.adjusted(-taboverlap, 0, taboverlap, 0));
    }
  }

  // Only draw the tear indicator if necessary. Most of the time we don't need too.
  if (dpriv->leftB->isVisible() && cut >= 0) {
    cutTab.rect = rect();
    cutTab.rect = style()->subElementRect(QStyle::SE_TabBarTearIndicator, &cutTab, this);
    p.drawPrimitive(QStyle::PE_IndicatorTabTear, cutTab);
  }
}

/*
  Given that index at position from moved to position to where return where index goes.
*/
int iTabBarMultiRowPrivate::calculateNewPosition(int from, int to, int index) const
{
  if (index == from)
    return to;

  int start = qMin(from, to);
  int end = qMax(from, to);
  if (index >= start && index <= end)
    index += (from < to) ? -1 : 1;
  return index;
}

/*!
  Moves the item at index position \a from to index position \a to.
  \since 4.5

  \sa tabMoved(), tabLayoutChange()
*/
void iTabBarMultiRow::moveTab(int from, int to)
{
  if (from == to
      || !dpriv->validIndex(from)
      || !dpriv->validIndex(to))
    return;

  bool vertical = verticalTabs(dpriv->shape);
  int oldPressedPosition = 0;
  if (dpriv->pressedIndex != -1) {
    // Record the position of the pressed tab before reordering the tabs.
    oldPressedPosition = vertical ? dpriv->tabList[dpriv->pressedIndex].rect.y()
      : dpriv->tabList[dpriv->pressedIndex].rect.x();
  }

  // Update the locations of the tabs first
  int start = qMin(from, to);
  int end = qMax(from, to);
  int width = vertical ? dpriv->tabList[from].rect.height() : dpriv->tabList[from].rect.width();
  if (from < to)
    width *= -1;
  bool rtl = isRightToLeft();
  for (int i = start; i <= end; ++i) {
    if (i == from)
      continue;
    if (vertical)
      dpriv->tabList[i].rect.moveTop(dpriv->tabList[i].rect.y() + width);
    else
      dpriv->tabList[i].rect.moveLeft(dpriv->tabList[i].rect.x() + width);
    int direction = -1;
    if (rtl && !vertical)
      direction *= -1;
    if (dpriv->tabList[i].dragOffset != 0)
      dpriv->tabList[i].dragOffset += (direction * width);
  }

  if (vertical) {
    if (from < to)
      dpriv->tabList[from].rect.moveTop(dpriv->tabList[to].rect.bottom() + 1);
    else
      dpriv->tabList[from].rect.moveTop(dpriv->tabList[to].rect.top() - width);
  } else {
    if (from < to)
      dpriv->tabList[from].rect.moveLeft(dpriv->tabList[to].rect.right() + 1);
    else
      dpriv->tabList[from].rect.moveLeft(dpriv->tabList[to].rect.left() - width);
  }

  // Move the actual data structures
  dpriv->tabList.move(from, to);

  // update lastTab locations
  for (int i = 0; i < dpriv->tabList.count(); ++i)
    dpriv->tabList[i].lastTab = dpriv->calculateNewPosition(from, to, dpriv->tabList[i].lastTab);

  // update external variables
  int previousIndex = dpriv->currentIndex;
  dpriv->currentIndex = dpriv->calculateNewPosition(from, to, dpriv->currentIndex);

  // If we are in the middle of a drag update the dragStartPosition
  if (dpriv->pressedIndex != -1) {
    dpriv->pressedIndex = dpriv->calculateNewPosition(from, to, dpriv->pressedIndex);
    int newPressedPosition = vertical ? dpriv->tabList[dpriv->pressedIndex].rect.top() : dpriv->tabList[dpriv->pressedIndex].rect.left();
    int diff = oldPressedPosition - newPressedPosition;
    if (isRightToLeft() && !vertical)
      diff *= -1;
    if (vertical)
      dpriv->dragStartPosition.setY(dpriv->dragStartPosition.y() - diff);
    else
      dpriv->dragStartPosition.setX(dpriv->dragStartPosition.x() - diff);
  }

  dpriv->layoutWidgets(start);
  update();
  emit tabMoved(from, to);
  if (previousIndex != dpriv->currentIndex)
    emit currentChanged(dpriv->currentIndex);
  emit tabLayoutChange();
}

void iTabBarMultiRowPrivate::slide(int from, int to)
{
  Q_Q(iTabBarMultiRow);
  if (from == to
      || !validIndex(from)
      || !validIndex(to))
    return;
  bool vertical = verticalTabs(shape);
  int preLocation = vertical ? q->tabRect(from).y() : q->tabRect(from).x();
  q->setUpdatesEnabled(false);
  q->moveTab(from, to);
  q->setUpdatesEnabled(true);
  int postLocation = vertical ? q->tabRect(to).y() : q->tabRect(to).x();
  int length = postLocation - preLocation;
  tabList[to].dragOffset -= length;
  tabList[to].startAnimation(this, ANIMATION_DURATION);
}

void iTabBarMultiRowPrivate::moveTab(int index, int offset)
{
  if (!validIndex(index))
    return;
  tabList[index].dragOffset = offset;
  layoutTab(index); // Make buttons follow tab
  q_func()->update();
}

/*!\reimp
 */
void iTabBarMultiRow::mousePressEvent(QMouseEvent *event)
{

  const QPoint pos = event->pos();
  const bool isEventInCornerButtons = (!dpriv->leftB->isHidden() && dpriv->leftB->geometry().contains(pos))
    || (!dpriv->rightB->isHidden() && dpriv->rightB->geometry().contains(pos));
  if (!isEventInCornerButtons) {
    const int index = dpriv->indexAtPos(pos);
    emit tabBarClicked(index);
  }

  if (event->button() != Qt::LeftButton) {
    event->ignore();
    return;
  }
  // Be safe!
  if (dpriv->pressedIndex != -1 && dpriv->movable)
    dpriv->moveTabFinished(dpriv->pressedIndex);

  dpriv->pressedIndex = dpriv->indexAtPos(event->pos());
  if (dpriv->validIndex(dpriv->pressedIndex)) {
    QStyleOptionTabBarBase optTabBase;
    optTabBase.init(this);
    optTabBase.documentMode = dpriv->documentMode;
    if (event->type() == style()->styleHint(QStyle::SH_TabBar_SelectMouseType, &optTabBase, this))
      setCurrentIndex(dpriv->pressedIndex);
    else
      repaint(tabRect(dpriv->pressedIndex));
    if (dpriv->movable) {
      dpriv->dragStartPosition = event->pos();
    }
  }
}

/*!\reimp
 */
void iTabBarMultiRow::mouseMoveEvent(QMouseEvent *event)
{
  if (dpriv->movable) {
    // Be safe!
    if (dpriv->pressedIndex != -1
        && event->buttons() == Qt::NoButton)
      dpriv->moveTabFinished(dpriv->pressedIndex);

    // Start drag
    if (!dpriv->dragInProgress && dpriv->pressedIndex != -1) {
      if ((event->pos() - dpriv->dragStartPosition).manhattanLength() > QApplication::startDragDistance()) {
        dpriv->dragInProgress = true;
        dpriv->setupMovableTab();
      }
    }

    int offset = (event->pos() - dpriv->dragStartPosition).manhattanLength();
    if (event->buttons() == Qt::LeftButton
        && offset > QApplication::startDragDistance()
        && dpriv->validIndex(dpriv->pressedIndex)) {
      bool vertical = verticalTabs(dpriv->shape);
      int dragDistance;
      if (vertical) {
        dragDistance = (event->pos().y() - dpriv->dragStartPosition.y());
      } else {
        dragDistance = (event->pos().x() - dpriv->dragStartPosition.x());
      }
      dpriv->tabList[dpriv->pressedIndex].dragOffset = dragDistance;

      QRect startingRect = tabRect(dpriv->pressedIndex);
      if (vertical)
        startingRect.moveTop(startingRect.y() + dragDistance);
      else
        startingRect.moveLeft(startingRect.x() + dragDistance);

      int overIndex;
      if (dragDistance < 0)
        overIndex = tabAt(startingRect.topLeft());
      else
        overIndex = tabAt(startingRect.topRight());

      if (overIndex != dpriv->pressedIndex && overIndex != -1) {
        int offset = 1;
        if (isRightToLeft() && !vertical)
          offset *= -1;
        if (dragDistance < 0) {
          dragDistance *= -1;
          offset *= -1;
        }
        for (int i = dpriv->pressedIndex;
             offset > 0 ? i < overIndex : i > overIndex;
             i += offset) {
          QRect overIndexRect = tabRect(overIndex);
          int needsToBeOver = (vertical ? overIndexRect.height() : overIndexRect.width()) / 2;
          if (dragDistance > needsToBeOver)
            dpriv->slide(i + offset, dpriv->pressedIndex);
        }
      }
      // Buttons needs to follow the dragged tab
      dpriv->layoutTab(dpriv->pressedIndex);

      update();
    }
  }

  if (event->buttons() != Qt::LeftButton) {
    event->ignore();
    return;
  }
  QStyleOptionTabBarBase optTabBase;
  optTabBase.init(this);
  optTabBase.documentMode = dpriv->documentMode;
}

void iTabBarMultiRowPrivate::setupMovableTab()
{
  Q_Q(iTabBarMultiRow);
  if (!movingTab)
    movingTab = new QMovableTabWidget(q);

  int taboverlap = q->style()->pixelMetric(QStyle::PM_TabBarTabOverlap, 0 ,q);
  QRect grabRect = q->tabRect(pressedIndex);
  grabRect.adjust(-taboverlap, 0, taboverlap, 0);

  QPixmap grabImage(grabRect.size() * q->devicePixelRatioF());
  grabImage.setDevicePixelRatio(q->devicePixelRatioF());
  grabImage.fill(Qt::transparent);
  QStylePainter p(&grabImage, q);
  p.initFrom(q);

  QStyleOptionTab tab;
  q->initStyleOption(&tab, pressedIndex);
  tab.rect.moveTopLeft(QPoint(taboverlap, 0));
  p.drawControl(QStyle::CE_TabBarTab, tab);
  p.end();

  movingTab->setPixmap(grabImage);
  movingTab->setGeometry(grabRect);
  movingTab->raise();

  // Re-arrange widget order to avoid overlaps
  if (tabList[pressedIndex].leftWidget)
    tabList[pressedIndex].leftWidget->raise();
  if (tabList[pressedIndex].rightWidget)
    tabList[pressedIndex].rightWidget->raise();
  if (leftB)
    leftB->raise();
  if (rightB)
    rightB->raise();
  movingTab->setVisible(true);
}

void iTabBarMultiRowPrivate::moveTabFinished(int index)
{
  Q_Q(iTabBarMultiRow);
  bool cleanup = (pressedIndex == index) || (pressedIndex == -1) || !validIndex(index);
  bool allAnimationsFinished = true;
#ifndef QT_NO_ANIMATION
  for(int i = 0; allAnimationsFinished && i < tabList.count(); ++i) {
    const Tab &t = tabList.at(i);
    if (t.animation && t.animation->state() == QAbstractAnimation::Running)
      allAnimationsFinished = false;
  }
#endif //QT_NO_ANIMATION
  if (allAnimationsFinished && cleanup) {
    if(movingTab)
      movingTab->setVisible(false); // We might not get a mouse release
    for (int i = 0; i < tabList.count(); ++i) {
      tabList[i].dragOffset = 0;
    }
    if (pressedIndex != -1 && movable) {
      pressedIndex = -1;
      dragInProgress = false;
      dragStartPosition = QPoint();
    }
    layoutWidgets();
  } else {
    if (!validIndex(index))
      return;
    tabList[index].dragOffset = 0;
  }
  q->update();
}

/*!\reimp
 */
void iTabBarMultiRow::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() != Qt::LeftButton) {
    event->ignore();
    return;
  }
  if (dpriv->movable && dpriv->dragInProgress && dpriv->validIndex(dpriv->pressedIndex)) {
    int length = dpriv->tabList[dpriv->pressedIndex].dragOffset;
    int width = verticalTabs(dpriv->shape)
      ? tabRect(dpriv->pressedIndex).height()
      : tabRect(dpriv->pressedIndex).width();
    int duration = qMin(ANIMATION_DURATION,
                        (qAbs(length) * ANIMATION_DURATION) / width);
    dpriv->tabList[dpriv->pressedIndex].startAnimation(dpriv, duration);
    dpriv->dragInProgress = false;
    dpriv->movingTab->setVisible(false);
    dpriv->dragStartPosition = QPoint();
  }

  int i = dpriv->indexAtPos(event->pos()) == dpriv->pressedIndex ? dpriv->pressedIndex : -1;
  dpriv->pressedIndex = -1;
  QStyleOptionTabBarBase optTabBase;
  optTabBase.initFrom(this);
  optTabBase.documentMode = dpriv->documentMode;
  if (style()->styleHint(QStyle::SH_TabBar_SelectMouseType, &optTabBase, this) == QEvent::MouseButtonRelease)
    setCurrentIndex(i);
}

/*!\reimp
 */
void iTabBarMultiRow::keyPressEvent(QKeyEvent *event)
{
  if (event->key() != Qt::Key_Left && event->key() != Qt::Key_Right) {
    event->ignore();
    return;
  }
  int offset = event->key() == (isRightToLeft() ? Qt::Key_Right : Qt::Key_Left) ? -1 : 1;
  dpriv->setCurrentNextEnabledIndex(offset);
}

/*!\reimp
 */
#ifndef QT_NO_WHEELEVENT
void iTabBarMultiRow::wheelEvent(QWheelEvent *event)
{
#ifndef Q_OS_MAC
  int offset = event->delta() > 0 ? -1 : 1;
  dpriv->setCurrentNextEnabledIndex(offset);
  QWidget::wheelEvent(event);
#else
  Q_UNUSED(event)
#endif
    }
#endif //QT_NO_WHEELEVENT

void iTabBarMultiRowPrivate::setCurrentNextEnabledIndex(int offset)
{
  Q_Q(iTabBarMultiRow);
  for (int index = currentIndex + offset; validIndex(index); index += offset) {
    if (tabList.at(index).enabled) {
      q->setCurrentIndex(index);
      break;
    }
  }
}

/*!\reimp
 */
void iTabBarMultiRow::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::StyleChange) {
    if (!dpriv->elideModeSetByUser)
      dpriv->elideMode = Qt::TextElideMode(style()->styleHint(QStyle::SH_TabBar_ElideMode, 0, this));
    if (!dpriv->useScrollButtonsSetByUser)
      dpriv->useScrollButtons = !style()->styleHint(QStyle::SH_TabBar_PreferNoArrows, 0, this);
    dpriv->refresh();
  } else if (event->type() == QEvent::FontChange) {
    dpriv->refresh();
  }
  QWidget::changeEvent(event);
}

/*!
  \reimp
*/
void iTabBarMultiRow::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == dpriv->switchTabTimerId) {
    killTimer(dpriv->switchTabTimerId);
    dpriv->switchTabTimerId = 0;
    setCurrentIndex(dpriv->switchTabCurrentIndex);
    dpriv->switchTabCurrentIndex = -1;
  }
  QWidget::timerEvent(event);
}

/*!
  \property iTabBarMultiRow::elideMode
  \brief how to elide text in the tab bar
  \since 4.2

  This property controls how items are elided when there is not
  enough space to show them for a given tab bar size.

  By default the value is style dependent.

  \sa QTabWidget::elideMode, usesScrollButtons, QStyle::SH_TabBar_ElideMode
*/

Qt::TextElideMode iTabBarMultiRow::elideMode() const
{
  return dpriv->elideMode;
}

void iTabBarMultiRow::setElideMode(Qt::TextElideMode mode)
{
  dpriv->elideMode = mode;
  dpriv->elideModeSetByUser = true;
  dpriv->refresh();
}

/*!
  \property iTabBarMultiRow::usesScrollButtons
  \brief Whether or not a tab bar should use buttons to scroll tabs when it
  has many tabs.
  \since 4.2

  When there are too many tabs in a tab bar for its size, the tab bar can either choose
  to expand its size or to add buttons that allow you to scroll through the tabs.

  By default the value is style dependant.

  \sa elideMode, QTabWidget::usesScrollButtons, QStyle::SH_TabBar_PreferNoArrows
*/
bool iTabBarMultiRow::usesScrollButtons() const
{
  return dpriv->useScrollButtons;
}

void iTabBarMultiRow::setUsesScrollButtons(bool useButtons)
{
  dpriv->useScrollButtonsSetByUser = true;
  if (dpriv->useScrollButtons == useButtons)
    return;
  dpriv->useScrollButtons = useButtons;
  dpriv->refresh();
}

/*!
  \property iTabBarMultiRow::tabsClosable
  \brief Whether or not a tab bar should place close buttons on each tab
  \since 4.5

  When tabsClosable is set to true a close button will appear on the tab on
  either the left or right hand side depending upon the style.  When the button
  is clicked the tab the signal tabCloseRequested will be emitted.

  By default the value is false.

  \sa setTabButton(), tabRemoved()
*/

bool iTabBarMultiRow::tabsClosable() const
{
  return dpriv->closeButtonOnTabs;
}

void iTabBarMultiRow::setTabsClosable(bool closable)
{
  if (dpriv->closeButtonOnTabs == closable)
    return;
  dpriv->closeButtonOnTabs = closable;
  ButtonPosition closeSide = (ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this);
  if (!closable) {
    for (int i = 0; i < dpriv->tabList.count(); ++i) {
      if (closeSide == LeftSide && dpriv->tabList[i].leftWidget) {
        dpriv->tabList[i].leftWidget->deleteLater();
        dpriv->tabList[i].leftWidget = 0;
      }
      if (closeSide == RightSide && dpriv->tabList[i].rightWidget) {
        dpriv->tabList[i].rightWidget->deleteLater();
        dpriv->tabList[i].rightWidget = 0;
      }
    }
  } else {
    bool newButtons = false;
    for (int i = 0; i < dpriv->tabList.count(); ++i) {
      if (tabButton(i, closeSide))
        continue;
      newButtons = true;
      QAbstractButton *closeButton = new iTabBarCloseButton(this);
      connect(closeButton, SIGNAL(clicked()), this, SLOT(_q_closeTab()));
      setTabButton(i, closeSide, closeButton);
    }
    if (newButtons)
      dpriv->layoutTabs();
  }
  update();
}

/*!
  \enum iTabBarMultiRow::ButtonPosition
  \since 4.5

  This enum type lists the location of the widget on a tab.

  \value LeftSide Left side of the tab.

  \value RightSide Right side of the tab.

*/

/*!
  \enum iTabBarMultiRow::SelectionBehavior
  \since 4.5

  This enum type lists the behavior of iTabBarMultiRow when a tab is removed
  and the tab being removed is also the current tab.

  \value SelectLeftTab  Select the tab to the left of the one being removed.

  \value SelectRightTab  Select the tab to the right of the one being removed.

  \value SelectPreviousTab  Select the previously selected tab.

*/

/*!
  \property iTabBarMultiRow::selectionBehaviorOnRemove
  \brief What tab should be set as current when removeTab is called if
  the removed tab is also the current tab.
  \since 4.5

  By default the value is SelectRightTab.

  \sa removeTab()
*/


iTabBarMultiRow::SelectionBehavior iTabBarMultiRow::selectionBehaviorOnRemove() const
{
  return dpriv->selectionBehaviorOnRemove;
}

void iTabBarMultiRow::setSelectionBehaviorOnRemove(iTabBarMultiRow::SelectionBehavior behavior)
{
  dpriv->selectionBehaviorOnRemove = behavior;
}

/*!
  \property iTabBarMultiRow::expanding
  \brief When expanding is true iTabBarMultiRow will expand the tabs to use the empty space.
  \since 4.5

  By default the value is true.

  \sa QTabWidget::documentMode
*/

bool iTabBarMultiRow::expanding() const
{
  return dpriv->expanding;
}

void iTabBarMultiRow::setExpanding(bool enabled)
{
  if (dpriv->expanding == enabled)
    return;
  dpriv->expanding = enabled;
  dpriv->layoutTabs();
}

/*!
  \property iTabBarMultiRow::movable
  \brief This property holds whether the user can move the tabs
  within the tabbar area.

  \since 4.5

  By default, this property is \c false;
*/

bool iTabBarMultiRow::isMovable() const
{
  return dpriv->movable;
}

void iTabBarMultiRow::setMovable(bool movable)
{
  dpriv->movable = movable;
}


/*!
  \property iTabBarMultiRow::documentMode
  \brief Whether or not the tab bar is rendered in a mode suitable for the main window.
  \since 4.5

  This property is used as a hint for styles to draw the tabs in a different
  way then they would normally look in a tab widget.  On OS X this will
  look similar to the tabs in Safari or Leopard's Terminal.app.

  \sa QTabWidget::documentMode
*/
bool iTabBarMultiRow::documentMode() const
{
  return dpriv->documentMode;
}

void iTabBarMultiRow::setDocumentMode(bool enabled)
{

  dpriv->documentMode = enabled;
  dpriv->updateMacBorderMetrics();
}

/*!
  \property iTabBarMultiRow::autoHide
  \brief If true, the tab bar is automatically hidden when it contains less
  than 2 tabs.
  \since 5.4

  By default, this property is false.

  \sa QWidget::visible
*/

bool iTabBarMultiRow::autoHide() const
{
  return dpriv->autoHide;
}

void iTabBarMultiRow::setAutoHide(bool hide)
{
  if (dpriv->autoHide == hide)
    return;

  dpriv->autoHide = hide;
  if (hide)
    dpriv->autoHideTabs();
  else
    setVisible(true);
}

/*!
  \property iTabBarMultiRow::changeCurrentOnDrag
  \brief If true, then the current tab is automatically changed when dragging
  over the tabbar.
  \since 5.4

  \note You should also set acceptDrops property to true to make this feature
  work.

  By default, this property is false.
*/

bool iTabBarMultiRow::changeCurrentOnDrag() const
{
  return dpriv->changeCurrentOnDrag;
}

void iTabBarMultiRow::setChangeCurrentOnDrag(bool change)
{
  dpriv->changeCurrentOnDrag = change;
  if (!change)
    dpriv->killSwitchTabTimer();
}

/*!
  Sets \a widget on the tab \a index.  The widget is placed
  on the left or right hand side depending upon the \a position.
  \since 4.5

  Any previously set widget in \a position is hidden.

  The tab bar will take ownership of the widget and so all widgets set here
  will be deleted by the tab bar when it is destroyed unless you separately
  reparent the widget after setting some other widget (or 0).

  \sa tabsClosable()
*/
void iTabBarMultiRow::setTabButton(int index, ButtonPosition position, QWidget *widget)
{
  if (index < 0 || index >= dpriv->tabList.count())
    return;
  if (widget) {
    widget->setParent(this);
    // make sure our left and right widgets stay on top
    widget->lower();
    widget->show();
  }
  if (position == LeftSide) {
    if (dpriv->tabList[index].leftWidget)
      dpriv->tabList[index].leftWidget->hide();
    dpriv->tabList[index].leftWidget = widget;
  } else {
    if (dpriv->tabList[index].rightWidget)
      dpriv->tabList[index].rightWidget->hide();
    dpriv->tabList[index].rightWidget = widget;
  }
  dpriv->layoutTabs();
  dpriv->refresh();
  update();
}

/*!
  Returns the widget set a tab \a index and \a position or 0 if
  one is not set.
*/
QWidget *iTabBarMultiRow::tabButton(int index, ButtonPosition position) const
{
  if (index < 0 || index >= dpriv->tabList.count())
    return 0;
  if (position == LeftSide)
    return dpriv->tabList.at(index).leftWidget;
  else
    return dpriv->tabList.at(index).rightWidget;
}

iTabBarCloseButton::iTabBarCloseButton(QWidget *parent)
  : QAbstractButton(parent)
{
  setFocusPolicy(Qt::NoFocus);
#ifndef QT_NO_CURSOR
  setCursor(Qt::ArrowCursor);
#endif
#ifndef QT_NO_TOOLTIP
  setToolTip(tr("Close Tab"));
#endif
  resize(sizeHint());
}

QSize iTabBarCloseButton::sizeHint() const
{
  ensurePolished();
  int width = style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, this);
  int height = style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, 0, this);
  return QSize(width, height);
}

void iTabBarCloseButton::enterEvent(QEvent *event)
{
  if (isEnabled())
    update();
  QAbstractButton::enterEvent(event);
}

void iTabBarCloseButton::leaveEvent(QEvent *event)
{
  if (isEnabled())
    update();
  QAbstractButton::leaveEvent(event);
}

void iTabBarCloseButton::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  QStyleOption opt;
  opt.init(this);
  opt.state |= QStyle::State_AutoRaise;
  if (isEnabled() && underMouse() && !isChecked() && !isDown())
    opt.state |= QStyle::State_Raised;
  if (isChecked())
    opt.state |= QStyle::State_On;
  if (isDown())
    opt.state |= QStyle::State_Sunken;

  if (const iTabBarMultiRow *tb = qobject_cast<const iTabBarMultiRow *>(parent())) {
    int index = tb->currentIndex();
    iTabBarMultiRow::ButtonPosition position = (iTabBarMultiRow::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, tb);
    if (tb->tabButton(index, position) == this)
      opt.state |= QStyle::State_Selected;
  }

  style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &p, this);
}

#ifndef QT_NO_ANIMATION
void iTabBarMultiRowPrivate::Tab::TabBarAnimation::updateCurrentValue(const QVariant &current)
{
  priv->moveTab(priv->tabList.indexOf(*tab), current.toInt());
}

void iTabBarMultiRowPrivate::Tab::TabBarAnimation::updateState(QAbstractAnimation::State, QAbstractAnimation::State newState)
{
  if (newState == Stopped) priv->moveTabFinished(priv->tabList.indexOf(*tab));
}
#endif
