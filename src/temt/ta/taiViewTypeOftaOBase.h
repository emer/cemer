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
  override bool         needSet() const {return true;} // always, so we can do dyn panels
  override int          BidForView(TypeDef*);
  override taiSigLink* GetSigLink(void* data_, TypeDef* el_typ); // optimized version of taiViewTypeOftaBase
  override void         CheckUpdateDataPanelSet(iPanelSet* pan);
  void                  Initialize() {}
  void                  Destroy() {}
protected:
  override taiSigLink* CreateSigLink_impl(taBase* data_);
  override void         CreateDataPanel_impl(taiSigLink* dl_);
};

#endif // taiViewTypeOftaOBase_h
