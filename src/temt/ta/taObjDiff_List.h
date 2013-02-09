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

#ifndef taObjDiff_List_h
#define taObjDiff_List_h 1

// parent includes:
#include <taPtrList>
#include <taObjDiffRec>

// member includes:
#include <voidptr_PArray>

// declare all other types mentioned but not required to include:
class taBase; //
class int_PArray;


taTypeDef_Of(taObjDiff_List);

class TA_API taObjDiff_List: public taPtrList<taObjDiffRec> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS list of object difference records
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;
  taPtrList_impl* El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it);
  void*         El_Copy_(void* trg, void* src);

public:
  String        name;           // of the list
#ifndef NO_TA_BASE
  taBase*       tab_obj_a;      // initial diff object for GetObjDiffVal and A comparison object for diff
  taBase*       tab_obj_b;      // original B comparison object as a taBase
#endif
  taObjDiff_List*       src_a; // source list of a items -- managed by this diff list -- create with CreateSrcs
  taObjDiff_List*       src_b; // source list of b items -- managed by this diff list -- create with CreateSrcs

  void          Initialize();

  taObjDiff_List()                              { Initialize(); }
  taObjDiff_List(const taObjDiff_List& cp)      { Initialize(); Borrow(cp); }
  ~taObjDiff_List();
  void operator=(const taObjDiff_List& cp)      { Borrow(cp); }

  void          HashToIntArray(int_PArray& array);
  // copy all hash values to given array -- for use in differencing
  void          SetAllFlag(int flg);
  // set taObjDiffRec::DiffFlag flag value for all items on the list

  void          CreateSrcs();
  // create the src_a and src_b lists, which then need to be populated with content (outside scope of this diff object)
  void          Diff();
  // perform a diff on src_a vs. src_b records -- must have already called CreateSrcs and filled in those lists with source item records to be compared -- resulting diffs all end up here in this list -- typically are links to recs in one or other of the src lists

protected:
  voidptr_PArray        nest_a_pars;
  // keeps track of current parents at each nest level, source a
  voidptr_PArray        nest_b_pars;
  // keeps track of current parents at each nest level, source b

  bool          DiffFlagParents(taObjDiffRec* rec);
  // flag parents of rec item
  int           DiffPurgeNoDiffs(taObjDiff_List* src);
  // purge all the items from src_a and src_b that have no diff items
};

#endif // taObjDiff_List_h
