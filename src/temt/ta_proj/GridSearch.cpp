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

#include "GridSearch.h"
#include <ClusterRun>
#include <GridSearch>
#include <taMisc>

bool GridSearchParameterRange::hasNext() const {
  if ((increment == 0) && (idx > 0)) return false;
  return ((minValue + idx*increment) <= maxValue);
}



double GridSearchParameterRangeSet::nextValue() {
  if (ranges.at(range_idx)->hasNext())
    return ranges.at(range_idx)->nextValue();
  else {
    while (++range_idx < ranges.length()) {
      if (ranges.at(range_idx)->hasNext())
        return ranges.at(range_idx)->nextValue();
    }
    return -1; //This shouldn't happen and -1 is not a helpful error value!
  }
}

bool GridSearchParameterRangeSet::hasNext() const {
  int range_idx_tmp = range_idx;
  if (ranges.at(range_idx_tmp)->hasNext()) return true;
  else {
    while (++range_idx_tmp < ranges.length()) {
      if (ranges.at(range_idx_tmp)->hasNext()) return true;
    }
    return false;
  }
}

void GridSearchParameterRangeSet::reset() {
  for (int i = 0; i < ranges.length(); i++) {
    ranges.at(i)->reset();
  }
  range_idx = 0;
  ps->next_val = ranges.at(0)->getFirstValue(); //Also reset the actual value as this is what is used downstream
}

TA_BASEFUNS_CTORS_DEFN(GridSearch);

void
GridSearch::Initialize()
{
  max_jobs = 20;
}


/**
 * Parse a string of the form minValue:increment:maxValue into a structure.
 * Allocates memory for a GridSearchParameterRange, that needs to be freed later on.
 */
GridSearchParameterRange * GridSearch::ParseSubRange(String sub_range) {
  int start_pos = 0;
  int idx;
  GridSearchParameterRange *pr;

  idx = sub_range.index(':', start_pos);

  //We assume that we can either have a sigular value, or we need to specify a start value, end value and increment.
  if (idx >= 0) {
    String start_s = sub_range.at(start_pos, idx - start_pos);
    start_pos = idx + 1;
    idx = sub_range.index(':', start_pos);
    if (idx < 0) {
      //This is an invalid syntax for subrange. Handle error messages in the down stream function,
      //where we more information to tell the user what is wrong.
      return NULL;
    }
    String step_s = sub_range.at(start_pos, idx - start_pos);
    String end_s = sub_range.after(idx);
    bool ok;
    double start = start_s.toDouble(&ok);
    if (!ok) return NULL;
    double increment = step_s.toDouble(&ok);
    if (!ok) return NULL;

    double end = end_s.toDouble(&ok);
    if (!ok) return NULL;

    if ((end < start) || ((end - start) / increment <= 0)) {
      taMisc::Error("Range specified does not provide a valid set of values: " + sub_range);
      return NULL;
    }
    pr = new GridSearchParameterRange(start, end, increment);
  } else {
    bool ok;
    double start = sub_range.toDouble(&ok);
    if (!ok) return NULL;
    pr = new GridSearchParameterRange(start,start, 0);
  }
  return pr;
}

/**
 * Parse a string of the form "1;2;3:1:5;10:2:20" into a list of ranges.
 * Allocates a variety of memory that needs to be freed again.
 * We need to use a list of pointers as the objects aren't immutable.
 */
QGridSearchParameterRangeList GridSearch::ParseRange(String range) {
  int idx, idx2;
  int start_pos = 0;
  int start_pos2 = 0;
  QGridSearchParameterRangeList list;

  idx = range.index(';', start_pos);

  //Iterate over all ; in the range string
  while (idx >= 0) {
    String sub_range = range.at(start_pos, idx - start_pos);
    GridSearchParameterRange * gspr = ParseSubRange(sub_range);
    if (gspr == NULL) {
      taMisc::Error("Range parameter has invalid syntax: " + range);
      return list;
    }
    list.append(gspr);

    start_pos = idx + 1;
    idx = range.index(';', start_pos);
  }
  String sub_range = range.after(start_pos - 1);
  GridSearchParameterRange * gspr = ParseSubRange(sub_range);
  if (gspr == NULL) {
    taMisc::Error("Range parameter has invalid syntax: " + range);
    return list;
  }
  list.append(gspr);
  return list;
}

void
GridSearch::Reset()
{
  // Build the m_counts and m_names arrays.
  m_names.Reset();
  m_counts.Reset();
  int total_jobs = 1;

  m_iter.SetSize(m_counts.size);
  m_iter.InitVals(0);

  //We are doing the initialisation of the range structure here, as I didn't know how to do it in
  //the reset function and pass it here.
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_cluster_run->mbrs) {
    EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      String name = mbr->GetName();
      GridSearchParameterRangeSet *gsprs = new GridSearchParameterRangeSet();
      gsprs->name = name;
      gsprs->ranges = ParseRange(ps.range);
      gsprs->ps = &ps;
      search_parameters.append(gsprs);
    }
  }

  m_cmd_id = 0;
  m_all_jobs_created = false;
}

