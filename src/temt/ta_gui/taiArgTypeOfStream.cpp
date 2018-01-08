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

#include "taiArgTypeOfStream.h"
#include <taFiler>
#include <taBase>
#include <taiWidgetFileButton>

#include <css_ta.h>

taTypeDef_Of(ios);
taTypeDef_Of(istream);
taTypeDef_Of(ostream);
taTypeDef_Of(fstream);

using namespace std;


void taiArgTypeOfStream::Initialize() {
  gf = NULL;
}

void taiArgTypeOfStream::Destroy() {
  if (gf) {
    gf->Close();
    taRefN::unRefDone(gf);
    gf = NULL;
  }
}

int taiArgTypeOfStream::BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) {
  if(argt->InheritsFrom(TA_ios))
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfStream::GetElFromArg(const char* nm, void*) {
  // arg_val is for the function
  arg_val = new cssIOS(NULL, 1, arg_typ, nm);
  arg_base = (void*)&(((cssTA*)arg_val)->ptr);
  return arg_val;
}

taiWidget* taiArgTypeOfStream::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  if (!gf) {
    // we get and initialize the filer once
    gf = taBase::StatGetFiler(meth);
    taRefN::Ref(gf);
  }
  if (arg_typ->InheritsFrom(TA_istream))
    return new taiWidgetFileButton(NULL, host_, par, gui_parent_, flags_, true);
  else if(arg_typ->InheritsFrom(TA_ostream))
    return new taiWidgetFileButton(NULL, host_, par, gui_parent_, flags_, false, true);
  return new taiWidgetFileButton(NULL, host_, par, gui_parent_, flags_);
}

void taiArgTypeOfStream::GetImage_impl(taiWidget* dat, const void* base){
  if (arg_base == NULL)
    return;
  taiWidgetFileButton* fbut = (taiWidgetFileButton*) dat;
  fbut->SetFiler(gf);
  fbut->GetImage();
}

void taiArgTypeOfStream::GetValue_impl(taiWidget* dat, void*) {
  if (arg_base == NULL)
    return;
  GetValueFromGF();
}

void taiArgTypeOfStream::GetValueFromGF() {
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
