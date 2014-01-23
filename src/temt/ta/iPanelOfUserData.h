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

  bool         HasChanged() CPP11_OVERRIDE; // 'true' if user has unsaved changes

  String       panel_type() const CPP11_OVERRIDE {return "User Data";}

  iPanelOfUserData(taiSigLink* dl_);
  ~iPanelOfUserData();

public: // ISigLinkClient interface
  void*        This() CPP11_OVERRIDE {return (void*)this;}
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_iPanelOfUserData;}
  bool         ignoreSigEmit() const CPP11_OVERRIDE;

protected:
  void         SigEmit_impl(int sls, void* op1, void* op2) CPP11_OVERRIDE; //
  void         OnWindowBind_impl(iPanelViewer* itv) CPP11_OVERRIDE;
  void         UpdatePanel_impl() CPP11_OVERRIDE;
  void         ResolveChanges_impl(CancelOp& cancel_op) CPP11_OVERRIDE;
};

#endif // iPanelOfUserData_h
