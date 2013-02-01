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

#ifndef iTabViewer_h
#define iTabViewer_h 1

// parent includes:
#include <iFrameViewer>

// member includes:
#include <ContextFlag>
#include <ISelectable>

// declare all other types mentioned but not required to include:
class iTabView;
class iDataPanel;
class PanelViewer;
class iTabBarBase;


class TA_API iTabViewer : public iFrameViewer { // viewer window used for tabbed window
    Q_OBJECT
INHERITED(iFrameViewer)
friend class iTabView;
public:
  virtual taiSigLink*  sel_link() const {return (cur_item) ? cur_item->link() : NULL;} // datalink of selected item that is controlling the current data panel view, ex. datalink of the selected tree node in a browser; return NULL if unknown, mult-select is in force, etc. -- controls things like clip handling
  virtual MemberDef*    sel_md() const {return (cur_item) ? cur_item->md() : NULL;}; // as for sel_link
  override int          stretchFactor() const {return 4;} // 3/2 default
  iTabView*             tabView() {return m_curTabView;} // currently active
//  iTabView_PtrList*   tabViews() {return m_tabViews;} // currently active
  iTabBarBase*          tabBar();

  virtual void          AddPanel(iDataPanel* panel); // adds a new pane, and sets active in current tab
  void                  AddPanelNewTab(iDataPanel* panel, bool lock = false); // adds a new tab, sets panel active in it, locks if requested
//obs  virtual iTabView*        AddTabView(QWidget* parCtrl); // adds a new tab view
  void                  ShowLink(taiSigLink* link, bool not_in_cur = false);
  void                  ShowPanel(iDataPanel* panel); // shows the panel, according to showing rules

  virtual void          TabView_Destroying(iTabView* tv); // called when a tabview deletes
  virtual void          TabView_Selected(iTabView* tv); // called when a tabview gets focus
  override void         UpdateTabNames(); // called by a datalink when a tab name might have changed
  iTabViewer(PanelViewer* viewer_, QWidget* parent = NULL); //
  ~iTabViewer();

public slots:
  virtual void          AddTab(); // causes tab bar to open a new tab, on current panel
  virtual void          CloseTab(); // causes current tab to close (unless only 1 tab)
  void                  Closing(CancelOp& cancel_op); // override

protected: // IDataViewWidget i/f
  override void         Refresh_impl();

protected:
//  iTabView_PtrList*   m_tabViews; // all created tab views
  iTabView*             m_curTabView; // tab view (split) that currently has the focus
  ISelectable*          cur_item; // the last item that was curItem -- NOTE: somewhat dangerous to cache, but according to spec, src_host should issue a new notify if this deletes
  ContextFlag           tab_changing; // lets us ignore spurious re-entrant tab changes, ex. bugID:817

  override void         Constr_post(); // called virtually, in DV::Constr_post
  override void         ResolveChanges_impl(CancelOp& cancel_op);
  override void         SelectionChanged_impl(ISelectableHost* src_host); // called when sel changes
  override void         GetWinState_impl();
  override void         SetWinState_impl();

  override void         focusInEvent(QFocusEvent* ev);

private:
  void                  Init();
};

#endif // iTabViewer_h
