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

#ifndef DMemShare_h
#define DMemShare_h 1

// parent includes:
#include <taBase_List>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API DMemShare : public taBase_List {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS ##NO_UPDATE_AFTER ##CAT_DMem collection of objects that are shared across distributed processes: link the objects into this list to share them
INHERITED(taBase_List)
public:
  static void 	ExtractLocalFromList(taPtrList_impl&, taPtrList_impl&) { };
  // #IGNORE

  void	Dummy(const DMemShare&) { };
  TA_BASEFUNS_NOCOPY(DMemShare);
private:
  void 	Initialize() 	{ };
  void 	Destroy()	{ };
};

#endif // DMemShare_h
