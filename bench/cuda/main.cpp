#include "embench_cuda.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

/* this is an emergent/leabra-like benchmark for computing neural network activation

  Written by: 
    Randall C. O'Reilly <oreilly@psych.colorado.edu>
    Brad Aisa <baisa@colorado.edu

  this version uses CUDA to calculate on NVIDA hardware

*/

// SWITCHES THAT CAN BE SET
//#define USE_VAL -- adds a "float* val" to point directly to correct net val
#define USE_RECV -- receiver-based
//#define USE_RECV_SMART

////////////////////////////////////////////////////////////////////////////////////
// timing code

#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

class Timer {
  // stores and computes time used for processing information
public:
  bool		rec;		// flag that determines whether to record timing information: OFF by default
  long		n;		// number of times time used collected using GetUsed

  long		ticksPer() const {if (ticks_per == 0) ticks_per = sysconf(_SC_CLK_TCK); return ticks_per;}
  double	elapsed() const {return tot / (double)ticksPer();} // return elapsed time in s
  
  void		Reset();	// initialize the times
  void		Start();  // continue recording time
  void		Stop(); // stop recording time
  void		OutString(FILE* fh);	// output string as seconds and fractions of seconds

  Timer();
protected:
  clock_t	tot_st;
  struct tms 	t_st; // time at start
  long 		usr;		// user clock ticks used
  long		sys;		// system clock ticks used
  long		tot;		// total time ticks used (all clock ticks on the CPU)
  mutable long	ticks_per;
};

Timer::Timer() {
  rec = true;
  ticks_per = 0;
  Reset();
}

void Timer::Reset() {
  usr = 0; sys = 0; tot = 0; n = 0;
}

void Timer::Start() {
  if(!rec) return;
  tot_st = times(&t_st);
}

void Timer::Stop() {
  if(!rec) return;
  struct tms t;
  clock_t tottime = times(&t);
  tot += ((long)tottime - (long)tot_st);
  usr += ((long)t.tms_utime - (long)t_st.tms_utime);
  sys += ((long)t.tms_stime - (long)t_st.tms_stime);
  ++n;
}

void Timer::OutString(FILE* fh) {
  if(!rec) return;
  long ticks_per = sysconf(_SC_CLK_TCK);
  float ustr = (float)((double)usr / (double)ticks_per);
  float sstr = (float)((double)sys / (double)ticks_per);
  float tstr = (float)((double)tot / (double)ticks_per);
  fprintf(fh, "usr: %g\t sys: %g\t tot: %g\tn: %ld", ustr, sstr, tstr, n);
}



class TimeUsed {
  // stores and computes time used for processing information
public:
  long 		ticks_per;
  bool		rec;		// flag that determines whether to record timing information: OFF by default
  long 		usr;		// user clock ticks used
  long		sys;		// system clock ticks used
  long		tot;		// total time ticks used (all clock ticks on the CPU)
  long		n;		// number of times time used collected using GetUsed

  void 		operator += (const TimeUsed& td)	{ usr += td.usr; sys += td.sys; tot += td.tot; }
  void 		operator -= (const TimeUsed& td)	{ usr -= td.usr; sys -= td.sys; tot -= td.tot; }
  void 		operator *= (const TimeUsed& td)	{ usr *= td.usr; sys *= td.sys; tot *= td.tot; }
  void 		operator /= (const TimeUsed& td)	{ usr /= td.usr; sys /= td.sys; tot /= td.tot; }
  TimeUsed 	operator + (const TimeUsed& td) const;
  TimeUsed 	operator - (const TimeUsed& td) const;
  TimeUsed 	operator * (const TimeUsed& td) const;
  TimeUsed 	operator / (const TimeUsed& td) const;

  void		InitTimes();	// initialize the times
  void		GetTimes();	// get the clock ticks used to this point
  void		GetUsed(const TimeUsed& start);
  // get amount of time used by subtracting start from current time and adding to me, and incrementing n
  void		OutString(FILE* fh);	// output string as seconds and fractions of seconds

  TimeUsed();
};

TimeUsed::TimeUsed() {
  rec = false;
  InitTimes();
  ticks_per = sysconf(_SC_CLK_TCK);

}

void TimeUsed::InitTimes() {
  usr = 0; sys = 0; tot = 0; n = 0;
}

void TimeUsed::GetTimes() {
  if(!rec) return;
  struct tms t;
  clock_t tottime = times(&t);
  tot = (long)tottime;
  usr = (long)t.tms_utime;
  sys = (long)t.tms_stime;
}

