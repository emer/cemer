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

#ifndef iDataPanel_PtrList_h
#define iDataPanel_PtrList_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class iTabView;  //
class iDataPanel; //

class TA_API iDataPanel_PtrList: public taPtrList<iDataPanel> { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  iTabView*     m_tabView; // optional, we manage the refs in panels if set
  iDataPanel_PtrList() {m_tabView = NULL;}
  ~iDataPanel_PtrList() {}
protected:
  override void*        El_Own_(void* it); // set ref to our tabview (on Add, not Link)
  override void         El_disOwn_(void* it_); // remove ref if it is our tabview
};

#endif // iDataPanel_PtrList_h
