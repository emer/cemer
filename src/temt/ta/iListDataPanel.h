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

#ifndef iListDataPanel_h
#define iListDataPanel_h 1

// parent includes:
#include <iDataPanelFrame>

// member includes:

// declare all other types mentioned but not required to include:
class iTreeView; //
class QTreeWidgetItem; //

TypeDef_Of(iListDataPanel);

class TA_API iListDataPanel: public iDataPanelFrame {
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
public:
  iTreeView*            list; //actually an iLDPListView

  override String       panel_type() const; // this string is on the subpanel button for this panel
  void                  ClearList(); // for when data changes -- we just rebuild the list
  void                  FillList();
  void                  RenumberList();

  override QWidget*     firstTabFocusWidget();

  iListDataPanel(taiSigLink* dl_, const String& custom_name = _nilString);
  ~iListDataPanel();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iListDataPanel;}

public slots:
  void                  list_itemDoubleClicked(QTreeWidgetItem* item, int col);
    // for editing

protected:
  String                m_custom_name; // used instead of "List View", typically for defchild lists
  void                  ConfigHeader();
  override void         SigEmit_impl(int sls, void* op1, void* op2); //
  override void         UpdatePanel_impl();
  override void         OnWindowBind_impl(iTabViewer* itv);
};

#endif // iListDataPanel_h
