// Copyright 2016-2018, Regents of the University of Colorado,
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
#include <QTabWidget>
#include <QToolTip>
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


void iTabBarMultiRow::updateTabList() {
  tabList.clear();
  for(int i=0; i<count(); i++) {
    tabList.append(Tab());
    Tab& tab = tabList.at(i);
    tab.rect = tabRect(i);
    tab.minRect = minimumTabSizeHint(i);
    tab.maxRect = tabSizeHint(i);
  }
}

inline static bool verticalTabs(iTabBarMultiRow::Shape shape)
{
  return shape == iTabBarMultiRow::RoundedWest
    || shape == iTabBarMultiRow::RoundedEast
    || shape == iTabBarMultiRow::TriangularWest
    || shape == iTabBarMultiRow::TriangularEast;
}

int iTabBarMultiRow::extraWidth() const
{
  return 2 * qMax(style()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, 0, q),
                  QApplication::globalStrut().width());
}

void iTabBarMultiRow::init()
{
  
}

void iTabBarMultiRow::layoutTabs()
{
  scrollOffset = 0;
  layoutDirty = false;
  QSize size = size();
  int last, available;
  int maxExtent;
  int i;
  bool vertTabs = verticalTabs(shape);
  int tabChainIndex = 0;

  Qt::Alignment tabAlignment = Qt::Alignment(style()->styleHint(QStyle::SH_TabBar_Alignment, 0, q));
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
      QSize sz = tabSizeHint(i);
      tabList[i].maxRect = QRect(x, 0, sz.width(), sz.height());
      x += sz.width();
      maxHeight = qMax(maxHeight, sz.height());
      sz = minimumTabSizeHint(i);
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
      QSize sz = tabSizeHint(i);
      tabList[i].maxRect = QRect(0, y, sz.width(), sz.height());
      y += sz.height();
      maxWidth = qMax(maxWidth, sz.width());
      sz = minimumTabSizeHint(i);
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
      Qt::LayoutDirection ld = layoutDirection();
      QRect arrows = QStyle::visualRect(ld, rect(),
                                        QRect(available - extra, 0, extra, size.height()));
      int buttonOverlap = style()->pixelMetric(QStyle::PM_TabBar_ScrollButtonOverlap, 0, q);

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
  tabLayoutChange();
}

void iTabBarMultiRow::makeVisible(int index)
{
  if (!validIndex(index) || leftB->isHidden())
    return;

  const QRect tabRect = tabList.at(index).rect;
  const int oldScrollOffset = scrollOffset;
  const bool horiz = !verticalTabs(shape);
  const int available = (horiz ? width() : height()) - extraWidth();
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
    update();
    layoutWidgets();
  }
}

void iTabBarMultiRow::killSwitchTabTimer()
{
  if (switchTabTimerId) {
    killTimer(switchTabTimerId);
    switchTabTimerId = 0;
  }
  switchTabCurrentIndex = -1;
}

