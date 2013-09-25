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

#ifndef taDataAnalParams_h
#define taDataAnalParams_h 1

//#include "ta_def.h"
#include <taString>

// parent includes:
#include <taNBase>

// member includes:
#include <taMath>

// declare all other types mentioned but not required to include:
class DataTable; //

taTypeDef_Of(taDataAnalParams);

class TA_API taDataAnalParams : public taNBase {
  // An object to contain all the parameters needed by an analysis method of taDataAnal
  INHERITED(taNBase)
public:
  void                  Init(DataTable* src_table, const String& src_col_name, DataTable* result_table);

  // these members used by most analyses
  DataTable*            src_data_table;
  DataTable*            result_data_table;
  String                data_column_name; // also for X var column in linear regression
  String                name_column_name; // also for Y var column in linear regression
  bool                  view; // does the user want the graph drawn after the analysis - also used for "render line" in linear regression

  // these members are used by some analyses
  taMath::DistMetric    distance_metric;
  bool                  norm;
  float                 tolerance;
  int                   x_axis_component;
  int                   y_axis_component;
  bool                  include_scalars;

  void Initialize();
  void Destroy() { };
  TA_SIMPLE_BASEFUNS(taDataAnalParams);

private:
};

#endif // taDataAnalParams_h
