#include "nb_netstru.h"

#include <math.h>
#include <stdio.h>
#include <iostream>

#include <QtCore/QFileInfo>

using namespace std;

/*void ConSpec::C_Send_Netin(void*, Conn* cn, Unit* ru, float su_act_eff) {
  ru->net += su_act_eff * cn->wt;
}
void ConSpec::Send_Netin(Unit* cg, Unit* su) {
  float su_act_eff = su->act;
  for(int i=0; i<cg->units.size; i++) \
    C_Send_Netin(NULL, cg->Cn(i), cg->Un(i), su_act_eff);
}*/

//////////////////////////
//  Conn		//
//////////////////////////

float* 		Conn::g_wts;
int		Conn::g_next_wti;

//////////////////////////
//  RecvCons		//
//////////////////////////

RecvCons::RecvCons() {
  send_idx = -1; 
  send_lay = NULL;
  dwt_mean = 0.0f;
}


//////////////////////////
//  ConSpec		//
//////////////////////////

ConSpec:: ConSpec() {
  cur_lrate = 0.01f;
  norm_pct = 1.0f;
  wt_limits.max = 1.0f;
}

void ConSpec::Compute_SRAvg(Unit* ru) {
  for(int g = 0; g < ru->recv.size; g++) {
    RecvCons* cg = ru->recv.FastEl(g);
    if (cg->cons.size <= 0) continue;
    for (int i = 0; i < cg->cons.size; ++i) {
      Conn* cn = cg->Cn(i);
      cn->sravg += ru->act() * Network::g_units[cg->units[i]]->act();
    }
  }
}

#ifdef SEND_CONS
void ConSpec::Compute_Weights(Unit* u) {
}
#else

static int ir;

void ConSpec::Compute_Weights_CtCAL(RecvCons* cg, Unit* /*ru*/) {
//  UnitSpec* rus = (UnitSpec*)ru->spec;
  
  //Compute_dWtMean(cg, ru);
  cg->dwt_mean = 0.0f;
  for (int i = 0; i < cg->cons.size; ++i) {
    Conn* cn = &(cg->cons[i]);
    cg->dwt_mean += cn->dwt;
  }
  cg->dwt_mean /= (float)cg->cons.size;
  
  float dwnorm = -norm_pct * cg->dwt_mean;
    
  for (int i = 0; i < cg->cons.size; ++i) {
    Conn* cn = &(cg->cons[i]);
    float& cn_wt = cn->wt();
    
    // C_Compute_ActReg_CtLeabraCAL(cn, cg, ru, su, rus);
    float dwinc = ((ir^i)  & 1) ? -0.2f : 0.2f; //0.0f; hack
   /* if(ru->act_avg <= rus->act_reg.min)
      dwinc = rus->act_reg.inc_wt;
    else if(ru->act_avg >= rus->act_reg.max)
      dwinc = -rus->act_reg.dec_wt;*/
    if(dwinc != 0.0f) {
      cn->dwt += cur_lrate * dwinc * cn_wt; // proportional to current weights
    }
    
    // C_Compute_Weights_Norm_CtLeabraCAL(cn, dwnorm);
    cn_wt += cn->dwt + dwnorm;	// weights always linear
    wt_limits.ApplyMinLimit(cn_wt); wt_limits.ApplyMaxLimit(cn_wt);
    cn->pdw = cn->dwt;
    cn->dwt = 0.0f;
  }
}

void ConSpec::Compute_Weights(Unit* u) {
  // CTLEABRA_CAL, DCAL
  ir = rand();
  for(int g = 0; g < u->recv.size; g++) {
    RecvCons* recv_gp = (RecvCons*)u->recv.FastEl(g);
    if (recv_gp->cons.size <= 0) continue;
    Compute_Weights_CtCAL(recv_gp, u);
  }
}
#endif // !SEND_CONS

//////////////////////////
//  UnitSpec		//
//////////////////////////

UnitSpec::UnitSpec() {
//  act_reg = 0.0f;
}

//////////////////////////
//  Unit		//
//////////////////////////

float* 		Unit::g_acts;
float* 		Unit::g_nets;

Unit::Unit() {
  //act = 0; 
  //net = 0;
  act_avg = 0.0f;
  cs = NULL; 
  spec = NULL;
  task_id = 0;
  flat_idx = -1;
  n_recv_cons = 0;
  my_rand = rand();
  do_delta = true; // for first iteration, before it is set in ComputeActs
  n_con_calc = 0;
}

