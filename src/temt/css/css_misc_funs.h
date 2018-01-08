// Copyright 2006-2018, Regents of the University of Colorado,
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


// miscellaneous functions available both in css and hard-coded C++ routines

#ifndef css_misc_funs_h
#define css_misc_funs_h

#include "css_def.h"
#include <String_Array>

CSS_API String_Array& Dir(const char* dir_nm = NULL);
CSS_API String_Array& ReadLine(std::istream& fh);

#endif  // css_misc_funs_h
