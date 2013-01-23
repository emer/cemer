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

#ifndef UserData_DocLink_h
#define UserData_DocLink_h 1

// parent includes:
#include <UserDataItemBase>

// member includes:
#include <taDoc>

// declare all other types mentioned but not required to include:


TypeDef_Of(UserData_DocLink);

class TA_API UserData_DocLink: public UserDataItemBase {
  // a link to a doc -- when added to the userdata of an obj, will auto display the doc
INHERITED(UserDataItemBase)
public:
  taDocRef              doc; // the doc

  override void         SmartRef_DataDestroying(taSmartRef* ref, taBase* obj);
  TA_BASEFUNS(UserData_DocLink)
private:
  void Copy_(const UserData_DocLink& cp){doc = cp.doc;}
  void Initialize();
  void Destroy() {}
};

#endif // UserData_DocLink_h