Unit::~Unit() {
}

void Unit::CalcDelta() {
  do_delta = ((Nb::this_rand ^ my_rand) & 0xffff) < Nb::send_act;
} 


//////////////////////////
//  Layer		//
//////////////////////////

Layer::Layer() {
  net = NULL;
  un_to_idx = 0;
}

Layer::~Layer() {
}


#ifdef SEND_CONS
void Layer::ConnectTo(Layer* lay_to) {
  Nb::n_prjns++;
  const int n_recv = lay_to->units.size;
  if (n_recv == 0) return; // shouldn't happen
  const int n_send = this->units.size;  
  // get a unit to determine receiving gp
  Unit* un = lay_to->units.FastEl(0);
  const int recv_idx = un->recv.size; // index of new recv gp...

  for (int i_fm = 0; i_fm < units.size; ++i_fm) {
    Unit* un_fm = units.FastEl(i_fm);
    const int send_idx = un_fm->send.size; // index of new send gp...
    SendCons* send_gp = un_fm->send.New();
    send_gp->cons.SetSize(n_recv);
    send_gp->units.SetSize(n_recv);
    send_gp->recv_idx = recv_idx;
    send_gp->recv_lay = lay_to;
    
    for (int i_to = 0; i_to < lay_to->units.size; ++i_to) {
      Unit* un_to = lay_to->units.FastEl(i_to);
      RecvCons* recv_gp;
      if (i_fm == 0) {
        recv_gp = un_to->recv.New();
        recv_gp->cons.SetSize(n_send);
        recv_gp->units.SetSize(n_send);
        recv_gp->send_idx = send_idx;
        recv_gp->send_lay = this;
      } else {
        recv_gp = un_to->recv.FastEl(recv_idx);
      }
      
      Conn* cn = &(send_gp->cons[i_to]);
      cn->wti = Conn::g_next_wti++;
      cn->wt() = (4.0 * (float)rand() / RAND_MAX) - 2.0;
      send_gp->units.Set(un_to->flat_idx, i_to);
      
      recv_gp->cons.Set(cn, i_fm);
      recv_gp->units.Set(un_fm->flat_idx, i_fm);
    }
  }
}
#else // RECV_CONS
void Layer::ConnectFrom(Layer* lay_fm) {
  if (Network::recv_based)
    ConnectFrom_Recv(lay_fm);
  else
    ConnectFrom_Send(lay_fm);
}

void Layer::ConnectFrom_Recv(Layer* lay_fm) {
  Nb::n_prjns++;
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
    recv_gp->send_lay = lay_fm;
    
    for (int i_fm = 0; i_fm < lay_fm->units.size; ++i_fm) {
      Unit* un_fm = lay_fm->units.FastEl(i_fm);
      SendCons* send_gp;
      if (i_to == 0) {
        send_gp = un_fm->send.New();
        send_gp->cons.SetSize(n_recv);
        send_gp->units.SetSize(n_recv);
        send_gp->recv_idx = recv_idx;
        send_gp->recv_lay = this;
      } else {
        send_gp = un_fm->send.FastEl(send_idx);
      }
      
      Conn* cn = &(recv_gp->cons[i_fm]);
      cn->wti = Conn::g_next_wti++;
      cn->wt() = (4.0 * (float)rand() / RAND_MAX) - 2.0;
      recv_gp->units.Set(un_fm->flat_idx, i_fm);
      
      send_gp->cons.Set(cn, i_to);
      send_gp->units.Set(un_to->flat_idx, i_to);
    }
  }
}
/*
Note:
  This routine causes the Conns in the sender to be ordered
  by receiver -- i.e., the wts array
*/

