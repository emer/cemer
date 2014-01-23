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

#ifndef iPanelOfSelectEdit_h
#define iPanelOfSelectEdit_h 1

// parent includes:
#include <iPanel>
#include <SelectEdit>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditorOfSelectEditBase; // #IGNORE

taTypeDef_Of(iPanelOfSelectEdit);

class TA_API iPanelOfSelectEdit: public iPanel {
// ##NO_CSS note: SelectEdit main panel (for users)
INHERITED(iPanel)
  Q_OBJECT
public:
  taiEditorOfSelectEditBase*     se;
  SelectEdit*           sele() {return (m_link) ? (SelectEdit*)(link()->data()) : NULL;}

  bool         HasChanged() override; // 'true' if user has unsaved changes

  String       panel_type() const override {return "Select Edit";}

  iPanelOfSelectEdit(taiSigLink* dl_);
  ~iPanelOfSelectEdit();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfSelectEdit;}
  bool         ignoreSigEmit() const override;

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override; //
  void         OnWindowBind_impl(iPanelViewer* itv) override;
  void         UpdatePanel_impl() override;
  void         ResolveChanges_impl(CancelOp& cancel_op) override;
  void         showEvent(QShowEvent* ev) override;
};

#endif // iPanelOfSelectEdit_h
