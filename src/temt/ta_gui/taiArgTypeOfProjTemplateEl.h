// Copyright 2016, Regents of the University of Colorado,
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

#ifndef taiArgTypeOfProjTemplateEl_h
#define taiArgTypeOfProjTemplateEl_h 1

// parent includes:
#include <taiArgTypeOfFromGroup>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiArgTypeOfProjTemplateEl);

class TA_API taiArgTypeOfProjTemplateEl : public taiArgTypeOfFromGroup {
  // for ProjTemplateEl* ptr args
  TAI_ARGTYPE_SUBCLASS(taiArgTypeOfProjTemplateEl, taiArgTypeOfFromGroup);
public:
  int           BidForArgType(int aidx, const TypeDef* argt, const MethodDef* md, const TypeDef* td) override;
  taiWidget*    GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_) override;
  void          GetImage_impl(taiWidget* dat, const void* base) override;
  void          GetValue_impl(taiWidget* dat, void* base) override;
private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // taiArgTypeOfProjTemplateEl_h
