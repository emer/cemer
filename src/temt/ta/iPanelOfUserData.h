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

#ifndef iPanelOfUserData_h
#define iPanelOfUserData_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditorWidgets; //
class UserDataItem_List; //

taTypeDef_Of(iPanelOfUserData);

class TA_API iPanelOfUserData: public iPanel {
// ##NO_CSS note: UserDataItem_List main panel (for users)
INHERITED(iPanel)
  Q_OBJECT
public:
  taiEditorWidgets*     se;
  UserDataItem_List*    udil() {return (m_link) ? (UserDataItem_List*)(link()->data()) : NULL;}

  bool         HasChanged() override; // 'true' if user has unsaved changes

  String       panel_type() const override {return "User Data";}

  iPanelOfUserData(taiSigLink* dl_);
  ~iPanelOfUserData();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfUserData;}
  bool         ignoreSigEmit() const override;

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) override; //
  void         OnWindowBind_impl(iPanelViewer* itv) override;
  void         UpdatePanel_impl() override;
  void         ResolveChanges_impl(CancelOp& cancel_op) override;
};

#endif // iPanelOfUserData_h
