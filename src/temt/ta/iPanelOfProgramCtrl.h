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

#ifndef iPanelOfProgramCtrl_h
#define iPanelOfProgramCtrl_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditorOfProgramCtrl; // #IGNORE
class Program; //


taTypeDef_Of(iPanelOfProgramCtrl);

class TA_API iPanelOfProgramCtrl: public iPanel {
// ##NO_CSS panel for program control panel
INHERITED(iPanel)
  Q_OBJECT
public:
  taiEditorOfProgramCtrl* pc;

  Program*              prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  String       panel_type() const CPP11_OVERRIDE {return "Program Ctrl";}

  bool         HasChanged_impl() CPP11_OVERRIDE; // 'true' if user has unsaved changes
  void                  FillList();

  iPanelOfProgramCtrl(taiSigLink* dl_);
  ~iPanelOfProgramCtrl();

public: // ISigLinkClient interface
  void*        This() CPP11_OVERRIDE {return (void*)this;}
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_iPanelOfProgramCtrl;}

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) CPP11_OVERRIDE;
  void         OnWindowBind_impl(iPanelViewer* itv) CPP11_OVERRIDE;
  void         UpdatePanel_impl() CPP11_OVERRIDE;
  void         ResolveChanges_impl(CancelOp& cancel_op) CPP11_OVERRIDE;
  void         showEvent(QShowEvent* ev) CPP11_OVERRIDE;
};

#endif // iPanelOfProgramCtrl_h
