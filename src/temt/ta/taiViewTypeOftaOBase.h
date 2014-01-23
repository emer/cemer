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

#ifndef taiViewTypeOftaOBase_h
#define taiViewTypeOftaOBase_h 1

// parent includes:
#include <taiViewTypeOftaBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiViewTypeOftaOBase);

class TA_API taiViewTypeOftaOBase: public taiViewTypeOftaBase { // for taOBase and descendants
  TAI_TYPEBASE_SUBCLASS(taiViewTypeOftaOBase, taiViewTypeOftaBase)
public:
  bool         needSet() const CPP11_OVERRIDE {return true;} // always, so we can do dyn panels
  int          BidForView(TypeDef*) CPP11_OVERRIDE;
  taiSigLink* GetSigLink(void* data_, TypeDef* el_typ) CPP11_OVERRIDE; // optimized version of taiViewTypeOftaBase
  void         CheckUpdateDataPanelSet(iPanelSet* pan) CPP11_OVERRIDE;
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  taiSigLink* CreateSigLink_impl(taBase* data_) CPP11_OVERRIDE;
  void         CreateDataPanel_impl(taiSigLink* dl_) CPP11_OVERRIDE;
};

#endif // taiViewTypeOftaOBase_h
