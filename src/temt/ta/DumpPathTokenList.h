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

#ifndef DumpPathTokenList_h
#define DumpPathTokenList_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API DumpPathTokenList : public taPtrList<DumpPathToken> {
  // ##NO_CSS ##NO_MEMBERS manages paths during both saving and loading -- uses a hash code on paths stored in DumpPathToken objects, plus an object-pointer hash table for finding objects
INHERITED(taPtrList<DumpPathToken>)
protected:
  void		El_Done_(void* it)	{ delete (DumpPathToken*)it; }
  String	El_GetName_(void* it) const { return ((DumpPathToken*)it)->path; }
public:
  taHashTable	obj_hash_table;	// optimized finding of objects

  //////////////////////////
  //	Main Interface:

  virtual String GetPath(taBase* obj);
  // get path for this object (either a token path or its GetPath if new) -- use this during *saving* a path

  virtual taBase* FindFromPath(String& pat, TypeDef* td, void* base, void* par=NULL,
			     MemberDef* memb_def=NULL);
  // find object from given path, where path could be a path token too -- use this during *loading* a path -- note: pat may be modified!


  //////////////////////////
  //	Internal utilities

  virtual void	ReInit(int obj_hash_size = 10000);
  // re-initialize for saving/loading, including resetting the hash table
  virtual int	FindObj(taBase* obj);
  // find path token for given object
  virtual DumpPathToken* AddObjPath(taBase* obj, const String& pat);
  // add token for object and path
  virtual DumpPathToken* NewLoadToken(const String& pat, const String& tok_id);
  // just loaded a new token definition, make a path token out of it
  virtual taBase* FixPathFind(const String& pat, taBase* init_find, 
			      TypeDef* trg_typ, const String& trg_nm);
  // after an initial FindFromPath, do some checking and re-finding to make sure to get the right object given saved trg_typ and trg_nm (if those are null, just returns init_find)

  override void	Reset();
  
  DumpPathTokenList();
  ~DumpPathTokenList()          { Reset(); }
};

#endif // DumpPathTokenList_h
