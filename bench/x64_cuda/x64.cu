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

#include "x64.h"

texture<float,1> tex_x_float;
inline void bind_x(const float * x)
{
  size_t offset = size_t(-1);
  cudaSafeCall(cudaBindTexture(&offset, tex_x_float, x));
}

inline void unbind_x(const float * x)
{
  cudaSafeCall(cudaUnbindTexture(tex_x_float));
}

__inline__ __device__ float fetch_x(const int& i, const float * x)
{
  return tex1Dfetch(tex_x_float, i);
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
        LeabraCon* cn = un->send[0].ConnectUnOwnCn(to_idx);
        cn->wt = rand_float();
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

  // And now for the kernel 
  int n_send = (int)(pct_delta * n_units);
  for(int s = 0; s<n_send; s++) {
    int si = rand() % n_units;
    LeabraUnit* un = units_flat[si];
    float su_act_delta = rand_float(); // fake
    un->send[0].Send_NetinDelta(this, su_act_delta);
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


/*
 * In this method, I build all the relevant data needed to run the sender calculation
 * on the GPU. This code proceeds in several steps:
 *
 * (1) First is builds a CSC (Compressed Sparse Column) matrix for doing the calculation
 * doing the update calculation. Imagine the following connectivity pattern:
 *
 * Input Neuron |  1      |   2      |  3    ...
 * ====================================================
 * Output    1  |  I      |   w_2_1  |  0
 *           2  |  w_1_2  |   I      |  0
 *           3  |  0      |   0      |  I
 *           4  |  w_1_4  |   w_2_4  |  w_3_4
 *           5  |  w_1_5  |   w_2_5  |  0
 *           6  |  w_1_6  |   0      |  0
 *           7  |  0      |   0      |  w_3_7
 *           8  |  0      |   0      |  0
 *           9  |  0      |   w_2_9  |  w_3_9
 *          10  |  w_1_10 |   w_2_10 |  w_3_10
 *
 * In the compressed sparse column form, the 0's and I's are not stored. There are three
 * arrays describing the compressed sparse column form of the matrix:
 *  (a) column_offsets
 *  (b) row_indices_offsets
 *  (c) weights
 * Each of these is a linear array in memory. The column_offsets is smaller vector
 * where each entry in gives the location of the start of a new column of data in the
 * row_indices and weights data structures. The first value in column_offsets is 0.
 * The last entry is the total number of entries in the column_indices and/or weights
 * vectors. column_offsets should have length n_units+1.
 *
 * Thus, in the array above, we  have
 *   column_offsets = [0, 5, 10, 14]
 *   row_indices    = [2, 4, 5, 6, 10, 1, 4, 5, 9, 10, 4, 7, 9, 10]
 *   weights        = [w_1_2, w_1_4, w_1_5, w_1_6, w_1_10, w_2_1, w_2_4, w_2_5, w_2_9, w_2_10, w_3_4, w_3_7, w_3_9, w_3_10]
 *
 * (2) Once the matrix is built, we do a calculation of the maximum number of connections 
 * in a column. This will prove useful later on as we add padding to the columns in 
 * order to get aligned memory. Currently, this is not used.
 *
 * (3) Finally, once this array is built on the CPU, we then allocate space on the GPU
 * for this data as well for several auxilliary data structures needed in the calculation.
 * 
 */
void LeabraNetwork::BuildCudaData(float pct_delta) {
  totalSize = 0;
  std::vector<int> column_offsets_cpu(n_units+1);
  std::fill(column_offsets_cpu.begin(), column_offsets_cpu.end(), 0);
  std::vector<int> column_count_cpu(n_units);
  std::fill(column_count_cpu.begin(), column_count_cpu.end(), 0);
  std::vector<int> row_indices_cpu(0);
  std::vector<float> wts_cpu(0);
  for(int s = 0; s<n_units; s++) {
    LeabraUnit* un = units_flat[s];    
    totalSize += un->send->size;
    column_offsets_cpu[s+1]=column_offsets_cpu[s]+un->send->size;
    column_count_cpu[s]=un->send->size;
    for(int i = 0; i<un->send->size; i++) {
      row_indices_cpu.push_back(un->send->UnIdx(i));
      wts_cpu.push_back(un->send->OwnCn(i)->wt);
    }
  }

  /* calculate the maximum number of entries in a column */
  std::vector<int>::iterator itmin = std::min_element(column_count_cpu.begin(),column_count_cpu.end());
  std::vector<int>::iterator itmax = std::max_element(column_count_cpu.begin(),column_count_cpu.end());
  maxConnections = *itmax;
  printf("%d %d min=%d max=%d\n",totalSize,row_indices_cpu.size(),*itmin,*itmax);

  /* Now, send the data to the GPU */
  cudaSafeCall(cudaMalloc((void **)&column_offsets_gpu,(n_units+1)*sizeof(int)));
  cudaSafeCall(cudaMemcpy(column_offsets_gpu,&(column_offsets_cpu[0]),(n_units+1)*sizeof(int),cudaMemcpyHostToDevice));

  cudaSafeCall(cudaMalloc((void **)&row_indices_gpu,totalSize*sizeof(int)));
  cudaSafeCall(cudaMemcpy(row_indices_gpu,&(row_indices_cpu[0]),totalSize*sizeof(int),cudaMemcpyHostToDevice));

  cudaSafeCall(cudaMalloc((void **)&wts_gpu,totalSize*sizeof(float)));
  cudaSafeCall(cudaMemcpy(wts_gpu,&(wts_cpu[0]),totalSize*sizeof(float),cudaMemcpyHostToDevice));

  /* Here I tried to put the weights in a texture. I thought this might give faster memory accesses.
     However since the reads into the wts vector is pretty much coalesced, this doesn't make any 
     real difference. Textures are useful when nearby threads on the GPU reads (not writes) data that 
     are scattered in memory. This may be useful later so we leave it in. */
  //bind_x(wts_gpu);

  /* CPU and GPU data structures for the send_netin vector */
  cudaSafeCall(cudaMalloc((void **)&send_netin_gpu,pct_delta*n_units*n_units*sizeof(float)));
  cudaSafeCall(cudaMallocHost((void **)&send_netin_cpu,n_units*sizeof(float)));

  /* Set up CURAND. Currently, this isn't used but if we did want to generate random numbers on the
   * GPU, we would need one of these generators. Should be error-checked. */
  curandCreateGenerator(&gen, CURAND_RNG_PSEUDO_DEFAULT);

  /* CPU and GPU data structures for the data which is randomly generated. This will not be needed
     for real simulations if a CUDA random number generator is ued. However, for the time being it 
     is useful to have these for testing correctness between CPU and GPU implementations */
  cudaSafeCall(cudaMalloc((void **)&su_act_delta_gpu,pct_delta*n_units*sizeof(float)));
  cudaSafeCall(cudaMallocHost((void **)&su_act_delta_cpu,pct_delta*n_units*sizeof(float)));
  cudaSafeCall(cudaMalloc((void **)&si_gpu,pct_delta*n_units*sizeof(float)));
  cudaSafeCall(cudaMallocHost((void **)&si_cpu,pct_delta*n_units*sizeof(float)));

  /* Build streams needed for asynchronous execution */
  cudaSafeCall(cudaStreamCreate(&stream1));
  cudaSafeCall(cudaStreamCreate(&stream2));

  /* Build events needed for timing */
  cudaSafeCall(cudaEventCreate(&event1));
  cudaSafeCall(cudaEventCreate(&event2));
  totalTime=0.0;
}

/*
 * Here, we free all the GPU data.
 */
void LeabraNetwork::FreeCudaData() {
  cudaSafeCall(cudaFree(column_offsets_gpu));
  cudaSafeCall(cudaFree(row_indices_gpu));
  //unbind_x(wts_gpu);
  cudaSafeCall(cudaFree(wts_gpu));

  cudaSafeCall(cudaFree(send_netin_gpu));
  cudaSafeCall(cudaFreeHost(send_netin_cpu));

  cudaSafeCall(cudaFree(su_act_delta_gpu));
  cudaSafeCall(cudaFreeHost(su_act_delta_cpu));
  cudaSafeCall(cudaFree(si_gpu));
  cudaSafeCall(cudaFreeHost(si_cpu));

  // Free events
  cudaSafeCall(cudaEventDestroy(event1));
  cudaSafeCall(cudaEventDestroy(event2));

  // Free streams
  cudaSafeCall(cudaStreamDestroy(stream1));
  cudaSafeCall(cudaStreamDestroy(stream2));
  curandDestroyGenerator(gen);
}

#ifndef USE_ATOMICS
#define USE_ATOMICS
#endif
//#undef USE_ATOMICS

/*
 * This kernel initializes an array to some value. Typically, this would be done with 
 * cudaMemset. However, as I wanted to be able to run this concurrently with data
 * transfers via cudaMemcpyAsync, I decided to write my own kernel for this.
 */
__global__ void kernel0(int n, float val, float *x) {
  int tid = blockIdx.x*blockDim.x+threadIdx.x;
  if (tid<n) { x[tid]=val; }
}

/*
 * This version of the send kernel updates the values in send_netin with either 
 * (1) atomics or (2) writing to unique data structure and accumulating with
 * a second kernel (which is kernel2). When using (1), kernel2 is not required.
 * Nonetheless, it works and gives a path forward for further improvement.
 */
__global__ void kernel1(const int n_units, const float * su_act_delta, const int * rand_indices, 
			const int * column_offsets, const int * row_indices, const float * wts, float * send_netin1) {

  __shared__ int column;
  __shared__ int cols[2];
  __shared__ float su_act;
  if (threadIdx.x<2) {
    if (threadIdx.x==0) {
      atomicExch(&column, rand_indices[blockIdx.y]); //(int)floor(rand_indices[blockIdx.y]*((float)n_units));
      atomicExch(&su_act,su_act_delta[blockIdx.y]);
    }
    cols[threadIdx.x] = column_offsets[column+threadIdx.x];
  }
  __syncthreads();

  int tid = blockIdx.x*blockDim.x + threadIdx.x;
  if (tid<cols[1]-cols[0]) {
    int index = cols[0]+tid;
    int row=row_indices[index];
    //float w=fetch_x(cols[0]+tid,wts);
    float w = wts[index];
#ifdef USE_ATOMICS
    atomicAdd(&(send_netin1[row]),w*su_act);
#else
    send_netin1[blockIdx.y*n_units+row]=w*su_act;
#endif
  }
}

__global__ void kernel2(const int n_units, const int n_send, float * send_netin_gpu) {

  __shared__ volatile float sdata[128];
  int tid = threadIdx.y*blockDim.x+threadIdx.x;
  sdata[tid] = 0.0;
  __syncthreads();

  int tidx = blockIdx.x*blockDim.x + threadIdx.x;
  int tidy = tidx + threadIdx.y*n_units;
  int i;
  if (tidx<n_units) {
    for (i=0; i<n_send; i+=4) {
      sdata[tid] += send_netin_gpu[tidy + i*n_units];
    }
    i-=4;
    if (threadIdx.y<n_send-i) {
      sdata[tid] += send_netin_gpu[tidy + i*n_units];
    }
  }
  __syncthreads();
  if (tidx<n_units) {
    if (threadIdx.y<2) sdata[tid]+=sdata[tid+2];
    if (threadIdx.y<1) send_netin_gpu[tidx] = sdata[tid]+sdata[tid+1];
  }
}

#ifndef COMPARE_RESULTS
#define COMPARE_RESULTS
#endif
#undef COMPARE_RESULTS

void LeabraNetwork::Send_NetinCuda(float pct_delta) {
  static bool did_stats = false;
  // init tmp
  int n_send = (int)(pct_delta * n_units);

  for(int s = 0; s<n_send; s++) {
    si_cpu[s] = rand() % n_units;
    su_act_delta_cpu[s] = 1.0; //rand_float(); // fake
  }

  cudaSafeCall(cudaMemcpyAsync(su_act_delta_gpu,su_act_delta_cpu,n_send*sizeof(float),cudaMemcpyHostToDevice, stream1));
  cudaSafeCall(cudaMemcpyAsync(si_gpu,si_cpu,n_send*sizeof(int),cudaMemcpyHostToDevice, stream1));

  int nThreads=128;
  int nBlocks=int(ceil(((float)n_units)/(float)nThreads));
#ifdef USE_ATOMICS
  kernel0<<<nBlocks,nThreads,0,stream2>>>(n_units, 0.0, send_netin_gpu);
#else
  kernel0<<<nBlocks,nThreads,0,stream2>>>(n_send*n_units, 0.0, send_netin_gpu);
#endif
  cudaSafeCall(cudaGetLastError());
  cudaSafeCall(cudaStreamSynchronize(stream1));

  nBlocks=int(ceil(((float)maxConnections)/(float)nThreads));
  dim3 grid(nBlocks,n_send,1);

  cudaSafeCall(cudaEventRecord(event1,stream2));
  kernel1<<<grid,nThreads,0,stream2>>>(n_units, su_act_delta_gpu, si_gpu, column_offsets_gpu, row_indices_gpu, wts_gpu, send_netin_gpu);
  cudaSafeCall(cudaGetLastError());
  cudaSafeCall(cudaEventRecord(event2,stream2));
  cudaSafeCall(cudaEventSynchronize(event2));
  float dt=0.0;
  cudaSafeCall(cudaEventElapsedTime(&dt,event1,event2));
  totalTime+=dt;

#ifdef USE_ATOMICS

#else
  nThreads=32;
  dim3 block(nThreads,4,1);
  nBlocks=int(ceil(((float)n_units)/(float)nThreads));
  kernel2<<<nBlocks,block,0,stream2>>>(n_units, n_send, send_netin_gpu);
  cudaSafeCall(cudaGetLastError());
#endif

  cudaSafeCall(cudaMemcpyAsync(send_netin_cpu,send_netin_gpu,n_units*sizeof(float),cudaMemcpyDeviceToHost,stream2));
  cudaSafeCall(cudaStreamSynchronize(stream2));

#ifdef COMPARE_RESULTS
  // integrate changes from tmp
  for(int i=0; i<n_units; i++) {
    send_netin_tmp[i] = 0.0f;
  }
  for(int s = 0; s<n_send; s++) {
    //int si = rand() % n_units;
    int si = si_cpu[s];
    LeabraUnit* un = units_flat[si];
    //float su_act_delta = rand_float(); // fake
    float su_act_delta = su_act_delta_cpu[s]; // fake
    un->send[0].Send_NetinDelta(this, su_act_delta);
  }  

  for(int i=0; i<n_units; i++) {
    float error = (send_netin_tmp[i]-send_netin_cpu[i])/send_netin_cpu[i];
    if (error>1.e-5)
      printf("%d : cpu=%1.6g\tgpu=%1.6g\terror=%1.6g\n",i,send_netin_tmp[i],send_netin_cpu[i],error);
  }
  exit(0);
#endif

// #pragma clang diagnostic pop
  int n_non_zero = 0;
  for(int i=0; i<n_units; i++) {
    // note: actual one has sub-loop per unit
    LeabraUnit* un = units_flat[i];
    float nd = send_netin_cpu[i];
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
  cons_own = (char*)calloc(alloc_size, con_size); // clear it
  unit_idxs = (int*)calloc(alloc_size, sizeof(int));
}

void LeabraSendCons::FreeCons() {
  if(cons_own) { free(cons_own); cons_own = NULL; }
  if(unit_idxs) { free(unit_idxs); unit_idxs = NULL; }
  size = 0;
  alloc_size = 0;
}

LeabraCon* LeabraSendCons::ConnectUnOwnCn(int fm_idx) {
  if(size >= alloc_size) return NULL;
  LeabraCon* rval = OwnCn(size);
  unit_idxs[size++] = fm_idx;
  return rval;
}


//////////////////////////////////////////////////
//      main

int main(int argc, char* argv[]) {

  
  int n_units = atoi(argv[1]);
  int n_per_un = atoi(argv[2]);
  int GPU = atoi(argv[3]);
  int n_layers = 5;
  int n_prjns = 2;

  int n_trials = 100;
  int n_epochs = 5;
  int cyc_per_trl = 70;

  float pct_delta = 0.02f;

  // cuda stuff for testing
  int devices;
  cudaSafeCall(cudaGetDeviceCount(&devices));
  printf("There are %d devices\n",devices);

  // todo: get all the args

  int tot_cyc = cyc_per_trl * n_trials * n_epochs;

  int szun = sizeof(LeabraUnit);
  int trgsz = 768;
  if(szun != trgsz) {
    std::cout << "LeabraUnit size is: " << szun << " should be: " << trgsz << std::endl;
  }

  LeabraNetwork net;
  net.BuildUnits(n_units);
  net.ConnectUnits(n_per_un, n_layers, n_prjns);

  net.BuildCudaData(pct_delta);

  TimeUsedHR time_used;
  time_used.StartTimer(true);
  
  for(int cyc = 0; cyc < tot_cyc; cyc++) {
    if (GPU)
      net.Send_NetinCuda(pct_delta);
    else
      net.Send_Netin(pct_delta);
  }

  time_used.EndTimer();

  printf("Average Time Per Send Call(seconds) on the GPU=%g\n",.001*net.totalTime/tot_cyc);
  printf("Total Number of Calls=%d\n",tot_cyc);

  double netin_flops = 2.0;
  double n_flops = (double)pct_delta * (double)n_units * (double)tot_cyc * n_per_un * netin_flops;

  double mflops_sec = (n_flops / time_used.s_used) / 1000000.0;

  std::cout << "total time used: " << time_used.s_used
            << " total flops: " << n_flops
            << " mflops/sec: " << mflops_sec << std::endl;

  net.FreeCudaData();
}

/* template<unsigned int THREADS_PER_ROW> */
/* __global__ void kernel2(int n_units, int n_send, float * rand_indices, float * send_netin_gpu, float * send_netin_small_gpu) { */

/*   extern __shared__ volatile float sdata[]; */
/*   int index = max((int)floor(rand_indices[threadIdx.x]*n_units),0); */
/*   if (threadIdx.x<n_send) sdata[threadIdx.x] = send_netin_gpu[blockIdx.x*n_units + index]; */
/*   __syncthreads(); */
  
/*   // reduce local sums to row sum                                                        */
/*   if (THREADS_PER_ROW > 32) sdata[threadIdx.x] += sdata[threadIdx.x + 32];                                                                                                        */
/*   __syncthreads(); */
/*   if (THREADS_PER_ROW > 16) sdata[threadIdx.x] += sdata[threadIdx.x + 16]; */
/*   if (THREADS_PER_ROW >  8) sdata[threadIdx.x] += sdata[threadIdx.x +  8]; */
/*   if (THREADS_PER_ROW >  4) sdata[threadIdx.x] += sdata[threadIdx.x +  4]; */
/*   if (THREADS_PER_ROW >  2) sdata[threadIdx.x] += sdata[threadIdx.x +  2]; */
/*   if (THREADS_PER_ROW >  1) sdata[threadIdx.x] += sdata[threadIdx.x +  1]; */
  
/*   if (threadIdx.x==0)  */
/*     send_netin_small_gpu[blockIdx.x] = sdata[0]; */
/* } */

/* __global__ void kernel1(int N, int n_units, int n_send, float su_act_delta, float * rand_indices,  */
/* 			       int * column_offsets, int * row_indices, float * wts, float * send_netin) { */

/*   __shared__ int column; */
/*   if (threadIdx.x==0) */
/*     column = max((int)floor(rand_indices[blockIdx.y]*n_units),0); */
  
/*   __syncthreads(); */
/*   int thisColumn = column_offsets[column]; */
/*   int nextColumn = column_offsets[column+1]; */

/*   int tid = blockIdx.x*blockDim.x + threadIdx.x; */
/*   if (tid<nextColumn-thisColumn) { */
/*     int row=row_indices[thisColumn+tid]; */
/*     send_netin[blockIdx.y + row] = wts[row]*su_act_delta; */
/*   } */
/* } */

// generate random numbers
//cudaMemset(send_netin_gpu,0,n_units*n_send*sizeof(float));
//curandGenerateUniform(gen, rand_indices_gpu, n_send);
// And now for the kernel 
//float su_act_delta = rand_float(); // BS for now
// collect based on the random indices
//kernel1<<<grid,nThreads>>>(totalSize, n_units, n_send, su_act_delta, rand_indices_gpu, column_offsets_gpu, row_indices_gpu, wts_gpu, send_netin_gpu);
//kernel2<64><<<n_units,64,96*sizeof(float)>>>(n_units, n_send, rand_indices_gpu, send_netin_gpu, send_netin_small_gpu);
//err = cudaGetLastError();
//if (err!= cudaSuccess) { printf("Kernel2 Error = %s at line %d",cudaGetErrorString(err),__LINE__); exit(0); }
