#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// this is a benchmark for computing a neural network activation updates with pointer-based
// encoding of connections (ptest)

// Written by: Randall C. O'Reilly; oreilly@psych.colorado.edu

// this version uses MPI to distribute computation across multiple nodes

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
  ticks_per;
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

// core version 
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QCoreApplication>
#include <pthread.h>
#include <sys/signal.h>

class NetInTask;

int core_nprocs;		// total number of processors
const int core_max_nprocs = 32; // maximum number of processors!
QThread* threads[core_max_nprocs]; // only core_nprocs-1 created, none for [0] (main thread)
NetInTask* netin_tasks[core_max_nprocs]; // only core_nprocs created

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


class Task {
public:
  int		task_id;
  virtual void	run() = 0;
  
  Task() {task_id = -1;}
};

class QTaskThread: public QThread {
public:
  Timer		start_latency; // amount of time waiting to start
  Timer		run_time; // amount of time actually running jobs
  
  inline bool	isActive() const {return m_active;}
  inline bool	isSuspended() const {return m_suspended;}
  
  Task*		task() const {return m_task;}
  void		setTask(Task* t) {m_task = t;}
  
  void 		suspend();
  void		resume();
  void		terminate();
  
  QTaskThread();
protected:
  Task*		m_task;
  QMutex	mutex;
  QWaitCondition	wc;
  bool		m_suspended;
  bool		m_active;
  Qt::HANDLE	m_thread_id; // for the thread, set in run
  
  void 		run();
  void		run_impl();
};

QTaskThread::QTaskThread() {
  m_task = NULL;
  m_thread_id = 0;
  m_active = true;
  m_suspended = true;
}

void QTaskThread::suspend() {
  if (m_suspended) return;
  mutex.lock();
  m_suspended = true;
  mutex.unlock();
}

void QTaskThread::resume() {
  if (!m_suspended) return;
  mutex.lock();
  m_suspended = false;
  start_latency.Start();
  wc.wakeAll();;
  mutex.unlock();
}

void QTaskThread::run() {
  m_thread_id = currentThreadId();
  while (m_active) {
    mutex.lock();
    while (m_suspended)
      wc.wait(&mutex);
      
    start_latency.Stop();
    if (m_task) {
      run_time.Start();
      m_task->run();
      run_time.Stop();
    }
    m_suspended = true;
    if (!m_active) break;
    mutex.unlock();
  }
}

void QTaskThread::terminate() {
  if (!m_active) return;
  
  mutex.lock();
  m_active = false;
  m_suspended = false;
  setTask(NULL);
  wc.wakeAll();;
  mutex.unlock();
//  QThread::terminate(); //WARNING: including this causes the dude to hang
}


void DeleteThreads() {
  for (int t = core_nprocs - 1; t >= 1; t--) {
    QTaskThread* th = (QTaskThread*)threads[t];
    if (th->isActive()) {
      th->terminate();
    }
    while (!th->isFinished());
    delete th;
  }
}

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
  float* val; // pointer to the net guy to use
  float dwt;			// delta-weight
  float	pdw;
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
  float act;			// activation value
  union {
    float net;			// net input value
    float net_p[core_max_nprocs]; // partial net for some algos
  };
  Connection* send_wts;		// sending weights
  Unit_List	targs;
  ConSpec*	cs;
  char dummy[20]; // bump a bit, to spread these guys out
  
  Connection* 	Cn(int i) const { return &(send_wts[i]); }
  // #CAT_Structure gets the connection at the given index
  Unit*		Un(int i) const { return targs.FastEl(i); }
  
  bool		DoDelta(); // returns true if we should do a delta; typically we just use a random %, ex. 8%
  
  
  Unit();
  ~Unit();
protected:
  int		my_rand; // random value
};

class Layer {
public:
  static int	un_to_idx; // circular guy we globally use to pick next target unit
  
  Unit_List	units;
  
  void		Connect(Layer* to); // connect from me to
  
  Layer();
  ~Layer();
};
int Layer::un_to_idx;


// network configuration information

