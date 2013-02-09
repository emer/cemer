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

#ifndef UserDataItemBase_h
#define UserDataItemBase_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(UserDataItemBase);

class TA_API UserDataItemBase: public taNBase {
  // ##INLINE ##NO_TOKENS base class for all simple user data -- name is key
INHERITED(taNBase)
public://
//note: we hide the name, because we don't want it inline since we have a dedicated editor
#ifdef __MAKETA__
  String                name; // #HIDDEN #READ_ONLY
#endif

  virtual bool          canDelete() const {return false;}
    // whether item can be manually deleted by user
  virtual bool          canRename() const {return false;}
    // but rename system-created items at your own peril!!!
  virtual bool          isSimple() const {return false;}
    // only true for UserDataItem class
  virtual bool          isVisible() const {return false;}
    // in general, custom guys are hidden on UserData page, unless they override

  virtual const Variant valueAsVariant() const {return _nilVariant;}
  virtual bool          setValueAsVariant(const Variant& value) {return false;}
  virtual bool          SetDesc(const String& desc) {return false;}

  TA_BASEFUNS(UserDataItemBase)
protected:
  UserDataItemBase(const String& type_name, const String& key); // for schema constructors
private:
  NOCOPY(UserDataItemBase)
  void Initialize() {}
  void Destroy() {}
};

TA_SMART_PTRS(UserDataItemBase);

#endif // UserDataItemBase_h
