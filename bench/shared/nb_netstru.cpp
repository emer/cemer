#include "nb_netstru.h"

#include <math.h>
#include <stdio.h>


/*void ConSpec::C_Send_Netin(void*, Connection* cn, Unit* ru, float su_act_eff) {
  ru->net += su_act_eff * cn->wt;
}
void ConSpec::Send_Netin(Unit* cg, Unit* su) {
  float su_act_eff = su->act;
  for(int i=0; i<cg->units.size; i++) \
    C_Send_Netin(NULL, cg->Cn(i), cg->Un(i), su_act_eff);
}*/

//////////////////////////
//  RecvCons		//
//////////////////////////


//////////////////////////
//  Unit		//
//////////////////////////

Unit::Unit() {
  act = 0; 
  cs = NULL; 
  task_id = 0;
  n_recv_cons = 0;
  net = 0;
  my_rand = rand();
#ifdef USE_RECV_SMART
  do_delta = true; // for first iteration, before it is set in ComputeActs
#endif
}

Unit::~Unit() {
}

bool Unit::DoDelta() {
#ifndef USE_RECV_SMART
  bool
#endif
  do_delta = ((Nb::this_rand ^ my_rand) & 0xffff) < Nb::send_act;
  return do_delta;
} 


//////////////////////////
//  Layer		//
//////////////////////////

Layer::Layer() {
  un_to_idx = 0;
}

Layer::~Layer() {
}

void Layer::ConnectFrom(Layer* lay_fm) {
  const int n_send = lay_fm->units.size;
  if (n_send == 0) return; // shouldn't happen
  const int n_recv = this->units.size;  
  // get a unit to determine sending gp
  Unit* un = lay_fm->units.FastEl(0);
  const int send_idx = un->send.size; // index of new send gp...

  for (int i_to = 0; i_to < units.size; ++i_to) {
    Unit* un_to = units.FastEl(i_to);
    const int recv_idx = un_to->recv.size; // index of new recv gp...
    RecvCons* recv_gp = un_to->recv.New();
    recv_gp->cons.SetSize(n_send);
    recv_gp->units.SetSize(n_send);
    recv_gp->send_idx = send_idx;
    
    for (int i_fm = 0; i_fm < lay_fm->units.size; ++i_fm) {
      Unit* un_fm = lay_fm->units.FastEl(i_fm);
      SendCons* send_gp;
      if (i_to == 0) {
        send_gp = un_fm->send.New();
        send_gp->cons.SetSize(n_recv);
        send_gp->units.SetSize(n_recv);
        send_gp->recv_idx = recv_idx;
      } else {
        send_gp = un_fm->send.FastEl(send_idx);
      }
      
      Connection& cn = recv_gp->cons[i_fm];
      cn.wt = (4.0 * (float)rand() / RAND_MAX) - 2.0;
      recv_gp->units.Set(un_fm, i_fm);
      
      send_gp->cons.Set(&cn, i_to);
      send_gp->units.Set(un_to, i_to);
    }
  }
}

//////////////////////////
//  Network		//
//////////////////////////

bool Network::recv_smart; 
int Network::algo = 0;

Network::Network() {
}

Network::~Network() {
}

void Network::Build() {
  layers.Alloc(Nb::n_layers);
  const int n_units_flat = Nb::n_layers * Nb::n_units;
  units_flat.Alloc(n_units_flat);
  
  ConSpec* cs = new ConSpec; // everyone shares
  // make all layers and units first
  for (int l = 0; l < Nb::n_layers; l++) {
    Layer* lay = layers.New();
    lay->units.Alloc(Nb::n_units);
    
    for (int i=0;i<Nb::n_units;i++) {
      Unit* un = lay->units.New();
      units_flat.Add(un);
      un->cs = cs;
    } 
  }
   
  // then connect all bidirectionally
  for (int lay_ths = 0; lay_ths < (Nb::n_layers); lay_ths++) {
    int lay_nxt = lay_ths + 1; 
    if (lay_nxt >= Nb::n_layers) lay_nxt = 0;
    int lay_prv = lay_ths - 1; 
    if (lay_prv < 0) lay_prv = Nb::n_layers - 1;
    layers[lay_ths]->ConnectFrom(layers[lay_prv]);
    layers[lay_nxt]->ConnectFrom(layers[lay_ths]);
    if (Nb::n_layers == 2) break; // special case
  }
  
  // then partition for multi-threading, if applicable
  if (Nb::n_procs > 0) {
    switch (algo) {
    case RECV: PartitionUnits_RoundRobin(); break;
    default:  break;
    }
  }
}

