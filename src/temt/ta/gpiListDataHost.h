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

#ifndef gpiListDataHost_h
#define gpiListDataHost_h 1

// parent includes:
#include <gpiMultiEditDataHost>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API gpiListDataHost : public gpiMultiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(gpiMultiEditDataHost)
public:
  gpiList_ElDataList    lst_data_el;    // list of data elements
  taList_impl*               cur_lst;
//  MemberSpace         lst_membs;      // list of members
  String_PArray         lst_membs; // list of members, by name
  int                   num_lst_fields; // number of editble list memberdefs with fields

  gpiListDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0); //(TypeDef* tp, void* base);
  gpiListDataHost()                             { };
  ~gpiListDataHost(); //

protected:
  override void         GetImage_Membs();
  override void         GetValue_Membs();
  override void         ClearMultiBody_impl(); // clears multi-body for reshowing

  override void         Constr_Strings();
  override void         Constr_MultiBody();
  virtual void          Constr_ElData();
  virtual void          Constr_ListData();      // construct list members themselves
  virtual void          Constr_ListLabels();    // construct list labels themselves
  override void         Constr_Final(); //TEMP
};

#endif // gpiListDataHost_h
