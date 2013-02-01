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

#ifndef iUserDataPanel_h
#define iUserDataPanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class taiEditorWidgets; //
class UserDataItem_List; //

TypeDef_Of(iUserDataPanel);

class TA_API iUserDataPanel: public iDataPanelFrame {
// ##NO_CSS note: UserDataItem_List main panel (for users)
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  taiEditorWidgets*     se;
  UserDataItem_List*    udil() {return (m_link) ? (UserDataItem_List*)(link()->data()) : NULL;}

  override bool         HasChanged(); // 'true' if user has unsaved changes

  override String       panel_type() const {return "User Data";}

  iUserDataPanel(taiSigLink* dl_);
  ~iUserDataPanel();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iUserDataPanel;}
  override bool         ignoreSigEmit() const;

protected:
  override void         SigEmit_impl(int sls, void* op1, void* op2); //
  override void         OnWindowBind_impl(iTabViewer* itv);
  override void         UpdatePanel_impl();
  override void         ResolveChanges_impl(CancelOp& cancel_op);
};

#endif // iUserDataPanel_h
