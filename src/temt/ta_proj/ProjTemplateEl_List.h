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

#ifndef ProjTemplateEl_List_h
#define ProjTemplateEl_List_h 1

// parent includes:
#include <ProjTemplateEl>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ProjTemplateEl_List);

class TA_API ProjTemplateEl_List : public taList<ProjTemplateEl> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CHILDREN_INLINE ##CAT_Project list of project template elements
INHERITED(taList<ProjTemplateEl>)
public:
  TA_SIMPLE_BASEFUNS(ProjTemplateEl_List);
protected:

private:
  void  Initialize();
  void  Destroy() { Reset(); CutLinks(); }
};

#endif // ProjTemplateEl_List_h
