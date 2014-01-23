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


taTypeDef_Of(taSigLinkTypeSpace);

class TA_API taSigLinkTypeSpace: public taSigLinkTypeItemSpace {
  // SigLink for TypeSpace objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkTypeItemSpace)
public:
  enum DisplayMode { // #BITS
    DM_None             = 0x0000, // #NO_BIT
    DM_ShowRoot         = 0x0001, // show global items (really only applies to a root type space)
    DM_ShowNonRoot      = 0x0002, // show inherited items (really only applies to a root type space)
    DM_ShowEnums        = 0x0004, // show types that are enums (really only applies to .sub_types)
    DM_ShowNonEnums     = 0x0008, // show types that are not enums (really only applies to .sub_types)

    DM_DefaultRoot      = DM_ShowRoot | DM_ShowNonEnums, // #NO_BIT
    DM_DefaultChildren  = DM_ShowNonRoot | DM_ShowNonEnums, // #NO_BIT
    DM_DefaultEnum      = DM_ShowEnums,                     // #NO_BIT
    DM_DefaultSubTypes  = DM_ShowNonRoot | DM_ShowNonEnums, // #NO_BIT
    DM_Default          = DM_DefaultChildren                // #NO_BIT
  };

  DisplayMode           dm;
  TypeSpace*            data() {return (TypeSpace*)m_data;}
  using inherited::GetListChild;
  taiSigLink* GetListChild(int itm_idx) CPP11_OVERRIDE; // returns NULL when no more
  int          NumListCols() const CPP11_OVERRIDE;
  const KeyString GetListColKey(int col) const CPP11_OVERRIDE;
  String       GetColHeading(const KeyString& key) const CPP11_OVERRIDE;
  String       ChildGetColText(taSigLink* child, const KeyString& key,
    int itm_idx = -1) const CPP11_OVERRIDE;

  bool                  ShowChild(TypeDef* td) const; // determine whether to show, based on dm

  taSigLinkTypeSpace(TypeSpace* data_);
  SL_FUNS(taSigLinkTypeSpace) //
};

#endif // taSigLinkTypeSpace_h
