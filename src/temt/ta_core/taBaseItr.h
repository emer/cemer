// Copyright 2017, Regents of the University of Colorado,
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

#ifndef taBaseItr_h
#define taBaseItr_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taBaseItr);

class TA_API taBaseItr : public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE base class for iterators over containers
INHERITED(taBase)
public:
  int           count;          // count number of iterations through foreach -- always goes 0..end sequentially
  int           el_idx;         // absolute index of current item in container
  
  inline bool   Done()          { return el_idx < 0; }
  // determines when done iterating -- when el_idx is < 0
  inline bool   More()          { return !Done(); }
  // if there is more to process 
  inline void   SetDone()      { el_idx = -1; }
  // set iterator to be done iterating

  TA_BASEFUNS_LITE(taBaseItr);
private:
  inline void   Copy_(const taBaseItr& cp) { count = cp.count; el_idx = cp.el_idx; }
  inline void   Initialize()            { count = 0; el_idx = 0; }
  inline void   Destroy()               { };
};

#endif // taBaseItr_h