void TimeUsed::GetUsed(const TimeUsed& start) {
  if(!rec || !start.rec) return;
  TimeUsed end;  end.rec = true;
  end.GetTimes();
  *this += (end - start);
  n++;
}

void TimeUsed::OutString(FILE* fh) {
  if(!rec) return;
  float ustr = (float)((double)usr / (double)ticks_per);
  float sstr = (float)((double)sys / (double)ticks_per);
  float tstr = (float)((double)tot / (double)ticks_per);
  fprintf(fh, "usr: %g\t sys: %g\t tot: %g\tn: %ld", ustr, sstr, tstr, n);
}

TimeUsed TimeUsed::operator+(const TimeUsed& td) const {
  TimeUsed rv;
  rv.usr = usr + td.usr; rv.sys = sys + td.sys; rv.tot = tot + td.tot;
  return rv;
}
TimeUsed TimeUsed::operator-(const TimeUsed& td) const {
  TimeUsed rv;
  rv.usr = usr - td.usr; rv.sys = sys - td.sys; rv.tot = tot - td.tot;
  return rv;
}
TimeUsed TimeUsed::operator*(const TimeUsed& td) const {
  TimeUsed rv;
  rv.usr = usr * td.usr; rv.sys = sys * td.sys; rv.tot = tot * td.tot;
  return rv;
}
TimeUsed TimeUsed::operator/(const TimeUsed& td) const {
  TimeUsed rv;
  rv.usr = usr / td.usr; rv.sys = sys / td.sys; rv.tot = tot / td.tot;
  return rv;
}

// timing code
////////////////////////////////////////////////////////////////////////////////////

//NOTE: the gcc version of __sync_fetch_and_add doesn't seem to exist
// it causes a linker fail so we define our own
// returns value of i before add, then adds requested amount
#if defined(Q_OS_WIN32)
# error not defined for Windows yet
#else // unix incl Intel Mac
inline int AtomicFetchAdd(int* operand, int incr)
{
  // atomically get value of operand before op, then add incr to it
  asm volatile (
    "lock\n\t"
    "xaddl %0, %1\n" // add incr to operand
    : "=r" (incr) // incr gets replaced by the value of operand before inc
    : "m"(*operand), "0"(incr)
  );
  return incr;
}
#endif



// cuda version 

class NetTask;

NetTask* net_tasks[1]; // only 1 created


class Task {
public:
  int		task_id;
  int		proc_id; // current proc being run
  
  virtual void	run() = 0;
  
  Task() {task_id = -1; proc_id = 0;}
  virtual ~Task() {}
};



class taPtrList_impl {
public:
  void**	el;		// #READ_ONLY #NO_SAVE #NO_SHOW the elements themselves
  int 		alloc_size;	// #READ_ONLY #NO_SAVE allocation size
  void*	hash_table;	// #READ_ONLY #NO_SAVE #HIDDEN a hash table (NULL if not used)
  int		size;		// #READ_ONLY #NO_SAVE #SHOW number of elements in the 
  
  void		Alloc(int i) {el = new void*[i]; alloc_size = i;}
  taPtrList_impl() {el = NULL; alloc_size = 0; hash_table = NULL; size = 0;}
  ~taPtrList_impl() {if (alloc_size) {delete[] el; el = NULL; alloc_size = size = 0;}}
};

template<class T> 
class taPtrList : public taPtrList_impl { // #INSTANCE
public:
  // element at index
  T**		Els() const		{ return (T**)el; }
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  T*		operator[](int i) const		{ return (T*)el[i]; }
  void		Set(T* it, int i)		{  el[i] = it; }
  taPtrList() {}
  explicit taPtrList(int alloc) {Alloc(alloc);}
};

// Now: actual neural network code
// units compute net = sum_j weight_j * act_j for all units that send input into them
// then share this net across nodes
// and then compute activations, and repeat.

class Connection;
class ConSpec;
class Unit;
class Layer;

typedef taPtrList<Unit>	Unit_List;

class Connection {
  // one connection between units
public:
  float wt;			// connection weight value
#ifdef USE_VAL
  float* val; // pointer to the net guy to use
#endif
  float dwt;			// delta-weight
//  float	pdw;
//NOTE: CUDA works with indexes, so we trade the float pdw for the
// target or src index here -- it doesn't affect the algorithm
// speed or the base comparison speed, because it is only used in Build
  int	val_idx; // flat index of Unit -- this is a crutc
};

