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

#ifndef FlatTreeEl_List_h
#define FlatTreeEl_List_h 1

// parent includes:
#include <taList>

// member includes:
#include <FlatTreeEl>

// declare all other types mentioned but not required to include:

taTypeDef_Of(FlatTreeEl_List);

class TA_API FlatTreeEl_List : public taList<FlatTreeEl> {
  // #CAT_FlatTree list of flat tree elements for representing object structure hierarchy in a flat list
INHERITED(taList<FlatTreeEl>)
public:
  taBaseRef     top_obj;  // top-level object on which the flat tree is computed -- paths for objects will be relative to this object

  virtual void  FlatTreeOf(taBase* obj);
  // populate this list with the flat tree starting at given object

  virtual void  GetFlatTreeMembers(FlatTreeEl* obj_fel, const taBase* obj);
  // #IGNORE get the flat tree reps for members of an object represented in obj_fel
  
  virtual void  GetFlatTreeMembers_ListsOnly(FlatTreeEl* obj_fel, const taBase* obj);
  // #IGNORE get the flat tree reps for members of an object represented in obj_fel -- only for taList_impl types!
  
  virtual FlatTreeEl*  GetFlatTreeMember(int nest, MemberDef* md, FlatTreeEl* obj_fel, const taBase* obj);
  // #IGNORE get the flat tree reps for given member of an object represented in obj_fel
 
  virtual FlatTreeEl* NewMember
    (int nest, MemberDef* md, const taBase* ta_obj, FlatTreeEl* par_el);
  // create a new flat tree element of member
  
  virtual FlatTreeEl* NewObject(int nest, const taBase* ta_obj, FlatTreeEl* par_el);
  // create a new flat tree element of ta object

  virtual void HashToIntArray(int_PArray& array);
  // #IGNORE generate hash codes for all elements into given array
  
  TA_BASEFUNS_NOCOPY(FlatTreeEl_List);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // FlatTreeEl_List_h
