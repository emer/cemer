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

#ifndef Modulo_h
#define Modulo_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(Modulo);

class TA_API Modulo : public taNBase {
  // ##NO_TOKENS ##NO_UPDATE_AFTER #INLINE #INLINE_DUMP ##CAT_Math modulo for things that happen periodically
  INHERITED(taNBase)
public:
  bool           flag;          // Modulo is active?
  int            m;             // Modulo N mod m, where N is counter
  int            off;           // Modulo Offset (actually (N - off) mod m

  TA_BASEFUNS_LITE(Modulo);
protected:
  void  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy()               { };
  void  Copy_(const Modulo& cp);
};


#endif // Modulo_h
