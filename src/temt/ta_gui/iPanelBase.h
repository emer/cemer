// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef iPanelBase_h
#define iPanelBase_h 1

// parent includes:
#include <ISigLinkClient>
#ifndef __MAKETA__
#include <QFrame>
#endif

// member includes:
#include <taString>
#include <taiMiscCore>
#include <iColor>
#ifndef __MAKETA__
#include <iTabBar>
#include <iTabView>
#else
class iTabBar; // #IGNORE
class iTabView; // #IGNORE
#endif

// declare all other types mentioned but not required to include:
class iTabBarBase; //
class iPanelViewer; //
class iMainWindowViewer; //
class QScrollBar; //
class QScrollArea; //
class QPoint; //
class QVBoxLayout; // 
class QHideEvent; //
class QShowEvent; //


taTypeDef_Of(iPanelBase);

class TA_API iPanelBase: public QFrame, public ISigLinkClient {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS interface for panels -- basic element that can appear in a tabbed viewing context (note: use iPanel or iViewPanel)
  Q_OBJECT
INHERITED(QFrame)
friend class taSigLink;
friend class iPanelTab;
friend class iPanelBase_PtrList;
friend class iPanelSetBase;
//friend class iPanelSet;
public:
#ifndef __MAKETA__
  enum CustomEventType { // note: just copied from taiEditorWidgetsMain, not all used
    CET_SHOW_PANEL      = QEvent::User + 1,  // to get panel to show
    CET_SHOW_PANEL_FOCUS,  // to get panel to show/focus
  };
#endif
  virtual QWidget*      centralWidget() const; // contents
  virtual void          setCentralWidget(QWidget* widg); // sets the contents
  virtual QWidget*      firstTabFocusWidget() { return NULL; } // first widget that accepts tab focus -- to set link between tab and contents of edit
  void                  setButtonsWidget(QWidget* widg); // is put at the bottom, not in a scroll
  virtual void          ClearDataPanelSet() {} // used for clearing by the set in its dtor
  virtual bool          dirty() {return HasChanged();}
    // true if panel should not be replaced, but a new panel should be opened for the new item
  virtual bool          lockInPlace() const {return false;}
    // true if panel should not be replaced
  virtual String        panel_type() const {return _nilString;}
   //  this string is on the subpanel button for a panel (n/a to panelsets)
  virtual taiSigLink*  par_link() const = 0; // *current* visual parent link of this data panel; this could change dynamically, if a datapanel is shared across all referring instances, ex. link lists, references, etc. -- return NULL if unknown, not set, or not applicable -- controls things like clip enabling etc.
  virtual MemberDef*    par_md() const = 0; // as for par_link
  inline bool           pinned() const {return m_pinned;}
  inline bool           rendered() const {return m_rendered;}
  void                  setPinned(bool value);
  bool                  updateOnShow() const {return m_update_on_show;} // usually true, but things like docs don't want it
  void                  setUpdateOnShow(bool val);
  bool                  isShowUpdating() const { return m_show_updt; }
  // true if currently updating due to a show event
//  taViewer*         viewer() {return (m_dps) ? m_dps->viewer() : m_tabView->viewer();}
#ifndef __MAKETA__
  iTabBar::TabIcon      tabIcon() const;
#endif
  iTabBarBase*          tabBar() {return NULL;}
  inline iTabView*      tabView() const {return m_tabView;} // tab view in which we are shown
  virtual void          setTabView(iTabView* value) {m_tabView = value;} // just set the value, no side effects
  virtual iPanelViewer*   tabViewerWin() const = 0;
  iMainWindowViewer*    viewerWindow() {return (m_tabView) ? m_tabView->viewerWindow() : NULL;}
  virtual bool          isViewPanelFrame() const {return false;} // we group the vpf's to the right, all others to the left

  virtual void          AddedToPanelSet() {} // called when fully added to DataPanelSet
  virtual void          Closing(CancelOp& cancel_op) {} // called to notify panel is(forced==true)/wants(forced=false) to close -- set cancel 'true' (if not forced) to prevent
  virtual void          ClosePanel() = 0; // anyone can call this to get the panel to close (ex. edit panel contents are deleted externally)
  //NOTE: due to various versioning/compatibility reasons, the following 2 routines get replaced sometimes in subclasses, to implement more elaborate rules before dispatching their worker bee impls
  virtual void          InitPanel() {InitPanel_impl();} // called when creating ViewPanels
  virtual void          UpdatePanel(); // called when reshowing a panel, to insure latest data
  virtual const iColor  GetTabColor(bool selected, bool& ok) const
    {ok = false; return iColor();} // special color for tab; NULL means use default
  virtual void          FrameShowing(bool showing, bool focus = false); // called esp by t3 frames when show/hide; lets us show hide the tabs

