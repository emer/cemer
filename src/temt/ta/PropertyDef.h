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

#ifndef PropertyDef_h
#define PropertyDef_h 1

// parent includes:
#include <MemberDefBase>

// member includes:
class MemberDef; //
class MethodDef; //


// declare all other types mentioned but not required to include:

typedef Variant (*ta_prop_get_fun)(const void*);
typedef void (*ta_prop_set_fun)(void*, const Variant&);

taTypeDef_Of(PropertyDef);

class TA_API PropertyDef : public MemberDefBase { //  defines a class member
INHERITED(MemberDefBase)
public:

#ifdef NO_TA_BASE
  MemberDef*            get_mbr; // if a member getter found
  MethodDef*            get_mth; // if a method getter found
  MemberDef*            set_mbr; // if a member setter found
  MethodDef*            set_mth; // if a method setter found
#endif
  ta_prop_get_fun       prop_get; // stub function to get the property (as Variant)
  ta_prop_set_fun       prop_set; // stub function to set the property (as Variant)

  override bool         isReadOnly() const;
  override bool         isGuiReadOnly() const;
  void                  setType(TypeDef* typ); // use this, to check for consistency between the various source -- should NOT be null!
#ifndef __MAKETA__
  override TypeInfoKinds TypeInfoKind() const {return TIK_PROPERTY;}
#endif
  override void*        This() {return this;}
  override TypeDef*     GetTypeDef() const {return &TA_PropertyDef;}
  override bool         ValIsDefault(const void* base,
                                     int for_show) const; // = taMisc::IS_EXPERT
  // true if the member contains its default value, either DEF_ or the implicit default; for_show is only for types, to choose which members to recursively include; we are usually only interested in Expert guys

  void                  Copy(const PropertyDef& cp);
  PropertyDef();
  PropertyDef(const char* nm);
  PropertyDef(TypeDef* ty, const char* nm, const char* dsc, const char* op,
    const char* lis, ta_prop_get_fun get, ta_prop_set_fun set,
    bool is_stat = false);
  PropertyDef(const PropertyDef& cp);
  ~PropertyDef();
  PropertyDef*  Clone()         { return new PropertyDef(*this); }
  PropertyDef*  MakeToken()     { return new PropertyDef(); }

  override const Variant GetValVar(const void* base) const;
  override void SetValVar(const Variant& val, void* base, void* par = NULL);
    // note: par is only needed really needed for owned taBase ptrs)

  String        GetHTML(bool gendoc=false, bool short_fmt=false) const;
  // gets an HTML representation of this object -- for help view etc -- gendoc = external html file rendering instead of internal help browser, short_fmt = no details, for summary guys

private:
  void          Initialize();
  void          Copy_(const PropertyDef& cp);
};

#endif // PropertyDef_h
