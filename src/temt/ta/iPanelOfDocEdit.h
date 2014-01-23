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

#ifndef iPanelOfDocEdit_h
#define iPanelOfDocEdit_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditorOfDoc; // #IGNORE
class taDoc; //


taTypeDef_Of(iPanelOfDocEdit);

class TA_API iPanelOfDocEdit: public iPanel {
  // a panel frame for editing doc raw HTML/wiki source text
  Q_OBJECT
INHERITED(iPanel)
public:
  taiEditorOfDoc*      de; // the doc editor

  taDoc*                doc() {return (m_link) ? (taDoc*)(link()->data()) : NULL;}
  String       panel_type() const override {return "Doc Source";}

  bool         HasChanged() override; // 'true' if user has unsaved changes
  void                  FillList();

  QWidget*     firstTabFocusWidget() override;

  iPanelOfDocEdit(taiSigLink* dl_);
  ~iPanelOfDocEdit();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfDocEdit;}
  bool         ignoreSigEmit() const override;

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override; //
  void         OnWindowBind_impl(iPanelViewer* itv) override;
  void         UpdatePanel_impl() override;
  void         ResolveChanges_impl(CancelOp& cancel_op) override;
};

#endif // iPanelOfDocEdit_h
