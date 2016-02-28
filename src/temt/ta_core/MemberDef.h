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

#ifndef MemberDef_h
#define MemberDef_h 1

// See Typedef.h
#ifdef SC_DEFAULT
  //#define SC_DEFAULT // uncomment to see previous macro definition
  #pragma message("Warning: undefining SC_DEFAULT macro")
  #undef SC_DEFAULT
#endif

// parent includes:
#include <MemberDefBase>

// member includes:

// declare all other types mentioned but not required to include:
class Member_List; //
class TypeSpace; //
class voidptr_PArray; //

taTypeDef_Of(MemberDef);

class TA_API MemberDef : public MemberDefBase { //  defines a class member
INHERITED(MemberDefBase)
public:
  enum DefaultStatus { //#BITS  status of default value comparison
    HAS_DEF     = 0x01, // member specified a default value
     IS_DEF     = 0x02, // member's value is the default

     NO_DEF     = 0x00, // #NO_BIT none defined
    NOT_DEF     = 0x01, // #NO_BIT default specified, current is not equal
    EQU_DEF     = 0x03, // #NO_BIT default specified, current is default
  };

  static void           GetMembDesc(MemberDef* md, String& dsc_str, String indent); // gets a detailed description, typically for tooltip

  ta_memb_ptr   off;            // offset of member from owner type
  int           base_off;       // offset for base of owner
  void*         addr;           // address of static member
  bool          fun_ptr;        // true if this is a pointer to a function

  bool         isReadOnly() const override;
  bool         isGuiReadOnly() const override;

  void*        This() override {return this;}
  TypeDef*     GetTypeDef() const override {return &TA_MemberDef;}
  TypeInfoKinds TypeInfoKind() const override {return TIK_MEMBER;}

  bool ValIsDefault(const void* base,
                    int for_show=IS_EXPERT) const override;
  // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys

  void          Copy(const MemberDef& cp);
  MemberDef();
  MemberDef(const String& nm);
  MemberDef(TypeDef* ty, const String& nm, const String& dsc, const String& op, const String& lis,
            ta_memb_ptr mptr, bool is_stat = false, void* maddr=NULL, bool funp = false);
  MemberDef(const MemberDef& cp);
  ~MemberDef();
  MemberDef*    Clone()         { return new MemberDef(*this); }
  MemberDef*    MakeToken()     { return new MemberDef(); }

  void*                 GetOff(const void* base) const;
  // get offset of member relative to overall class base pointer
  static void*          GetOff_static(const void* base, int base_off_, ta_memb_ptr off_);
  // get offset of member -- static version that takes args
  const String GetPathName() const override;
    // name used for saving a reference in stream files, can be used to lookup again

  const Variant GetValVar(const void* base) const override;
  void          SetValVar(const Variant& val, void* base, void* par = NULL) override;
    // note: par is only needed really needed for owned taBase ptrs)

  String        GetValStr(const void* base, StrContext vc = SC_DEFAULT,
                          bool force_inline = false) const;
  // get a string representation of value -- this is very widely used in the code
  void          SetValStr(const String& val, void* base, StrContext vc = SC_DEFAULT,
                          bool force_inline = false);
  // set the value from a string representation -- this is very widely used in the code

  DefaultStatus         GetDefaultStatus(const void* base);
  // get status of value of member at given base addr of class object that this member is in compared to DEF_ value(s) defined in directive

  void          CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from same type
  void          CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members from same type (no inherited)
  bool          CompareSameType(Member_List& mds, TypeSpace& base_types,
                            voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                            TypeDef* base_typ, void* trg_base, void* src_base,
                            int show_forbidden=NO_HIDDEN, int show_allowed=SHOW_CHECK_MASK,
                            bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects)

  void          PrintType(String& col1, String& col2) const;
  void          Print(String& col1, String& col2, void* base, int indent=0) const;

  String        GetHTML(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of this object -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys

  // for dump files
  bool          DumpMember(void* par);          // decide whether to dump or not
  int           Dump_Save(std::ostream& strm, void* base, void* par, int indent);
  int           Dump_SaveR(std::ostream& strm, void* base, void* par, int indent);
  int           Dump_Save_PathR(std::ostream& strm, void* base, void* par, int indent);
  int           Dump_Load(std::istream& strm, void* base, void* par); //

private:
  void          Initialize();
  void          Copy_(const MemberDef& cp);
};

#endif // MemberDef_h
