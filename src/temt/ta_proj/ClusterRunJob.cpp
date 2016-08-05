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

#include "ClusterRunJob.h"
#include <ClusterRun>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>

TA_BASEFUNS_CTORS_DEFN(ClusterRunJob);

ClusterRunJob* ClusterRunJob::cur_job = NULL;

void ClusterRunJob::Initialize() {
  command_id = 0;
  ram_gb = 0;
  n_threads = 0;
  mpi_nodes = 0;
  mpi_per_node = 0;
  pb_batches = 0;
  pb_nodes = 0;
  term_state_saved = false;
}

void ClusterRunJob::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(running_time.empty()) {
    int secs = start_time.secsTo(end_time);
    running_time = taDateTime::SecondsToDHM(secs);
  }

  int rth = ClusterRun::RunTimeHrs(run_time);
  run_time_end = start_time;
  run_time_end.addHours(rth);  
}

void ClusterRunJob::MakeCurJobObj() {
  if(cur_job) return;
  cur_job = new ClusterRunJob;
  taBase::Own(cur_job, tabMisc::root);
  // NOTE: this object will leak until deleted by job cleanup at termination
  // could add a cleanup routine but really who cares?
}

bool ClusterRunJob::CurJobCheckSaveTermState() {
  if(!cur_job) return false;
  if(cur_job->term_state_saved) return false;
  taDateTime now; now.currentDateTime();
  int secs_to_term = now.secsTo(cur_job->run_time_end);
  if(secs_to_term < 5 * 60) {
    cur_job->term_state_saved = true;
    return true;
  }
  return false;
}
