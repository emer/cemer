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

#ifndef taiVariant_h
#define taiVariant_h 1

// parent includes:
#include <taiVariantBase>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taiVariant: public taiVariantBase {
  Q_OBJECT
  INHERITED(taiVariantBase)
public:
  inline QWidget*       rep() const { return (QWidget*)m_rep; }

  void          GetImage(const Variant& var) {GetImage_Variant(var);}
  void          GetValue(Variant& var) const {GetValue_Variant(var);}

  taiVariant(IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiVariant();
protected:
  override void         GetImageVar_impl(const Variant& val) {GetImage(val);}
  override void         GetValueVar_impl(Variant& val) const {GetValue(val);}
};


#endif // taiVariant_h
