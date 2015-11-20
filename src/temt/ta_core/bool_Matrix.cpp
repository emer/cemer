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

#include "bool_Matrix.h"

TA_BASEFUNS_CTORS_DEFN(bool_Matrix);
SMARTREF_OF_CPP(bool_Matrix);


const bool bool_Matrix::blank = false;

bool bool_Matrix::StrValIsValid(const String& str, String* err_msg) const {
  bool rval = true;
#ifdef TA_USE_QT
  rval = str.toBool();
#endif
  if (!rval && (err_msg != NULL))
    *err_msg = "not a valid bool value";
  return rval;
}

