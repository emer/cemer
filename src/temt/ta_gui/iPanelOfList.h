// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef iPanelOfList_h
#define iPanelOfList_h 1

// parent includes:
#include <iPanel>

// member includes:

// declare all other types mentioned but not required to include:
class iTreeView; //
class QTreeWidgetItem; //

taTypeDef_Of(iPanelOfList);

class TA_API iPanelOfList: public iPanel {
  Q_OBJECT
#ifndef __MAKETA__
typedef iPanel inherited;
#endif
public:
  iTreeView*            list; //actually an iLDPListView

  String       panel_type() const override; // this string is on the subpanel button for this panel
  void         ClearList(); // for when data changes -- we just rebuild the list
  void         FillList();
  void         RenumberList();

  QWidget*     firstTabFocusWidget() override;

  iPanelOfList(taiSigLink* dl_, const String& custom_name = _nilString);
  ~iPanelOfList();

public: // ISigLinkClient interface
  void*        This() override {return (void*)this;}
  TypeDef*     GetTypeDef() const override {return &TA_iPanelOfList;}

public slots:
  void         list_itemDoubleClicked(QTreeWidgetItem* item, int col);
    // for editing

protected:
  String       m_custom_name; // used instead of "List View", typically for defchild lists
  void         ConfigHeader();
  void         SigEmit_impl(int sls, void* op1, void* op2) override; //
  void         UpdatePanel_impl() override;
  void         OnWindowBind_impl(iPanelViewer* itv) override;
};

#endif // iPanelOfList_h
