// Copyright, 1995-2007, Regents of the University of Colorado,
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


// TA dump file implementation

#ifndef ta_dump_h
#define ta_dump_h 1

#include "ta_base.h"

class 	VPUList;
class	DumpPathSubList;
class	DumpPathTokenList;
class	DumpFileCvtList;

class TA_API dumpMisc {
  // #NO_TOKENS ##NO_CSS ##NO_MEMBERS miscellaneous stuff for dump files
public:
  static taBase_PtrList		update_after; // objects to update after loading (still in is_loading context)
  static taBase_PtrList		post_update_after; // objects to update after loading, dispatched from event loop 
  static DumpPathSubList 	path_subs;     // path substitutions
  static DumpPathTokenList	path_tokens;  // path tokens
  static VPUList 		vpus;	      // pointers that couldn't get cached out
  static taBase*		dump_root;    // top-level object for load or save 
  static String			dump_root_path; // path of top-level object for load or save 
  
  static void 		 	PostUpdateAfter(); // called by taiMiscCore when there were guys on pua list
};

// an object to store an Value that couldn't be cashed out
// only works with taBase elements because they have paths..

class TA_API VPUnref {			// ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  String	name;		// just for looking up purposes
  MemberDef*	memb_def;	// memberdef of the pointer
  void* 	base;		// location of ptr to set, usually taBase**, but could be taSmartRef*
  taBase* 	parent;		// location's parent to update
  String 	path;

  taBase* 	Resolve();

  VPUnref(void* base, taBase* par, const String& p, MemberDef* md = NULL);
};

// a list of unresolved variable pointers that are to be resolved later..

class TA_API VPUList : public taPtrList<VPUnref> {
  // ##NO_CSS ##NO_MEMBERS
protected:
  void		El_Done_(void* it)	{ delete (VPUnref*)it; }
  String El_GetName_(void* it) const { return ((VPUnref*)it)->name; }
public:
  void	Resolve();	// attempt to resolve references

  virtual void	AddVPU(void* b, taBase* par, const String& p, MemberDef* md = NULL);

  ~VPUList()                            { Reset(); }
};

class TA_API DumpPathSub {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Path element substitution
public:
  TypeDef*	type;
  taBase*	parent;		// for relative paths
  String	old_path;
  String	new_path;

  DumpPathSub(TypeDef* td, taBase* par, const String& o, const String& n);
};

class TA_API DumpPathSubList : public taPtrList<DumpPathSub> {
  // ##NO_CSS ##NO_MEMBERS
INHERITED(taPtrList<DumpPathSub>)
protected:
  void		El_Done_(void* it)	{ delete (DumpPathSub*)it; }
  String	El_GetName_(void* it) const { return ((DumpPathSub*)it)->old_path; }
public:
  void	AddPath(TypeDef* td, taBase* par, String& o, String& n);
  void 	FixPath(TypeDef* td, taBase* par, String& path);
  // fixes given path (td and par are for scoping search)
  void 	unFixPath(TypeDef* td, taBase* par, String& path);
  // un-fixes given path to original path value

  ~DumpPathSubList()            { Reset(); }
};

class TA_API DumpPathToken {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Path tokens for quicker loading
public:
  taBase*	object;
  String	path;
  String	token_id;

  DumpPathToken(taBase* obj, const String& pat, const String& tok_id);
};

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

#endif // ta_dump_h
