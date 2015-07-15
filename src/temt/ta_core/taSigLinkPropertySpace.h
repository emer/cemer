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

#ifndef taSigLinkPropertySpace_h
#define taSigLinkPropertySpace_h 1

// parent includes:
#include <taSigLinkTypeItemSpace>

// member includes:

// declare all other types mentioned but not required to include:
class PropertySpace;

taTypeDef_Of(taSigLinkPropertySpace);

class TA_API taSigLinkPropertySpace: public taSigLinkTypeItemSpace {
  // SigLink for PropertySpace objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkTypeItemSpace)
public:
  PropertySpace*                data() {return (PropertySpace*)m_data;}
  using inherited::GetListChild;
  taiSigLink* GetListChild(int itm_idx) override; // returns NULL when no more
  int          NumListCols() const override;
  const KeyString GetListColKey(int col) const override;
  String       GetColHeading(const KeyString& key) const override;
  String       ChildGetColText(taSigLink* child, const KeyString& key,
    int itm_idx = -1) const override;

  taSigLinkPropertySpace(PropertySpace* data_);
  SL_FUNS(taSigLinkPropertySpace) //
};

#endif // taSigLinkPropertySpace_h