void iTabBarMultiRow::layoutTab(int index)
{
  Q_ASSERT(index >= 0);

  Tab &tab = tabList[index];
  bool vertical = verticalTabs(shape);
  if (!(tab.leftWidget || tab.rightWidget))
    return;

  QStyleOptionTab opt;
  initStyleOption(&opt, index);
  if (tab.leftWidget) {
    QRect rect = style()->subElementRect(QStyle::SE_TabBarTabLeftButton, &opt, q);
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
    QRect rect = style()->subElementRect(QStyle::SE_TabBarTabRightButton, &opt, q);
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

void iTabBarMultiRow::layoutWidgets(int start)
{
  for (int i = start; i < count(); ++i) {
    layoutTab(i);
  }
}

void iTabBarMultiRow::refresh()
{
  // be safe in case a subclass is also handling move with the tabs
  if (pressedIndex != -1
      && movable
      && QApplication::mouseButtons() == Qt::NoButton) {
    moveTabFinished(pressedIndex);
    if (!validIndex(pressedIndex))
      pressedIndex = -1;
  }

  if (!isVisible()) {
    layoutDirty = true;
  } else {
    layoutTabs();
    makeVisible(currentIndex);
    update();
    updateGeometry();
  }
}

/*!
  Creates a new tab bar with the given \a parent.
*/
iTabBarMultiRow::iTabBarMultiRow(QWidget* parent)
  :QTabBar(parent)
{
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
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


/*!
  Returns the visual rectangle of the tab at position \a
  index, or a null rectangle if \a index is out of range.
*/
QRect iTabBarMultiRow::tabRect(int index) const
{
  if (const iTabBarMultiRow::Tab *tab = at(index)) {
    if (layoutDirty)
      const_cast<iTabBarMultiRow*>(dpriv)->layoutTabs();
    QRect r = tab->rect;
    if (verticalTabs(shape))
      r.translate(0, -scrollOffset);
    else
      r.translate(-scrollOffset, 0);
    if (!verticalTabs(shape))
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
  if (validIndex(currentIndex)
      && tabRect(currentIndex).contains(position)) {
    return currentIndex;
  }
  const int max = tabList.size();
  for (int i = 0; i < max; ++i) {
    if (tabRect(i).contains(position)) {
      return i;
    }
  }
  return -1;
}


/*!\reimp
 */
QSize iTabBarMultiRow::sizeHint() const
{
  if (layoutDirty)
    const_cast<iTabBarMultiRow*>(dpriv)->layoutTabs();
  QRect r;
  for (int i = 0; i < tabList.count(); ++i)
    r = r.united(tabList.at(i).maxRect);
  QSize sz = QApplication::globalStrut();
  return r.size().expandedTo(sz);
}

/*!\reimp
 */
QSize iTabBarMultiRow::minimumSizeHint() const
{
  if (layoutDirty)
    const_cast<iTabBarMultiRow*>(dpriv)->layoutTabs();
  if (!useScrollButtons) {
    QRect r;
    for (int i = 0; i < tabList.count(); ++i)
      r = r.united(tabList.at(i).minRect);
    return r.size().expandedTo(QApplication::globalStrut());
  }
  if (verticalTabs(shape))
    return QSize(sizeHint().width(), rightB->sizeHint().height() * 2 + 75);
  else
    return QSize(rightB->sizeHint().width() * 2 + 75, sizeHint().height());
}

/*!
  Returns the minimum tab size hint for the tab at position \a index.
  \since Qt 5.0
*/


/*!\reimp
 */
void iTabBarMultiRow::showEvent(QShowEvent *)
{
  if (layoutDirty)
    refresh();
  if (!validIndex(currentIndex))
    setCurrentIndex(0);
  updateMacBorderMetrics();
}

/*!\reimp
 */
void iTabBarMultiRow::hideEvent(QHideEvent *)
{
  updateMacBorderMetrics();
}

/*!\reimp
 */
bool iTabBarMultiRow::event(QEvent *event)
{
  if (event->type() == QEvent::HoverMove
      || event->type() == QEvent::HoverEnter) {
    QHoverEvent *he = static_cast<QHoverEvent *>(event);
    if (!hoverRect.contains(he->pos())) {
      QRect oldHoverRect = hoverRect;
      for (int i = 0; i < tabList.count(); ++i) {
        QRect area = tabRect(i);
        if (area.contains(he->pos())) {
          hoverRect = area;
          break;
        }
      }
      if (he->oldPos() != QPoint(-1, -1))
        update(oldHoverRect);
      update(hoverRect);
    }
    return true;
  } else if (event->type() == QEvent::HoverLeave ) {
    QRect oldHoverRect = hoverRect;
    hoverRect = QRect();
    update(oldHoverRect);
    return true;
#ifndef QT_NO_TOOLTIP
  } else if (event->type() == QEvent::ToolTip) {
    if (const iTabBarMultiRow::Tab *tab = at(tabAt(static_cast<QHelpEvent*>(event)->pos()))) {
      if (!tab->toolTip.isEmpty()) {
        QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tab->toolTip, this);
        return true;
      }
    }
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
  } else if (event->type() == QEvent::QueryWhatsThis) {
    const iTabBarMultiRow::Tab *tab = at(indexAtPos(static_cast<QHelpEvent*>(event)->pos()));
    if (!tab || tab->whatsThis.isEmpty())
      event->ignore();
    return true;
  } else if (event->type() == QEvent::WhatsThis) {
    if (const iTabBarMultiRow::Tab *tab = at(indexAtPos(static_cast<QHelpEvent*>(event)->pos()))) {
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
    for (int i = 0; i < tabList.count(); ++i) {
      const iTabBarMultiRow::Tab *tab = &tabList.at(i);
      if (tab->shortcutId == se->shortcutId()) {
        setCurrentIndex(i);
        return true;
      }
    }
#endif
  } else if (event->type() == QEvent::MouseButtonDblClick) { // ### fixme Qt 6: move to mouseDoubleClickEvent(), here for BC reasons.
    const QPoint pos = static_cast<const QMouseEvent *>(event)->pos();
    const bool isEventInCornerButtons = (!leftB->isHidden() && leftB->geometry().contains(pos))
      || (!rightB->isHidden() && rightB->geometry().contains(pos));
    if (!isEventInCornerButtons)
      emit tabBarDoubleClicked(tabAt(pos));
  } else if (event->type() == QEvent::Move) {
    updateMacBorderMetrics();
    return QWidget::event(event);

#ifndef QT_NO_DRAGANDDROP
  } else if (event->type() == QEvent::DragEnter) {
    if (changeCurrentOnDrag)
      event->accept();
  } else if (event->type() == QEvent::DragMove) {
    if (changeCurrentOnDrag) {
      const int tabIndex = tabAt(static_cast<QDragMoveEvent *>(event)->pos());
      if (isTabEnabled(tabIndex) && switchTabCurrentIndex != tabIndex) {
        switchTabCurrentIndex = tabIndex;
        if (switchTabTimerId)
          killTimer(switchTabTimerId);
        switchTabTimerId = startTimer(style()->styleHint(QStyle::SH_TabBar_ChangeCurrentDelay));
      }
      event->ignore();
    }
  } else if (event->type() == QEvent::DragLeave || event->type() == QEvent::Drop) {
    killSwitchTabTimer();
    event->ignore();
#endif
  }
  return QWidget::event(event);
}

/*!\reimp
 */
void iTabBarMultiRow::resizeEvent(QResizeEvent *)
{
  if (layoutDirty)
    updateGeometry();
  layoutTabs();

  makeVisible(currentIndex);
}

/*!\reimp
 */
void iTabBarMultiRow::paintEvent(QPaintEvent *)
{

  QStyleOptionTabBarBase optTabBase;
  iTabBarMultiRow::initStyleBaseOption(&optTabBase, this, size());

  QStylePainter p(this);
  int selected = -1;
  int cut = -1;
  bool rtl = optTabBase.direction == Qt::RightToLeft;
  bool vertical = verticalTabs(shape);
  QStyleOptionTab cutTab;
  selected = currentIndex;
  if (dragInProgress)
    selected = pressedIndex;

  for (int i = 0; i < tabList.count(); ++i)
    optTabBase.tabBarRect |= tabRect(i);

  optTabBase.selectedTabRect = tabRect(selected);

  if (drawBase)
    p.drawPrimitive(QStyle::PE_FrameTabBarBase, optTabBase);

  for (int i = 0; i < tabList.count(); ++i) {
    QStyleOptionTab tab;
    initStyleOption(&tab, i);
    if (paintWithOffsets && tabList[i].dragOffset != 0) {
      if (vertical) {
        tab.rect.moveTop(tab.rect.y() + tabList[i].dragOffset);
      } else {
        tab.rect.moveLeft(tab.rect.x() + tabList[i].dragOffset);
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
    if (paintWithOffsets && tabList[selected].dragOffset != 0) {
      if (vertical)
        tab.rect.moveTop(tab.rect.y() + tabList[selected].dragOffset);
      else
        tab.rect.moveLeft(tab.rect.x() + tabList[selected].dragOffset);
    }
    if (!dragInProgress)
      p.drawControl(QStyle::CE_TabBarTab, tab);
    else {
      int taboverlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, 0, this);
      movingTab->setGeometry(tab.rect.adjusted(-taboverlap, 0, taboverlap, 0));
    }
  }

  // Only draw the tear indicator if necessary. Most of the time we don't need too.
  if (leftB->isVisible() && cut >= 0) {
    cutTab.rect = rect();
    cutTab.rect = style()->subElementRect(QStyle::SE_TabBarTearIndicator, &cutTab, this);
    p.drawPrimitive(QStyle::PE_IndicatorTabTear, cutTab);
  }
}

/*
  Given that index at position from moved to position to where return where index goes.
*/
int iTabBarMultiRow::calculateNewPosition(int from, int to, int index) const
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
      || !validIndex(from)
      || !validIndex(to))
    return;

  bool vertical = verticalTabs(shape);
  int oldPressedPosition = 0;
  if (pressedIndex != -1) {
    // Record the position of the pressed tab before reordering the tabs.
    oldPressedPosition = vertical ? tabList[pressedIndex].rect.y()
      : tabList[pressedIndex].rect.x();
  }

  // Update the locations of the tabs first
  int start = qMin(from, to);
  int end = qMax(from, to);
  int width = vertical ? tabList[from].rect.height() : tabList[from].rect.width();
  if (from < to)
    width *= -1;
  bool rtl = isRightToLeft();
  for (int i = start; i <= end; ++i) {
    if (i == from)
      continue;
    if (vertical)
      tabList[i].rect.moveTop(tabList[i].rect.y() + width);
    else
      tabList[i].rect.moveLeft(tabList[i].rect.x() + width);
    int direction = -1;
    if (rtl && !vertical)
      direction *= -1;
    if (tabList[i].dragOffset != 0)
      tabList[i].dragOffset += (direction * width);
  }

  if (vertical) {
    if (from < to)
      tabList[from].rect.moveTop(tabList[to].rect.bottom() + 1);
    else
      tabList[from].rect.moveTop(tabList[to].rect.top() - width);
  } else {
    if (from < to)
      tabList[from].rect.moveLeft(tabList[to].rect.right() + 1);
    else
      tabList[from].rect.moveLeft(tabList[to].rect.left() - width);
  }

  // Move the actual data structures
  tabList.move(from, to);

  // update lastTab locations
  for (int i = 0; i < tabList.count(); ++i)
    tabList[i].lastTab = calculateNewPosition(from, to, tabList[i].lastTab);

  // update external variables
  int previousIndex = currentIndex;
  currentIndex = calculateNewPosition(from, to, currentIndex);

  // If we are in the middle of a drag update the dragStartPosition
  if (pressedIndex != -1) {
    pressedIndex = calculateNewPosition(from, to, pressedIndex);
    int newPressedPosition = vertical ? tabList[pressedIndex].rect.top() : tabList[pressedIndex].rect.left();
    int diff = oldPressedPosition - newPressedPosition;
    if (isRightToLeft() && !vertical)
      diff *= -1;
    if (vertical)
      dragStartPosition.setY(dragStartPosition.y() - diff);
    else
      dragStartPosition.setX(dragStartPosition.x() - diff);
  }

  layoutWidgets(start);
  update();
  emit tabMoved(from, to);
  if (previousIndex != currentIndex)
    emit currentChanged(currentIndex);
  emit tabLayoutChange();
}

void iTabBarMultiRow::slide(int from, int to)
{
  if (from == to
      || !validIndex(from)
      || !validIndex(to))
    return;
  bool vertical = verticalTabs(shape);
  int preLocation = vertical ? tabRect(from).y() : tabRect(from).x();
  setUpdatesEnabled(false);
  moveTab(from, to);
  setUpdatesEnabled(true);
  int postLocation = vertical ? tabRect(to).y() : tabRect(to).x();
  int length = postLocation - preLocation;
  tabList[to].dragOffset -= length;
  tabList[to].startAnimation(this, ANIMATION_DURATION);
}

void iTabBarMultiRow::moveTab(int index, int offset)
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
  const bool isEventInCornerButtons = (!leftB->isHidden() && leftB->geometry().contains(pos))
    || (!rightB->isHidden() && rightB->geometry().contains(pos));
  if (!isEventInCornerButtons) {
    const int index = indexAtPos(pos);
    emit tabBarClicked(index);
  }

  if (event->button() != Qt::LeftButton) {
    event->ignore();
    return;
  }
  // Be safe!
  if (pressedIndex != -1 && movable)
    moveTabFinished(pressedIndex);

  pressedIndex = indexAtPos(event->pos());
  if (validIndex(pressedIndex)) {
    QStyleOptionTabBarBase optTabBase;
    optTabBase.init(this);
    optTabBase.documentMode = documentMode;
    if (event->type() == style()->styleHint(QStyle::SH_TabBar_SelectMouseType, &optTabBase, this))
      setCurrentIndex(pressedIndex);
    else
      repaint(tabRect(pressedIndex));
    if (movable) {
      dragStartPosition = event->pos();
    }
  }
}

/*!\reimp
 */
void iTabBarMultiRow::mouseMoveEvent(QMouseEvent *event)
{
  if (movable) {
    // Be safe!
    if (pressedIndex != -1
        && event->buttons() == Qt::NoButton)
      moveTabFinished(pressedIndex);

    // Start drag
    if (!dragInProgress && pressedIndex != -1) {
      if ((event->pos() - dragStartPosition).manhattanLength() > QApplication::startDragDistance()) {
        dragInProgress = true;
        setupMovableTab();
      }
    }

    int offset = (event->pos() - dragStartPosition).manhattanLength();
    if (event->buttons() == Qt::LeftButton
        && offset > QApplication::startDragDistance()
        && validIndex(pressedIndex)) {
      bool vertical = verticalTabs(shape);
      int dragDistance;
      if (vertical) {
        dragDistance = (event->pos().y() - dragStartPosition.y());
      } else {
        dragDistance = (event->pos().x() - dragStartPosition.x());
      }
      tabList[pressedIndex].dragOffset = dragDistance;

      QRect startingRect = tabRect(pressedIndex);
      if (vertical)
        startingRect.moveTop(startingRect.y() + dragDistance);
      else
        startingRect.moveLeft(startingRect.x() + dragDistance);

      int overIndex;
      if (dragDistance < 0)
        overIndex = tabAt(startingRect.topLeft());
      else
        overIndex = tabAt(startingRect.topRight());

      if (overIndex != pressedIndex && overIndex != -1) {
        int offset = 1;
        if (isRightToLeft() && !vertical)
          offset *= -1;
        if (dragDistance < 0) {
          dragDistance *= -1;
          offset *= -1;
        }
        for (int i = pressedIndex;
             offset > 0 ? i < overIndex : i > overIndex;
             i += offset) {
          QRect overIndexRect = tabRect(overIndex);
          int needsToBeOver = (vertical ? overIndexRect.height() : overIndexRect.width()) / 2;
          if (dragDistance > needsToBeOver)
            slide(i + offset, pressedIndex);
        }
      }
      // Buttons needs to follow the dragged tab
      layoutTab(pressedIndex);

      update();
    }
  }

  if (event->buttons() != Qt::LeftButton) {
    event->ignore();
    return;
  }
  QStyleOptionTabBarBase optTabBase;
  optTabBase.init(this);
  optTabBase.documentMode = documentMode;
}

void iTabBarMultiRow::setupMovableTab()
{
  if (!movingTab)
    movingTab = new QMovableTabWidget(q);

  int taboverlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, 0 ,q);
  QRect grabRect = tabRect(pressedIndex);
  grabRect.adjust(-taboverlap, 0, taboverlap, 0);

  QPixmap grabImage(grabRect.size());
  // grabImage.setDevicePixelRatio(devicePixelRatioF());
  grabImage.fill(Qt::transparent);
  QStylePainter p(&grabImage, q);
  p.initFrom(q);

  QStyleOptionTab tab;
  initStyleOption(&tab, pressedIndex);
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

void iTabBarMultiRow::moveTabFinished(int index)
{
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
  update();
}

/*!\reimp
 */
void iTabBarMultiRow::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() != Qt::LeftButton) {
    event->ignore();
    return;
  }
  if (movable && dragInProgress && validIndex(pressedIndex)) {
    int length = tabList[pressedIndex].dragOffset;
    int width = verticalTabs(shape)
      ? tabRect(pressedIndex).height()
      : tabRect(pressedIndex).width();
    int duration = qMin(ANIMATION_DURATION,
                        (qAbs(length) * ANIMATION_DURATION) / width);
    tabList[pressedIndex].startAnimation(dpriv, duration);
    dragInProgress = false;
    movingTab->setVisible(false);
    dragStartPosition = QPoint();
  }

  int i = indexAtPos(event->pos()) == pressedIndex ? pressedIndex : -1;
  pressedIndex = -1;
  QStyleOptionTabBarBase optTabBase;
  optTabBase.initFrom(this);
  optTabBase.documentMode = documentMode;
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
  setCurrentNextEnabledIndex(offset);
}

