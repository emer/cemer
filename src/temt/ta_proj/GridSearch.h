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
#include <EditParamSearch>
#include <EditMbrItem_Group>

// declare all other types mentioned but not required to include:
taTypeDef_Of(GridSearch);

class TA_API GridSearch : public ParamSearchAlgo {
  // Grid Search algorithm -- searches in uniform increments of values along each parameter, crossing all values across all parameters (the most expensive form of search!)
  INHERITED(ParamSearchAlgo)
public:
  bool  StartSearch() override;
  bool  CreateJobs() override;
  void  ProcessResults() override;

  virtual int   FindParamNameIdx(const String& nm);
  // find parameter index by name
  
  TA_BASEFUNS_NOCOPY(GridSearch)
protected:
  EditMbrItem_Group m_params;   // active parameters to be searching on
  EditMbrItem_Group m_yoked;    // items that are yoked to other searched params
  int_PArray    m_counts;       // total number of values in each param
  int_PArray    m_iter;         // current index of value in each param
  int           m_tot_count;    // total number of values to search -- prod(m_counts)
  int           m_cur_idx;      // current index out of m_tot_count
  int           m_cmd_id;

private:
  void Initialize();
  void Destroy() { };
};

#endif // GridSearch_h