  virtual bool          HasChanged() {return HasChanged_impl();} // 'true' if user has unsaved changes -- used to autosave when browsing away etc.
  virtual bool          HasChanged_impl() {return false;} // this impl applies only to a single panel, not to sets
  virtual void          OnWindowBind(iPanelViewer* itv);
    // called in post, when all windows are built
  virtual void          ResolveChanges(CancelOp& cancel_op);
  virtual void          Render(); // actually create content; override _impl; used to defer creation of button panels
  virtual String        TabText() const; // text for the panel tab -- usually just the view_name of the curItem
  virtual void          GetWinState(); // when saving view state
  virtual void          SetWinState(); // when showing, from view state

  /////////////////////////////////////////////////////////////////
  //            ScrollArea Management

  virtual void          SaveScrollPos();
  // save the current vertical scroll position for later restore
  virtual void          RestoreScrollPos();
  // restore the current vertical scroll position
  virtual QScrollArea*  ScrollArea()    { return scr; }
  // controlling scroll area for this panel
  virtual QScrollBar*   ScrollBarV();
  // controlling vertical scroll bar for this panel
  virtual void          ScrollTo(int scr_pos);
  // scroll vertically to given position -- directly controls vertical scroll bar
  virtual void          CenterOn(QWidget* widg);
  // center the scrollbar on center of given widget
  virtual void          KeepInView(QWidget* widg);
  // ensure that the given widget is fully in view -- just move up or down as needed to keep fully in view
  virtual bool          PosInView(int scr_pos);
  // is given position within the main scroll area (in coordinates relative to central widget) within view?
  virtual QPoint        MapToPanel(QWidget* widg, const QPoint& pt);
  // map coordinate point within given child widget on panel to the coordinates of the panel scroll area
  virtual int           MapToPanelV(QWidget* widg, int pt_y);
  // map vertical coordinate value within given child widget on panel to the coordinates of the panel scroll area

  iPanelBase(taiSigLink* dl_); //note: created with no parent -- later added to stack
  ~iPanelBase();

public slots:
  void                Pin() {setPinned(true);}
  void                Unpin() {setPinned(false);}
  void                OpenInWindow();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelBase;}
  bool         ignoreSigEmit() const override {return (!isVisible());}
  void         SigLinkRecv(taSigLink*, int sls, void* op1, void* op2) override
    {SigEmit_impl(sls, op1, op2);} // called when the data item has changed, esp. ex lists and groups
  void         SigLinkDestroying(taSigLink* dl) override {} // called by SigLink when it is destroying --

protected:
  bool                  m_pinned;
  bool                  m_rendered; // set once rendered
  bool                  show_req; // set when we send the SHOW_REQ event
  bool                  m_update_on_show;
  bool                  m_show_updt;  // currently updating due to show
  bool                  m_update_req; // for when data changes while hidden
  QVBoxLayout*          layOuter;
  QScrollArea*          scr; // central scrollview
  int                   m_saved_scroll_pos;
  void         customEvent(QEvent* ev_) override;
  void         hideEvent(QHideEvent* ev) override; // auto-apply
  void         showEvent(QShowEvent* ev) override;
  virtual void          SigEmit_impl(int sls, void* op1, void* op2); // tab name may have changed
  virtual void          OnWindowBind_impl(iPanelViewer* itv) {}
  virtual void          Render_impl() {} // only called once, when content needs to be created
  virtual void          ResolveChanges_impl(CancelOp& cancel_op) {}

  virtual void          InitPanel_impl() {}
  virtual void          UpdatePanel_impl();
  virtual void          GetWinState_impl() {} // when saving view state
  virtual void          SetWinState_impl() {} // when showing, from view state
protected slots:
  virtual void          FrameShowing_Async(bool focus); // we forward async from FS (only when true) as a useful hack to make sure all constr etc is done before doing it

private:
  iTabView*             m_tabView; // force access through accessors only
};

#endif // iPanelBase_h
