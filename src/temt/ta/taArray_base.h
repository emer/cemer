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
#include <taSmartPtrT>           // taBasePtr
#include <taSmartRefT>

// declare all other types mentioned but not required to include:

#ifdef SC_DEFAULT
#undef SC_DEFAULT
#endif

taTypeDef_Of(taArray_base);

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

  bool         IsContainer()   override { return true; }
  taMatrix*    ElView() const  override { return (taMatrix*)el_view.ptr(); }
  IndexMode    ElViewMode() const  override { return el_view_mode; }
  int          ElemCount() const override { return size; }
  Variant      Elem(const Variant& idx, IndexMode mode = IDX_UNK) const override;
  Variant      IterElem(taBaseItr* itr) const override;
  taBaseItr*   Iter() const override;
  virtual bool          SetElView(taMatrix* view_mat, IndexMode md = IDX_COORDS);
  // #CAT_Access #EXPERT set el view to given new case -- just sets the members
  virtual taArray_base* NewElView(taMatrix* view_mat, IndexMode md = IDX_COORDS) const;
  // #CAT_Access make a new view of this array -- always does a full data copy using view

  String&      Print(String& strm, int indent = 0) const override;

  String GetValStr(void* par = NULL, MemberDef* md = NULL,
                            TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                            bool force_inline = false) const override;
  bool  SetValStr(const String& val, void* par = NULL, MemberDef* md = NULL,
                           TypeDef::StrContext sc = TypeDef::SC_DEFAULT,
                           bool force_inline = false) override ;
  int  ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                              void* par = NULL, TypeDef* par_typ=NULL, MemberDef* md = NULL,
                              TypeDef::StrContext sc = TypeDef::SC_DEFAULT) override;

#ifndef __MAKETA__
  int           Dump_Save_Value(std::ostream& strm, taBase* par=NULL, int indent = 0);
  int           Dump_Load_Value(std::istream& strm, taBase* par=NULL);
#endif

  void SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;

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

#define TA_ARRAY_FUNS(y,T) \
public: \
  explicit y(int init_size) {Initialize(); SetSize(init_size); } \
  T&            operator[](int i) { return el[i]; } \
  const T&      operator[](int i) const { return el[i]; } \
protected: \
  virtual const void*  El_GetBlank_() const { return (const void*)&blank; }

#define TA_ARRAY_OPS(y) \
  inline bool operator ==(const y& a, const y& b) {return a.Equal_(b);} \
  inline bool operator !=(const y& a, const y& b) {return !(a.Equal_(b));} \
  TA_SMART_PTRS(y)

#endif // taArray_base_h
