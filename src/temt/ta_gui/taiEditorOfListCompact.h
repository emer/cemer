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

#ifndef taiEditorOfListCompact_h
#define taiEditorOfListCompact_h 1

// parent includes:
#include <taiEditorWidgetsMulti>

// member includes:
#include <taiListTypeWidgets_List>

// declare all other types mentioned but not required to include:

class TA_API taiEditorOfListCompact : public taiEditorWidgetsMulti {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS compact vertical list for when the els have an inline rep
INHERITED(taiEditorWidgetsMulti)
public:
  taList_impl*               cur_lst;
  taiListTypeWidgets_List     lst_widget_el;    // list of (inline) data elements

  taiEditorOfListCompact(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0); //(TypeDef* tp, void* base);
  taiEditorOfListCompact()                              { };
  ~taiEditorOfListCompact();

  bool          ShowMember(MemberDef* md) const;

protected:
  void         GetImage_Membs() override;
  void         GetValue_Membs() override;
  void         ClearMultiBody_impl() override; // clears multi-body for reshowing

  void         Constr_Strings() override;
  void         Constr_MultiBody() override;
  virtual void          Constr_ElWidget();
  virtual void          Constr_ListWidget();      // construct list members themselves
  void         Constr_Final() override; //TEMP
};

#endif // taiEditorOfListCompact_h
