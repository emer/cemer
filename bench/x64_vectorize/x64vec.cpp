// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "x64vec.h"
#include <iostream>
#include <memory>
#include <cstring>
#include <stdlib.h>

// todo: ifdef for gcc instead of clang
// #include <random>

using namespace std;

float rand_float() {
  float r = (float)rand()/(float)RAND_MAX;
  return r;
}

static int n_bad_alloc = 0;

//////////////////////////////////////////////////
//      LeabraNetwork

LeabraNetwork::LeabraNetwork() {
  n_units = 0;
  units_flat = NULL;
  send_netin_tmp = NULL;
}

LeabraNetwork::~LeabraNetwork() {
  FreeUnits();
}

void LeabraNetwork::BuildUnits(int n_un) {
  FreeUnits();
  if(n_un == 0) return;
  n_units = n_un;
  units_flat = (LeabraUnit**)calloc(n_units, sizeof(LeabraUnit*));
  send_netin_tmp = (float*)calloc(n_units, sizeof(float));

  for(int i=0; i<n_units; i++) {
    units_flat[i] = new LeabraUnit;
  }
}

void LeabraNetwork::FreeUnits() {
  for(int i=0; i<n_units; i++) {
    delete units_flat[i];
  }

  if(units_flat) { free(units_flat); units_flat = NULL; }
  if(send_netin_tmp) { free(send_netin_tmp); send_netin_tmp = NULL; }
  n_units = 0;
}

void LeabraNetwork::ConnectUnits(int n_per_un, int n_layers, int n_prjns) {
  int n_per_lay = n_units / n_layers;
  int n_per_prjn = n_per_un / n_prjns;
  n_per_prjn = (n_per_prjn / 8) * 8; // make sure it is modulus 8
  while(n_per_lay < n_per_prjn) { // make sure it fits
    n_prjns++;
    n_per_prjn = n_per_un / n_prjns;
    n_per_prjn = (n_per_prjn / 8) * 8;
    std::cerr << "incremeted prjns to: " << n_prjns << std::endl;
  }

  for(int i=0; i<n_units; i++) {
    LeabraUnit* un = units_flat[i];
    un->send[0].AllocCons(n_per_un);

    int lay_no = i / n_per_lay;
    int lay_pos = i % n_per_lay;

    for(int j=0; j<n_prjns; j++) {
      int to_lay;
      if(j == 0)
        to_lay = (lay_no - 1);
      else
        to_lay = lay_no + j;
      if(to_lay < 0) to_lay += n_layers;
      if(to_lay >= n_layers) to_lay -= n_layers;

      int to_lay_st = to_lay * n_per_un; // + lay_pos; // start at our offset in layer
      if(to_lay_st >= n_units)
        to_lay_st = to_lay_st % n_units;
      for(int k=0; k<n_per_prjn; k++) {
        int to_idx = to_lay_st + k;
        if(to_idx >= n_units) {
          std::cerr << "prjn allocation error -- to_idx >= n_units: " << to_idx
                    << std::endl;
          break;
        }
        int idx = un->send[0].ConnectUnOwnCn(to_idx);
        un->send[0].OwnCn(idx, LeabraSendCons::WT) = rand_float();
      }
    }
  }
}

void LeabraNetwork::Send_Netin(float pct_delta) {
  static bool did_stats = false;
  // init tmp
  for(int i=0; i<n_units; i++) {
    send_netin_tmp[i] = 0.0f;
  }

  // todo: unsuccessful attempt to focus warnings..
// #pragma clang diagnostic push
// #pragma clang diagnostic warning "-Weverything"

  // send to send_netin_tmp
  int n_send = (int)(pct_delta * n_units);
  if(n_send < 1) n_send = 1;
  for(int s = 0; s<n_units; s++) {
    int rnd_no = rand() % n_units;
    if(rnd_no < n_send) {       // got below low probability number, go for it
      LeabraUnit* un = units_flat[s];
      float su_act_delta = rand_float(); // fake
      un->send[0].Send_NetinDelta(this, su_act_delta);
    }
  }

// #pragma clang diagnostic pop

  // integrate changes from tmp

  int n_non_zero = 0;
  for(int i=0; i<n_units; i++) {
    // note: actual one has sub-loop per unit
    LeabraUnit* un = units_flat[i];
    float nd = send_netin_tmp[i];
    if(nd != 0.0f)
      n_non_zero++;
    un->net_delta = nd;
    // todo: could do further things here to simulate more computation
  }

  if(!did_stats) {
    // std::cout << "n_non_zero: " << n_non_zero << " of total: " << n_units << std::endl;
    did_stats = true;
  }
}


//////////////////////////////////////////////////
//      LeabraUnit

LeabraUnit::LeabraUnit() {
  send = new LeabraSendCons;    // just make one!
}

LeabraUnit::~LeabraUnit() {
  delete send;
}

//////////////////////////////////////////////////
//      LeabraSendCons

LeabraSendCons::LeabraSendCons() {
  size = 0; alloc_size = 0; other_idx = 0;
  con_size = sizeof(LeabraCon);
  cons_own = NULL;  unit_idxs = NULL;
}
  
LeabraSendCons::~LeabraSendCons() {
  FreeCons();
}

