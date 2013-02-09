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

#ifndef taiMultiMimeItem_h
#define taiMultiMimeItem_h 1

// parent includes:
#include <taiMimeItem>

// member includes:
#include <taiMimeItem_List>

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiMultiMimeItem);

class TA_API taiMultiMimeItem: public taiMimeItem { // #VIRT_BASE
INHERITED(taiMimeItem)
public:
  
  inline int		count() const {return items.size;}
  taiMimeItem*		item(int idx) const {return items.FastEl(idx);} //note: can be replaced with strongly typed version
  
  void	InitLinks();
  void	CutLinks();
  TA_ABSTRACT_BASEFUNS(taiMultiMimeItem);
  
protected:
  taiMimeItem_List	items; // the subitems
  
private:
  NOCOPY(taiMultiMimeItem)
  void	Initialize();
  void	Destroy() {CutLinks();}
};

#endif // taiMultiMimeItem_h
