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

#ifndef taClassDataLink_h
#define taClassDataLink_h 1

// parent includes:
#include <taiDataLink>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taClassDataLink: public taiDataLink { // DataLink for TypeInfo objects
INHERITED(taiDataLink)
public:
  const taMisc::TypeInfoKind    tik;
  TypeDef*              type() const {return m_type;}

  override TypeDef*     GetDataTypeDef() const;

  DL_FUNS(taClassDataLink); //

protected:
  TypeDef*              m_type;
  taClassDataLink(taMisc::TypeInfoKind tik_, void* data_, taDataLink* &link_ref_);  //
};

#endif // taClassDataLink_h
