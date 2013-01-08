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

#ifndef taBaseItr_h
#define taBaseItr_h 1

// parent includes:
#include <taBase>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taBaseItr : public taBase {
  // #STEM_BASE ##NO_TOKENS #NO_UPDATE_AFTER ##INLINE ##INLINE_DUMP base class for iterators over containers
INHERITED(taBase)
public:
  int           count;          // count number of iterations through foreach -- always goes 0..end sequentially
  int           el_idx;         // absolute index of current item in container

  TA_BASEFUNS_LITE(taBaseItr);
private:
  inline void   Copy_(const taBaseItr& cp) { count = cp.count; el_idx = cp.el_idx; }
  inline void   Initialize()            { count = 0; el_idx = 0; }
  inline void   Destroy()               { };
};

#endif // taBaseItr_h
