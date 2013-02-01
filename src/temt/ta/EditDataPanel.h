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

#ifndef EditDataPanel_h
#define EditDataPanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class iColor;
class taiEditorWidgetsOfClass; // #IGNORE


TypeDef_Of(EditDataPanel);

class TA_API EditDataPanel: public iDataPanelFrame {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for any edit dialog-like data panel
INHERITED(iDataPanelFrame)
friend class taiEditorWidgetsOfClass;
public:
  iColor*               bgcol; // temp holding spot
  taiEditorWidgetsOfClass*      editDataHost() {return owner;}
  override String       panel_type() const; // this string is on the subpanel button for this panel
  override void         Closing(CancelOp& cancel_op);
  override const iColor GetTabColor(bool selected, bool& ok) const; // special color for tab; NULL means use default
  override bool         HasChanged_impl(); // 'true' if user has unsaved changes -- used to prevent browsing away
  override void         UpdatePanel(); // always do it, even when hidden; the edit sorts it out
  override QWidget*     firstTabFocusWidget();

  EditDataPanel(taiEditorWidgetsOfClass* owner_, taiSigLink* dl_);
  ~EditDataPanel();

public: // ISigLinkClient interface
  override TypeDef*     GetTypeDef() const {return &TA_EditDataPanel;}

protected:
  taiEditorWidgetsOfClass*      owner;
  override void         UpdatePanel_impl(); // the refresh guy!
  override void         Render_impl();
  override void         ResolveChanges_impl(CancelOp& cancel_op);
  override void         showEvent(QShowEvent* ev);

};//

#endif // EditDataPanel_h
