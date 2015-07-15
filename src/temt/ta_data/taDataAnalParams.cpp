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

#include "taDataAnalParams.h"

TA_BASEFUNS_CTORS_DEFN(taDataAnalParams);

// these params have no defaults
void taDataAnalParams::Init(DataTable* src_table, const String& src_col_name, DataTable* result_table)
{
  src_data_table = src_table;
  data_column_name = src_col_name;
  result_data_table = result_table;
  view = true;
}

// params where the analysis methods have defaults already - setting to those
void taDataAnalParams::Initialize()
{
  distance_metric = taMath::EUCLIDIAN;
  norm = false;
  tolerance = 0.0f;
  x_axis_component = 0;
  y_axis_component = 1;
  include_scalars = false;
}