bool
GridSearch::CreateJobs()
{
  String cmd;
  String params;

  // If we've already created all jobs for this search, don't create any more.
  if (m_all_jobs_created) return false;

  // If max_jobs is 0, create them all.
  bool create_all = (max_jobs <= 0);

  // Check how many jobs are currently running so we know how many more
  // we can create.
  int num_jobs = max_jobs;
  if (!create_all) {
    int running_jobs = m_cluster_run->CountJobs(
      m_cluster_run->jobs_running,
      "SUBMITTED|QUEUED|RUNNING");
    num_jobs -= running_jobs;

    // Check if we can't create any more jobs because the maximum number
    // allowed are already running.
    if (num_jobs <= 0) {
      return false;
    }
  }



  int count = 0;
  for (int i = 0; i < search_parameters.length(); i++) {
    search_parameters.at(i)->reset();
    //Call nextValue() on all but the last search parameter, as the way the itteration
    //works is to use the first value after reset without calling "nextValue()" and so without
    //calling nextValue() ahead of time, the first value would be used twice.
    if (i + 1 < search_parameters.length()) search_parameters.at(i)->nextValue();
  }

  bool finished = false;
  int inner_idx = search_parameters.length() - 1;  //start iterating from the last parameter first.

  //We are doing a combinatorial search over all combinations of parameters that are supposed to be searched over.
  while(!finished) {
    if (search_parameters.at(inner_idx)->hasNext()) {
      double value = search_parameters.at(inner_idx)->nextValue();
      search_parameters.at(inner_idx)->ps->next_val = value;

      m_cluster_run->RunCommand(cmd, params, false); // use next vals
      m_cluster_run->AddJobRow(cmd, params, m_cmd_id);
      num_jobs--;
      if (num_jobs <= 0) {
        for (int i = 0; i < search_parameters.length(); i++) delete search_parameters.at(i);
        return false;
      }
    } else {
      //This should only be the case, if we only have one parameter over which to search,
      //in which case if the last parameter is done iterating, we are altogether done.
      if (inner_idx == 0) {
        finished = true;
        break;
      }

      for (int i = inner_idx; i < search_parameters.length(); i++) {
        //At this point, the inner_index should always be the last
        //parameter. So this for loop shouldn't be necessary.
        //We do need to reset all parameters after the one we increment
        search_parameters.at(i)->reset();
      }

      inner_idx--;
      //Move backwards over the list of parameters to find the next parameter that as still a value
      //left that needs to be iterated over.
      while (!search_parameters.at(inner_idx)->hasNext()) {
        search_parameters.at(inner_idx)->reset();
        if (inner_idx + 1 < search_parameters.length()) search_parameters.at(inner_idx)->nextValue();
        if (inner_idx == 0) {
          finished = true;
          break;
        }
        inner_idx--;
      }
      if (!finished) {
        double value = search_parameters.at(inner_idx)->nextValue();
        search_parameters.at(inner_idx)->ps->next_val = value;
        //Reset the inner index, as once we have changed one parameters value,
        //we now need to iterate over all possible value combinations from the end backwards.
        inner_idx = search_parameters.length() - 1;
      }
    }
  }

  qDeleteAll(search_parameters);
  search_parameters.clear();
  // Success: jobs created.
  return true;
}

bool
GridSearch::nextParamCombo()
{
  // Cycle parameters for the next job by adding 1 to the least-significant
  // element in the iterator array, and carrying if needed.
  int idx = 0;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_cluster_run->mbrs) {
    EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      // Sanity check that the parameters to search haven't changed.
      String name = mbr->GetName();
      if (name != m_names[idx]) {
        taMisc::Error("Search parameters have changed; expected parameter",
          m_names[idx], "at index", String(idx), "but got", name);
        return false;
      }

      // Increment the current iterator element.
      if (++m_iter[idx] < m_counts[idx]) {
        // No carry, so incrementing the iterator is complete.
        ps.next_val = ps.min_val + (m_iter[idx] * ps.incr);
        return true;
      }

      // Iterator elem rolled over; reset to 0/min and carry the 1 to
      // the next element (if there is one).
      m_iter[idx] = 0;
      ps.next_val = ps.min_val;
      if (++idx >= m_iter.size) {
        // The iterator overflowed (all elements rolled over to 0)
        // so we're done creating jobs.
        return false;
      }
    }
  }

  // Should never get here due to the idx>=m_iter.size check.
  return false;
}

void
GridSearch::ProcessResults()
{
  // TODO: load result files??
}

