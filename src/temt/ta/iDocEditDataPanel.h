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

#ifndef iDocEditDataPanel_h
#define iDocEditDataPanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class DocEditDataHost; // #IGNORE
class taDoc; //


class TA_API iDocEditDataPanel: public iDataPanelFrame {
  // a panel frame for editing doc raw HTML/wiki source text
  Q_OBJECT
INHERITED(iDataPanelFrame)
public:
  DocEditDataHost*      de; // the doc editor

  taDoc*                doc() {return (m_link) ? (taDoc*)(link()->data()) : NULL;}
  override String       panel_type() const {return "Doc Source";}

  override bool         HasChanged(); // 'true' if user has unsaved changes
  void                  FillList();

  override QWidget*     firstTabFocusWidget();

  iDocEditDataPanel(taiDataLink* dl_);
  ~iDocEditDataPanel();

public: // IDataLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iDocEditDataPanel;}
  override bool         ignoreDataChanged() const;

protected:
  override void         DataChanged_impl(int dcr, void* op1, void* op2); //
  override void         OnWindowBind_impl(iTabViewer* itv);
  override void         UpdatePanel_impl();
  override void         ResolveChanges_impl(CancelOp& cancel_op);
};

#endif // iDocEditDataPanel_h
