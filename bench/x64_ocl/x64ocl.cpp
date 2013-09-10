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

#include "x64ocl.h"
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <cstring>
#include <CL/cl.hpp>

// todo: ifdef for gcc instead of clang
// #include <random>

using namespace std;

inline void
checkErr(cl_int err, const char * name)
{
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: " << name
                  << " (" << err << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

float rand_float() {
  float r = (float)rand()/(float)RAND_MAX;
  return r;
}


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
  send_act_tmp = (float*)calloc(n_units, sizeof(float));

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
  while(n_per_lay < n_per_prjn) { // make sure it fits
    n_prjns++;
    n_per_prjn = n_per_un / n_prjns;
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

      int to_lay_st = to_lay * n_per_un + lay_pos; // start at our offset in layer
      for(int k=0; k<n_per_prjn; k++) {
        int to_idx = (to_lay_st + k) % n_units;
        int idx = un->send[0].ConnectUnOwnCn(to_idx);
        un->send[0].OwnCn(idx, LeabraSendCons::WT) = rand_float();
      }
    }
  }
}

void LeabraNetwork::Send_Netin_opencl(float pct_delta) {
    cl_int err;

    for(int i=0; i<n_units; i++) {
        send_netin_tmp[i] = 0.0f;
        send_act_tmp[i] = units_flat[i]->act;
    }

    //err = cl_queue.enqueueWriteBuffer(cl_netin, CL_FALSE, 0, sizeof(float)*n_units, send_netin_tmp, NULL);
    //checkErr(err, "enqueue activations");
    err = cl_queue.enqueueWriteBuffer(cl_activations, CL_TRUE, 0, sizeof(float)*n_units, send_act_tmp, NULL);
    checkErr(err, "enqueue activations");

     cl_kernel.setArg(0, cl_netin);
    cl_kernel.setArg(1, cl_weights);
    cl_kernel.setArg(2, cl_sendidx);
    cl_kernel.setArg(3, cl_unitsweightidx);
    cl_kernel.setArg(4, cl_unitsweightlength);
    cl_kernel.setArg(5, cl_activations);


    err = cl_queue.enqueueNDRangeKernel(cl_kernel, cl::NullRange, cl::NDRange(n_units), cl::NDRange(1,1), NULL, &cl_event);
    checkErr(err, "ComamndQueue::enqueueNDRangeKernel()");

    cl_event.wait();

    err = cl_queue.enqueueReadBuffer(cl_netin, CL_TRUE, 0, sizeof(float)*n_units, send_netin_tmp, NULL);
    checkErr(err, "enqueue activations");

    for(int i=0; i<n_units; i++) {
        units_flat[i]->net = send_netin_tmp[i];
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
    std::cout << "n_non_zero: " << n_non_zero << " of total: " << n_units << std::endl;
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
  for(int i=0; i< N_CON_VARS; i++) {
    cons_own[i] = (float*)calloc(alloc_size, sizeof(float));
  }
  unit_idxs = (int*)calloc(alloc_size, sizeof(int));
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
//      openCL upload net
// This function extracts the network structure into memory arrays used in OpenCL
//
int LeabraNetwork::opencl_upload() {
    cl_int err;
    int no_prj = 0;

    int *   unitweightsidx = new int[n_units]; //index into the flat weights array where unit[i]s weights start
    int *   unitweightslength = new int[n_units]; //number of weights unit[i] has

    for (int i = 0; i < n_units; i++) {
        unitweightsidx[i] = no_prj;
        unitweightslength[i] = units_flat[i]->send->size;
        no_prj += units_flat[i]->send->size;
    }
    printf("Network has %i weights\n", no_prj);
    float * weights = new float[no_prj];
    int *   weightsidx = new int[no_prj]; //index to the unit this weight sends to

    for (int i = 0; i < n_units; i++) {
        for (int j = 0; j < units_flat[i]->send->size; j++) {
            //units_flat[i]->send->OwnCnVar(j);
            weights[unitweightsidx[i] + j] = 1.3; 
            weightsidx[unitweightsidx[i] + j] = units_flat[i]->send->UnIdx(j);
        }
    }
    

    float * myactivations = new float[n_units];
    float * myresults = new float[n_units];

    cl_weights = cl::Buffer(
                     cl_context,
                     CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                     sizeof(float)*no_prj,
                     weights,
                     &err);
    checkErr(err, "Buffer::Buffer() - weights");
    cl_sendidx = cl::Buffer(
                     cl_context,
                     CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                     sizeof(int)*no_prj,
                     weightsidx,
                     &err);
    cl_unitsweightidx = cl::Buffer(
                     cl_context,
                     CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                     sizeof(int)*n_units,
                     weightsidx,
                     &err);

    cl_unitsweightlength = cl::Buffer(
                     cl_context,
                     CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                     sizeof(int)*n_units,
                     weightsidx,
                     &err);

    cl_activations = cl::Buffer(
                     cl_context,
                     CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                     sizeof(float)*n_units,
                     myactivations,
                     &err);

    cl_netin = cl::Buffer(
                     cl_context,
                     CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                     sizeof(float)*n_units,
                     send_netin_tmp,
                     &err);
    
    checkErr(err, "Buffer::Buffer()");

    err = cl_queue.enqueueWriteBuffer(cl_weights, CL_FALSE, 0, sizeof(float)*no_prj, weights, NULL);
    checkErr(err, "enqueue weights");
    err = cl_queue.enqueueWriteBuffer(cl_sendidx, CL_FALSE, 0, sizeof(int)*no_prj, weightsidx, NULL);
    checkErr(err, "enqueue weights index");
    err = cl_queue.enqueueWriteBuffer(cl_unitsweightidx, CL_FALSE, 0, sizeof(int)*n_units, unitweightsidx, NULL);
    checkErr(err, "enqueue unit weights index");
    err = cl_queue.enqueueWriteBuffer(cl_unitsweightlength, CL_TRUE, 0, sizeof(int)*n_units, unitweightslength, NULL);
    checkErr(err, "enqueue unit weights length");
}


//////////////////////////////////////////////////
//      init openCL

int LeabraNetwork::init_openCL() {
    cl_int err;
    std::vector< cl::Platform > platformList;
    
    cl::Platform::get(&platformList);
    checkErr(platformList.size()!=0 ? CL_SUCCESS : -1, "cl::Platform::get");
    std::cerr << "Platform number is: " << platformList.size() << std::endl;
    std::string platformVendor;
    platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
    std::cerr << "Platform is by: " << platformVendor << "\n";
    cl_context_properties cprops[3] =
        {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
    
    cl_context = cl::Context(CL_DEVICE_TYPE_DEFAULT, cprops, NULL, NULL, &err);
    checkErr(err, "Conext::Context()");


    cl_devices = cl_context.getInfo<CL_CONTEXT_DEVICES>();
    std::cerr << "Devices found: " << cl_devices.size() << "\n";
    checkErr( cl_devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

    //Load and compile kernels
    std::ifstream file("kernels.cl");
    checkErr(file.is_open() ? CL_SUCCESS:-1, "opening kernel.cl");
    std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
    cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()+1));
    cl::Program program(cl_context, source);
    err = program.build(cl_devices,"");
    checkErr(err, "Program::build()");

    cl_kernel = cl::Kernel(program, "send_netin", &err);
    checkErr(err, "Kernel::Kernel()");

    cl_queue = cl::CommandQueue(cl_context, cl_devices[0], 0, &err);
    checkErr(err, "CommandQueue::CommandQueue()");
}

//////////////////////////////////////////////////
//      main

int main(int argc, char* argv[]) {
  int n_units = 2048;
  int n_per_un = 512;
  int n_layers = 5;
  int n_prjns = 2;

  int n_trials = 100;
  int n_epochs = 20;
  int cyc_per_trl = 70;

  float pct_delta = 0.02f;

  // very basic arg setting
  for(int i=1; i < argc; i++) {
    if(strcmp("n_units", argv[i]) == 0) {
      n_units = atoi(argv[i+1]);
      std::cout << "n_units=" << n_units << std::endl;
    }
    else if(strcmp("n_per_un", argv[i]) == 0) {
      n_per_un = atoi(argv[i+1]);
      std::cout << "n_per_un=" << n_per_un << std::endl;
    }
    else if(strcmp("n_layers", argv[i]) == 0) {
      n_layers = atoi(argv[i+1]);
      std::cout << "n_layers=" << n_layers << std::endl;
    }
    else if(strcmp("n_prjns", argv[i]) == 0) {
      n_prjns = atoi(argv[i+1]);
      std::cout << "n_prjns=" << n_prjns << std::endl;
    }
    else if(strcmp("n_trials", argv[i]) == 0) {
      n_trials = atoi(argv[i+1]);
      std::cout << "n_trials=" << n_trials << std::endl;
    }
    else if(strcmp("n_epochs", argv[i]) == 0) {
      n_epochs = atoi(argv[i+1]);
      std::cout << "n_epochs=" << n_epochs << std::endl;
    }
    else if(strcmp("cyc_per_trl", argv[i]) == 0) {
      cyc_per_trl = atoi(argv[i+1]);
      std::cout << "cyc_per_trl=" << cyc_per_trl << std::endl;
    }
    else if(strcmp("pct_delta", argv[i]) == 0) {
      pct_delta = atof(argv[i+1]);
      std::cout << "pct_delta=" << pct_delta << std::endl;
    }
  }

  

  int tot_cyc = cyc_per_trl * n_trials * n_epochs;

  int szun = sizeof(LeabraUnit);
  int trgsz = 768;
  if(szun != trgsz) {
    std::cout << "LeabraUnit size is: " << szun << " should be: " << trgsz << std::endl;
  }

  LeabraNetwork net;
  net.BuildUnits(n_units);
  net.ConnectUnits(n_per_un, n_layers, n_prjns);

  net.init_openCL();
  net.opencl_upload();

  TimeUsedHR time_used;
  time_used.StartTimer(true);
  
  for(int cyc = 0; cyc < tot_cyc; cyc++) {
      net.Send_Netin_opencl(pct_delta);
      //    net.Send_Netin(pct_delta);
  }

  time_used.EndTimer();

  double netin_flops = 2.0;
  double n_flops = (double)pct_delta * (double)n_units * (double)tot_cyc * n_per_un * netin_flops;

  double mflops_sec = (n_flops / time_used.s_used) / 1000000.0;

  std::cout << "total time used: " << time_used.s_used
            << " total flops: " << n_flops
            << " mflops/sec: " << mflops_sec << std::endl;


}
