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

#ifndef taiViewTypeOftaBase_h
#define taiViewTypeOftaBase_h 1

// parent includes:
#include <taiViewType>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; //
class iPanelOfEditor; //


TypeDef_Of(taiViewTypeOftaBase);

class TA_API taiViewTypeOftaBase: public taiViewType { // for taBase and descendants
  TAI_TYPEBASE_SUBCLASS(taiViewTypeOftaBase, taiViewType)
public:
  override int          BidForView(TypeDef*);
  override const iColor GetEditColorInherit(taiSigLink* dl, bool& ok) const;// #IGNORE background color for edit
  override iPanelBase*  CreateDataPanel(taiSigLink* dl_);
  override taiSigLink* GetSigLink(void* data_, TypeDef* el_typ);
  void                  Initialize() { edit_panel = NULL; }
  void                  Destroy() {}
protected:
  virtual taiSigLink*  CreateSigLink_impl(taBase* data_);
  override void         CreateDataPanel_impl(taiSigLink* dl_);
private:
  iPanelOfEditor*        edit_panel; // cached during construction
};

#endif // taiViewTypeOftaBase_h
