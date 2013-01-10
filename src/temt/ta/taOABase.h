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

#ifndef taOABase_h
#define taOABase_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:
class taBaseAdapter; // 


class TA_API taOABase : public taOBase {
  // #NO_TOKENS #NO_UPDATE_AFTER owned base class with QObject adapter for signals/slots
INHERITED(taOBase)
friend class taBaseAdapter;
public:
#ifdef TA_USE_QT
  taBaseAdapter*        adapter; // #IGNORE
  void                  SetAdapter(taBaseAdapter* adapter_);
  void  Initialize()    {adapter = NULL;}
#else
  void  Initialize()    {}
#endif
  void  CutLinks();
  TA_BASEFUNS_NOCOPY(taOABase); //

private:
  void  Destroy() {CutLinks();}
};

#endif // taOABase_h
