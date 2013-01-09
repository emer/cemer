// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_matrix.h -- implementation header for matrix data (used by data system)

#ifndef TA_MATRIX_H
#define TA_MATRIX_H

// #include "ta_variant.h"
// #include "ta_base.h"
// #include "ta_group.h"
// #include "ta_TA_type_WRAPPER.h"

// #ifndef __MAKETA__
// #  include <QAbstractTableModel>
// #endif

// #include <string.h>

#ifdef _WIN32
  // The USING(taMatrix::operator=) statements in this file prevent compiler
  // warnings on Mac/Linux indicating that base-class operator=() functions
  // are being hidden by derived class overloads.  However, on Windows, those
  // lines cause many warnings: "multiple assignment operators specified".
  // The USING statements seem warranted on all OSes, so the MS warnings have
  // been suppressed.
  #pragma warning(push)
  #pragma warning(disable: 4522)
#endif

// #define IMatrix taMatrix

#include <CellRange>
#include <MatrixIndex>
#include <MatrixGeom>
#include <taMatrix>
#include <taMatrixT>
#include <taMatrix_PList>
#include <taMatrix_Group>
#include <String_Matrix>
#include <float_Matrix>
#include <double_Matrix>
#include <int_Matrix>
#include <byte_Matrix>
#include <Variant_Matrix>
#include <slice_Matrix>
#include <rgb_Matrix>
#include <voidptr_Matrix>
#include <CircMatrix>

#ifdef _WIN32
  #pragma warning(pop)
#endif

#endif
