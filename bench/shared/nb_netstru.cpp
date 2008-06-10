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

Unit::Unit() {
  act = 0; 
  send_wts = NULL; 
  cs = NULL; 
  task_id = 0;
  net = 0;
  my_rand = rand();
#ifdef USE_RECV_SMART
  do_delta = true; // for first iteration, before it is set in ComputeActs
#endif
}

Unit::~Unit() {
  delete[] send_wts;
}

bool Unit::DoDelta() {
#ifndef USE_RECV_SMART
  bool
#endif
  do_delta = ((Nb::this_rand ^ my_rand) & 0xffff) < Nb::send_act;
  return do_delta;
} 

// construct the network

int Layer::un_to_idx;

Layer::Layer() {
}

Layer::~Layer() {
  while (units.size > 0) 
    delete units.FastEl(--(units.size));
}

void Layer::Connect(Layer* lay_to) {
  for (int u_fr = 0; u_fr < this->units.size; u_fr++) {
    Unit* un_fr = units.FastEl(u_fr);
    for (int con = 0; con < Nb::n_cons; con++) {
      Unit* un_to = lay_to->units.FastEl(un_to_idx);
      un_fr->targs.Set(un_to, un_fr->targs.size);
      Connection& cn = un_fr->send_wts[un_fr->targs.size++];
      cn.wt = (4.0 * (float)rand() / RAND_MAX) - 2.0;
#ifdef USE_VAL
      cn.val = &(un_to->net);
#endif      
      if (++un_to_idx >= Nb::n_units) un_to_idx = 0;
    }
  }
}


// delete the network


bool Network::recv_based; // true for recv, false for send
bool Network::recv_smart; 

Network::Network() {
}

Network::~Network() {
  for(int l=0;l<Nb::n_layers;l++) {
    Layer* lay = layers[l];
    delete lay;
  }
}

void Network::Build() {
  layers.Alloc(Nb::n_layers);
  Network::n_units_flat = Nb::n_units * Nb::n_layers;
  units_flat.Alloc(n_units_flat);
  
  ConSpec* cs = new ConSpec; // everyone shares
  // make all layers and units first
  for (int l = 0; l < Nb::n_layers; l++) {
    Layer* lay = new Layer;
    layers.Set(lay, layers.size++);
    lay->units.Alloc(Nb::n_units);
    
    for (int i=0;i<Nb::n_units;i++) {
      Unit* un = new Unit;
      un->send_wts = new Connection[Nb::n_cons * 2];
      un->targs.Alloc(Nb::n_cons * 2);
      
      lay->units.Set(un, lay->units.size++);
      un->task_id = units_flat.size % Nb::n_procs;
      units_flat.Set(un, units_flat.size++);
      un->cs = cs;
    } 
  }
   
  // then connect all bidirectionally
  for (int lay_ths = 0; lay_ths < (Nb::n_layers); lay_ths++) {
    int lay_nxt = lay_ths + 1; 
    if (lay_nxt >= Nb::n_layers) lay_nxt = 0;
    int lay_prv = lay_ths - 1; 
    if (lay_prv < 0) lay_prv = Nb::n_layers - 1;
    layers[lay_prv]->Connect(layers[lay_ths]);
    layers[lay_ths]->Connect(layers[lay_nxt]);
    if (Nb::n_layers == 2) break; // special case
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
      if (tsk->g_u < Network::n_units_flat)
        tsk->run();
    } else { // need to sync
      QTaskThread* th = (QTaskThread*)Nb::threads[t];
      th->suspend(); // suspending is syncing with completion of loop
    }
  }
}


void Network::ComputeNets() {
  if (recv_based)
    DoProc(NetTask::P_Recv_Netin);
  else
    DoProc(NetTask::P_Send_Netin);
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
  Connection* cns = su->send_wts; // array pointer
#ifdef USE_VAL
  for(int i=0; i<su->targs.size; i++)
    Send_Netin_inner_0(cns[i].wt, cns[i].val, su_act_eff);
#else
  Unit** uns = su->targs.Els(); // unit pointer
  for(int i=0; i<su->targs.size; i++)
    Send_Netin_inner_0(cns[i].wt, &(uns[i]->net), su_act_eff);
#endif
}

void NetTask::Recv_Netin_0(Unit* ru) {
  float ru_net = 0.0f;
  Connection* cns = ru->send_wts; // array pointer
#ifdef USE_VAL
  for(int i=0; i<ru->targs.size; i++)
    ru_net += *(cns[i].val) * cns[i].wt;
#else
  Unit** uns = ru->targs.Els(); // unit pointer
  for(int i=0; i<ru->targs.size; i++)
#ifdef USE_RECV_SMART
    if (uns[i]->do_delta)
#endif
      ru_net += uns[i]->act * cns[i].wt;
#endif
  ru->net = ru_net;
}


void NetTask::Recv_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  while (g_u < Network::n_units_flat) {
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
  while (g_u < Network::n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    ComputeAct_inner(un);
    my_act += un->act;
    ++g_u;
  }
}


void NetTask_0::Send_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  while (g_u < Network::n_units_flat) {
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
  while (my_u < Network::n_units_flat) {
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
  while (my_u < Network::n_units_flat) {
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
  while (my_u < Network::n_units_flat) {
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
int Network::n_units_flat; // total number of units (flattened, all layers)
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

