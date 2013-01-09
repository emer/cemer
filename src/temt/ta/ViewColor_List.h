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

#ifndef ViewColor_List_h
#define ViewColor_List_h 1

// parent includes:
#include <taList>
#include <ViewColor>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API ViewColor_List : public taList<ViewColor> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of ViewColor items
INHERITED(taList<ViewColor>)
public:

  virtual bool  FindMakeViewColor(const String& nm, const String& dsc,
                                  bool fg, const String& fg_color_name,
                                  bool bg=false, const String& bg_color_name="");
  // find view color of given name -- if not there, make it, with given params (returns false if didn't already exist)

  TA_BASEFUNS_NOCOPY(ViewColor_List);
private:
  void  Initialize()            { SetBaseType(&TA_ViewColor); }
  void  Destroy()               { };
};

#endif // ViewColor_List_h
