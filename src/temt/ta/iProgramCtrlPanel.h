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

#ifndef iProgramCtrlPanel_h
#define iProgramCtrlPanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class iProgramCtrlDataHost; // #IGNORE
class Program; //


TypeDef_Of(iProgramCtrlPanel);

class TA_API iProgramCtrlPanel: public iDataPanelFrame {
// ##NO_CSS panel for program control panel
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  iProgramCtrlDataHost* pc;

  Program*              prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  override String       panel_type() const {return "Program Ctrl";}

  override bool         HasChanged_impl(); // 'true' if user has unsaved changes
  void                  FillList();

  iProgramCtrlPanel(taiSigLink* dl_);
  ~iProgramCtrlPanel();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iProgramCtrlPanel;}

protected:
  override void         DataChanged_impl(int dcr, void* op1, void* op2);
  override void         OnWindowBind_impl(iTabViewer* itv);
  override void         UpdatePanel_impl();
  override void         ResolveChanges_impl(CancelOp& cancel_op);
  override void         showEvent(QShowEvent* ev);
};

#endif // iProgramCtrlPanel_h
