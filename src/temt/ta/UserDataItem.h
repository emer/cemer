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

#ifndef UserDataItem_h
#define UserDataItem_h 1

// parent includes:
#include <UserDataItemBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(UserDataItem);

class TA_API UserDataItem: public UserDataItemBase {
  // an item of simple user data
INHERITED(UserDataItemBase)
public:
  Variant               value;  // #FIXTYPE_ON_val_type_fixed value for this user data item
  String                desc; // #NO_SAVE_EMPTY optional description (typ. used for schema, not items)
  bool                  val_type_fixed;  // #READ_ONLY is the value type fixed -- can be set programmatically to create simpler user data interfaces -- gui will not show type selector if this is true

  bool         canDelete() const override {return true;}
  bool         canRename() const override {return true;}
  bool         isSimple() const override {return true;}
  bool         isVisible() const override {return true;}

  const Variant valueAsVariant() const override {return value;}
  bool         setValueAsVariant(const Variant& v) override {value = v; return true;}

  String       GetDesc() const override {return desc;}
  bool         SetDesc(const String& d) override {desc = d; return true;}
  TA_BASEFUNS(UserDataItem);
  UserDataItem(const String& type_name, const String& key, const Variant& value,
               const String& desc = _nilString);
  // #IGNORE constructor for creating static (compile-time) schema instances
private:
  void Copy_(const UserDataItem& cp){value = cp.value; desc = cp.desc; val_type_fixed = cp.val_type_fixed; }
  void Initialize() { val_type_fixed = false; }
  void Destroy() {}
};

TA_SMART_PTRS(UserDataItem);

#endif // UserDataItem_h
