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

#ifndef iPanel_h
#define iPanel_h 1

// parent includes:
#include <iPanelBase>

// member includes:

// declare all other types mentioned but not required to include:
class iPanelSet; //
class QHBoxLayout; //


taTypeDef_Of(iPanel);

class TA_API iPanel: public iPanelBase {
  // this is the base for all panels for regular data items that the user controls the viewing of -- as contrasted with iViewPanel which is only for view control panels
  Q_OBJECT
INHERITED(iPanelBase)
friend class iPanelSet;
public:
  QWidget*              minibarCtrls() const {return m_minibar_ctrls;}
    //non-null if any have been added
  inline bool           hasMinibarCtrls() const {return (m_minibar_ctrls);}
  override taiSigLink* par_link() const; // taken from dps if any, else from tabview
  override MemberDef*   par_md() const;
  override iPanelViewer*  tabViewerWin() const;

  iPanelSet*        data_panel_set() { return m_dps; }
  override void         ClearDataPanelSet() {m_dps = NULL;}
  override void         ClosePanel();
  override void         UpdatePanel(); // we add a test for HasChanged and invoke the inherited
  override String       TabText() const; // text for the panel tab -- usually just the text of the sel_node

  void                  AddMinibarWidget(QWidget* ctrl);
    // adds the ctrl (typically a tool button) to the minibar (area to right of PanelSet selector buttons); note: right-justified, and fills inward; ctrl should be parentless; can force the DPF to be put into a set, if wouldn't have been otherwise

  iPanel(taiSigLink* dl_);
  ~iPanel();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override void         SigLinkDestroying(taSigLink* dl); // called by SigLink when it is destroying --
  override TypeDef*     GetTypeDef() const {return &TA_iPanel;}

protected:
  iPanelSet*        m_dps; // set if we are in a datapanelset
  QWidget*              m_minibar_ctrls;
  QHBoxLayout*          lay_minibar_ctrls;
};

#endif // iPanel_h