void Layer::ConnectFrom_Send(Layer* lay_fm) {
  Nb::n_prjns++;
  const int n_send = lay_fm->units.size;
  if (n_send == 0) return; // shouldn't happen
  const int n_recv = this->units.size;  
  // get a unit to determine sending gp
  Unit* un = this->units.FastEl(0);
  const int recv_idx = un->recv.size; // index of new recv gp...
  un = lay_fm->units.FastEl(0);
  const int send_idx = un->send.size; // index of new send gp...
  
  
  for (int i_fm = 0; i_fm < lay_fm->units.size; ++i_fm) {
    Unit* un_fm = lay_fm->units.FastEl(i_fm);
    SendCons* send_gp = un_fm->send.New();
    send_gp->cons.SetSize(n_recv);
    send_gp->units.SetSize(n_recv);
    send_gp->recv_idx = recv_idx;
    send_gp->recv_lay = this;
    
    for (int i_to = 0; i_to < units.size; ++i_to) {
      Unit* un_to = units.FastEl(i_to);
      RecvCons* recv_gp;
      if (i_fm == 0) {
        recv_gp = un_to->recv.New();
        recv_gp->cons.SetSize(n_send);
        recv_gp->units.SetSize(n_send);
        recv_gp->send_idx = send_idx;
        recv_gp->send_lay = lay_fm;
      } else {
        recv_gp = un_to->recv.FastEl(send_idx);
      }
  
      Conn* cn = &(recv_gp->cons[i_fm]);
      cn->wti = Conn::g_next_wti++;
      cn->wt() = (4.0 * (float)rand() / RAND_MAX) - 2.0;
      recv_gp->units.Set(un_fm->flat_idx, i_fm);
      
      send_gp->cons.Set(cn, i_to);
      send_gp->units.Set(un_to->flat_idx, i_to);
    }
  }
}
#endif


//////////////////////////
//  Network		//
//////////////////////////

bool 		Network::recv_based; 
bool 		Network::recv_smart; 
Unit** 		Network::g_units; 
//Conn* 	Network::g_cons; 

Network::Network() {
  engine = NULL;
  cycle = 0;
}

Network::~Network() {
  SetEngine(NULL);
  g_units = NULL;
  Unit::g_acts = NULL;
  Unit::g_nets = NULL;
  Conn::g_wts = NULL;
}

void Network::Initialize() {
  if (engine) engine->Initialize();
}

void Network::Build() {
  const int n_units_flat = Nb::n_layers * Nb::n_units;
  const int n_prjns = (Nb::n_layers - 1) * 2;
  const int n_cons_flat = n_prjns * Nb::n_units * Nb::n_cons;
  
  layers.Alloc(Nb::n_layers);
  // global allocs and ptrs
  units_flat.Alloc(n_units_flat);
  g_units = units_flat.Els();
  acts_flat.SetSize(n_units_flat);
  Unit::g_acts = acts_flat.el;
  nets_flat.SetSize(n_units_flat);
  Unit::g_nets = nets_flat.el;
  
  wts_flat.SetSize(n_cons_flat);
  Conn::g_wts = wts_flat.el;
  
  Conn::g_next_wti = 0;
  ConSpec* cs = new ConSpec; // everyone shares
  UnitSpec* uspec = new UnitSpec; // everyone shares
  // make all layers and units first
  for (int l = 0; l < Nb::n_layers; l++) {
    Layer* lay = layers.New();
    lay->net = this;
    lay->units.Alloc(Nb::n_units);
    
    for (int i=0;i<Nb::n_units;i++) {
      Unit* un = lay->units.New();
      un->flat_idx = units_flat.size; // next index
      units_flat.Add(un);
      un->cs = cs;
      un->spec = uspec;
    } 
  }
   
  // then connect all bidirectionally 
  for (int lay_ths = 0; lay_ths < (Nb::n_layers); lay_ths++) {
    int lay_nxt = lay_ths + 1; 
    //if (lay_nxt >= Nb::n_layers) lay_nxt = 0;
    int lay_prv = lay_ths - 1; 
    //if (lay_prv < 0) lay_prv = Nb::n_layers - 1;
    if (lay_prv >= 0) 
      layers[lay_ths]->ConnectFrom(layers[lay_prv]);
    if (lay_nxt < Nb::n_layers) 
      layers[lay_nxt]->ConnectFrom(layers[lay_ths]);
    if (Nb::n_layers == 2) break; // special case
  }
  
  engine->OnBuild();
}


void Network::ComputeNets() {
  engine->ComputeNets();
}

float Network::ComputeActs() {
  float rval = engine->ComputeActs();
  Nb::this_rand = rand(); // for next cycle
  return rval;
}

void Network::Compute_SRAvg() {
  engine->Compute_SRAvg();
}

