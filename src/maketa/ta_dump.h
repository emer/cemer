// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

class TA_API dumpMisc {
  // #NO_TOKENS ##NO_CSS ##NO_MEMBERS miscellaneous stuff for dump files
public:
  static taBase_PtrList		update_after; // objects to update after loading
  static DumpPathSubList 	path_subs;     // path substitutions
  static DumpPathTokenList	path_tokens;  // path tokens
  static VPUList 		vpus;	      // pointers that couldn't get cached out
};

// an object to store an Value that couldn't be cashed out
// only works with taBase elements because they have paths..

class TA_API VPUnref {			// ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  String	name;		// just for looking up purposes
  MemberDef*	memb_def;	// memberdef of the pointer
  TAPtr* 	base;		// location of ptr to set
  TAPtr 	parent;		// location's parent to update
  String 	path;

  TAPtr 	Resolve();

  VPUnref(TAPtr* b, TAPtr par, const String& p, MemberDef* md = NULL);
};

// a list of unresolved variable pointers that are to be resolved later..

class TA_API VPUList : public taPtrList<VPUnref> {
  // ##NO_CSS ##NO_MEMBERS
protected:
  void		El_Done_(void* it)	{ delete (VPUnref*)it; }
  String El_GetName_(void* it) const { return ((VPUnref*)it)->name; }
public:
  void	Resolve();	// attempt to resolve references
  void	Add(VPUnref* it)	{ taPtrList<VPUnref>::Add(it); }
  void	Add(TAPtr* b, TAPtr par, const String& p, MemberDef* md = NULL);

  ~VPUList()                            { Reset(); }
};

class TA_API DumpPathSub {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Path element substitution
public:
  TypeDef*	type;
  TAPtr		parent;		// for relative paths
  String	old_path;
  String	new_path;

  DumpPathSub(TypeDef* td, TAPtr par, const String& o, const String& n);
};

class TA_API DumpPathSubList : public taPtrList<DumpPathSub> {
  // ##NO_CSS ##NO_MEMBERS
protected:
  void		El_Done_(void* it)	{ delete (DumpPathSub*)it; }
  String	El_GetName_(void* it) const { return ((DumpPathSub*)it)->old_path; }
public:
  void	Add(DumpPathSub* it)	{ taPtrList<DumpPathSub>::Add(it); }
  void	Add(TypeDef* td, TAPtr par, String& o, String& n);
  void 	FixPath(TypeDef* td, TAPtr par, String& path);
  // fixes given path (td and par are for scoping search)
  void 	unFixPath(TypeDef* td, TAPtr par, String& path);
  // un-fixes given path to original path value

  ~DumpPathSubList()            { Reset(); }
};

class TA_API DumpPathToken {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS Path tokens for quicker loading
public:
  TAPtr		object;
  String	path;
  String	token_id;

  DumpPathToken(TAPtr obj, const String& pat, const String& tok_id);
};

class TA_API DumpPathTokenList : public taPtrList<DumpPathToken> {
  // ##NO_CSS ##NO_MEMBERS
protected:
  void		El_Done_(void* it)	{ delete (DumpPathToken*)it; }
  String	El_GetName_(void* it) const { return ((DumpPathToken*)it)->path; }
public:
  void		Add(DumpPathToken* it)	{ taPtrList<DumpPathToken>::Add(it); }
  void		Add(TAPtr obj, const String& pat);

  int		FindObj(TAPtr obj); // find path token for given object

  String	GetPath(TAPtr obj);
  // get path for this object (either a token path or its GetPath if new)

  void		NewLoadToken(String& pat, String& tok_id);
  // just loaded a new token definition, make a path token out of it
  TAPtr		FindFromPath(String& pat, TypeDef* td, void* base, void* par=NULL,
			     MemberDef* memb_def=NULL);
  // find object from given path, where path could be a path token too

  ~DumpPathTokenList()          { Reset(); }
};


#endif // ta_dump_h
