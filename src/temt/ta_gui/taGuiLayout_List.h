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

#ifndef taGuiLayout_List_h
#define taGuiLayout_List_h 1

// parent includes:
#include <taGuiLayout>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taGuiLayout_List);

class TA_API taGuiLayout_List : public taList<taGuiLayout> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Gui list of ta gui layouts
INHERITED(taList<taGuiLayout>)
public:
  TA_SIMPLE_BASEFUNS(taGuiLayout_List);
protected:

private:
  void  Initialize();
  void  Destroy() { Reset(); CutLinks(); }
};

#endif // taGuiLayout_List_h