class ConSpec {
  int	dummy[8];
public:
  
//  void C_Send_Netin(void*, Connection* cn, Unit* ru, float su_act_eff);
//  void Send_Netin(Unit* cg, Unit* su);
};

class Unit {
  // a simple unit
public:
#ifdef USE_RECV_SMART
  bool		do_delta; // for recv_smart
#endif
  float act;			// activation value
  float net;			// net input value
  Connection* 	cons; // recv: senders/wts to this; send: targets/wts from this
//nn  int		task_id; // which task will process this guy
  int		flat_idx; // global index, used for cuda build
  Unit_List	targs; // for recv, is the recv from units; for send, is the send targets
  ConSpec*	cs;
  char dummy[16]; // bump a bit, to spread these guys out
  
  Connection* 	Cn(int i) const { return &(cons[i]); }
  // #CAT_Structure gets the connection at the given index
  Unit*		Un(int i) const { return targs.FastEl(i); }
  
  bool		DoDelta(); // returns true if we should do a delta; also sets the do_delta
  
  
  Unit();
  ~Unit();
protected:
  int		my_rand; // random value
};

class Layer {
public:
  static int	next_un_idx; // circular guy we globally use to pick next target unit
  
  Unit_List	units;
  
  void		Connect(Layer* to);
  
  Layer();
  ~Layer();
protected:
  void 		Connect_To_Send(Layer* lay_to);
  void 		Connect_From_Recv(Layer* lay_fr);
};
int Layer::next_un_idx;

typedef taPtrList<Layer>	Layer_List;

class Network {
public:
  static bool	recv_based; // true for recv, false for send
  static bool	recv_smart; // for recv-based, does the smart recv algo
  
  Unit_List 	units_flat;		// layers = arrays of units
  Layer_List	layers;
  
  void		Build(); 
  void 		ComputeNets();
  float 	ComputeActs();
  
  Network();
  ~Network();
};
bool Network::recv_based; // true for recv, false for send
bool Network::recv_smart; 

class NetTask: public Task {
public:
  enum Proc {
    P_Send_Netin,
    P_Recv_Netin,
    P_ComputeAct
  };
  
// All
  int		g_u; 
  int		t_tot; // shared by Xxx_Netin
  void		run();

// Send_Netin
  inline static void Send_Netin_inner_0(float cn_wt, float* ru_net, float su_act_eff);
    // highly optimized inner loop
  inline static void 	Send_Netin_0(Unit* su); // shared by 0 and N
  virtual void	Send_Netin(); // default is the _0 version
  
// Recv_Netin
  inline static void 	Recv_Netin_0(Unit* ru); // shared by 0 and N
  virtual void	Recv_Netin(); // default is the _0 version
  
  
// ComputeAct  
  inline static void ComputeAct_inner(Unit* un);
  float		my_act;
  virtual void	ComputeAct(); // default does it globally
  
  NetTask();
};

// network configuration information

int n_layers;
int n_units;			// number of units per layer
int n_cons; // number of cons per unit
int n_units_flat; // total number of units (flattened, all layers)
int n_cycles;			// number of cycles of updating
Network net;		// global network 
int n_tot; // total units (reality check)
float tot_act; // check on tot act
int n_procs; // always 1 in this bench
bool single = false; // true for local, false for CUDA
int send_act = 0x10000; // send activation, as a fraction of 2^16 
int this_rand; // assigned a new random value each cycle, to let us randomize unit acts
bool calc_act = true;

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
  cons = NULL; 
  cs = NULL; 
  flat_idx = -1;
  net = 0;
  my_rand = rand();
#ifdef USE_RECV_SMART
  do_delta = true; // for first iteration, before it is set in ComputeActs
#endif
}

Unit::~Unit() {
  delete[] cons;
}

bool Unit::DoDelta() {
#ifndef USE_RECV_SMART
  bool
#endif
  do_delta = ((this_rand ^ my_rand) & 0xffff) < send_act;
  return do_delta;
} 

// construct the network

Layer::Layer() {
}

Layer::~Layer() {
  while (units.size > 0) 
    delete units.FastEl(--(units.size));
}

void Layer::Connect(Layer* to) {
  if (Network::recv_based) 
    to->Connect_From_Recv(this);
  else 
    Connect_To_Send(to);
}