void DoProc(int proc_id) {
  // start all the other threads first...
  // have to suspend then resume in case not finished from last time
  for (int t = 1; t < Nb::n_procs; ++t) {
    QTaskThread* th = (QTaskThread*)Nb::threads[t];
    NetTask* tsk = Nb::netin_tasks[t];
    th->suspend(); // prob already suspended
    tsk->g_u = t;
    tsk->proc_id = proc_id;
    th->resume();
  }
  
  // then do my part
  NetTask* tsk = Nb::netin_tasks[0];
  tsk->g_u = 0;
  tsk->proc_id = proc_id;
  tsk->run_time.Start(false);
  tsk->run();
  tsk->run_time.Stop(); // TODO: should add NIBBLE timer!
  // then lend a "helping hand"
  for (int t = 1; t < Nb::n_procs; ++t) {
    if (Nb::nibble) {
      NetTask* tsk = Nb::netin_tasks[t];
      // note: its ok if tsk finishes between our test and calling run
      if (tsk->g_u < Nb::net.units_flat.size)
        tsk->run();
    } else { // need to sync
      QTaskThread* th = (QTaskThread*)Nb::threads[t];
      th->suspend(); // suspending is syncing with completion of loop
    }
  }
}


void Network::ComputeNets() {
  switch (algo) {
  case RECV:
    DoProc(NetTask::P_Recv_Netin);
    break;
  case SEND_CLASH:
    DoProc(NetTask::P_Send_Netin);
    break;
  default: break;
  }
}

float Network::ComputeActs() {
  // compute activations (only order number of units)
  Nb::tot_act = 0.0f;
  DoProc(NetTask::P_ComputeAct);
  for (int t = 0; t < Nb::n_procs; ++t) {
    NetTask* tsk = Nb::netin_tasks[t];
    Nb::tot_act += tsk->my_act;
  }
  Nb::this_rand = rand(); // for next cycle
  return Nb::tot_act;
}

void Network::PartitionUnits_RoundRobin() {
  // we just partition them round-robin
  for (int i = 0; i < units_flat.size; ++i) {
    Unit* un = units_flat[i];
    un->task_id = i % Nb::n_procs;
  }
}

/*void Network::PartitionUnits_Send() {
}*/


//////////////////////////////////
// NetTask			//
//////////////////////////////////

NetTask::NetTask() {
  g_u = 0;
  t_tot = 0;
  my_act = 0.0f;
}

void NetTask::run() {
  switch (proc_id) {
  case P_Send_Netin: Send_Netin(); break;
  case P_Recv_Netin: Recv_Netin(); break;
  case P_ComputeAct: ComputeAct(); break;
  }
}

void NetTask::Send_Netin_0(Unit* su) {
  float su_act_eff = su->act;
  for (int j = 0; j < su->send.size; ++j) {
    SendCons* send_gp = su->send[j];
    Connection** cns = send_gp->cons.Els(); // conn pointer
    Unit** uns = send_gp->units.Els(); // unit pointer
    for (int i=0; i < send_gp->units.size; i++)
      Send_Netin_inner_0(cns[i]->wt, &(uns[i]->net), su_act_eff);
  }
}

void NetTask::Recv_Netin_0(Unit* ru) {
  float ru_net = 0.0f;
  for (int j = 0; j < ru->recv.size; ++j) {
    RecvCons* recv_gp = ru->recv[j];
    Connection* cns = &(recv_gp->cons[0]); // array pointer
    Unit** uns = recv_gp->units.Els(); // unit pointer
    for(int i=0; i < recv_gp->units.size; ++i)
#ifdef USE_RECV_SMART
      if (uns[i]->do_delta)
#endif
        ru_net += uns[i]->act * cns[i].wt;
  }
  ru->net = ru_net;
}


