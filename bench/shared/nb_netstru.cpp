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
  flat_idx = -1;
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

Network::Network() {
  engine = NULL;
}

Network::~Network() {
  SetEngine(NULL);
}

void Network::Initialize() {
  if (engine) engine->Initialize();
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
      un->flat_idx = units_flat.size; // next index
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
  
  engine->OnBuild();
/*  // then partition for multi-threading, if applicable
  if (Nb::n_procs > 0) {
    switch (algo) {
    case RECV:
    case SEND_CLASH:
      PartitionUnits_RoundRobin(); break;
    default:  break;
    }
  }*/
}


void Network::ComputeNets() {
  engine->ComputeNets();
}

float Network::ComputeActs() {
  float rval = engine->ComputeActs();
  Nb::this_rand = rand(); // for next cycle
  return rval;
}

void Network::SetEngine(NetEngine* engine_) {
  if (engine == engine_) return;
  if (engine) {
    delete engine; // removes ref
  }
  engine = engine_;
  if (engine) {
    engine->net = this;
  }
}



/*void Network::PartitionUnits_Send() {
}*/

//////////////////////////
//  NetEngine		//
//////////////////////////

int NetEngine::algo = 0;
const int NetEngine::core_max_nprocs; // maximum number of processors!
int NetEngine::n_procs;		// total number of processors
NetTaskList NetEngine::net_tasks; // only n_procs created

NetEngine::~NetEngine() {
  if (net) {
    net->engine = NULL;
  }
}

void NetEngine::Initialize() {
  NetTask* tsk = new NetTask_0;
  tsk->task_id = 0;
  net_tasks.Add(tsk);
}

void NetEngine::DoProc(int proc_id) {
  NetTask* tsk = net_tasks.SafeEl(0);
  //if (!tsk) return;
  tsk->g_u = 0;
  tsk->proc_id = proc_id;
  tsk->run();
}

void NetEngine::ComputeNets() {
  switch (algo) {
  case RECV:
  case RECV_SMART:
    DoProc(NetTask::P_Recv_Netin);
    break;
  case SEND_CLASH:
  case SEND_ARRAY: // ignore for single, trapped for N
    DoProc(NetTask::P_Send_Netin_Clash);
    break;
  default: break;
  }
}



float NetEngine::ComputeActs() {
  // compute activations (only order number of units)
  Nb::tot_act = 0.0f;
  DoProc(NetTask::P_ComputeAct);
  for (int t = 0; t < net_tasks.size; ++t) {
    NetTask* tsk = net_tasks[t];
    Nb::tot_act += tsk->my_act;
  }
  return Nb::tot_act;
}

//////////////////////////////////
//  ThreadNetEngine		//
//////////////////////////////////

QThread* ThreadNetEngine::threads[core_max_nprocs]; // only n_procs-1 created, none for [0] (main thread)

ThreadNetEngine::~ThreadNetEngine() {
  DeleteThreads();
}

void ThreadNetEngine::Initialize() {
  net_tasks.Alloc(n_procs);
  for (int i = 0; i < n_procs; i++) {
    NetTask* tsk = new NetTask_N;
    tsk->task_id = i;
    net_tasks.Add(tsk);
    if (i == 0) continue;
    QTaskThread* th = new QTaskThread;
    threads[i] = th;
    th->setTask(tsk);
    th->start(); // starts paused
  }
  
}

void ThreadNetEngine::DeleteThreads() {
  for (int t = n_procs - 1; t >= 1; t--) {
    QTaskThread* th = (QTaskThread*)threads[t];
    if (th->isActive()) {
      th->terminate();
    }
    while (!th->isFinished());
    delete th;
  }
}
void ThreadNetEngine::ComputeNets() {
  if (algo == SEND_ARRAY)
    ComputeNets_SendArray();
  else inherited::ComputeNets();
}



void ThreadNetEngine::DoProc(int proc_id) {
  const int n_units_flat = net->units_flat.size;
  const int n_nibb_thresh = (int)(n_units_flat * Nb::nibble_thresh);
  // start all the other threads first...
  // have to suspend then resume in case not finished from last time
  for (int t = 1; t < n_procs; ++t) {
    QTaskThread* th = (QTaskThread*)threads[t];
    NetTask* tsk = net_tasks[t];
    th->suspend(); // prob already suspended
    tsk->g_u = t;
    tsk->proc_id = proc_id;
    th->resume();
  }
  
  // then do my part
  NetTask* tsk = net_tasks[0];
  tsk->g_u = 0;
  tsk->proc_id = proc_id;
  tsk->run_time.Start(false);
  tsk->run();
  tsk->run_time.Stop(); 
  // then either sync or "nibble"
  for (int t = 1; t < n_procs; ++t) {
    bool nibble = false;
    switch (Nb::nibble_mode) {
    case 0: nibble = false; break;
    case 1: nibble = true; break;
    default: // 2: auto, only if 
      nibble = (tsk->g_u < n_nibb_thresh); break;
    }
    if (nibble) {
      NetTask* tsk = net_tasks[t];
      // note: its ok if tsk finishes between our test and calling run
      if (tsk->g_u < n_units_flat) {
        tsk->nibble_time.Start(false);
        tsk->run();
        tsk->nibble_time.Stop(); 
      }
    } else { // sync
      tsk->sync_time.Start(false);
      QTaskThread* th = (QTaskThread*)threads[t];
      th->suspend(); // suspending is syncing with completion of loop
      tsk->sync_time.Stop(); 
    }
  }
}

