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

#ifndef iDataPanelSet_h
#define iDataPanelSet_h 1

// parent includes:
#include <iDataPanelSetBase>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditDataHost; //
class QStackedLayout; //
class iDataPanelFrame; //
class QHBoxLayout; // 
class QButtonGroup; //
class QFrame; //

TypeDef_Of(iDataPanelSet);

class TA_API iDataPanelSet: public iDataPanelSetBase { //  contains 0 or more sub-data-panels, and a small control bar for selecting panels
  Q_OBJECT
INHERITED(iDataPanelSetBase)
public:
//QVBoxLayout*          layDetail;
  QFrame*                 frmButtons;
  QHBoxLayout*            layButtons;
  QButtonGroup*             buttons; // one QPushButton for each (note: not a widget)
  QStackedLayout*           layMinibar; // if any panels use the minibar, created
  taiEditDataHost*      method_box_mgr; // edh object that manages the method box!

  void                  setPanelAvailable(iDataPanel* pn); // dynamically show/hide a btn/pn

  void                  SetMenu(QWidget* menu); // sets the menu (s/b a menubar; or toolbar on mac)
  void                  AddSubPanel(iDataPanelFrame* pn);
  void                  AllSubPanelsAdded(); // call after all subpanels added, to finalize layout
  void                  AddSubPanelDynamic(iDataPanelFrame* pn); // call this after fully built to dynamically add a new frame
  void                  SetMethodBox(QWidget* meths,
                                     taiEditDataHost* mgr);
  // sets a box that contains methods, on bottom, along with manager of those buttons
  void                  UpdateMethodButtons();
  // update the method buttons, by calling on manager

  override QWidget*     firstTabFocusWidget();

  iDataPanelSet(taiSigLink* dl_);
  ~iDataPanelSet();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
//  override void               SigLinkDestroying(taSigLink* dl) {} // nothing for us; subpanels handle
  override TypeDef*     GetTypeDef() const {return &TA_iDataPanelSet;}
protected:
  override void         SigEmit_impl(int sls, void* op1, void* op2); // dyn subpanel detection

protected:
  override void         setCurrentPanelId_impl(int id);
  void                  AddMinibar();
  void                  AddMinibarCtrls();
  void                  AddMinibarCtrl(iDataPanelFrame* pn);
};

#endif // iDataPanelSet_h