int n_layers;
int n_units;			// number of units per layer
int n_cons; // number of cons per unit
int n_units_flat; // total number of units (flattened, all layers)
int n_cycles;			// number of cycles of updating
Layer** layers;		// layers 
Unit_List units_flat;		// layers = arrays of units
int n_tot; // total units (reality check)
bool nibble = true; // setting false disables nibbling and adds sync to loop
bool single = false; // true for single thread mode, to compare against nprocs=1
bool safe = false; // default is true for single, false for multi -- whether uses the net_p[]
int send_act = 0x10000; // send activation, as a fraction of 2^16 -- 0 if 100%
int this_rand; // assigned a new random value each cycle, to let us randomize unit acts

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
  for (int i = 0; i < core_max_nprocs; ++i) net_p[i] = 0;
  my_rand = rand();
}

Unit::~Unit() {
  delete[] send_wts;
}

bool Unit::DoDelta() {
  return ((this_rand ^ my_rand) & 0xffff) < send_act;
} 

// construct the network

Layer::Layer() {
}

Layer::~Layer() {
  while (units.size > 0) 
    delete units.FastEl(units.size--);
}

void Layer::Connect(Layer* lay_to) {
  for (int u_fr = 0; u_fr < this->units.size; u_fr++) {
    Unit* un_fr = units.FastEl(u_fr);
    for (int con = 0; con < n_cons; con++) {
      Unit* un_to = lay_to->units.FastEl(un_to_idx);
      un_fr->targs.Set(un_to, un_fr->targs.size);
      Connection& cn = un_fr->send_wts[un_fr->targs.size++];
      cn.wt = (4.0 * (float)rand() / RAND_MAX) - 2.0;
      
      if (++un_to_idx >= n_units) un_to_idx = 0;
    }
  }
}

