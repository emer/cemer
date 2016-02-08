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

#ifndef FunCall_RefList_h
#define FunCall_RefList_h 1

// parent includes:
#include <taBase_RefList>

// member includes:
#include <String_Array>

// declare all other types mentioned but not required to include:


taTypeDef_Of(FunCall_RefList);

class TA_API FunCall_RefList : public taBase_RefList {
  // adds a StringArray list so we can keep it in sync with the ref list
  
INHERITED(taBase_RefList)
public:
  virtual void        AddWithFunName(taBase* obj, String fun_name);
  virtual bool        RemoveIdxAndFunName(int index);
  virtual String      GetFunName(int index);
  void                Reset() override;

protected:
  String_Array      delayed_funcalls_name;
  
  void              SigLinkDestroying(taSigLink* dl) override; // #IGNORE
  
private:
};

#endif // FunCall_RefList_h
