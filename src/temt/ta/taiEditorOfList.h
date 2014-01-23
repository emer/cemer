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

#ifndef taiEditorOfList_h
#define taiEditorOfList_h 1

// parent includes:
#include <taiEditorWidgetsMulti>

// member includes:
#include <taiListMemberWidgets_List>
#include <String_PArray>


// declare all other types mentioned but not required to include:

class TA_API taiEditorOfList : public taiEditorWidgetsMulti {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditorWidgetsMulti)
public:
  taiListMemberWidgets_List    lst_widget_el;    // list of widget elements
  taList_impl*               cur_lst;
//  MemberSpace         lst_membs;      // list of members
  String_PArray         lst_membs; // list of members, by name
  int                   num_lst_fields; // number of editble list memberdefs with fields

  taiEditorOfList(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0); //(TypeDef* tp, void* base);
  taiEditorOfList()                             { };
  ~taiEditorOfList(); //

protected:
  void         GetImage_Membs() CPP11_OVERRIDE;
  void         GetValue_Membs() CPP11_OVERRIDE;
  void         ClearMultiBody_impl() CPP11_OVERRIDE; // clears multi-body for reshowing

  void         Constr_Strings() CPP11_OVERRIDE;
  void         Constr_MultiBody() CPP11_OVERRIDE;
  virtual void          Constr_ElWidget();
  virtual void          Constr_ListWidget();      // construct list members themselves
  virtual void          Constr_ListLabels();    // construct list labels themselves
  void         Constr_Final() CPP11_OVERRIDE; //TEMP
};

#endif // taiEditorOfList_h
