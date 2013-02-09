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

#ifndef RndSeed_List_h
#define RndSeed_List_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <taList>
#include <RndSeed>

// declare all other types mentioned but not required to include:

typedef taList<RndSeed> RndSeed_List_impl; // basic list without container methods

taTypeDef_Of(RndSeed_List);

class TA_API RndSeed_List : public taNBase {
  // ##CAT_Math ##DEF_CHILD_seeds ##DEF_CHILDNAME_Random_Seeds list of random seeds
INHERITED(taNBase)
public:
  RndSeed_List_impl     seeds;  // the list of random seeds

  override taList_impl* children_() {return &seeds;}
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const
  { return seeds.Elem(idx, mode); }

  virtual void   MakeSeeds(int n_seeds);
  // #BUTTON make set of random seed holder objects of given size
  virtual void   NewSeeds();
  // #BUTTON #CONFIRM get new seeds for all items in the list (overwrites existing seeds!)
  virtual void   UseSeed(int idx);
  // #BUTTON use seed at given index in the list (does OldSeed on it); wraps around (modulus) if idx is > list size (issues warning)

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md);

  TA_SIMPLE_BASEFUNS(RndSeed_List);
private:
  void  Initialize()            { seeds.SetBaseType(&TA_RndSeed); }
  void  Destroy()               { };
};

#endif // RndSeed_List_h