void Layer::Connect_To_Send(Layer* lay_to) {
  for (int u_fr = 0; u_fr < this->units.size; u_fr++) {
    Unit* un_fr = units.FastEl(u_fr);
    for (int con = 0; con < n_cons; con++) {
      Unit* un_to = lay_to->units.FastEl(next_un_idx);
      un_fr->targs.Set(un_to, un_fr->targs.size);
      Connection& cn = un_fr->cons[un_fr->targs.size++];
      cn.wt = (4.0 * (float)rand() / RAND_MAX) - 2.0;
#ifdef USE_VAL
      cn.val = &(un_to->net);
#endif      
      cn.val_idx = un_to->flat_idx;
      if (++next_un_idx >= n_units) next_un_idx = 0;
    }
  }
}

void Layer::Connect_From_Recv(Layer* lay_fr) {
  for (int u_to = 0; u_to < this->units.size; u_to++) {
    Unit* un_to = units.FastEl(u_to);
    for (int con = 0; con < n_cons; con++) {
      Unit* un_fr = lay_fr->units.FastEl(next_un_idx);
      un_to->targs.Set(un_fr, un_to->targs.size);
      Connection& cn = un_to->cons[un_to->targs.size++];
      cn.wt = (4.0 * (float)rand() / RAND_MAX) - 2.0;
#ifdef USE_VAL
      cn.val = &(un_fr->act);
#endif      
      cn.val_idx = un_fr->flat_idx;
      if (++next_un_idx >= n_units) next_un_idx = 0;
    }
  }
}


// delete the network

Network::Network() {
}

Network::~Network() {
  for(int l=0;l<n_layers;l++) {
    Layer* lay = layers[l];
    delete lay;
  }
}

void Network::Build() {
  layers.Alloc(n_layers);
  n_units_flat = n_units * n_layers;
  units_flat.Alloc(n_units_flat);
  
  ConSpec* cs = new ConSpec; // everyone shares
  // make all layers and units first
  for (int l = 0; l < n_layers; l++) {
    Layer* lay = new Layer;
    layers.Set(lay, layers.size++);
    lay->units.Alloc(n_units);
    
    for (int i=0;i<n_units;i++) {
      Unit* un = new Unit;
      un->cons = new Connection[n_cons * 2];
      un->targs.Alloc(n_cons * 2);
      
      lay->units.Set(un, lay->units.size++);
      un->flat_idx = units_flat.size;
      units_flat.Set(un, units_flat.size++);
      un->cs = cs;
    } 
  }
   
  // then connect all bidirectionally
  for (int lay_ths = 0; lay_ths < (n_layers); lay_ths++) {
    int lay_nxt = lay_ths + 1; 
    if (lay_nxt >= n_layers) lay_nxt = 0;
    int lay_prv = lay_ths - 1; 
    if (lay_prv < 0) lay_prv = n_layers - 1;
    layers[lay_prv]->Connect(layers[lay_ths]);
    layers[lay_ths]->Connect(layers[lay_nxt]);
    if (n_layers == 2) break; // special case
  }
}


class NetTask_C: public NetTask {
// for CUDA approach -- optimum for that
public:
  static float* acts; // net guys
  static float* nets; // net guys
  
  static bool	GetCon(int un_idx, int con_idx, int* snd_idx, float* wt);
    // callback for net building
  static int	Init(); // inits
  
  void		Send_Netin();
  void		Recv_Netin();
  void		ComputeAct();
};
float* NetTask_C::acts; 
float* NetTask_C::nets; 

void DoProc(int proc_id) {
  NetTask* tsk = net_tasks[0];
  tsk->g_u = 0;
  tsk->proc_id = proc_id;
  tsk->run();
}


void Network::ComputeNets() {
  if (recv_based)
    DoProc(NetTask::P_Recv_Netin);
  else
    DoProc(NetTask::P_Send_Netin);
}

float Network::ComputeActs() {
  // compute activations (only order number of units)
  tot_act = 0.0f;
  DoProc(NetTask::P_ComputeAct);
  tot_act += net_tasks[0]->my_act;
  this_rand = rand(); // for next cycle
  return tot_act;
}

////////////////////////////////
// core code


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


float tru_net;
void NetTask::Send_Netin_inner_0(float cn_wt, float* ru_net, float su_act_eff) {
  *ru_net += su_act_eff * cn_wt;
  //tru_net = *ru_net + su_act_eff * cn_wt;
}

