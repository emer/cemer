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

#ifndef taiObjectsMimeItem_h
#define taiObjectsMimeItem_h 1

// parent includes:
#include <taiMultiMimeItem>

// member includes:
#include <taiObjectMimeItem>

// declare all other types mentioned but not required to include:


class TA_API taiObjectsMimeItem: public taiMultiMimeItem { // for tacss objects
INHERITED(taiMultiMimeItem)
public:
  bool			allBase() const; // true if all objs derive from taBase
  bool			isMulti() const  {return (items.size > 1);}
  taiObjectMimeItem*	item(int idx) const 
    {return (taiObjectMimeItem*)items.FastEl(idx);} 

  TypeDef*		CommonSubtype() const; // type of item (if 1) or common subtype if multiple

  TA_BASEFUNS_NOCOPY(taiObjectsMimeItem);
  
public: // TAI_xxx instance interface -- used for dynamic creation
  override taiMimeItem* Extract(taiMimeSource* ms, 
    const String& mimetype = _nilString);

protected:
  mutable signed char	m_all_base; // 0=unknown; -1 no, 1=yes
  override bool		Constr_impl(const String&);
private:
  void	Initialize();
  void	Destroy() {}
}; //

#endif // taiObjectsMimeItem_h
