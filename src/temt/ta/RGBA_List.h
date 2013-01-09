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

#ifndef RGBA_List_h
#define RGBA_List_h 1

class TA_API RGBA_List : public taList<RGBA> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of RGBA objects
INHERITED(taList<RGBA>)
public:
  void  Initialize()            {SetBaseType(&TA_RGBA); };
  void  Destroy()               { };
  TA_BASEFUNS_NOCOPY(RGBA_List);
};


#endif // RGBA_List_h
