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

#ifndef taPluginInst_PList_h
#define taPluginInst_PList_h 1

// parent includes:
#include <taPluginInst>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taPluginInst_PList: public taPtrList<taPluginInst> { // #NO_INSTANCE
INHERITED(taPtrList<taPluginInst>)
public:
  taPluginInst_PList() {}
  ~taPluginInst_PList() {Reset();}
protected:
  void		El_Done_(void* it) override { delete ((taPluginInst*)it);}
};

#endif // taPluginInst_PList_h
