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

#ifndef taRefN_h
#define taRefN_h 1

// parent includes:

// member includes:
#include <taAtomicInt>

// declare all other types mentioned but not required to include:


class TA_API taRefN {
  // #NO_TOKENS #NO_MEMBERS #NO_CSS reference counting base class
public:
  static void           Ref(taRefN* it) { it->refn.ref(); }
  static void           Ref(taRefN& it) { it.refn.ref(); }
  static void           unRef(taRefN* it)       { it->refn.deref(); }
  static void           Done(taRefN* it)        { if(it->refn == 0) delete it; }
  static void           unRefDone(taRefN* it)
    {if (!it->refn.deref()) delete it; }
  static void           SafeUnRefDone(taRefN* it) { if(it) unRefDone(it); }
  static void           SetRefDone(taRefN*& var, taRefN* it) // does it fast, but safe, even for var==it
  {
    if (it)
      Ref(it);
    if (var != NULL)
      unRefDone(var);
    var = it;
  }

protected:
  taRefN()              {}
  virtual ~taRefN()     { }; // all instances should consistently use ref counting for lifetime management

  taAtomicInt            refn;
};

#endif // taRefN_h
