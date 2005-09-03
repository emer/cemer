/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

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
