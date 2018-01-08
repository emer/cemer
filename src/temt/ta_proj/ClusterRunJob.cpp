// Co2018ght 2016-2017, Regents of the University of Colorado,
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

  int rtm = ClusterRun::RunTimeMins(run_time);
  if(rtm > 0) {
    taDateTime new_end;
    new_end = start_time;
    new_end.addMinutes(rtm);
    if(!run_time_end.isValid() || run_time_end.isNull()) {
      run_time_end = new_end;
    }
    else {
      if(new_end != run_time_end) {
        taMisc::Info("ClusterRunJob: updated run_end_time:",
                     run_time_end.toString(ClusterRun::timestamp_fmt));
      }
    }
  }
  else {
    taMisc::Info("ClusterRunJob: got null run time from:", run_time,
                 "defaulting to 7 days");
    if(!run_time_end.isValid() || run_time_end.isNull()) {
      run_time_end = start_time;
      run_time_end.addMinutes(7*24*60); // 7 days default until something valid comes in
    }
  }
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
    taMisc::Info("Cluster Run Job: 5 minutes prior to end of job at:",
                 cur_job->run_time_end.toString(ClusterRun::timestamp_fmt),
                 "signaling to save state at this point");
    return true;
  }
  return false;
}
