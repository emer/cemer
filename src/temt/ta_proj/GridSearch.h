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



#ifdef __MAKETA__
class   QGridSearchParameterRangeList;        // #IGNORE
class   QGridSearchParameterRangeSetList;     // #IGNORE
class   GridSearchParameterRange;             // #IGNORE
#else

#include <QList>
#include "EditParamSearch"

class GridSearchParameterRange { // #IGNORE

private:
  double minValue;
  double maxValue;
  double increment;

  int idx;

public:
  GridSearchParameterRange(double minV, double maxV, double incr) {
    minValue = minV;
    maxValue = maxV;
    increment = incr;
  };
  double getFirstValue() const { return minValue; }
  bool hasNext() const;
  double nextValue() { return minValue + idx++ * increment; };
  void reset() { idx = 0; };
};

class GridSearchParameterRangeSet {
private:
  int range_idx;
public:
  taString name;
  int values;
  EditParamSearch * ps;
  QList<GridSearchParameterRange *> ranges;
  bool hasNext() const;
  double nextValue();
  void reset();

  ~GridSearchParameterRangeSet() {
    qDeleteAll(ranges);
    ranges.clear();
  }
};



typedef QList<GridSearchParameterRange *> QGridSearchParameterRangeList;
typedef QList<GridSearchParameterRangeSet *> QGridSearchParameterRangeSetList;
#endif


// parent includes:
#include <ParamSearchAlgo>

// member includes:

// declare all other types mentioned but not required to include:





taTypeDef_Of(GridSearch);

class TA_API GridSearch : public ParamSearchAlgo {
  // Grid Search algorithm.
  INHERITED(ParamSearchAlgo)
public:
  int max_jobs; // The maximum number of jobs that may be run concurrently on the cluster.

  TA_BASEFUNS_NOCOPY(GridSearch)
  void Reset() override;
  bool CreateJobs() override;
  void ProcessResults() override;
protected:
  String_PArray m_names;
  int_PArray m_counts;
  int_PArray m_iter;
  int m_cmd_id;
  bool m_all_jobs_created;

  virtual bool nextParamCombo();

private:
  void Initialize();
  void Destroy() { };

  QGridSearchParameterRangeSetList search_parameters;

  GridSearchParameterRange * ParseSubRange(String sub_range);
  QGridSearchParameterRangeList ParseRange(String range); // #IGNORE

};

#endif // GridSearch_h