void NetTask::Recv_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  const int n_units_flat =  Nb::net.units_flat.size;
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    Recv_Netin_0(un);
    AtomicFetchAdd(&Nb::n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
      //++t_tot;
    ++g_u;
  }
}

void NetTask::ComputeAct_inner(Unit* un) {
  un->act = 1.0f / (1.0f + expf(-un->net));
  un->net = 0.0f; // only needed for sender-based, but cheaper to just do than test
#ifdef USE_RECV_SMART
  if (Network::recv_smart) un->DoDelta(); // sets flag
#endif
}

void NetTask::ComputeAct() {
  Unit** units = Nb::net.units_flat.Els();
  // compute activations (only order number of units)
  my_act = 0.0f;
  const int n_units_flat =  Nb::net.units_flat.size;
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    ComputeAct_inner(un);
    my_act += un->act;
    ++g_u;
  }
}


void NetTask_0::Send_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  const int n_units_flat =  Nb::net.units_flat.size;
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    if (un->DoDelta()) {
      Send_Netin_0(un);
      AtomicFetchAdd(&Nb::n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
      //++t_tot;
    }
    ++g_u;
  }
}

void NetTask_N::Send_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, Nb::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = units[my_u]; //note: accessed flat
    if (un->DoDelta()) {
      Send_Netin_0(un);
      AtomicFetchAdd(&Nb::n_tot, 1);
      //AtomicFetchAdd(&t_tot, 1); // because of helping hand clobbers
    }
    my_u = AtomicFetchAdd(&g_u, Nb::n_procs);
  }
}

void NetTask_N::Recv_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, Nb::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = units[my_u]; //note: accessed flat
    Recv_Netin_0(un);
    AtomicFetchAdd(&Nb::n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
      //++t_tot;
    my_u = AtomicFetchAdd(&g_u, Nb::n_procs);
  }
}

void NetTask_N::ComputeAct() {
  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, Nb::n_procs);
  my_act = 0.0f;
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = units[my_u];
    ComputeAct_inner(un);
    my_act += un->act;
    my_u = AtomicFetchAdd(&g_u, Nb::n_procs);
  }
}


bool Nb::hdr = false;
int Nb::n_procs;		// total number of processors
const int Nb::core_max_nprocs; // maximum number of processors!
NetTask* Nb::netin_tasks[core_max_nprocs]; // only n_procs created
QThread* Nb::threads[core_max_nprocs]; // only n_procs-1 created, none for [0] (main thread)

int Nb::n_layers;
int Nb::n_units;			// number of units per layer
int Nb::n_cons; // number of cons per unit
int Nb::n_cycles;			// number of cycles of updating
Network Nb::net;		// global network 
int Nb::n_tot; // total units (reality check)
float Nb::tot_act; // check on tot act
bool Nb::nibble = true; // setting false disables nibbling and adds sync to loop
bool Nb::single = false; // true for single thread mode, to compare against nprocs=1
int Nb::send_act = 0x10000; // send activation, as a fraction of 2^16 
int Nb::this_rand; // assigned a new random value each cycle, to let us randomize unit acts
bool Nb::calc_act = true;


void Nb::DeleteThreads() {
  for (int t = n_procs - 1; t >= 1; t--) {
    QTaskThread* th = (QTaskThread*)threads[t];
    if (th->isActive()) {
      th->terminate();
    }
    while (!th->isFinished());
    delete th;
  }
}

void Nb::MakeThreads() {
  if (Nb::single) {
    NetTask* tsk = new NetTask_0;
    tsk->task_id = 0;
    Nb::netin_tasks[0] = tsk;
  } else for (int i = 0; i < Nb::n_procs; i++) {
    NetTask* tsk = new NetTask_N;
    tsk->task_id = i;
    Nb::netin_tasks[i] = tsk;
    if (i == 0) continue;
    QTaskThread* th = new QTaskThread;
    Nb::threads[i] = th;
    th->setTask(tsk);
    th->start(); // starts paused
  }
}