void ThreadNetEngine::Log(bool hdr) {
  // note: always make/extend the thread log
  if (true/*use_log_file*/) {
  FILE* logfile = NULL;
    if (hdr) {
      logfile = fopen("nb_thread.log", "w");
      fprintf(logfile,"\nthread\tt_tot\tstart lat\trun time\tnibble time\tsync time\n");
    } else {
      logfile = fopen("nb_thread.log", "a");
    }
    for (int t = 0; t < net_tasks.size; t++) {
      NetTask* tsk = net_tasks[t];
      fprintf(logfile,"%d\t%d\t%g\t%g\t%g\t%g\n", 
        t, 
        tsk->t_tot, 
        tsk->start_latency.s_used, 
        tsk->run_time.s_used,
        tsk->nibble_time.s_used,
        tsk->sync_time.s_used
      );
    }
    fclose(logfile);
  }
}

void ThreadNetEngine::OnBuild() {
  // we just partition them round-robin
  const int n_flat_units = net->units_flat.size;
  for (int i = 0; i < n_flat_units; ++i) {
    Unit* un = net->units_flat[i];
    un->task_id = i % n_procs;
  }
  // Send_Array only
  if (algo == SEND_ARRAY) {
    for (int t = 0; t < net_tasks.size; t++) {
      NetTask_N* tsk = dynamic_cast<NetTask_N*>(net_tasks[t]);
      tsk->excit.SetSize(n_flat_units);
    }
  }
}

void ThreadNetEngine::ComputeNets_SendArray() {
  DoProc(NetTask::P_Send_Netin_Array);

  // post stuff
//  RollupWritebackScratch_Netin();
  const int n_flat_units = net->units_flat.size;
  for (int i = 0; i < n_flat_units; ++i) {
    Unit* un = net->units_flat[i];
    un->net = 0.0f;
    for (int t = 0; t < net_tasks.size; t++) {
      NetTask_N* tsk = dynamic_cast<NetTask_N*>(net_tasks[t]);
      un->net += tsk->excit[i];
    }
  }
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
  case P_Recv_Netin: Recv_Netin(); break;
  case P_Send_Netin_Clash: Send_Netin_Clash(); break;
  case P_Send_Netin_Array: Send_Netin_Array(); break;
  case P_ComputeAct: ComputeAct(); break;
  }
}

void NetTask::Send_Netin_0(Unit* su) {
  float su_act_eff = su->act;
  for (int j = 0; j < su->send.size; ++j) {
    SendCons* send_gp = su->send[j];
    Connection** cns = send_gp->cons.Els(); // conn pointer
    Unit** uns = send_gp->units.Els(); // unit pointer
    const int send_sz = send_gp->units.size;
    for (int i=0; i < send_sz; i++)
      Send_Netin_inner_0(cns[i]->wt, &(uns[i]->net), su_act_eff);
  }
}

void NetTask::Recv_Netin_0(Unit* ru) {
  float ru_net = 0.0f;
  for (int j = 0; j < ru->recv.size; ++j) {
    RecvCons* recv_gp = ru->recv[j];
    Connection* cns = &(recv_gp->cons[0]); // array pointer
    Unit** uns = recv_gp->units.Els(); // unit pointer
    const int recv_sz = recv_gp->units.size;
    for(int i=0; i < recv_sz; ++i)
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


void NetTask_0::Send_Netin_Clash() {
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

void NetTask_N::Send_Netin_Clash() {
  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = units[my_u]; //note: accessed flat
    if (un->DoDelta()) {
      Send_Netin_0(un);
      AtomicFetchAdd(&Nb::n_tot, 1);
      //AtomicFetchAdd(&t_tot, 1); // because of helping hand clobbers
    }
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}

void NetTask_N::Send_Netin_Array() {
  memset(excit.el, 0, sizeof(float) * excit.size);

  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* su = units[my_u]; //note: accessed flat
    if (su->DoDelta()) {
      float su_act_eff = su->act;
      for (int j = 0; j < su->send.size; ++j) {
        SendCons* send_gp = su->send[j];
        Connection** cns = send_gp->cons.Els(); // conn pointer
        Unit** uns = send_gp->units.Els(); // unit pointer
        const int send_sz = send_gp->units.size;
        for (int i=0; i < send_sz; i++) {
          int targ_i = uns[i]->flat_idx;
          Send_Netin_inner_0(cns[i]->wt, &(excit.el[targ_i]), su_act_eff);
        }
      }
      AtomicFetchAdd(&Nb::n_tot, 1);
      //AtomicFetchAdd(&t_tot, 1); // because of helping hand clobbers
    }
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}

void NetTask_N::Recv_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = units[my_u]; //note: accessed flat
    Recv_Netin_0(un);
    AtomicFetchAdd(&Nb::n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
      //++t_tot;
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}

void NetTask_N::ComputeAct() {
  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  my_act = 0.0f;
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = units[my_u];
    ComputeAct_inner(un);
    my_act += un->act;
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}


bool Nb::hdr = false;

int Nb::n_layers;
int Nb::n_units;			// number of units per layer
int Nb::n_cons; // number of cons per unit
int Nb::n_cycles;			// number of cycles of updating
Network Nb::net;		// global network 
int Nb::n_tot; // total units (reality check)
float Nb::tot_act; // check on tot act
float Nb::nibble_thresh = 0.8f;
signed char Nb::nibble_mode = 0;
bool Nb::single = false; // true for single thread mode, to compare against nprocs=1
int Nb::send_act = 0x10000; // send activation, as a fraction of 2^16 
int Nb::this_rand; // assigned a new random value each cycle, to let us randomize unit acts
bool Nb::calc_act = true;

