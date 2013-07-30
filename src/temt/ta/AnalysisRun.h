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

#ifndef AnalysisRun_h
#define AnalysisRun_h 1

#include "ta_def.h"
#include <taString>
#include <taDataAnalParams>

// member includes:

// declare all other types mentioned but not required to include:
class DataTable; //

taTypeDef_Of(AnalysisRun);

class TA_API AnalysisRun : public taNBase {
  // The AnalysisRun class collects the needed input and parameters and runs a data analysis
  INHERITED(taNBase)
public:
  enum AnalysisType {
    CLUSTER,
    PCA2d
  };

  AnalysisType        analysis_type;
  taDataAnalParams    params;

public:
  bool                Init(AnalysisType type, DataTable* src_table, const String& src_col_name, DataTable* result_table);
  bool                Run();

  void Initialize() { };
  void Destroy() { };
  TA_SIMPLE_BASEFUNS(AnalysisRun);

protected:
  virtual bool        CollectParametersCluster(taDataAnalParams& params);
  virtual bool        CollectParametersPCA2d(taDataAnalParams& params);

private:
};

#endif // AnalysisRun_h
