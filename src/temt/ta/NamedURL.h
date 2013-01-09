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

#ifndef NamedURL_h
#define NamedURL_h 1

// parent includes:
#include "ta_def.h"
#include <taString>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API NamedURL {
  // ##INLINE a named URL
public:
  String        name;           // the name of the website
  String        url;            // the URL -- uniform resource location path

  NamedURL()    { };
  NamedURL(const String& nm, const String& rl)  { name = nm; url = rl; }
  NamedURL(const NamedURL& cp)  { name = cp.name; url = cp.url; }
  ~NamedURL()   { };
};

#endif // NamedURL_h
