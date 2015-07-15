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

#ifndef taiMimeItem_List_h
#define taiMimeItem_List_h 1

// parent includes:
#include <taList>
#include <taiMimeItem>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiMimeItem_List);

class TA_API taiMimeItem_List: public taList<taiMimeItem> {
INHERITED(taList<taiMimeItem>)
public:
  TA_BASEFUNS(taiMimeItem_List);
private:
  NOCOPY(taiMimeItem_List)
  void	Initialize() {SetBaseType(&TA_taiMimeItem);}
  void	Destroy() {}
};

#endif // taiMimeItem_List_h
