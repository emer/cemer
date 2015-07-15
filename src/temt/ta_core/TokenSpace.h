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

#ifndef TokenSpace_h
#define TokenSpace_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
#ifndef NO_TA_BASE
class taSigLink; //
#endif

taTypeDef_Of(TokenSpace);

class TA_API TokenSpace : public taPtrList<void> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;

public:
  static String tmp_el_name;    // for element names that need to be created

  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
  bool          keep;           // true if tokens are kept
  taAtomicInt   sub_tokens;     // number of tokens in sub-types
#ifndef NO_TA_BASE
  taSigLink*   sig_link;
#endif

  virtual void  Initialize();
  TokenSpace()                          { Initialize(); }
  TokenSpace(const TokenSpace& cp)      { Initialize(); Borrow(cp); }
  ~TokenSpace();
  void operator=(const TokenSpace& cp)  { Borrow(cp); }

  String& Print(String& strm, int indent=0) const override;
};

#endif // TokenSpace_h
