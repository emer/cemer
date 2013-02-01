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

#ifndef iDataPanelSetBase_h
#define iDataPanelSetBase_h 1

// parent includes:
#include <iDataPanel>

// member includes:
#include <iDataPanel_PtrList>

// declare all other types mentioned but not required to include:
class iTabView; //
class QStackedWidget; //

TypeDef_Of(iDataPanelSetBase);

class TA_API iDataPanelSetBase: public iDataPanel { //  common subclass for iDataPanelSet and iViewPanelSet
  Q_OBJECT
INHERITED(iDataPanel)
public:
  QWidget*              widg;
  QVBoxLayout*          layDetail;
  QStackedWidget*         wsSubPanels; // subpanels -- note, built by descendents

  int                   cur_panel_id; // -1 if none
  iDataPanel_PtrList    panels;

  override taiSigLink* par_link() const {return (m_tabView) ? m_tabView->par_link() : NULL;}
  override MemberDef*   par_md() const {return (m_tabView) ? m_tabView->par_md() : NULL;}
  override iTabViewer* tabViewerWin() const {return (m_tabView) ? m_tabView->tabViewerWin() : NULL;}

  iDataPanel*           curPanel() const {return panels.SafeEl(cur_panel_id);} // NULL if none
  override void         setTabView(iTabView* tv); // set for kids too

  override void         Closing(CancelOp& cancel_op);
  override void         ClosePanel();
  iDataPanel*           GetDataPanelOfType(TypeDef* typ, int& start_idx);
    // get the first data panel of the specified type, starting at panel index; NULL if none
  override const iColor GetTabColor(bool selected, bool& ok) const;
  override bool         HasChanged();
  override void         ResolveChanges(CancelOp& cancel_op); // do the children first, then our impl
  override void         UpdatePanel(); // iterate over all kiddies
  override void         GetWinState(); // when saving view state
  override void         SetWinState(); // when showing, from view state

  iDataPanelSetBase(taiSigLink* dl_);
  ~iDataPanelSetBase();

public slots:
  void                  setCurrentPanelId(int id);

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override void         DataLinkDestroying(taSigLink* dl);
  override TypeDef*     GetTypeDef() const {return &TA_iDataPanelSetBase;}

protected:
  virtual void          setCurrentPanelId_impl(int id) {}
  void                  removeChild(QObject* obj);
  override void         OnWindowBind_impl(iTabViewer* itv);
};

#endif // iDataPanelSetBase_h