void LeabraSendCons::AllocCons(int sz) {
  if(sz == alloc_size) return;
  FreeCons();
  alloc_size = sz;
  if(alloc_size == 0) return;
  cons_own = (float**)calloc(N_CON_VARS, sizeof(float*));
  //  cons_own = (float**)aligned_alloc(32, N_CON_VARS* sizeof(float*));
  //  posix_memalign((void**)&cons_own, 64, N_CON_VARS * sizeof(float*));
  for(int i=0; i< N_CON_VARS; i++) {
    cons_own[i] = (float*)calloc(alloc_size, sizeof(float));
    //    posix_memalign((void**)(cons_own + i), 32, alloc_size * sizeof(float));
    if(((size_t)cons_own[i] & 31) != 0) { // check for alignment
      n_bad_alloc++;
    }
  }
  //  unit_idxs = (int*)calloc(alloc_size, sizeof(int));
  posix_memalign((void**)(&unit_idxs), 32, alloc_size * sizeof(int));
}

void LeabraSendCons::FreeCons() {
  if(cons_own) {
    for(int i=0; i< N_CON_VARS; i++) {
      free(cons_own[i]);
    }
    free(cons_own); cons_own = NULL;
  }
  if(unit_idxs) { free(unit_idxs); unit_idxs = NULL; }
  size = 0;
  alloc_size = 0;
}

int LeabraSendCons::ConnectUnOwnCn(int fm_idx) {
  if(size >= alloc_size) return -1;
  int rval = size;
  unit_idxs[size++] = (conidx_t)fm_idx;
  return rval;
}


//////////////////////////////////////////////////
//      main

int main(int argc, char* argv[]) {
  int n_units = 8192;
  int n_per_un = 2048;
  int n_epochs = 2;

  int n_layers = 4;
  int n_prjns = 2;

  int n_trials = 100;
  int cyc_per_trl = 70;

  float pct_delta = 0.02f;

  // very basic arg setting
  for(int i=1; i < argc; i++) {
    if(strcmp("n_units", argv[i]) == 0) {
      n_units = atoi(argv[i+1]);
    }
    else if(strcmp("n_per_un", argv[i]) == 0) {
      n_per_un = atoi(argv[i+1]);
    }
    else if(strcmp("n_layers", argv[i]) == 0) {
      n_layers = atoi(argv[i+1]);
    }
    else if(strcmp("n_epochs", argv[i]) == 0) {
      n_epochs = atoi(argv[i+1]);
    }
    else if(strcmp("n_prjns", argv[i]) == 0) {
      n_prjns = atoi(argv[i+1]);
    }
    else if(strcmp("n_trials", argv[i]) == 0) {
      n_trials = atoi(argv[i+1]);
    }
    else if(strcmp("cyc_per_trl", argv[i]) == 0) {
      cyc_per_trl = atoi(argv[i+1]);
    }
    else if(strcmp("pct_delta", argv[i]) == 0) {
      pct_delta = atof(argv[i+1]);
    }
    else if(strcmp("vec", argv[i]) == 0) {
      if((strcmp("SSE4", argv[i+1]) == 0) || (strcmp("sse4", argv[i+1]) == 0))
        vec_type = SSE4;
      if((strcmp("SSE8", argv[i+1]) == 0) || (strcmp("sse8", argv[i+1]) == 0))
        vec_type = SSE8;
    }
  }

  const char* vecstr = "cpu_only";
  if(vec_type == SSE4)
    vecstr = "sse4";
  else if(vec_type == SSE8)
    vecstr = "sse8";

  std::cout << " "
    << "vec=" << vecstr << " "
    << "n_units=" << n_units << " "
    << "n_per_un=" << n_per_un << " "
    << "n_epochs=" << n_epochs << " "
    << "n_layers=" << n_layers << " "
    << "n_prjns=" << n_prjns << " "
    << "n_trials=" << n_trials << " "
    << "cyc_per_trl=" << cyc_per_trl << " "
    << "pct_delta=" << pct_delta << " "
    << std::endl;

  int tot_cyc = cyc_per_trl * n_trials * n_epochs;

  int szun = sizeof(LeabraUnit);
  int trgsz = 768;
  if(szun != trgsz) {
    std::cout << "LeabraUnit size is: " << szun << " should be: " << trgsz << std::endl;
  }

  LeabraNetwork net;
  net.BuildUnits(n_units);
  net.ConnectUnits(n_per_un, n_layers, n_prjns);

  if(n_bad_alloc > 0) {
    std::cout << "n bad alloc (not 32 bit aligned): " << n_bad_alloc << std::endl;
  }
  
  TimeUsedHR time_used;
  time_used.StartTimer(true);
  
  for(int cyc = 0; cyc < tot_cyc; cyc++) {
    net.Send_Netin(pct_delta);
  }

  time_used.EndTimer();

  double netin_flops = 2.0;
  double n_flops = (double)pct_delta * (double)n_units * (double)tot_cyc * n_per_un * netin_flops;

  double mflops_sec = (n_flops / time_used.s_used) / 1000000.0;

  std::cout << " total time used: " << time_used.s_used
            << " total flops: " << n_flops
            << " mflops/sec: " << mflops_sec << std::endl;


}
