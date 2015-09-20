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


TA_BASEFUNS_CTORS_DEFN(GridSearch);

void GridSearch::Initialize() {
}

bool GridSearch::StartSearch() {
  m_counts.Reset();
  m_params.Reset();

  FOREACH_ELEM_IN_GROUP(EditMbrItem, mbr, m_cluster_run->mbrs) {
    EditParamSearch &ps = mbr->param_search;
    if (ps.search) {
      bool ok = ps.ParseRange();          // just to be sure
      if(ok) {
        m_params.Link(mbr);     // link does not transfer ownership to this tmp list!
        m_counts.Add(ps.srch_vals.size);
      }
    }
  }

  m_cmd_id = 0;
  m_tot_count = 0;
  m_cur_idx = 0;
  m_iter.SetSize(m_counts.size); // this only makes sense after getting counts!
  m_iter.InitVals(0);
  if(TestError(m_counts.size == 0, "StartSearch",
               "didn't find any parameters to search on!")) {
    return false;
  }
  
  m_tot_count = m_counts[0];
  for(int i=1; i<m_counts.size; i++) {
    m_tot_count *= m_counts[i];
  }

  int chs = taMisc::Choice("GridSearch: will generate a total of " + String(m_tot_count) + " jobs", "Continue", "Cancel");
  if(chs == 1) return false;

  return true;
}

bool GridSearch::CreateJobs() {
  String cmd;
  String params;

  // If we've already created all jobs for this search, don't create any more.
  if (m_cur_idx >= m_tot_count) {
    return false;
  }

  int num_jobs = m_tot_count - m_cur_idx;
  
  for(int j=0; j<num_jobs; j++) {
    // first, run current job on current values
    // taMisc::Info("----");
    for(int p=0;p<m_counts.size; p++) {
      int val_idx = m_iter[p];
      EditMbrItem* mbr = m_params[p];
      EditParamSearch &ps = mbr->param_search;
      double value = ps.srch_vals[val_idx];
      ps.next_val = value;
      // taMisc::Info("param:", String(p), "idx:", String(val_idx), "val:", String(value));
    }
    // taMisc::Info("----");
    
    String params;
    m_cluster_run->RunCommand(cmd, params, false); // use next vals
    m_cluster_run->AddJobRow(cmd, params, m_cmd_id);

    m_cur_idx++;
    if(m_cur_idx >= m_tot_count) break;  // done!

    // then find next parameter to increment -- if an even divisor of count multiples, then that is what is incremented, otherwise increment inner-most
    int cnt_idx = m_tot_count / m_counts.Peek(); // last one
    for(int p = m_counts.size-1; p>= 0; p--) {
      if(cnt_idx == 0 || (m_cur_idx % cnt_idx == 0)) { // even hit!
        ++(m_iter[p]);               // increment iter
        if(m_iter[p] >= m_counts[p]) {
          m_iter[p] = 0;
        }
        // taMisc::Info("hit:", String(p), "cnt_idx:", String(cnt_idx), "cur:",
        //              String(m_cur_idx));
      }
      if(p >= 1)
        cnt_idx /= m_counts[p-1];   // go to next level down
    }
  }

  m_counts.Reset();
  m_params.Reset();
  
  return true;
}


void GridSearch::ProcessResults() {
  // TODO: load result files??
}

