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

#ifndef taSigLinkMemberSpace_h
#define taSigLinkMemberSpace_h 1

// parent includes:
#include <taSigLinkTypeItemSpace>

// member includes:

// declare all other types mentioned but not required to include:
class MemberSpace;


TypeDef_Of(taSigLinkMemberSpace);

class TA_API taSigLinkMemberSpace: public taSigLinkTypeItemSpace {
  // SigLink for MemberSpace objects -- note that it also manages the ListView nodes
INHERITED(taSigLinkTypeItemSpace)
public:
  MemberSpace*          data() {return (MemberSpace*)m_data;}
  USING(inherited::GetListChild)
  override taiSigLink* GetListChild(int itm_idx); // returns NULL when no more
  override int          NumListCols() const;
  override const KeyString GetListColKey(int col) const;
  override String       GetColHeading(const KeyString& key) const;
  override String       ChildGetColText(taSigLink* child, const KeyString& key,
    int itm_idx = -1) const;

  taSigLinkMemberSpace(MemberSpace* data_);
  SL_FUNS(taSigLinkMemberSpace) //
};

#endif // taSigLinkMemberSpace_h
