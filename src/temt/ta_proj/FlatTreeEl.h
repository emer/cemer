// Co2018ght 2017-2017, Regents of the University of Colorado,
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

#ifndef FlatTreeEl_h
#define FlatTreeEl_h 1

// parent includes:
#include <taBase>
#include <taSmartRefT>

// member includes:
#include <taHashEl>

// declare all other types mentioned but not required to include:

taTypeDef_Of(FlatTreeEl);

class TA_API FlatTreeEl : public taBase {
  // ##NO_TOKENS #CAT_FlatTree an element of the flattened tree structure -- flat version of the structure hierarchy of taBase objects -- used in computing diffs for example
INHERITED(taBase)
public:
  int           idx;            // index in the list of flat elements
  int           nest_level;     // how deeply nested or embedded is this object in the obj hierarchy
  String        name;           // object name (member name, object name)
  String        value;          // string representation of value -- does NOT contain values for class object types
  TypeDef*      type;           // type of this object (same as mdef->type if a member) -- not type of parent
  MemberDef*    mdef;           // memberdef if this is a member of a parent object
  void*         addr;           // address in memory of this object -- already has the GetOff for members -- points directly to the memory
  taBaseRef     obj;            // the taBase object to which this belongs -- if mdef is set then this is the object to which this member belongs, NOT the taBase of the member itself!
  FlatTreeEl*   parent_el;      // flat tree element representing the parent of this one
  int           size;           // size of this object itself
  int           tot_size;       // size of this object and all the elements under it


  inline bool   IsObj() const { return (obj.ptr() != NULL); }
  // is this an object
  inline bool   IsNonMemberObj() const
  { return (obj.ptr() != NULL && mdef == NULL); }
  // is this a separate non-member object (i.e., lives on a list or group..)

  virtual taHashVal  ComputeHashCode() const;
  // compute the hash_code based on name, value and level -- used for differencing

  virtual bool MemberNoShow() const;
  // is this a member field, and if so, does it have CONDSHOW OFF at the moment?
  virtual bool MemberNoEdit() const;
  // is this a member field, and if so, does it have CONDEDIT OFF at the moment?
  
  int          GetIndex() const override { return idx;}
  void         SetIndex(int dx) override { idx = dx; }

  String       GetTypeDecoKey() const override;
  String       GetDisplayName() const override;
  
  TA_BASEFUNS(FlatTreeEl);
private:
  SIMPLE_COPY(FlatTreeEl);
  void Initialize();
  void Destroy()     { };
};

#endif // FlatTreeEl_h
