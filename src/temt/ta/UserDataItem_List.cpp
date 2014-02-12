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

#include "UserDataItem_List.h"
#include <UserDataItem>

TA_BASEFUNS_CTORS_DEFN(UserDataItem_List);

bool UserDataItem_List::hasVisibleItems() const {
  // iterate all items and return true on first isVisible found
  FOREACH_ELEM_IN_GROUP(UserDataItemBase, udi, *this) {
    if (udi->isVisible()) return true;
  }
  return false;
}

UserDataItem* UserDataItem_List::NewItem(const String& name, const Variant& value,
    const String& desc)
{
  if (TestError((name.empty() || (FindName(name) != NULL)),
    "UserDataItem_List::NewItem",
    "name must be a valid name, not already in the list")) {
    return NULL;
  }
  // note: make the item w/o owner, then rename and insert, to avoid unnecessary updates
  UserDataItem* udi = new UserDataItem;
  udi->name = name;
  udi->value = value;
  udi->desc = desc;
  Add(udi);
  return udi;
}

