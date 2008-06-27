#ifndef NB_NETSTRU_CUDA_H
#define NB_NETSTRU_CUDA_H

#include "nb_netstru.h"
#include "nb_cuda_lib.h"


class CudaNetEngine: public NetEngine {
INHERITED(NetEngine) 
public:
  
  override void		OnBuild(); 
    
//  override void 	Log(bool hdr); // save a log file
  //CudaNetEngine();
  //~CudaNetEngine();
protected:
  override void		Initialize_impl();
//  override void 	DoProc(int proc_id);
//  override void 	ComputeNets_impl();
//  void 			ComputeNets_SendArray();
};


class CudaNetEngine_Recv: public CudaNetEngine {
INHERITED(CudaNetEngine) 
public:
  
//  override void		OnBuild(); 
    
//  override void 	Log(bool hdr); // save a log file
  //CudaNetEngine();
 // ~CudaNetEngine();
protected:
//  override void		Initialize_impl();
//  override void 	DoProc(int proc_id);
//  override void 	ComputeNets_impl();
//  void 			ComputeNets_SendArray();
};

class NetTask_Cuda: public NetTask {
// for CUDA approach -- optimum for that
public:
  static bool	GetCon(int un_idx, int con_idx, int* snd_idx, float* wt);
    // callback for net building
 
  void		Send_Netin();
  void		Recv_Netin();
  void		ComputeAct();
};

class NbCuda: public Nb {
INHERITED(Nb)
public:
  NbCuda(int argc_, char** argv_):inherited(argc_, argv_) {}
protected:
  override void		ParseCmdLine(int& rval);
  override NetEngine*	CreateNetEngine();
};

#endif
