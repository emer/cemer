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
class iTabBarMultiRowPrivate;
class QStyleOptionTab;


// todo: need to rely exclusively on underlying qt data -- QTabWidget plugs into NON VIRTUAL
// methods here so we have no option unfortunately.

class TA_API iTabBarMultiRow: public QTabBar {
  Q_OBJECT

 public:
  explicit iTabBarMultiRow(QWidget *parent = Q_NULLPTR);
  ~iTabBarMultiRow();

  Shape shape() const;
  void setShape(Shape shape);

  int addTab(const QString &text);
  int addTab(const QIcon &icon, const QString &text);

  int insertTab(int index, const QString &text);
  int insertTab(int index, const QIcon&icon, const QString &text);

  void removeTab(int index);
  void moveTab(int from, int to);

  bool isTabEnabled(int index) const;
  void setTabEnabled(int index, bool);

  QString tabText(int index) const;
  void setTabText(int index, const QString &text);

  QColor tabTextColor(int index) const;
  void setTabTextColor(int index, const QColor &color);

  QIcon tabIcon(int index) const;
  void setTabIcon(int index, const QIcon &icon);

  Qt::TextElideMode elideMode() const;
  void setElideMode(Qt::TextElideMode);

#ifndef QT_NO_TOOLTIP
  void setTabToolTip(int index, const QString &tip);
  QString tabToolTip(int index) const;
#endif

#ifndef QT_NO_WHATSTHIS
  void setTabWhatsThis(int index, const QString &text);
  QString tabWhatsThis(int index) const;
#endif

  void setTabData(int index, const QVariant &data);
  QVariant tabData(int index) const;

  QRect tabRect(int index) const;
  int tabAt(const QPoint &pos) const;

  int currentIndex() const;
  int count() const;

  QSize sizeHint() const Q_DECL_OVERRIDE;
  QSize minimumSizeHint() const Q_DECL_OVERRIDE;

  void setDrawBase(bool drawTheBase);
  bool drawBase() const;

  QSize iconSize() const;
  void setIconSize(const QSize &size);

  bool usesScrollButtons() const;
  void setUsesScrollButtons(bool useButtons);

  bool tabsClosable() const;
  void setTabsClosable(bool closable);

  void setTabButton(int index, ButtonPosition position, QWidget *widget);
  QWidget *tabButton(int index, ButtonPosition position) const;

  SelectionBehavior selectionBehaviorOnRemove() const;
  void setSelectionBehaviorOnRemove(SelectionBehavior behavior);

  bool expanding() const;
  void setExpanding(bool enabled);

  bool isMovable() const;
  void setMovable(bool movable);

  bool documentMode() const;
  void setDocumentMode(bool set);

  bool autoHide() const;
  void setAutoHide(bool hide);

  bool changeCurrentOnDrag() const;
  void setChangeCurrentOnDrag(bool change);

 public Q_SLOTS:
  void setCurrentIndex(int index);
  void _q_scrollTabs();
  void _q_closeTab();
  
 Q_SIGNALS:
  void currentChanged(int index);
  void tabCloseRequested(int index);
  void tabMoved(int from, int to);
  void tabBarClicked(int index);
  void tabBarDoubleClicked(int index);

 protected:
  QSize tabSizeHint(int index) const override;
  QSize minimumTabSizeHint(int index) const override;
  void tabInserted(int index) override;
  void tabRemoved(int index) override;
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
 private:
  iTabBarMultiRowPrivate* dpriv;
  friend class iTabBarMultiRowPrivate;
  Q_DISABLE_COPY(iTabBarMultiRow)
};


//////////////////////////////////////////
//      pimpl

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


class TA_API iTabBarMultiRowPrivate 
{
public:
  iTabBarMultiRowPrivate()
    :currentIndex(-1), pressedIndex(-1), shape(iTabBarMultiRow::RoundedNorth), layoutDirty(false),
     drawBase(true), scrollOffset(0), elideModeSetByUser(false), useScrollButtonsSetByUser(false), expanding(true), closeButtonOnTabs(false),
     selectionBehaviorOnRemove(iTabBarMultiRow::SelectRightTab), paintWithOffsets(true), movable(false),
     dragInProgress(false), documentMode(false), autoHide(false), changeCurrentOnDrag(false),
     switchTabCurrentIndex(-1), switchTabTimerId(0), movingTab(0)
  {}

