// Co2018ght 2016-2017, Regents of the University of Colorado,
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

#ifndef iTabBarMultiRow_h
#define iTabBarMultiRow_h 1

// parent includes:
#include "ta_def.h"
#include <QTabBar>
#include <QVariantAnimation>
#include <QAbstractButton>

class QToolButton;

// member includes:

// declare all other types mentioned but not required to include:

class QIcon;
class QStyleOptionTab;

class TA_API iTabBarMultiRow: public QTabBar {
  Q_OBJECT

 public:
  explicit iTabBarMultiRow(QWidget *parent = Q_NULLPTR);
  ~iTabBarMultiRow();

  QSize sizeHint() const Q_DECL_OVERRIDE;
  QSize minimumSizeHint() const Q_DECL_OVERRIDE;

 protected:
  void tabLayoutChange() override;

  bool event(QEvent *) Q_DECL_OVERRIDE;
  void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
  void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
  void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
  void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
  void mousePressEvent (QMouseEvent *) Q_DECL_OVERRIDE;
  void mouseMoveEvent (QMouseEvent *) Q_DECL_OVERRIDE;
  void mouseReleaseEvent (QMouseEvent *) Q_DECL_OVERRIDE;
#ifndef QT_NO_WHEELEVENT
  void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
#endif
  void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
  void changeEvent(QEvent *) Q_DECL_OVERRIDE;
  void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
  void initStyleOption(QStyleOptionTab *option, int tabIndex) const;

#ifndef QT_NO_ACCESSIBILITY
  friend class QAccessibleTabBar;
#endif

 protected:

  // below is all the stuff from QTabBarPrivate that we need..
  
  bool layoutDirty;
  bool drawBase;
  int scrollOffset;

  struct Tab {
    inline Tab()
      : dragOffset(0)
      , animation(0)
    {}
    bool operator==(const Tab &other) const { return &other == this; }
    QRect rect;
    QRect minRect;
    QRect maxRect;
    int dragOffset;

#ifndef QT_NO_ANIMATION
    ~Tab() { delete animation; }
    struct TabBarAnimation : public QVariantAnimation {
      TabBarAnimation(Tab *t, iTabBarMultiRow *_priv) : tab(t), priv(_priv)
      { setEasingCurve(QEasingCurve::InOutQuad); }

      void updateCurrentValue(const QVariant &current) Q_DECL_OVERRIDE;

      void updateState(State, State newState) Q_DECL_OVERRIDE;
    private:
      //these are needed for the callbacks
      Tab *tab;
      iTabBarMultiRow *priv;
    } *animation;

    void startAnimation(iTabBarMultiRow *priv, int duration) {
      if (!priv->isAnimated()) {
        priv->moveTabFinished(priv->tabList.indexOf(*this));
        return;
      }
      if (!animation)
        animation = new TabBarAnimation(this, priv);
      animation->setStartValue(dragOffset);
      animation->setEndValue(0);
      animation->setDuration(duration);
      animation->start();
    }
#else
    void startAnimation(iTabBarMultiRow *priv, int duration)
    { Q_UNUSED(duration); priv->moveTabFinished(priv->tabList.indexOf(*this)); }
#endif //QT_NO_ANIMATION
  };
  QList<Tab> tabList;

  void  updateTabList();
  // key routine to deal with fact that we don't have access to undelying data -- thanks pimpl!
  
  int calculateNewPosition(int from, int to, int index) const;
  void slide(int from, int to);
  int extraWidth() const;

  inline bool isAnimated() const { Q_Q(const iTabBarMultiRow); return q->style()->styleHint(QStyle::SH_Widget_Animate, 0, q); }
  inline bool validIndex(int index) const { return index >= 0 && index < tabList.count(); }
  void setCurrentNextEnabledIndex(int offset);

  void moveTab(int index, int offset);
  void moveTabFinished(int index);
  QRect hoverRect;

  void refresh();
  void layoutTabs();
  void layoutWidgets(int start = 0);
  void layoutTab(int index);
  bool isTabInMacUnifiedToolbarArea() const;
  void setupMovableTab();
  void autoHideTabs();

  void makeVisible(int index);

  QPoint dragStartPosition;
  bool paintWithOffsets;
  bool dragInProgress;
  int switchTabCurrentIndex;
  int switchTabTimerId;

  QMovableTabWidget *movingTab;
  // shared by tabwidget and qtabbar
  static void initStyleBaseOption(QStyleOptionTabBarBase *optTabBase, iTabBarMultiRow *tabbar, QSize size)
  {
    QStyleOptionTab tabOverlap;
    tabOverlap.shape = (QTabBar::Shape)tabbar->shape();
    int overlap = tabbar->style()->pixelMetric(QStyle::PM_TabBarBaseOverlap, &tabOverlap, tabbar);
    QWidget *theParent = tabbar->parentWidget();
    optTabBase->init(tabbar);
    optTabBase->shape = (QTabBar::Shape)tabbar->shape();
    optTabBase->documentMode = tabbar->documentMode();
    if (theParent && overlap > 0) {
      QRect rect;
      switch (tabOverlap.shape) {
      case iTabBarMultiRow::RoundedNorth:
      case iTabBarMultiRow::TriangularNorth:
        rect.setRect(0, size.height()-overlap, size.width(), overlap);
        break;
      case iTabBarMultiRow::RoundedSouth:
      case iTabBarMultiRow::TriangularSouth:
        rect.setRect(0, 0, size.width(), overlap);
        break;
      case iTabBarMultiRow::RoundedEast:
      case iTabBarMultiRow::TriangularEast:
        rect.setRect(0, 0, overlap, size.height());
        break;
      case iTabBarMultiRow::RoundedWest:
      case iTabBarMultiRow::TriangularWest:
        rect.setRect(size.width() - overlap, 0, overlap, size.height());
        break;
      }
      optTabBase->rect = rect;
    }
  }

  void killSwitchTabTimer();

 private:
  Q_DISABLE_COPY(iTabBarMultiRow)
};


#define ANIMATION_DURATION 250

#include <QStyleOption>

class QMovableTabWidget : public QWidget
{
public:
  explicit QMovableTabWidget(QWidget *parent = Q_NULLPTR);
  void setPixmap(const QPixmap &pixmap);

protected:
  void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
  QPixmap m_pixmap;
};


#endif // iTabBarMultiRow_h