void MakeNet() {
  layers = new Layer*[n_layers];
  n_units_flat = n_units * n_layers;
  units_flat.Alloc(n_units_flat);
  
  ConSpec* cs = new ConSpec; // everyone shares
  // make all layers and units first
  for (int l = 0; l < n_layers; l++) {
    Layer* lay = new Layer;
    layers[l] = lay;
    lay->units.Alloc(n_units);
    
    for (int i=0;i<n_units;i++) {
      Unit* un = new Unit;
      un->send_wts = new Connection[n_cons * 2];
      un->targs.Alloc(n_cons * 2);
      
      lay->units.Set(un, lay->units.size++);
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
  }
}

// delete the network

void DeleteNet() {
  for(int l=0;l<n_layers;l++) {
    Layer* lay = layers[l];
    delete lay;
  }
  delete[] layers;  
}


class NetInTask: public Task {
public:
  int		g_u; 
  int		t_tot;
  
  NetInTask();
};
//int NetInTask::g_u; 

class NetInTask_0: public NetInTask {
// for single threaded approach -- optimum for that
public:
  void		run();
};

class NetInTask_N: public NetInTask {
// for N threaded approach -- optimum for that
public:
  void		run();
};

void ComputeNets() {
  this_rand = rand(); // for this cycle
  // compute net input = activation times weights
  // this is the "inner loop" that takes all the time!
  
 // NetInTask::g_u = 0;
  // start all the other threads first...
  // have to suspend then resume in case not finished from last time
  for (int t = 1; t < core_nprocs; ++t) {
    QTaskThread* th = (QTaskThread*)threads[t];
    NetInTask* tsk = netin_tasks[t];
//    th->suspend(); // prob already suspended
    tsk->g_u = t;
    th->resume();
  }
  
  // then do my part
  NetInTask* tsk = netin_tasks[0];
  tsk->g_u = 0;
  tsk->run();
  // then lend a "helping hand"
  for (int t = 1; t < core_nprocs; ++t) {
    if (nibble) {
      NetInTask* tsk = netin_tasks[t];
      // note: its ok if tsk finishes between our test and calling run
      if (tsk->g_u < n_units_flat)
        tsk->run();
    } else { // need to sync
      QTaskThread* th = (QTaskThread*)threads[t];
      th->suspend(); // suspending is syncing with completion of loop
    }
  }
}


float ComputeActs() {
  // compute activations (only order number of units)
  float tot_act = 0.0f;
  if (!safe) {
    for(int l=0;l<n_layers;l++) {
      Layer* lay = layers[l];
      for(int i=0;i<lay->units.size;i++) {
        Unit& un = *(lay->units.FastEl(i));
        un.act = 1.0f / (1.0f + expf(-un.net));
        un.net = 0.0f; // only needed for sender-based, but cheaper to just do than test
        tot_act += un.act;
      }
    }
  } else { // safe
    for(int l=0;l<n_layers;l++) {
      Layer* lay = layers[l];
      for(int i=0;i<lay->units.size;i++) {
        Unit& un = *(lay->units.FastEl(i));
        // accumulate subnets into net[0]
        float& net = un.net_p[0];
        for (int j = 1; j < core_nprocs; ++j) {
          net += un.net_p[j];
          un.net_p[j] = 0; // for next time
        }
        un.act = 1.0f / (1.0f + expf(-net));
        un.net = 0.0f; // for next time
        tot_act += un.act;
      }
    }
  }
  return tot_act;
}

////////////////////////////////
// core code


NetInTask::NetInTask() {
  g_u = 0;
  t_tot = 0;
}

float tru_net;
inline void Send_Netin_inner_0(float cn_wt, float* ru_net, float su_act_eff) {
  *ru_net += su_act_eff * cn_wt;
  //tru_net = *ru_net + su_act_eff * cn_wt;
}
void Send_Netin_0(Unit* su) {
  float su_act_eff = su->act;
  Connection* cns = su->send_wts; // array pointer
  Unit** uns = su->targs.Els(); // unit pointer
  for(int i=0; i<su->targs.size; i++)
    Send_Netin_inner_0(cns[i].wt, &(uns[i]->net), su_act_eff);
}


void NetInTask_0::run() {
  while (g_u < n_units_flat) {
    Unit* un = units_flat.FastEl(g_u); //note: accessed flat
    if (un->DoDelta()) {
      Send_Netin_0(un);
      AtomicFetchAdd(&n_tot, 1); // note: we use this because we have to measure it regardless, don't penalize
      //++t_tot;
    }
    ++g_u;
  }
}

void Send_Netin_N_safe(Unit* su, int task_id) {
  float su_act_eff = su->act;
  Connection* cns = su->send_wts; // array pointer
  Unit** uns = su->targs.Els(); // unit pointer
  for(int i=0; i< su->targs.size; i++)
    Send_Netin_inner_0(cns[i].wt, &(uns[i]->net_p[task_id]), su_act_eff);
}

void NetInTask_N::run() {
  int my_u = AtomicFetchAdd(&g_u, core_nprocs);
  while (my_u < n_units_flat) {
    Unit* un = units_flat.FastEl(my_u); //note: accessed flat
    if (un->DoDelta()) {
      if (safe) Send_Netin_N_safe(un, task_id);
      else      Send_Netin_0(un);
      AtomicFetchAdd(&n_tot, 1);
      //AtomicFetchAdd(&t_tot, 1); // because of helping hand clobbers
    }
    my_u = AtomicFetchAdd(&g_u, core_nprocs);
  }
}


void MakeThreads() {
  if (single) {
    NetInTask* tsk = new NetInTask_0;
    tsk->task_id = 0;
    netin_tasks[0] = tsk;
  } else for (int i = 0; i < core_nprocs; i++) {
    NetInTask* tsk = new NetInTask_N;
    tsk->task_id = i;
    netin_tasks[i] = tsk;
    if (i == 0) continue;
    QTaskThread* th = new QTaskThread;
    threads[i] = th;
    th->setTask(tsk);
    th->start(); // starts paused
  }
}



int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  bool hdr = false;
  
  if(argc < 4) {
    printf("must have min 3 args:\n"
      "\t<n_units>\tnumber of units in each of the layers\n"
      "\t<n_cycles>\tnumber of cycles\n"
      "\t<n_procs>\tnumber of cores or procs (0=fast single-threaded model)\n"
      "optional positional params -- none can be skipped: \n"
      "\t<n_lays>\tnumber of layers (min 1, def=2)\n"
      "\t<n_cons>\tnumber of cons per unit (def=n_units)\n"
      "\t<n_cons>\tnumber of cons per unit (def=n_units)\n"
      "\t<send_act>\tpercent (/100) avg activation level (def = 100)\n"
      "optional commands: \n"
      "\t-safe=1\tfor n_procs>=1 use the safe (vectored .net[] in) (def=1)\n"
      "\t-safe=0\tfor n_procs>=1 use the unsafe (single .net in) (def=1)\n"
      "\t-log=0\tdo not log optional values to ptest_core.log\n"
      "\t-log=1\t(def) log optional values to ptest_core.log\n"
    );
    return 1;
  }

  
  bool use_log_file = true;

  srand(56);			// always start with the same seed!

  n_units = (int)strtol(argv[1], NULL, 0);
  //TODO: parameterize this!
  n_units_flat = n_units * n_layers;
  n_cycles = (int)strtol(argv[2], NULL, 0);
  core_nprocs = (int)strtol(argv[3], NULL, 0);
  if (core_nprocs <= 0) {
    single = true;
    safe = false;
    core_nprocs = 1;
  } else {
    single = false;
    safe = true;
    if (core_nprocs > core_max_nprocs) core_nprocs = core_max_nprocs;
  }
  
  // optional positional params
  n_layers = 2; // def
  if ((argc > 4) && (*argv[4] != '-')) {
    n_layers = (int)strtol(argv[4], NULL, 0);
    if (n_layers <= 0) n_layers = 1;
    if (n_layers > 128) n_layers = 128;
  }
  n_cons = n_units; // def
  if ((argc > 5) && (*argv[5] != '-')) {
    n_cons = (int)strtol(argv[5], NULL, 0);
    if ((n_cons <= 0) || (n_cons > n_units)) 
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
    if (strcmp(argv[arg], "-safe=1") == 0)
      single = true;
    if (strcmp(argv[arg], "-safe=0") == 0)
      single = false;
    if (strcmp(argv[arg], "-header") == 0)
      hdr = true;
    if (strcmp(argv[arg], "-log=1") == 0)
      use_log_file = true;
    if (strcmp(argv[arg], "-log=0") == 0)
      use_log_file = false;
    //TODO: any more
  }
  
  MakeThreads();
  MakeNet();

  FILE* logfile;
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
    ComputeNets();
    float tot_act = ComputeActs();
    if(use_log_file)
      fprintf(logfile,"%d\t%g\n", cyc, tot_act);
  }

  time_used.GetUsed(start);