void NetTask::Send_Netin_0(Unit* su) {
  float su_act_eff = su->act;
  Connection* cns = su->cons; // array pointer
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
  Connection* cns = ru->cons; // array pointer
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


void NetTask::Send_Netin() {
  Unit** units = net.units_flat.Els();
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    if (un->DoDelta()) {
      Send_Netin_0(un);
      AtomicFetchAdd(&n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
      //++t_tot;
    }
    ++g_u;
  }
}

void NetTask::Recv_Netin() {
  Unit** units = net.units_flat.Els();
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    Recv_Netin_0(un);
    AtomicFetchAdd(&n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
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
  Unit** units = net.units_flat.Els();
  // compute activations (only order number of units)
  my_act = 0.0f;
  while (g_u < n_units_flat) {
    Unit* un = units[g_u]; //note: accessed flat
    ComputeAct_inner(un);
    my_act += un->act;
    ++g_u;
  }
}

int NetTask_C::Init() {
  acts = (float*)calloc(n_units_flat, sizeof(float));
  nets = (float*)calloc(n_units_flat, sizeof(float));
  // silo size will be based on N_THREAD-normalized units 
  uint n_units_gran = (uint)((n_units_flat + (N_THREADS - 1)) & ~(N_THREADS-1));
  // chunks per unit also need to be normalized (rounded up to nearest chunk size)
  uint n_gran2 = (uint)(((n_cons * 2) + (CON_CHUNK_SZ - 1)) & ~(CON_CHUNK_SZ-1));
  uint silo_sz = (uint)((n_units_gran * n_gran2) / (CON_CHUNK_SZ * N_THREADS));
cerr << "calling cuAllocMem: n_units=" << n_units_flat << ", silo_sz="
  << silo_sz << " (n_gran2=" << n_gran2 << ")\n";
  int result = cuAllocMem((uint)n_units_flat, silo_sz);
  if (result != 0) return result;
cerr << "calling cuCpHD_Cons\n";
  result = cuCpHD_Cons(GetCon);
  return result; // s/b 0
}

bool NetTask_C::GetCon(int un_idx, int con_idx, int* snd_idx, float* wt) {
  Unit* un = net.units_flat.FastEl(un_idx);
  //NOTE: because con size is same everywhere, we can test easily, 
  // but real algo must take from Cons
  if (con_idx < (n_cons * 2)) {
    Connection& cn = un->cons[con_idx];
    *snd_idx = cn.val_idx;
    *wt = cn.wt;
    return false;
  } 
  return true;
}

void NetTask_C::Send_Netin() {
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

void NetTask_C::Recv_Netin() {
  cuRecv_Netin();
  cuCpDH_Nets(nets);
  AtomicFetchAdd(&n_tot, n_units_flat);
}

void NetTask_C::ComputeAct() {
  my_act = 0.0f;
  for (int i = 0; i < n_units_flat; i++) {
    acts[i] = 1.0f / (1.0f + expf(-nets[i]));
    my_act += acts[i];
  }
  cuCpHD_Acts(acts);
}


void MakeTasks() {
  if (single) {
    NetTask* tsk = new NetTask;
    net_tasks[0] = tsk;
  } else {
    NetTask_C* tsk = new NetTask_C;
    net_tasks[0] = tsk;
  }
  net_tasks[0]->task_id = 0;
}

void DeleteTasks() {
  delete net_tasks[0];
}

int main(int argc, char* argv[]) {
  bool hdr = false;
  
  if(argc < 4) {
    printf("must have min 3 args:\n"
      "\t<n_units>\tnumber of units in each of the layers\n"
      "\t<n_cycles>\tnumber of cycles\n"
      "\t<n_procs>\t0:local, 1:CUDA\n"
      "optional positional params -- none can be skipped: \n"
      "\t<n_lays>\tnumber of layers (min 2, def=3, max 128)\n"
      "\t<n_cons>\tnumber of cons per unit-projection (def=n_units)\n"
      "\t<send_act>\tpercent (/100) avg activation level (def = 100)\n"
      "optional commands: \n"
      "\t-log=0\t(def) do not log optional values to ptest_core.log\n"
      "\t-log=1\tlog optional values to ptest_core.log\n"
      "\t-act=0\tdo not calculate activation\n"
    );
    return 1;
  }
#ifdef USE_RECV
  net.recv_based = true;
# ifdef USE_RECV_SMART
  net.recv_smart = true;
# endif  
#endif  
  bool use_log_file = false;

  srand(56);			// always start with the same seed!

  n_units = (int)strtol(argv[1], NULL, 0);
  //TODO: parameterize this!
  n_units_flat = n_units * n_layers;
  n_cycles = (int)strtol(argv[2], NULL, 0);
  n_procs = (int)strtol(argv[3], NULL, 0);
  if (n_procs <= 0) {
    single = true;
  } else {
    single = false;
  }
  n_procs = 1;
  
  // optional positional params
  n_layers = 3; // def
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
  
  int tsend_act = 100; // def
  if ((argc > 6) && (*argv[6] != '-')) {
    tsend_act = (int)strtol(argv[6], NULL, 0);
    if (tsend_act < 0)
      tsend_act = 0;
    else if (tsend_act > 100)
      tsend_act = 100;
  }
  if (tsend_act != 100) {
    send_act = (int)(0x10000 * (tsend_act / 100.0f));
  }
  
  
  // switch params
  for (int arg = 4; arg < argc; arg++) {
#ifdef USE_SAFE
    if (strcmp(argv[arg], "-safe=1") == 0)
      safe = true;
    if (strcmp(argv[arg], "-safe=0") == 0)
      safe = false;
#endif
    if (strcmp(argv[arg], "-header") == 0)
      hdr = true;
    if (strcmp(argv[arg], "-log=1") == 0)
      use_log_file = true;
    if (strcmp(argv[arg], "-log=0") == 0)
      use_log_file = false;
    if (strcmp(argv[arg], "-act=0") == 0)
      calc_act = false;
    //TODO: any more
  }
  
  MakeTasks();
  net.Build();
  if (!single) {
    int res = NetTask_C::Init();
    if (res != 0) {
      cerr << "NetTask_C:Init() failed with code: " << res << "\n";
      return res;
    }
  }

  FILE* logfile = 0;
  if (use_log_file) {
    logfile = fopen("ptest_core.log", "w");
    fprintf(logfile,"cyc\ttot_act\n");
  }

  TimeUsed time_used;	
  time_used.rec = true;
  TimeUsed start;  	
  start.rec = true;
  start.GetTimes();

  // this is the key computation loop
  for(int cyc = 0; cyc < n_cycles; cyc++) {
    net.ComputeNets();
    if (calc_act) {
      float tot_act = net.ComputeActs();
      if(use_log_file)
        fprintf(logfile,"%d\t%g\n", cyc, tot_act);
    }
  }

  time_used.GetUsed(start);


//     time_used.OutString(stdout);
//     printf("\n");

  double tot_time = time_used.tot / (double)time_used.ticks_per/*1.0e2*/;

  double n_wts = n_layers * n_units * n_cons * 2.0;   // bidirectional connected layers
  // cons travelled will depend on activation percent, so we calc exactly
  double n_con_trav = n_tot * (n_cons * 2.0);
  double con_trav_sec = ((double)n_con_trav / tot_time) / 1.0e6;
  // but effective is based on raw numbers
  double n_eff_con_trav = n_layers * n_units * n_cycles * (n_cons * 2.0);
  double eff_con_trav_sec = ((double)n_eff_con_trav / tot_time) / 1.0e6;

  if(use_log_file) {
/*nn    fprintf(logfile,"\nthread\tt_tot\tstart lat\trun time\n");
    for (int t = 0; t < n_procs; t++) {
      NetTask* tsk = net_tasks[t];
      QTaskThread* th = (QTaskThread*)threads[t];
      double start_lat = 0;
      double run_time = 0;
      if (t > 0) {
        start_lat = th->start_latency.elapsed();
        run_time = th->run_time.elapsed();
      }
      fprintf(logfile,"%d\t%d\t%g\t%g\n", t, tsk->t_tot, start_lat, run_time);
    }*/
  }
  if(use_log_file)
    fclose(logfile);

  if (hdr)
  printf("eMcon\tMcon\tsnd_act\tprocs\tlayers\tunits\tcons\tweights\tcycles\tcon_trav\tsecs\tn_tot\n");
  if (single) n_procs = 0;
  printf("%g\t%g\t%d\t%d\t%d\t%d\t%d\t%g\t%d\t%g\t%g\t%d\n",
    eff_con_trav_sec, con_trav_sec, tsend_act, n_procs, n_layers, n_units, n_cons, n_wts, n_cycles, n_con_trav, tot_time, n_tot);

  DeleteTasks();
//  DeleteNet();
}

