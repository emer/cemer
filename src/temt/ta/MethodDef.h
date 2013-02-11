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

#ifndef MethodDef_h
#define MethodDef_h 1

// parent includes:
#include <TypeItem>

// member includes:
#include <String_PArray>
#include <TypeSpace>

// declare all other types mentioned but not required to include:
class MethodSpace; //
class taiMethod; //

taTypeDef_Of(MethodDef);

class TA_API MethodDef : public TypeItem {// defines a class method
#ifndef __MAKETA__
typedef TypeItem inherited;
#endif
public:
  MethodSpace*  owner;

  TypeDef*      type;           // of the return value
  bool          is_static;      // true if this method is static
  bool          is_virtual;     // true if virtual (1st or subsequent overrides)
  bool          is_override;    // true if (virtual) override of a base
  bool          is_lexhide;     // true if lexically hides a non-virtual base -- could be an error
  ta_void_fun   addr;           // address (only for static or reg_fun functions)
  String_PArray inh_opts;       // inherited options ##xxx
#ifdef TA_GUI
  taiMethod*    im;             // gui structure for edit representation
#endif // def TA_GUI
  short         fun_overld;     // number of times function is overloaded (i.e., diff args)
  short         fun_argc;       // nofun, or # of parameters to the function
  short         fun_argd;       // indx for start of the default args (-1 if none)
  TypeSpace     arg_types;      // argument types
  String_PArray arg_names;      // argument names
  String_PArray arg_defs;       // argument default values
  String_PArray arg_vals;       // argument values (previous)

  css_fun_stub_ptr stubp;       // css function stub pointer

  override TypeInfoKinds TypeInfoKind() const {return TIK_METHOD;}
  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_MethodDef;}

  void          Initialize();
  void          Copy(const MethodDef& cp);
  MethodDef();
  MethodDef(const char* nm);
  MethodDef(TypeDef* ty, const char* nm, const char* dsc, const char* op, const char* lis,
            int fover, int farc, int fard, bool is_stat = false, ta_void_fun funa = NULL,
            css_fun_stub_ptr stb = NULL, bool is_virt = false);
  MethodDef(const MethodDef& md);       // copy constructor
  ~MethodDef();

  const String          prototype() const; // text depiction of fun, ex "void MyFun(int p)"

  MethodDef*            Clone()         { return new MethodDef(*this); }
  MethodDef*            MakeToken()     { return new MethodDef(); }
  override TypeDef*     GetOwnerType() const;
  override const String GetPathName() const;
  bool                  CheckList(const String_PArray& lst) const;
  // check if method has a list in common with given one
  bool                  CompareArgs(MethodDef* it) const;       // true if same, false if not
  void                  PrintType(String& col1, String& col2) const;
  void                  CallFun(void* base) const;
  // call the function, using gui dialog if need to get args
  const String          ParamsAsString() const; // returns what would be in () for a definition
  bool                  ShowMethod(int show = USE_SHOW_GUI_DEF) const;

  String        GetHTML(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of this object -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys

protected:
  mutable int  show_any; // bits for show any -- 0 indicates not determined yet, 0x80 is flag
  void          ShowMethod_CalcCache() const; // called when show_any=0, ie, not configured yet
  void          ShowMethod_CalcCache_impl(int& show) const;
};



#endif // MethodDef_h
