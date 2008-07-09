#include "nb_netstru_cuda.h"

#include <math.h>
#include <stdio.h>
#include <iostream>

using namespace std;


//////////////////////////////////
//  CudaNetEngine		//
//////////////////////////////////

void CudaNetEngine::Initialize_impl() {
  net_tasks.Alloc(1);
  for (int i = 0; i < 1; i++) {
    NetTask_Cuda* tsk = new NetTask_Cuda;
    tsk->task_id = i;
    net_tasks.Add(tsk);
  }
}

void CudaNetEngine::OnBuild() {
  const int n_units_flat = net->units_flat.size;
  int rval = 0; // not used
  if (Network::recv_based) {
    rval = cuRecv_AllocMem((uint)n_units_flat, (n_cons * 2));

    if (rval != 0) {
    } else {
      rval = cuRecv_CpHD_Cons(GetCon);
    }
  } else {
  }
#ifdef DEBUG
  if (rval != 0)
    cerr < "ERROR CudaNetEngine::OnBuild could not alloc mem or copy cons\n";
#endif
}



//////////////////////////////////
//  NetTask_Cuda		//
//////////////////////////////////

bool NetTask_Cuda::GetCon(int un_idx, int con_idx, int* snd_idx, float* wt) {
  Unit* un = net.units_flat.FastEl(un_idx);
  //NOTE: following assumes all prjns have same num of cons
#ifdef SEND_CONS
  taPtrList_impl* cons = &(un->send);
  div_t qr = div(con_idx, Nb::n_cons);
  ConsBase* cb = un->send->SafeEl(qr.quot);
#else
  taPtrList_impl* cons = &(un->recv);
  div_t qr = div(con_idx, Nb::n_cons);
  ConsBase* cb = un->recv->SafeEl(qr.quot);
#endif
  
  //NOTE: because con size is same everywhere, we can test easily, 
  // but real algo must take from Cons
  Connection* cn;
  if (cb && ((cn = cb->SafeEl(qr.rem)))) {
    *snd_idx = cb->units(qr.rem);
    *wt = cn->wt();
    return false;
  } 
  return true;
}

void NetTask_Cuda::Send_Netin() {
/*TODO  Unit** units = net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, n_procs);
  while (my_u < n_units_flat) {
    Unit* un = units[my_u]; //note: accessed flat
    if (un->DoDelta()) {
      Send_Netin_0(un);
      AtomicFetchAdd(&n_tot, 1);
      //AtomicFetchAdd(&t_tot, 1); // because of helping hand clobbers
    }
    my_u = AtomicFetchAdd(&g_u, n_procs);
  }*/
}

void NetTask_Cuda::Recv_Netin() {
  cuRecv_Netin();
//nn  cuCpDH_Nets(nets);
  AtomicFetchAdd(&n_tot, n_units_flat);
}

void NetTask_Cuda::ComputeAct() {
  cuComputeActs(acts);
/*  
  my_act = 0.0f;
  for (int i = 0; i < n_units_flat; i++) {
    acts[i] = 1.0f / (1.0f + expf(-nets[i]));
    my_act += acts[i];
  }*/
  cuCpHD_Acts(Unit::g_acts);
}



//////////////////////////////////
//  NbCuda			//
//////////////////////////////////


void NbCuda::ParseCmdLine(int& rval) {
  inherited::ParseCmdLine(rval);
  
  // confirm that RECV is correct for algo if using CUDA
  if ( (NetEngine::algo & NetEngine::CUDA_FLAG) &&
    ((sndcn && !(NetEngine::algo & NetEngine::SEND_FLAG)) ||
    (!sndcn && (NetEngine::algo & NetEngine::SEND_FLAG)))
  ) {
    rval = 1;
    cerr << "RECV must be compiled w/o SEND_CONS and vice versa\n";
    return;
  }
  
//TEMP SEND tba
  if (NetEngine::algo != NetEngine::RECV_CUDA) {
    rval = 1;
    cerr << "SEND_CUDA not supported yet\n";
    return;
  }
}

NetEngine* NbCuda::CreateNetEngine() {
  if (!(NetEngine::algo & NetEngine::CUDA_FLAG))

  switch (NetEngine::algo) {
  case NetEngine::RECV_CUDA:
    return new CudaNetEngine_Recv;
  //TODO case NetEngine::SEND_CUDA:
    //return new CudaNetEngine_Send;
  default: break; // compiler food
  }
  return inherited::CreateNetEngine();
}  