//     time_used.OutString(stdout);
//     printf("\n");

  double tot_time = time_used.tot / (double)time_used.ticks_per/*1.0e2*/;

  double n_wts = n_layers * n_units * n_cons * 2.0;   // bidirectional connected layers
  // cons travelled will depend on activation percent, so we calc exactly
  double n_con_trav = n_tot * n_cons * 2.0;
  double con_trav_sec = ((double)n_con_trav / tot_time) / 1.0e6;

  if(use_log_file) {
    fprintf(logfile,"\nthread\tt_tot\tstart lat\trun time\n");
    for (int t = 0; t < core_nprocs; t++) {
      NetInTask* tsk = netin_tasks[t];
      QTaskThread* th = (QTaskThread*)threads[t];
      double start_lat = 0;
      double run_time = 0;
      if (t > 0) {
        start_lat = th->start_latency.elapsed();
        run_time = th->run_time.elapsed();
      }
      fprintf(logfile,"%d\t%d\t%g\t%g\n", t, tsk->t_tot, start_lat, run_time);
    }
  }
  if(use_log_file)
    fclose(logfile);

  if (hdr)
  printf("Mcon\tsnd_act\tprocs\tlayers\tunits\tcons\tweights\tcycles\tcon_trav\tsecs\tn_tot\n");
  if (single) core_nprocs = 0;
  printf("%g\t%d\t%d\t%d\t%d\t%d\t%g\t%d\t%g\t%g\t%d\n",
    con_trav_sec, tsend_act, core_nprocs, n_layers, n_units, n_cons, n_wts, n_cycles, n_con_trav, tot_time, n_tot);

  DeleteThreads();
  DeleteNet();
}

