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

#ifndef iPanelOfEditor_h
#define iPanelOfEditor_h 1

// parent includes:
#include <iPanel>

// member includes:
#include <taString>

// declare all other types mentioned but not required to include:
class iColor;
class taiEditorOfClass; // #IGNORE


TypeDef_Of(iPanelOfEditor);

class TA_API iPanelOfEditor: public iPanel {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for any edit dialog-like data panel
INHERITED(iPanel)
friend class taiEditorOfClass;
public:
  iColor*               bgcol; // temp holding spot
  taiEditorOfClass*      editDataHost() {return owner;}
  override String       panel_type() const; // this string is on the subpanel button for this panel
  override void         Closing(CancelOp& cancel_op);
  override const iColor GetTabColor(bool selected, bool& ok) const; // special color for tab; NULL means use default
  override bool         HasChanged_impl(); // 'true' if user has unsaved changes -- used to prevent browsing away
  override void         UpdatePanel(); // always do it, even when hidden; the edit sorts it out
  override QWidget*     firstTabFocusWidget();

  iPanelOfEditor(taiEditorOfClass* owner_, taiSigLink* dl_);
  ~iPanelOfEditor();

public: // ISigLinkClient interface
  override TypeDef*     GetTypeDef() const {return &TA_iPanelOfEditor;}

protected:
  taiEditorOfClass*      owner;
  override void         UpdatePanel_impl(); // the refresh guy!
  override void         Render_impl();
  override void         ResolveChanges_impl(CancelOp& cancel_op);
  override void         showEvent(QShowEvent* ev);

};//

#endif // iPanelOfEditor_h
