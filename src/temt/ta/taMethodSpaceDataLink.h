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

#ifndef taMethodSpaceDataLink_h
#define taMethodSpaceDataLink_h 1

// parent includes:
#include <taTypeSpaceDataLink_Base>

// member includes:

// declare all other types mentioned but not required to include:
class MethodSpace;


TypeDef_Of(taMethodSpaceDataLink);

class TA_API taMethodSpaceDataLink: public taTypeSpaceDataLink_Base {
  // DataLink for MethodSpace objects -- note that it also manages the ListView nodes
INHERITED(taTypeSpaceDataLink_Base)
public:
  MethodSpace*          data() {return (MethodSpace*)m_data;}
  USING(inherited::GetListChild)
  override taiDataLink* GetListChild(int itm_idx); // returns NULL when no more
  override int          NumListCols() const;
  static const KeyString key_rval;
  static const KeyString key_params;
  override const KeyString GetListColKey(int col) const;
  override String       GetColHeading(const KeyString& key) const;
  override String       ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const;

  taMethodSpaceDataLink(MethodSpace* data_);
  DL_FUNS(taMethodSpaceDataLink) //
};

#endif // taMethodSpaceDataLink_h
