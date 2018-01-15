// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef TypeItem_h
#define TypeItem_h 1

// See TypeDef.h:
#ifdef SC_DEFAULT
  //#define SC_DEFAULT // uncomment to see previous macro definition
  #pragma message("Warning: undefining SC_DEFAULT macro")
  #undef SC_DEFAULT
#endif

// parent includes:
#include <taRefN>
#include <ITypedObject>

// member includes:
#include <String_PArray>

// declare all other types mentioned but not required to include:
class TypeDef; //
class taSigLink; //

taTypeDef_Of(TypeItem);

class TA_API TypeItem: public taRefN, public ITypedObject {
  // ##INSTANCE ##NO_TOKENS ##NO_MEMBERS ##NO_CSS ##MEMB_NO_SHOW_TREE base class for TypeDef, MemberDef, MethodDef, EnumDef, and TypedefDef
INHERITED(taRefN)
public:
  enum TypeInfoKinds {
    TIK_ENUM,
    TIK_MEMBER,
    TIK_METHOD,
    TIK_PROPERTY,
    TIK_TYPE,
    TIK_ENUMSPACE,
    TIK_TOKENSPACE,
    TIK_MEMBERSPACE,
    TIK_METHODSPACE,
    TIK_PROPERTYSPACE, // note: holds mix of PropertyDef and MemberDef
    TIK_TYPESPACE,
    TIK_UNKNOWN
  };

  enum StrContext { // context for getting or setting a string value
    SC_DEFAULT,         // default (for compat) -- if taMisc::is_loading/saving true, then STREAMING else VALUE
    SC_STREAMING,       // value is being used for streaming, ex. strings are quoted/escaped
    SC_VALUE,           // value is being manipulated programmatically, ex. strings are not quoted/escaped
    SC_DISPLAY,         // value is being used for display purposes, and result includes html (rich text) formatting tags, etc
    SC_SEARCH,          // value is used for searching -- like value but does not include paths or other things that will confuse searching
  };

  int           idx;            // the index number for this type
  String        name;
  String        desc;           // a description
  String_PArray opts;           // user-spec'd options (#xxx)
  String_PArray lists;          // user-spec'd lists   (#LIST_xxx)
  taSigLink*    sig_link;       // for gui type browser of type info

  void*        This() override {return this;}
  TypeDef*     GetTypeDef() const override {return &TA_TypeItem; }
  virtual TypeInfoKinds TypeInfoKind() const {return TIK_UNKNOWN;}
  virtual TypeDef*      GetOwnerType() const {return NULL;}
  virtual const String  GetPathName() const {return name;}
  // name used for saving a reference in stream files, can be used to lookup again
  
  void          Copy(const TypeItem& cp);
  void          AddOption(const String& op) { opts.AddUnique(op); }
  bool          HasOption(const String& op) const { return (opts.FindEl(op) >= 0); }
    // check if option is set
  bool          HasOptionAfter(const String& prefix, const String& op) const;
    // returns true if any prefix (Xxx_) has the value after of op; enables multi options of same prefix
  virtual String        OptionAfter(const String& prefix) const;
  // return portion of option after given option header
  virtual bool          NextOptionAfter(const String& prefix, int& itr, String& result) const; // enables enumeration of multi-valued prefixes; start w/ itr=0; true if a new value was returned in result
  virtual String        GetLabel() const;
  // checks for option of LABEL_xxx and returns it or name
  virtual String        GetCat() const { return OptionAfter("CAT_"); }
  // get category of this item, from CAT_ option -- empty if not specified

  virtual String        GetOptsHTML() const;
  // get options in HTML format
  virtual bool          GetCondOpt(const String condkey, const TypeDef* base_td,
                                   const void* base,
                                   bool& is_on, bool& val_is_eq) const;
  // return true if item has the condkey (e.g., CONDEDIT, CONDSHOW, GHOST), and if so, sets is_on if the cond type is ON (else OFF) and if the match value was in the list (val_is_eq) -- format: condkey_[ON|OFF]_member[:value{,value}[&&,||member[:value{,value}...]] -- must all be && or || for logic -- base and base_td refer to the the parent object that owns this one -- used for searching for the member to conditionalize on
  virtual bool          GetCondOptTest(const String condkey, const TypeDef* base_td, const void* base) const;
  // returns true if item does NOT have condkey (e.g., CONDEDIT, CONDSHOW, GHOST), or the condition evaluates to true, otherwise false -- this is a simpler interface than GetCondOpt, suitable for most uses -- format: condkey_[ON|OFF]_member[:value{,value}[&&,||member[:value{,value}...]] -- must all be && or || for logic  -- base and base_td refer to the the parent object that owns this one -- used for searching for the member to conditionalize on
  virtual void          PrintType_OptsLists(String& strm) const;
  // for printing type information -- renders opts and lists into a formatted string

  TypeItem();
  TypeItem(const TypeItem& cp); // copy constructor
  ~TypeItem();

private:
  void          init(); // #IGNORE
  void          Copy_(const TypeItem& cp);
};

#endif // TypeItem_h
