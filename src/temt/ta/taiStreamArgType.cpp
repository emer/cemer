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

#include "taiStreamArgType.h"
#include <taFiler>
#include <taBase>
#include <taifileButton>

#include <css_ta.h>

using namespace std;


void taiStreamArgType::Initialize() {
  gf = NULL;
}

void taiStreamArgType::Destroy() {
  if (gf) {
    gf->Close();
    taRefN::unRefDone(gf);
    gf = NULL;
  }
}

int taiStreamArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if(argt->InheritsFrom(TA_ios))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiStreamArgType::GetElFromArg(const char* nm, void*) {
  // arg_val is for the function
  arg_val = new cssIOS(NULL, 1, arg_typ, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiData* taiStreamArgType::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  if (!gf) {
    // we get and initialize the filer once
    gf = taBase::StatGetFiler(meth);
    taRefN::Ref(gf);
  }
  if (arg_typ->InheritsFrom(TA_istream))
    return new taiFileButton(NULL, host_, par, gui_parent_, flags_, true);
  else if(arg_typ->InheritsFrom(TA_ostream))
    return new taiFileButton(NULL, host_, par, gui_parent_, flags_, false, true);
  return new taiFileButton(NULL, host_, par, gui_parent_, flags_);
}

void taiStreamArgType::GetImage_impl(taiData* dat, const void* base){
  if (arg_base == NULL)
    return;
  taiFileButton* fbut = (taiFileButton*) dat;
  fbut->SetFiler(gf);
  fbut->GetImage();
}

void taiStreamArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  GetValueFromGF();
}

void taiStreamArgType::GetValueFromGF() {
  if (arg_typ->InheritsFrom(TA_fstream)) {
    if (gf->fstrm == NULL) {
      *((void**)arg_base) = NULL;
      err_flag = true;          // error-value not set..
      return;
    }
    *((fstream**)arg_base) = gf->fstrm;
    return;
  }
  if (arg_typ->InheritsFrom(TA_istream)) {
    if(gf->istrm == NULL) {
      *((void**)arg_base) = NULL;
      err_flag = true;          // error-value not set..
      return;
    }
    *((istream**)arg_base) = gf->istrm;
    return;
  }
  if(arg_typ->InheritsFrom(TA_ostream)) {
    if (gf->ostrm == NULL) {
      *((void**)arg_base) = NULL;
      err_flag = true;          // error-value not set..
      return;
    }
    *((ostream**)arg_base) = gf->ostrm;
    return;
  }
}
