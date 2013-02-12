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

#ifndef TypeItem_h
#define TypeItem_h 1

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
  enum ShowContext {
    SC_ANY,             // any context -- directives like "SHOW"
    SC_EDIT,            // for in edit dialogs -- directives like "SHOW_EDIT"
    SC_TREE             // in tree views (browsing) -- directives like "SHOW_TREE"
  }; 

  // note: don't rationalize the memb bits, because it breaks the user prefs file
  // the NO_xxx guys are for use in 'forbidden' contexts (legacy 'show')
  // the IS_xxx guys are for use in 'allowed' contexts, and/or categorizing guys
  enum ShowMembs { // #BITS
    NO_HIDDEN           = 0x01, // don't show items marked READ_ONLY w/o SHOW or HIDDEN
    NO_unused1          = 0x02, // #IGNORE
    NO_unused2          = 0x04, // #IGNORE
    NO_NORMAL           = 0x08, // #NO_SHOW don't show items normally shown (helps indicate, ex. EXPERT items)
    NO_EXPERT           = 0x10, // don't show items marked EXPERT (often only for advanced sims)

    ALL_MEMBS           = 0x00, // #NO_BIT
    NORM_MEMBS          = 0x11, // #NO_BIT
    EXPT_MEMBS          = 0x09, // #NO_BIT
    HIDD_MEMBS          = 0x18, // #NO_BIT

    IS_HIDDEN           = 0x01, // #IGNORE used in MemberDef::ShowMember to flag RO w/o SHOW or HIDDEN guys
    IS_NORMAL           = 0x08, // #IGNORE used in MemberDef::ShowMember to flag NORMAL guys
    IS_EXPERT           = 0x10, // #IGNORE used in MemberDef::ShowMember to flag EXPERT guys
    SHOW_CHECK_MASK     = IS_HIDDEN | IS_NORMAL | IS_EXPERT, // #IGNORE #NO_BIT used in MemberDef::ShowMember checks, default for "allowed" param
    USE_SHOW_GUI_DEF    = 0x40  // #NO_BIT use default from taMisc::show_gui, only applies to forbidden
  };

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

  // const guys help speed up oft-looked up values by avoiding String churning
  static const String opt_show; // "SHOW"
  static const String opt_no_show; // "NO_SHOW"
  static const String opt_hidden; // "HIDDEN"
  static const String opt_read_only; // "READ_ONLY"
  static const String opt_expert; // "EXPERT"
  static const String opt_edit_show; // "EDIT_SHOW"
  static const String opt_edit_no_show; // "EDIT_NO_SHOW"
  static const String opt_edit_hidden; // "EDIT_HIDDEN"
  static const String opt_edit_read_only; // "EDIT_READ_ONLY"
  static const String opt_edit_detail; // "EDIT_DETAIL"
  static const String opt_edit_expert; // "EDIT_EXPERT"
  static const String opt_APPLY_IMMED; // "APPLY_IMMED"
  static const String opt_NO_APPLY_IMMED; // "NO_APPLY_IMMED"
  static const String opt_inline; // "INLINE"
  static const String opt_edit_inline; // "EDIT_INLINE"
  static const String opt_EDIT_DIALOG; // "EDIT_DIALOG"

  static const String opt_bits; // "BITS"
  static const String opt_instance; // "INSTANCE"

  int           idx;            // the index number for this type
  String        name;
  String        desc;           // a description
  String_PArray opts;           // user-spec'd options (#xxx)
  String_PArray lists;          // user-spec'd lists   (#LIST_xxx)
  taSigLink*   sig_link;

  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_TypeItem; }
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
  virtual void		PrintType_OptsLists(String& strm) const;
  // for printing type information -- renders opts and lists into a formatted string

  TypeItem();
  TypeItem(const TypeItem& cp); // copy constructor
  ~TypeItem();

private:
  void          init(); // #IGNORE
  void          Copy_(const TypeItem& cp);
};

#endif // TypeItem_h
