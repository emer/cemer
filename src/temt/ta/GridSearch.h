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

#ifndef GridSearch_h
#define GridSearch_h 1

// parent includes:
#include <ParamSearchAlgo>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(GridSearch);

class TA_API GridSearch : public ParamSearchAlgo {
  // Grid Search algorithm.
  INHERITED(ParamSearchAlgo)
public:
  int max_jobs; // The maximum number of jobs that may be run concurrently on the cluster.

private:
  String_PArray m_names;
  int_PArray m_counts;
  int_PArray m_iter;
  int m_cmd_id;
  bool m_all_jobs_created;

public:
  TA_BASEFUNS_NOCOPY(GridSearch)
  override void Reset();
  override bool CreateJobs();
  override void ProcessResults();
private:
  bool nextParamCombo();
  void Initialize();
  void Destroy() { }
};

#endif // GridSearch_h
