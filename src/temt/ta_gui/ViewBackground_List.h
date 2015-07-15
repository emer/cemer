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

#ifndef ViewBackground_List_h
#define ViewBackground_List_h 1

// parent includes:
#include <ViewBackground>
#include <taList>

// member includes:
#ifndef __MAKETA__
  #include <QBrush>
#else
  class QBrush; // #IGNORE
#endif

// declare all other types mentioned but not required to include:


taTypeDef_Of(ViewBackground_List);

class TA_API ViewBackground_List : public taList<ViewBackground> {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Display list of ViewBackground items
INHERITED(taList<ViewBackground>)
public:
#ifndef __MAKETA__
        virtual bool  FindMakeViewBackground(const String& nm, const String& dsc, Qt::BrushStyle style);
    // find view background (pattern) of given name -- if not there, make it, with given params (returns false if didn't already exist)
#endif

  TA_BASEFUNS_NOCOPY(ViewBackground_List);
private:
  void  Initialize()            { SetBaseType(&TA_ViewBackground); }
  void  Destroy()               { };
};

#endif // ViewBackground_List_h
