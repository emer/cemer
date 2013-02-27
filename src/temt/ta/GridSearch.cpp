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

void
GridSearch::Initialize()
{
  max_jobs = 20;
}

void
GridSearch::Reset()
{
  // Build the m_counts and m_names arrays.
  m_names.Reset();
  m_counts.Reset();
  int total_jobs = 1;
  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_cluster_run->mbrs) {
    EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      // Keep track of names to make sure they haven't changed by the time
      // ProcessResults is called.
      String name = mbr->GetName();
      m_names.Push(name);

      // Sanity check range.
      if (ps.max_val <= ps.min_val || ps.incr > (ps.max_val - ps.min_val)) {
        taMisc::Warning("ClusterRun search range invalid for parameter", name);
      }

      // Count how many values this parameter will take.  The +1 is to
      // include the beginning and end points; the +.01 is for rounding
      // error.  For example, (0.9 - 0.2) / 0.1 + 1.01 == 8.
      int count = (ps.max_val - ps.min_val) / ps.incr + 1.01;
      m_counts.Push(count);
      total_jobs *= count;

      // Reset the next_val to the minimum.
      ps.next_val = ps.min_val;
    }
  }

  // Initialize the iterator array to all 0s.
  m_iter.SetSize(m_counts.size);
  m_iter.InitVals(0);

  m_cmd_id = 0;
  m_all_jobs_created = false;
}

bool
GridSearch::CreateJobs()
{
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

  // Create num_jobs jobs (or all jobs if create_all set).
  m_cluster_run->jobs_submit.ResetData();
  for (int idx = 0; idx < num_jobs || create_all; ++idx, ++m_cmd_id) {
    // Add the current job.
    String cmd = m_cluster_run->RunCommand(false); // use next vals
    m_cluster_run->AddJobRow(cmd, m_cmd_id);

    // Cycle parameters for the next job.
    if (!nextParamCombo()) {
      // No next combo means all jobs have been created.
      m_all_jobs_created = true;
      break;
    }
  }

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

