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

#ifndef ProgVarRef_List_h
#define ProgVarRef_List_h 1

// parent includes:
#include <ProgVarRef>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class ProgVar; // 
class taBase; // 
class TypeDef; // 


class TA_API ProgVarRef_List: public taPtrList<ProgVarRef> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Program list of program variable references
INHERITED(taList<ProgVarRef>)
public:
  ProgVarRef*   FindVar(ProgVar* var, int& idx) const;
  // return ref pointing to given var pointer (NULL if not found)
  ProgVarRef*   FindVarName(const String& var_nm, int& idx) const;
  // return ref pointing to given var name (NULL if not found)

  virtual int   UpdatePointers_NewPar(taBase* lst_own, taBase* old_par, taBase* new_par);
  // lst_own is the owner of this list
  virtual int   UpdatePointers_NewParType(taBase* lst_own, TypeDef* par_typ, taBase* new_par);
  // lst_own is the owner of this list
  virtual int   UpdatePointers_NewObj(taBase* lst_own, taBase* ptr_owner, taBase* old_ptr, taBase* new_ptr);
  // lst_own is the owner of this list

  void          operator=(const ProgVarRef_List& cp) { Reset(); Duplicate(cp); }

  ProgVarRef_List() {Initialize();}
  ProgVarRef_List(const ProgVarRef_List& cp) { Reset(); Duplicate(cp); }
  ~ProgVarRef_List();
protected:
  String        El_GetName_(void* it) const { if(((ProgVarRef*)it)->ptr()) return ((ProgVarRef*)it)->ptr()->GetName(); return _nilString; }
  void          El_Done_(void* it)      { delete (ProgVarRef*)it; }
  void*         El_MakeToken_(void* it) { return new ProgVarRef; }
  void*         El_Copy_(void* trg, void* src)
  { *((ProgVarRef*)trg) = *((ProgVarRef*)src); return trg; }

private:
  void Initialize();
};

#endif // ProgVarRef_List_h
