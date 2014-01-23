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

#ifndef taSigLinkMethodSpace_h
#define taSigLinkMethodSpace_h 1

// parent includes:
#include <taSigLinkTypeItemSpace>

// member includes:

// declare all other types mentioned but not required to include:
class MethodSpace;


taTypeDef_Of(taSigLinkMethodSpace);

class TA_API taSigLinkMethodSpace: public taSigLinkTypeItemSpace {
  // SigLink for MethodSpace objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkTypeItemSpace)
public:
  MethodSpace*          data() {return (MethodSpace*)m_data;}
  using inherited::GetListChild;
  taiSigLink* GetListChild(int itm_idx) CPP11_OVERRIDE; // returns NULL when no more
  int          NumListCols() const CPP11_OVERRIDE;
  static const KeyString key_rval;
  static const KeyString key_params;
  const KeyString GetListColKey(int col) const CPP11_OVERRIDE;
  String       GetColHeading(const KeyString& key) const CPP11_OVERRIDE;
  String       ChildGetColText(taSigLink* child, const KeyString& key,
    int itm_idx = -1) const CPP11_OVERRIDE;

  taSigLinkMethodSpace(MethodSpace* data_);
  SL_FUNS(taSigLinkMethodSpace) //
};

#endif // taSigLinkMethodSpace_h
