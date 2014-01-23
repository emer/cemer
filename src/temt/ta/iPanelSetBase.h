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

#ifndef iPanelSetBase_h
#define iPanelSetBase_h 1

// parent includes:
#include <iPanelBase>

// member includes:
#include <iPanelBase_PtrList>

// declare all other types mentioned but not required to include:
class iTabView; //
class QStackedWidget; //

taTypeDef_Of(iPanelSetBase);

class TA_API iPanelSetBase: public iPanelBase { //  common subclass for iPanelSet and iViewPanelSet
  Q_OBJECT
INHERITED(iPanelBase)
public:
  QWidget*              widg;
  QVBoxLayout*          layDetail;
  QStackedWidget*         wsSubPanels; // subpanels -- note, built by descendents

  int                   cur_panel_id; // -1 if none
  iPanelBase_PtrList    panels;

  taiSigLink* par_link() const CPP11_OVERRIDE {return (m_tabView) ? m_tabView->par_link() : NULL;}
  MemberDef*   par_md() const CPP11_OVERRIDE {return (m_tabView) ? m_tabView->par_md() : NULL;}
  iPanelViewer* tabViewerWin() const CPP11_OVERRIDE {return (m_tabView) ? m_tabView->tabViewerWin() : NULL;}

  iPanelBase*           curPanel() const {return panels.SafeEl(cur_panel_id);} // NULL if none
  void         setTabView(iTabView* tv) CPP11_OVERRIDE; // set for kids too

  void         Closing(CancelOp& cancel_op) CPP11_OVERRIDE;
  void         ClosePanel() CPP11_OVERRIDE;
  iPanelBase*           GetDataPanelOfType(TypeDef* typ, int& start_idx);
    // get the first data panel of the specified type, starting at panel index; NULL if none
  const iColor GetTabColor(bool selected, bool& ok) const CPP11_OVERRIDE;
  bool         HasChanged() CPP11_OVERRIDE;
  void         ResolveChanges(CancelOp& cancel_op) CPP11_OVERRIDE; // do the children first, then our impl
  void         UpdatePanel() CPP11_OVERRIDE; // iterate over all kiddies
  void         GetWinState() CPP11_OVERRIDE; // when saving view state
  void         SetWinState() CPP11_OVERRIDE; // when showing, from view state

  iPanelSetBase(taiSigLink* dl_);
  ~iPanelSetBase();

public slots:
  void                  setCurrentPanelId(int id);

public: // ISigLinkClient interface
  void*        This() CPP11_OVERRIDE {return (void*)this;}
  void         SigLinkDestroying(taSigLink* dl) CPP11_OVERRIDE;
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_iPanelSetBase;}

protected:
  virtual void          setCurrentPanelId_impl(int id) {}
  void                  removeChild(QObject* obj);
  void         OnWindowBind_impl(iPanelViewer* itv) CPP11_OVERRIDE;
};

#endif // iPanelSetBase_h