  iTabBarMultiRow* q_obj;
  iTabBarMultiRow* q_func() { return q_obj; }
  const iTabBarMultiRow* q_func() const { return q_obj; }
  int currentIndex;
  int pressedIndex;
  iTabBarMultiRow::Shape shape;
  bool layoutDirty;
  bool drawBase;
  int scrollOffset;

  struct Tab {
    inline Tab(const QIcon &ico, const QString &txt)
      : enabled(true) , shortcutId(0), text(txt), icon(ico),
        leftWidget(0), rightWidget(0), lastTab(-1), dragOffset(0)
#ifndef QT_NO_ANIMATION
      , animation(0)
#endif //QT_NO_ANIMATION
    {}
    bool operator==(const Tab &other) const { return &other == this; }
    bool enabled;
    int shortcutId;
    QString text;
#ifndef QT_NO_TOOLTIP
    QString toolTip;
#endif
#ifndef QT_NO_WHATSTHIS
    QString whatsThis;
#endif
    QIcon icon;
    QRect rect;
    QRect minRect;
    QRect maxRect;

    QColor textColor;
    QVariant data;
    QWidget *leftWidget;
    QWidget *rightWidget;
    int lastTab;
    int dragOffset;

#ifndef QT_NO_ANIMATION
    ~Tab() { delete animation; }
    struct TabBarAnimation : public QVariantAnimation {
      TabBarAnimation(Tab *t, iTabBarMultiRowPrivate *_priv) : tab(t), priv(_priv)
      { setEasingCurve(QEasingCurve::InOutQuad); }

      void updateCurrentValue(const QVariant &current) Q_DECL_OVERRIDE;

      void updateState(State, State newState) Q_DECL_OVERRIDE;
    private:
      //these are needed for the callbacks
      Tab *tab;
      iTabBarMultiRowPrivate *priv;
    } *animation;

    void startAnimation(iTabBarMultiRowPrivate *priv, int duration) {
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
    void startAnimation(iTabBarMultiRowPrivate *priv, int duration)
    { Q_UNUSED(duration); priv->moveTabFinished(priv->tabList.indexOf(*this)); }
#endif //QT_NO_ANIMATION
  };
  QList<Tab> tabList;

  int calculateNewPosition(int from, int to, int index) const;
  void slide(int from, int to);
  void init();
  int extraWidth() const;

  Tab *at(int index);
  const Tab *at(int index) const;

  int indexAtPos(const QPoint &p) const;

  inline bool isAnimated() const { Q_Q(const iTabBarMultiRow); return q->style()->styleHint(QStyle::SH_Widget_Animate, 0, q); }
  inline bool validIndex(int index) const { return index >= 0 && index < tabList.count(); }
  void setCurrentNextEnabledIndex(int offset);

  QToolButton* rightB; // right or bottom
  QToolButton* leftB; // left or top

  void _q_scrollTabs();
  void _q_closeTab();
  void moveTab(int index, int offset);
  void moveTabFinished(int index);
  QRect hoverRect;

  void refresh();
  void layoutTabs();
  void layoutWidgets(int start = 0);
  void layoutTab(int index);
  void updateMacBorderMetrics();
  bool isTabInMacUnifiedToolbarArea() const;
  void setupMovableTab();
  void autoHideTabs();

  void makeVisible(int index);
  QSize iconSize;
  Qt::TextElideMode elideMode;
  bool elideModeSetByUser;
  bool useScrollButtons;
  bool useScrollButtonsSetByUser;

  bool expanding;
  bool closeButtonOnTabs;
  iTabBarMultiRow::SelectionBehavior selectionBehaviorOnRemove;

  QPoint dragStartPosition;
  bool paintWithOffsets;
  bool movable;
  bool dragInProgress;
  bool documentMode;
  bool autoHide;
  bool changeCurrentOnDrag;

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

};


class TA_API iTabBarCloseButton : public QAbstractButton
{
  Q_OBJECT

public:
  iTabBarCloseButton(QWidget *parent = 0);

  QSize sizeHint() const Q_DECL_OVERRIDE;
  QSize minimumSizeHint() const Q_DECL_OVERRIDE
  { return sizeHint(); }
  void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
  void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
  void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};


#endif // iTabBarMultiRow_h
