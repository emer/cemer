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

#ifndef gpiArrayEditDataHost_h
#define gpiArrayEditDataHost_h 1

// parent includes:
#include <taiEditDataHost>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API gpiArrayEditDataHost : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditDataHost)
public:
  bool          ShowMember(MemberDef* md) const;

  gpiArrayEditDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  gpiArrayEditDataHost()                { };
  ~gpiArrayEditDataHost();
protected:
  int                   array_set; // index of array_set
  override void         GetValue_Membs();
  override void         GetImage_Membs();
  override void         ClearBody_impl();
  override void         Constr_Data_Labels();
  virtual void          Constr_AryData_Labels();
};

#endif // gpiArrayEditDataHost_h
