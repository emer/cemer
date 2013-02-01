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

#ifndef taSigLinkTypeSpace_h
#define taSigLinkTypeSpace_h 1

// parent includes:
#include <taSigLinkTypeItemSpace>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taSigLinkTypeSpace);

class TA_API taSigLinkTypeSpace: public taSigLinkTypeItemSpace {
  // DataLink for TypeSpace objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkTypeItemSpace)
public:
  enum DisplayMode { // #BITS
    DM_None             = 0x0000, // #NO_BIT
    DM_ShowRoot         = 0x0001, // show global items (really only applies to a root type space)
    DM_ShowNonRoot      = 0x0002, // show inherited items (really only applies to a root type space)
    DM_ShowEnums        = 0x0004, // show types that are enums (really only applies to .sub_types)
    DM_ShowNonEnums     = 0x0008, // show types that are not enums (really only applies to .sub_types)

#ifndef __MAKETA__
    DM_DefaultRoot      = DM_ShowRoot | DM_ShowNonEnums,
    DM_DefaultChildren  = DM_ShowNonRoot | DM_ShowNonEnums,
    DM_DefaultEnum      = DM_ShowEnums,
    DM_DefaultSubTypes  = DM_ShowNonRoot | DM_ShowNonEnums,
    DM_Default          = DM_DefaultChildren
#endif
  };

  DisplayMode           dm;
  TypeSpace*            data() {return (TypeSpace*)m_data;}
  USING(inherited::GetListChild)
  override taiSigLink* GetListChild(int itm_idx); // returns NULL when no more
  override int          NumListCols() const;
  override const KeyString GetListColKey(int col) const;
  override String       GetColHeading(const KeyString& key) const;
  override String       ChildGetColText(taSigLink* child, const KeyString& key,
    int itm_idx = -1) const;

  bool                  ShowChild(TypeDef* td) const; // determine whether to show, based on dm

  taSigLinkTypeSpace(TypeSpace* data_);
  DL_FUNS(taSigLinkTypeSpace) //
};

#endif // taSigLinkTypeSpace_h
