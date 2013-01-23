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

#ifndef UserDataItem_List_h
#define UserDataItem_List_h 1

// parent includes:
#include <UserDataItemBase>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class UserDataItem; // 


TypeDef_Of(UserDataItem_List);

class TA_API UserDataItem_List: public taGroup<UserDataItemBase> {
  // #CHILDREN_INLINE list of user data items
INHERITED(taGroup<UserDataItemBase>)
public:
  bool                  hasVisibleItems() const; // #IGNORE lets gui avoid putting up panel unless any user-visible items are present

  UserDataItem*         NewItem(const String& name, const Variant& value,
    const String& desc); // #CAT_UserData #BUTTON #NO_SAVE_ARG_VAL Make a new simple user data entry with given name and value (desc optional)

  TA_BASEFUNS_NOCOPY(UserDataItem_List)
protected:

private:
  void Initialize() {SetBaseType(&TA_UserDataItemBase);}
  void Destroy() {}
};

TA_SMART_PTRS(UserDataItem_List) // UserDataItem_ListPtr

#endif // UserDataItem_List_h