/*!\reimp
 */
#ifndef QT_NO_WHEELEVENT
void iTabBarMultiRow::wheelEvent(QWheelEvent *event)
{
#ifndef Q_OS_MAC
  int offset = event->delta() > 0 ? -1 : 1;
  setCurrentNextEnabledIndex(offset);
  QWidget::wheelEvent(event);
#else
  Q_UNUSED(event)
#endif
    }
#endif //QT_NO_WHEELEVENT

void iTabBarMultiRow::setCurrentNextEnabledIndex(int offset)
{
  for (int index = currentIndex + offset; validIndex(index); index += offset) {
    if (tabList.at(index).enabled) {
      setCurrentIndex(index);
      break;
    }
  }
}

/*!\reimp
 */
void iTabBarMultiRow::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::StyleChange) {
    if (!elideModeSetByUser)
      elideMode = Qt::TextElideMode(style()->styleHint(QStyle::SH_TabBar_ElideMode, 0, this));
    if (!useScrollButtonsSetByUser)
      useScrollButtons = !style()->styleHint(QStyle::SH_TabBar_PreferNoArrows, 0, this);
    refresh();
  } else if (event->type() == QEvent::FontChange) {
    refresh();
  }
  QWidget::changeEvent(event);
}

/*!
  \reimp
*/
void iTabBarMultiRow::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == switchTabTimerId) {
    killTimer(switchTabTimerId);
    switchTabTimerId = 0;
    setCurrentIndex(switchTabCurrentIndex);
    switchTabCurrentIndex = -1;
  }
  QWidget::timerEvent(event);
}


#ifndef QT_NO_ANIMATION
void iTabBarMultiRow::Tab::TabBarAnimation::updateCurrentValue(const QVariant &current)
{
  priv->moveTab(priv->tabList.indexOf(*tab), current.toInt());
}

void iTabBarMultiRow::Tab::TabBarAnimation::updateState(QAbstractAnimation::State, QAbstractAnimation::State newState)
{
  if (newState == Stopped) priv->moveTabFinished(priv->tabList.indexOf(*tab));
}
#endif
