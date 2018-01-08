// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiEditOfDefault.h"
#include <taiEditorOfClass>
#include <MemberDef>
#include <taiMember>
#include <taiMemberOfTypeDefault>
#include <taBase_List>
#include <TypeDefault>


// this special edit is for defualt instances
class taiEditOfDefaultDataHost : public taiEditorOfClass {
INHERITED(taiEditorOfClass)
public:
  void GetValue() override;

  MemberSpace   mspace;         // special copy of the mspace (added toggles)

//obs:  taiEditOfDefaultDataHost(TypeDef* tp, void * base);
  taiEditOfDefaultDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  ~taiEditOfDefaultDataHost();
protected:
  void     Constr_Members() override; // called by Constr_impl to fill memb_el[]
private:
  explicit taiEditOfDefaultDataHost(taiEditOfDefaultDataHost&)      { }; // avoid copy constr bug
};

taiEditOfDefaultDataHost::taiEditOfDefaultDataHost(void* base, TypeDef* typ_, bool read_only_,
        bool modal_, QObject* parent)
: taiEditorOfClass(base, typ_, read_only_, modal_, parent)
{
}

taiEditOfDefaultDataHost::~taiEditOfDefaultDataHost(){
  for (int i = mspace.size - 1; i >= 0; --i) {
    MemberDef* md = mspace.FastEl(i);
    if (md->im)
      delete md->im;
    md->im = NULL;
  }
  mspace.Reset();
}

void taiEditOfDefaultDataHost::Constr_Members() {
  for (int i = 0; i < typ->static_members.size; ++i){
    MemberDef* org_md = typ->static_members.FastEl(i);
    MemberDef* md = new MemberDef(*org_md);
    mspace.Add(md);
    md->im = org_md->im;        // set this here
    if (md->im != NULL) {
      taiMemberOfTypeDefault* tdm = new taiMemberOfTypeDefault(md, typ);
      tdm->bid = md->im->bid + 1;
      tdm->AddMember(md);
    }
    memb_el(0).Add(md);
  }
  for (int i = 0; i < typ->members.size; ++i){
    MemberDef* org_md = typ->members.FastEl(i);
    MemberDef* md = new MemberDef(*org_md);
    mspace.Add(md);
    md->im = org_md->im;        // set this here
    if (md->im != NULL) {
      taiMemberOfTypeDefault* tdm = new taiMemberOfTypeDefault(md, typ);
      tdm->bid = md->im->bid + 1;
      tdm->AddMember(md);
    }
    memb_el(0).Add(md);
  }
}

void taiEditOfDefaultDataHost::GetValue() {
  inherited::GetValue();
  taBase* rbase = Base();
  if (rbase) {
    taBase_List* gp = typ->defaults;
    TypeDefault* tpdflt = NULL;
    if (gp != NULL) {
      for (int i = 0; i < gp->size; ++i) {
        TypeDefault* td = (TypeDefault*)gp->FastEl(i);
        if (td->token == rbase) {
          tpdflt = td;
          break;
        }
      }
    }
    if (tpdflt != NULL)
      tpdflt->UpdateToNameValue();
  }
}

////////////////////////////////
//       taiEditOfDefault       //
////////////////////////////////

taiEditorOfClass* taiEditOfDefault::CreateDataHost(void* base, bool readonly, bool modal) {
  return new taiEditOfDefaultDataHost(base, typ, readonly, modal);
}