void Network::Compute_Weights() {
  engine->Compute_Weights();
}

double Network::GetNTot() {
  double rval = 0;
  for (int i = 0; i < units_flat.size; ++i) {
    Unit* un = units_flat[i];
    rval += un->n_con_calc;
  }
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
  Initialize_impl();
  switch (algo) {
  case RECV:
    NetTask::Recv_Netin_0 = *NetTask::Recv_Netin_0_Dumb;
    break;
  case RECV_SMART:
    NetTask::Recv_Netin_0 = *NetTask::Recv_Netin_0_Smart;
    break;
  default: break;
  }
}

void NetEngine::Initialize_impl() {
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
  Nb::n_tot = 0;
  ComputeNets_impl();
#ifdef DEBUG
  if (algo < RECV_SMART) && (Nb::n_tot != net->units_flat.size)) {
    cerr << "ERROR: NetEngine::ComputeNets: n_tot != n_units, was: "
      << Nb::n_tot << "\n";
  }
#endif
}

void NetEngine::ComputeNets_impl() {
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

void NetEngine::Compute_SRAvg() {
  DoProc(NetTask::P_ComputeSRAvg);
}

void NetEngine::Compute_Weights() {
  DoProc(NetTask::P_ComputeWeights);
}

void NetEngine::Log(bool hdr) {
  // note: always make/extend the thread log
  if (true/*use_log_file*/) {
  FILE* logfile = NULL;
    if (hdr) {
      logfile = fopen("ThreadNetEngine.log", "w");
      fprintf(logfile,"thread\tt_tot\tn_run\tstart lat\trun time\tnibble time\tsync time\toverhead\n");
    } else {
      logfile = fopen("ThreadNetEngine.log", "a");
    }
    for (int t = 0; t < net_tasks.size; t++) {
      NetTask* tsk = net_tasks[t];
      fprintf(logfile,"%d\t%d\t%d\t%g\t%g\t%g\t%g\t%g\n", 
        t, 
        tsk->t_tot,
        tsk->n_run, 
        tsk->start_latency.s_used, 
        tsk->run_time.s_used,
        tsk->nibble_time.s_used,
        tsk->sync_time.s_used,
        tsk->overhead.s_used
      );
    }
    fclose(logfile);
  }
}
//////////////////////////////////
//  ThreadNetEngine		//
//////////////////////////////////

QThread* ThreadNetEngine::threads[core_max_nprocs]; // only n_procs-1 created, none for [0] (main thread)

ThreadNetEngine::~ThreadNetEngine() {
  DeleteThreads();
}

void ThreadNetEngine::Initialize_impl() {
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
void ThreadNetEngine::ComputeNets_impl() {
  if (algo == SEND_ARRAY)
    ComputeNets_SendArray();
  else inherited::ComputeNets_impl();
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

void ThreadNetEngine::OnBuild() {
  // we just partition them round-robin
  const int n_units_flat = net->units_flat.size;
  for (int i = 0; i < n_units_flat; ++i) {
    Unit* un = net->units_flat[i];
    un->task_id = i % n_procs;
  }
  // Send_Array only
  if (algo == SEND_ARRAY) {
    for (int t = 0; t < net_tasks.size; t++) {
      NetTask_N* tsk = dynamic_cast<NetTask_N*>(net_tasks[t]);
      tsk->excit.SetSize(n_units_flat);
    }
  }
}

void ThreadNetEngine::ComputeNets_SendArray() {
  DoProc(NetTask::P_Send_Netin_Array);

  // post stuff
//  RollupWritebackScratch_Netin();
  // called in Thread 0, so allocate the Task0
  NetTask* nt0 = net_tasks[0];
  nt0->overhead.Start(false);
  const int n_units_flat = net->units_flat.size;
  for (int i = 0; i < n_units_flat; ++i) {
    Unit* un = net->units_flat[i];
    //NOTE: un.net was reset to 0.0 in previous Act calc
    float& un_net = un->net();
    for (int t = 0; t < net_tasks.size; t++) {
      NetTask_N* tsk = dynamic_cast<NetTask_N*>(net_tasks[t]);
      un_net += tsk->excit[i];
    }
  }
  nt0->overhead.Stop();
}

//////////////////////////////////
//  NetTask			//
//////////////////////////////////

Recv_Netin_0_t NetTask::Recv_Netin_0 = NULL;

NetTask::NetTask() {
  g_u = 0;
  t_tot = 0;
  my_act = 0.0f;
  n_run = 0;
}

void NetTask::run() {
  ++n_run;
  switch (proc_id) {
  case P_Recv_Netin: Recv_Netin(); break;
  case P_Send_Netin_Clash: Send_Netin_Clash(); break;
  case P_Send_Netin_Array: Send_Netin_Array(); break;
  case P_ComputeAct: ComputeAct(); break;
  case P_ComputeSRAvg: Compute_SRAvg(); break;
  case P_ComputeWeights: Compute_Weights(); break;
  }
}

void NetTask::Send_Netin_0(Unit* su) {
  float su_act_eff = su->act();
  for (int j = 0; j < su->send.size; ++j) {
    SendCons* send_gp = su->send[j];
    const int send_sz = send_gp->units.size;
    if (send_sz == 0) continue; // guard
    su->n_con_calc += send_sz;
    int* uns = send_gp->units.Els(); // unit pointer
    int cni = send_gp->Cn(0)->wti; // first weight
    //note: for RECV_CONS/send-based, we assigned wti's in sender order
#ifdef SEND_CONS
    int cni = send_gp->cons[0].wti; // first weight
    for (int i=0; i < send_sz; i++)
      Send_Netin_inner_0(Conn::g_wts[cni++], Unit::g_nets[uns[i]], su_act_eff);
#else    
    for (int i=0; i < send_sz; i++)
      Send_Netin_inner_0(Conn::g_wts[cni++], Unit::g_nets[uns[i]], su_act_eff);
#endif
  }
}

void NetTask::Recv_Netin_0_Dumb(Unit* ru) {
  float ru_net = 0.0f;
  for (int j = 0; j < ru->recv.size; ++j) {
    RecvCons* recv_gp = ru->recv[j];
    int* uns = recv_gp->units.Els(); // unit pointer
    const int recv_sz = recv_gp->units.size;
    ru->n_con_calc += recv_sz;
#ifdef SEND_CONS
    Conn** cns = recv_gp->cons.Els(); // array pointer
    for(int i=0; i < recv_sz; ++i)
      ru_net += Unit::g_acts[uns[i]] * cns[i]->wt();
#else  
//    Conn* cns = &(recv_gp->cons[0]); // array pointer
    int wti = recv_gp->cons[0].wti; // first con
    for(int i=0; i < recv_sz; ++i)
      ru_net += Unit::g_acts[uns[i]] * Conn::g_wts[wti++];
#endif
  }
  ru->net() = ru_net;
}

void NetTask::Recv_Netin_0_Smart(Unit* ru) {
  float ru_net = 0.0f;
  for (int j = 0; j < ru->recv.size; ++j) {
    RecvCons* recv_gp = ru->recv[j];
    int* uns = recv_gp->units.Els(); // unit pointer
    const int recv_sz = recv_gp->units.size;
    ru->n_con_calc += (recv_sz / Nb::inv_act);
#ifdef SEND_CONS
    Conn** cns = recv_gp->cons.Els(); // array pointer
#else
    Conn* cns = &(recv_gp->cons[0]); // array pointer
#endif
    for(int i=0; i < recv_sz; ++i) {
      const int uni = uns[i];
      if (Network::g_units[uni]->do_delta)
#ifdef SEND_CONS
        ru_net += Unit::g_acts[uns[i]] * cns[i]->wt();
#else
        ru_net += Unit::g_acts[uns[i]] * cns[i].wt();
#endif
    }
  }
  ru->net() = ru_net;
}


void NetTask::Recv_Netin() {
  Unit** units = Nb::net.units_flat.Els();
  const int n_units_flat =  Nb::net.units_flat.size;
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    Recv_Netin_0(un);
    AtomicFetchAdd(&Nb::n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
    ++g_u;
  }
}

float NetTask::ComputeAct_inner(int uni) {
  float& un_net = Unit::g_nets[uni];
  float& un_act = Unit::g_acts[uni];
  un_act = 1.0f / (1.0f + expf(-un_net));
  un_net = 0.0f; // only needed for sender-based, but cheaper to just do than test
  //NOTE: following is not used for vanilla RECV, but we include it
  // as shared overhead anyway
  Nb::net.units_flat[uni]->CalcDelta(); // sets flag, except for RECV
  return un_act;
}

void NetTask::ComputeAct() {
//  Unit** units = Nb::net.units_flat.Els();
  // compute activations (only order number of units)
  my_act = 0.0f;
  const int n_units_flat =  Nb::net.units_flat.size;
  while (g_u < n_units_flat) {
    my_act += ComputeAct_inner(g_u);
    ++g_u;
  }
}

void NetTask::Compute_Weights() {
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = Network::g_units[my_u]; //note: accessed flat
    un->cs->Compute_Weights(un);
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}

void NetTask::Compute_SRAvg() {
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = Network::g_units[my_u]; //note: accessed flat
    un->cs->Compute_SRAvg(un);
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}


void NetTask_0::Send_Netin_Clash() {
  Unit** units = Nb::net.units_flat.Els();
  const int n_units_flat =  Nb::net.units_flat.size;
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    if (un->do_delta) {
      Send_Netin_0(un);
      AtomicFetchAdd(&Nb::n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
    }
    ++g_u;
  }
}

void NetTask_N::Send_Netin_Clash() {
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = Network::g_units[my_u]; //note: accessed flat
    if (un->do_delta) {
      Send_Netin_0(un);
      AtomicFetchAdd(&Nb::n_tot, 1);
    }
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}

void NetTask_N::Send_Netin_Array() {
  overhead.Start(false);
  memset(excit.el, 0, sizeof(float) * excit.size);
  overhead.Stop();

  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* su = Network::g_units[my_u]; //note: accessed flat
    if (su->do_delta) {
      float su_act_eff = su->act();
      for (int j = 0; j < su->send.size; ++j) {
        SendCons* send_gp = su->send[j];
        const int send_sz = send_gp->units.size;
        if (send_sz == 0) continue; // guard
        su->n_con_calc += send_sz;
        int* uns = send_gp->units.Els(); // unit pointer
    //note: for RECV_CONS/send-based, we assigned wti's in sender order
        int cni = send_gp->Cn(0)->wti; // first weight
#ifdef SEND_CONS
        for (int i=0; i < send_sz; i++) {
          int targ_i = uns[i];
          Send_Netin_inner_0(Conn::g_wts[cni++], excit.el[targ_i], su_act_eff);
        }
#else
        for (int i=0; i < send_sz; i++) {
          int targ_i = uns[i];
          Send_Netin_inner_0(Conn::g_wts[cni++], excit.el[targ_i], su_act_eff);
        }
#endif
      }
      AtomicFetchAdd(&Nb::n_tot, 1);
    }
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}

void NetTask_N::Recv_Netin() {
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  while (my_u < Nb::net.units_flat.size) {
    Unit* un = Network::g_units[my_u]; //note: accessed flat
    Recv_Netin_0(un);
    AtomicFetchAdd(&Nb::n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}

void NetTask_N::ComputeAct() {
//  Unit** units = Nb::net.units_flat.Els();
  int my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  my_act = 0.0f;
  const int unit_sz = Nb::net.units_flat.size;
  while (my_u < unit_sz) {
    my_act += ComputeAct_inner(my_u);
    my_u = AtomicFetchAdd(&g_u, ThreadNetEngine::n_procs);
  }
}


bool Nb::hdr = false;

int Nb::n_layers;
int Nb::n_units;			// number of units per layer
int Nb::n_cons; // number of cons per unit
int Nb::n_cycles;			// number of cycles of updating
Network Nb::net;		// global network 
int Nb::n_tot;
int Nb::n_prjns;
float Nb::tot_act; // check on tot act
float Nb::nibble_thresh = 0.8f;
signed char Nb::nibble_mode = 0;
signed char Nb::fast_prjn = 0; // fast prjns directly access target unit array
bool Nb::single = false; // true for single thread mode, to compare against nprocs=1
int Nb::send_act = 0x10000; // send activation, as a fraction of 2^16 
int Nb::tsend_act; 
int Nb::dwt_rate = 50;
int Nb::sra_rate = 5;
int Nb::inv_act = 1; // inverse of activation -- can use to divide
int Nb::this_rand; // assigned a new random value each cycle, to let us randomize unit acts
bool Nb::use_log_file = false;
bool Nb::calc_act = true;
#ifdef SEND_CONS
signed char Nb::sndcn = 1; 
#else
signed char Nb::sndcn = 0; 
#endif

int Nb::main() {
  Initialize();
  
  net.Initialize();
  net.Build();

  FILE* logfile = NULL;
  if (use_log_file) {
    logfile = fopen("ptest_core.log", "w");
    fprintf(logfile,"cyc\ttot_act\n");
  }

  time_used.Start();

  // this is the key computation loop
  for (net.cycle = 0; net.cycle < n_cycles; net.cycle++) {
    net.ComputeNets();
    if (calc_act) {
      float tot_act = net.ComputeActs();
      if(use_log_file)
        fprintf(logfile,"%d\t%g\n", net.cycle, tot_act);
    }
    // note: add 1 to cycle so we do it after dwt_rate full counts
    int nc1 = net.cycle + 1;
    if ((sra_rate > 0) && ((nc1 % sra_rate) == 0)) 
      net.Compute_SRAvg();
    if ((dwt_rate > 0) && ((nc1 % dwt_rate) == 0)) 
      net.Compute_Weights();
  }

  time_used.Stop();
  if (use_log_file) {
    fclose(logfile);
    logfile = NULL;
  }

  // note: always make/extend the thread log
  net.engine->Log(hdr);

  PrintResults();

  net.SetEngine(NULL); // controlled delete
  return 0;
}

int Nb::PreInitialize() {
  int rval = 0;
  prompt = 
    "must have min 3 args (you can use \"0\" for def of positionals):\n"
    "\t<n_units>\tnumber of units in each of the layers\n"
    "\t<n_cycles>\tnumber of cycles\n"
    "\t<n_procs>\tnumber of cores or procs (0=fast single-threaded model)\n"
    "optional positional params -- none can be skipped: \n"
    "\t<n_lays>\tnumber of layers (min 2, def=5, max 128)\n"
    "\t<n_cons>\tIGNORED number of cons per unit-projection (def=n_units)\n"
    "\t<send_act>\tpercent avg activation level (1-100, def = 5)\n"
    "optional commands: \n"
    "\t-header\t output a header line\n"
    "\t-log=1/0(def)\tlog/do-not-log optional values to ptest_core.log\n"
    "\t-nibble=n\tnibble: 0(def)=none, 1=on, 2=auto (> .20 left)\n"
    "\t-act=0\tdo not calculate activation\n"
    "\t-sra_rate=5\tcalc sravg every n cycles, 0=never\n"
    "\t-dwt_rate=50\tcalc dwt every n cycles, 0=never\n"
    "\t-suff=xxx\tlog file suffix\n"
    "-algo=n (def=0) is one of the following:\n"
    "\t 0 receiver-based\n"
    "\t 1 receiver-based, with smart calc (ignore senders below thresh)\n"
    "\t 2 sender-based -- clashes allowed \n"
    "\t 3 sender-based -- array-based, 1 array of nets per thread, then rolled up \n"
    "\t-fast_prjn=0(def)/1\tuse fast prjn mode to directly access target units\n"
  ;
  PreInitialize_impl(rval);
  if (rval != 0) return rval;
  if (argc < 4) {
    printf(prompt.toLatin1());
    return 1;
  }
  ParseCmdLine(rval);
  return rval;
}

void Nb::PreInitialize_impl(int& /*rval*/) {
}

NetEngine* Nb::CreateNetEngine() {
  NetEngine* rval = NULL;
  if (NetEngine::n_procs <= 0) {
    single = true;
    NetEngine::n_procs = 1;
    rval = new NetEngine; // just the default
  } else {
    single = false;
    if (NetEngine::n_procs > NetEngine::core_max_nprocs) 
      NetEngine::n_procs = NetEngine::core_max_nprocs;
    rval = new ThreadNetEngine; // just the default
  }
  return rval;
}  

void Nb::ParseCmdLine(int& /*rval*/) {
  n_units = (int)strtol(argv[1], NULL, 0);
  n_cycles = (int)strtol(argv[2], NULL, 0);
  NetEngine::n_procs = (int)strtol(argv[3], NULL, 0);
  
  // optional positional params
  n_layers = 5; // def
  if ((argc > 4) && (*argv[4] != '-')) {
    n_layers = (int)strtol(argv[4], NULL, 0);
    if (n_layers < 2) n_layers = 2;
    if (n_layers > 128) n_layers = 128;
  }
  n_cons = n_units; // def
  if ((argc > 5) && (*argv[5] != '-')) {
    n_cons = (int)strtol(argv[5], NULL, 0);
    if (n_cons <= 0) 
      n_cons = n_units;
  }
  
  tsend_act = 5; // def
  if ((argc > 6) && (*argv[6] != '-')) {
    int ttsend_act = (int)strtol(argv[6], NULL, 0);
    if ((ttsend_act < 0) && (ttsend_act <= 100)) {
      tsend_act = ttsend_act;
    }
  }
  send_act = (int)(0x10000 * (tsend_act / 100.0f));
  inv_act = 100 / tsend_act;
  
  
  // switch params
  for (int arg = 4; arg < argc; arg++) {
    QString targ(argv[arg]);
#ifdef USE_SAFE
    if (targ == "-safe=1")
      safe = true;
    if (targ == "-safe=0")
      safe = false;
#endif
    if (targ == "-header")
      hdr = true;
    if (targ.startsWith("-algo=")) {
      NetEngine::algo = targ.remove("-algo=").toInt();
      continue; }
    if (targ.startsWith("-dwt_rate=")) {
      dwt_rate = targ.remove("-dwt_rate=").toInt();
      continue; }
    if (targ.startsWith("-sra_rate=")) {
      sra_rate = targ.remove("-sra_rate=").toInt();
      continue; }
      
    if (targ == "-log=1") {
      use_log_file = true; continue;}
    if (targ == "-log=0") {
      use_log_file = false; continue;}
    if (targ == "-act=0") {
      calc_act = false; continue;}
    if (targ.startsWith("-nibble=")) {
      nibble_mode = targ.remove("-nibble=").toInt();
      continue;}
    if (targ == "-fast_prjn=1") {
      fast_prjn = true; continue;}
    if (targ.startsWith("-suff=")) {
      log_suff = targ.remove("-suff=");
      continue;}
  }
  log_filename = QFileInfo(argv[0]).baseName() + log_suff + ".log"; 

}
void Nb::Initialize() {
  srand(56);			// always start with the same seed!
  
  Network::recv_based = !(NetEngine::algo & NetEngine::SEND_FLAG);
  if (NetEngine::algo == NetEngine::RECV_SMART)
    Network::recv_smart = true;

  Initialize_impl();
  
  Nb::net.SetEngine(CreateNetEngine());
}

void Nb::PrintResults() {
  tot_time = time_used.s_used;
  n_wts = n_prjns * n_units * n_cons;
  n_con_trav = net.GetNTot();
  // actual con_trav / sec based on total actual cons
  con_trav_sec = (n_con_trav / tot_time) / 1.0e6;
  // but effective is based on total number 
  n_eff_con_trav = n_wts * n_cycles;
  eff_con_trav_sec = ((double)n_eff_con_trav / tot_time) / 1.0e6;
  
  FILE* logfile = NULL;
  if (hdr) {
    logfile = fopen(log_filename.toLatin1(), "w");
    const char* hdr_str = 
    "algo\teMcon\tMcon\tsnd_act\tprocs"
    "\tlayers\tunits\tcons\tKwts\tcycles"
    "\tKcn_tot\tsecs\tnibble\tsndcn\tfstprjn"
    "\tdwtrate\n";
    
    if (logfile) fprintf(logfile, hdr_str);
    printf(hdr_str);
  } else {
    logfile = fopen(log_filename.toLatin1(), "a");
  }
  if (single) NetEngine::n_procs = 0;
  char buf[256]; // s/b enough
  sprintf(buf, 
    "%d\t%.4g\t%.4g\t%d\t%d"
    "\t%d\t%d\t%d\t%g\t%d"
    "\t%g\t%.3g\t%d\t%d\t%d"
    "\t%d\n",
    NetEngine::algo, eff_con_trav_sec, con_trav_sec, tsend_act, NetEngine::n_procs,
    n_layers, n_units, n_cons, n_wts / 1000, n_cycles,
    n_con_trav / 1000, tot_time, nibble_mode, sndcn, fast_prjn,
    dwt_rate);
  cout << buf;
  if (logfile) {
    fprintf(logfile, buf);
    fclose(logfile);
    logfile = NULL;
  }
}
