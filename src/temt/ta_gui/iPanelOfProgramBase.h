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

#ifndef iPanelOfProgramBase_h
#define iPanelOfProgramBase_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class iProgramEditor;

taTypeDef_Of(iPanelOfProgramBase);

class TA_API iPanelOfProgramBase: public iPanel {
// ##NO_CSS note: ProgramPanel and ProgramGroupPanel are almost identical
INHERITED(iPanel)
  Q_OBJECT
public:
  iProgramEditor*       pe;

  bool         HasChanged_impl() override; // 'true' if user has unsaved changes
  void                  FillList();
  QWidget*     firstTabFocusWidget() override;

  iPanelOfProgramBase(taiSigLink* dl_);

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfProgramBase;}

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override; //
  void         OnWindowBind_impl(iPanelViewer* itv) override;
  void         UpdatePanel_impl() override;
  void         ResolveChanges_impl(CancelOp& cancel_op) override;
  void         showEvent(QShowEvent* ev) override;

protected slots:
  void                  mb_Expert(bool checked); // expert button on minibar
  void                  mb_Lines(int val); // lines spin on minibar
};

#endif // iPanelOfProgramBase_h
