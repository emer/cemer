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

#ifndef taArray_base_h
#define taArray_base_h 1

// parent includes:
#include <taNBase>
#include <taArray_impl>

// member includes:

// declare all other types mentioned but not required to include:
class taBasePtr; // 
class taBase; // 
class TypeDef; // 
class taMatrix; // 
class taBaseItr; // 
class MemberDef; // 
class istream; // 
class ostream; // 
class taBase_PtrList; // 
class taObjDiff_List; // 
class taDoc; // 
class SelectEdit; // 
class MethodDef; // 
class UserDataItem_List; // 
class UserDataItemBase; // 
class UserDataItem; // 


class TA_API taArray_base : public taNBase, public taArray_impl {
  // #VIRT_BASE #NO_TOKENS #NO_UPDATE_AFTER ##CAT_Data base for arrays (from taBase)
INHERITED(taNBase)
public:
  taBasePtr     el_view;        // #EXPERT #NO_SAVE #CAT_taArray matrix with indicies providing view into items in this list, if set -- determines the items and the order in which they are presented for the iteration operations -- otherwise ignored in other contexts
  IndexMode     el_view_mode;   // #EXPERT #NO_SAVE #CAT_taArray what kind of information is present in el_view to determine view mode -- only valid cases are IDX_COORDS and IDX_MASK

  Variant       FastElAsVar(int idx) const { return El_GetVar_(FastEl_(idx)); }
  // #CAT_Access get element without range checking as a variant
  Variant       SafeElAsVar(int idx) const { return El_GetVar_(SafeEl_(idx)); }
  // #CAT_Access get element with safe range checking as a variant

  override bool         IsContainer()   { return true; }
  override taMatrix*    ElView() const  { return (taMatrix*)el_view.ptr(); }
  override IndexMode    ElViewMode() const  { return el_view_mode; }
  override int          ElemCount() const { return size; }
  override Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const;
  override Variant      IterElem(taBaseItr* itr) const;
  override taBaseItr*   Iter() const;
  virtual bool          SetElView(taMatrix* view_mat, IndexMode md = IDX_COORDS);
  // #CAT_Access #EXPERT set el view to given new case -- just sets the members
  virtual taArray_base* NewElView(taMatrix* view_mat, IndexMode md = IDX_COORDS) const;
  // #CAT_Access make a new view of this array -- always does a full data copy using view

  override String&      Print(String& strm, int indent = 0) const;

  override String GetValStr(void* par = NULL, MemberDef* md = NULL,
                            TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                            bool force_inline = false) const;
  override bool  SetValStr(const String& val, void* par = NULL, MemberDef* md = NULL,
                           TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                           bool force_inline = false);
  override int  ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                              void* par = NULL, TypeDef* par_typ=NULL, MemberDef* md = NULL,
                              TypeDef::StrContext sc = TypeDef::SC_DEFAULT);

  int           Dump_Save_Value(ostream& strm, taBase* par=NULL, int indent = 0);
  int           Dump_Load_Value(istream& strm, taBase* par=NULL);

  override void DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL);

  void  CutLinks();
  TA_ABSTRACT_BASEFUNS(taArray_base);

protected:
  virtual Variant       El_GetVar_(const void* itm) const { return _nilVariant; }
  // #IGNORE convert item to Variant

private:
  void  Copy_(const taArray_base& cp) {taArray_impl::Copy_Duplicate(cp);}
    //WARNING: Copy_Duplicate is not a true copy, but we retain the behavior for compatibility
  void  Initialize();
  void  Destroy()       { CutLinks(); }
};

#endif // taArray_base_h
