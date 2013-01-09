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

#ifndef MemberSpace_h
#define MemberSpace_h 1

// parent includes:
#include <MemberDefBase_List>

// member includes:
#include <int_PArray>

// declare all other types mentioned but not required to include:
class Member_List; //
class voidptr_PArray; //
class TypeSpace; //


class TA_API MemberSpace: public MemberDefBase_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of members
INHERITED(MemberDefBase_List)
public:
  MemberSpace()                         {  }
  MemberSpace(const MemberSpace& cp)    { Borrow(cp); }

  void operator=(const MemberSpace& cp) { Borrow(cp); }

  MemberDef*    FindCheck(const char* nm, void* base, void*& ptr) const;
  // breadth-first find pass for the recursive procedures

  int           FindNameOrType(const char* nm) const;
  // checks name and type name in 2 passes
  int           FindTypeName(const char* nm) const;
  // find by name of type
  MemberDef*    FindNameR(const char* nm) const;
  // recursive find of name (or type name)
  MemberDef*    FindNameAddr(const char* nm, void* base, void*& ptr) const;
  // find of name returning address of found member

  MemberDef*    FindType(TypeDef* it) const;
  // find by type, inherits from
  MemberDef*    FindTypeR(TypeDef* it) const;
  // recursive find of type
  MemberDef*    FindTypeAddr(TypeDef* it, void* base, void*& ptr) const;
  // find of type returning address of found member

  int           FindDerives(TypeDef* it) const;
  MemberDef*    FindTypeDerives(TypeDef* it) const;
  // find by type, derives from

  MemberDef*    FindAddr(void* base, void* mbr, int& idx) const;
  // find by address given base of class and address of member
  int           FindPtr(void* base, void* mbr) const;
  MemberDef*    FindAddrPtr(void* base, void* mbr, int& idx) const;
  // find by address of a member that is a pointer given base and pointer addr

  void          CopyFromSameType(void* trg_base, void* src_base);
  // copy all members from class of the same type as me
  void          CopyOnlySameType(void* trg_base, void* src_base);
  // copy only those members in my type (no inherited ones)

  bool          CompareSameType(Member_List& mds, TypeSpace& base_types,
                                voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
                                TypeDef* base_typ, void* trg_base, void* src_base,
                                int show_forbidden, int show_allowed, 
                                bool no_ptrs = true, bool test_only = false);
  // compare all member values from class of the same type as me, adding ones that are different to the mds, trg_bases, src_bases lists (unless test_only == true, in which case it just does the tests and returns true if any diffs -- for inline objects) -- default args: taMisc::NO_HIDDEN, taMisc::SHOW_CHECK_MASK

  // IO
  String&       PrintType(String& strm, int indent = 0) const;
  String&       Print(String& strm, int indent = 0) const
  { return PrintType(strm, indent); }
  String&       Print(String& strm, void* base, int indent=0) const;

  // for dump files
  int           Dump_Save(ostream& strm, void* base, void* par, int indent);
  int           Dump_SaveR(ostream& strm, void* base, void* par, int indent);
  int           Dump_Save_PathR(ostream& strm, void* base, void* par, int indent);

  int           Dump_Load(istream& strm, void* base, void* par,
                          const char* prv_read_nm = NULL, int prv_c = 0); //

public: // lexical hacks
  inline MemberDef*     operator[](int i) const {return (MemberDef*)inherited::FastEl(i);}
  inline MemberDef*     FastEl(int i) const {return (MemberDef*)inherited::FastEl(i);}
  inline MemberDef*     SafeEl(int i) {return (MemberDef*)inherited::SafeEl(i);}
  inline MemberDef*     PosSafeEl(int i) {return (MemberDef*)inherited::PosSafeEl(i);}
  //#IGNORE
  inline MemberDef*     FindName(const String& item_nm) const
    {return (MemberDef*)inherited::FindName(item_nm);}
};

#endif // MemberSpace_h
