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

class TimeUsed {
  // stores and computes time used for processing information
public:
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
  long ticks_per = sysconf(_SC_CLK_TCK);
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
const int core_max_nprocs = 64; // maximum number of processors!
const int unit_gran = 16; // granularity per greedy grab
QThread* threads[core_max_nprocs]; // only core_nprocs created, none for [0] (done in main thread)
NetInTask* netin_tasks[core_max_nprocs]; // only core_nprocs created, none for [0] (done in main thread)

// returns value of i before inc, then incs
#ifdef Q_OS_MAC
#include <libkern/OSAtomic.h>
#define  __sync_fetch_and_add(a, b) (OSAtomicAdd32(b,a)-1)
#else // linux
inline int __sync_fetch_and_add(int * operand, int incr)
{
    asm volatile (
        "lock xaddl %0, %1\n" // add incr to operand
        :  "=r" (incr) // no output
        : "m" (*operand), "0" (incr)
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
  inline bool	isActive() const {return m_active;}
  inline bool	isSuspended() const {return m_suspended;}
  
  Task*		task() const {return m_task;}
  void		setTask(Task* t) {m_task = t;}
  
//  void 		suspend();
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
  m_active = false;
  m_suspended = true;
}

/*void QTaskThread::suspend() {
  if (m_suspended) return;
  m_suspended = true;
  wc.wait(&mutex);;
}*/

void QTaskThread::resume() {
  if (!m_suspended) return;
  mutex.lock();
  m_suspended = false;
  wc.wakeAll();;
  mutex.unlock();
}

void QTaskThread::run() {
  m_active = true;
  m_thread_id = currentThreadId();
  while (m_active) {
    mutex.lock();
    while (m_suspended)
      wc.wait(&mutex);
      
    if (m_task)
      m_task->run();
    m_suspended = true;
    mutex.unlock();
  }
}

void QTaskThread::terminate() {
  m_active = false;
  if (!isFinished()) resume();
  QThread::terminate();
}


void DeleteThreads() {
  for (int t = core_nprocs - 1; t >= 1; t--) {
    QTaskThread* th = (QTaskThread*)threads[t];
    if (th->isRunning()) {
      th->terminate();
    }
    //delete th;
  }
}

// Now: actual neural network code
// units compute net = sum_j weight_j * act_j for all units that send input into them
// then share this net across nodes
// and then compute activations, and repeat.

class Unit;

class Connection {
  // one connection between units
public:
  float wt;			// connection weight value
  float dwt;			// delta-weight
  Unit* su;			// sending unit
};

class Unit {
  // a simple unit
public:
  float act;			// activation value
  float net;			// net input value
  Connection* recv_wts;		// receiving weights

};

// network configuration information

const int n_layers = 2;
int n_units;			// number of units per layer
int n_units_flat; // total number of units (flattened, all layers)
int n_cycles;			// number of cycles of updating
Unit* layers[n_layers];		// layers = arrays of units
Unit** layers_flat;		// layers = arrays of units
int n_tot; // total units (reality check)

// construct the network

void MakeNet() {
  layers[0] = new Unit[n_units];
  layers[1] = new Unit[n_units];
  layers_flat = new Unit*[n_units_flat];
  int i_fl = 0;
  for(int i=0;i<n_units;i++) {
    Unit& un1 = layers[0][i];
    Unit& un2 = layers[1][i];
    layers_flat[i_fl++] = &un1;
    layers_flat[i_fl++] = &un2;

    // random initial activations [0..1]
    un1.act = (float)rand() / RAND_MAX;
    un2.act = (float)rand() / RAND_MAX;

    un1.recv_wts = new Connection[n_units];
    un2.recv_wts = new Connection[n_units];

    // setup reciprocal connections between units
    for(int j=0;j<n_units;j++) {
      Connection& cn1 = un1.recv_wts[j];
      Connection& cn2 = un2.recv_wts[j];
      cn1.su = &(layers[1][j]);
      cn2.su = &(layers[0][j]);

      // random weight values [-2..2]
      cn1.wt = (4.0 * (float)rand() / RAND_MAX) - 2.0;
      cn2.wt = (4.0 * (float)rand() / RAND_MAX) - 2.0;
    }
  }
}

// delete the network

void DeleteNet() {
  for(int l=0;l<n_layers;l++) {
    for(int i=0;i<n_units;i++) {
      Unit& un = layers[l][i];
      delete un.recv_wts;
    }
    delete layers[l];  
  }
}

int l;
int g_u;

class NetInTask: public Task {
public:
  int		t_tot;
  void		run();
  
  NetInTask();
};


void ComputeNets() {
  // compute net input = activation times weights
  // this is the "inner loop" that takes all the time!
  
/*  for(l=0;l<n_layers;l++) {
    g_u = 0;
    // start all the other threads first...
    for (int t = 1; t < core_nprocs; ++t)
      threads[t]->start();
      
    // and then work on it myself too (if others haven't finished yet!)
    ComputeNets_inner();
  }*/
  g_u = 0;
  // start all the other threads first...
  for (int t = 1; t < core_nprocs; ++t) {
    QTaskThread* th = (QTaskThread*)threads[t];
    th->resume();
  }
  
  // and then work on it myself too (if others haven't finished yet!)
  netin_tasks[0]->run();
}


float ComputeActs() {
  // compute activations (only order number of units)
  float tot_act = 0.0f;
  for(int l=0;l<n_layers;l++) {
    for(int i=0;i<n_units;i++) {
      Unit& un = layers[l][i];
      un.act = 1.0f / (1.0f + expf(-un.net));
      tot_act += un.act;
    }
  }
  return tot_act;
}

////////////////////////////////
// core code


NetInTask::NetInTask() {
  t_tot = 0;
}

void NetInTask::run() {
// granular greedy
  int my_u = __sync_fetch_and_add(&g_u, unit_gran);
  int gran_ct = unit_gran;
  while (my_u < n_units_flat) {
    Unit& un = *(layers_flat[my_u]); //note: accessed flat
    un.net = 0.0f;

    for(int j=0;j<n_units;j++) {
      un.net += un.recv_wts[j].wt * un.recv_wts[j].su->act;
    }
    __sync_fetch_and_add(&n_tot, 1);
    ++t_tot;
    if (--gran_ct == 0) {
      gran_ct = unit_gran;
      my_u = __sync_fetch_and_add(&g_u, unit_gran);
    } else ++my_u;
  }
}

void MakeThreads() {
  for (int i = 0; i < core_nprocs; i++) {
    NetInTask* tsk = new NetInTask;
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
  
  if(argc < 3) {
    printf("must have 3 args:\n\t<n_units>\tnumber of units in each of 2 layers\n");
    printf("\t<n_cycles>\tnumber of cycles\n");
    printf("\t<n_procs>\tnumber of cores or procs\n");
    return 1;
  }

  
  bool use_log_file = false;

  srand(56);			// always start with the same seed!

  n_units = (int)strtol(argv[1], NULL, 0);
  n_units_flat = n_units * n_layers;
  n_cycles = (int)strtol(argv[2], NULL, 0);
  core_nprocs = (int)strtol(argv[3], NULL, 0);
  if (core_nprocs <= 0) core_nprocs = 1;
  else if (core_nprocs > core_max_nprocs) core_nprocs = core_max_nprocs;

  MakeThreads();
  MakeNet();

  FILE* logfile;
  if(use_log_file)
    logfile = fopen("ptest_core.log", "w");

  TimeUsed time_used;	time_used.rec = true;
  TimeUsed start;  	start.rec = true;
  start.GetTimes();

  // this is the key computation loop
  for(int cyc = 0; cyc < n_cycles; cyc++) {
    ComputeNets();
    float tot_act = ComputeActs();
    if(use_log_file)
      fprintf(logfile,"%d\t%g\n", cyc, tot_act);
  }

  time_used.GetUsed(start);

  if(use_log_file)
    fclose(logfile);

//     time_used.OutString(stdout);
//     printf("\n");

  double tot_time = time_used.tot / 1.0e2;

  double n_wts = n_units * n_units * 2.0f;   // 2 fully connected layers
  double n_con_trav = n_wts * n_cycles;
  double con_trav_sec = ((double)n_con_trav / tot_time) / 1.0e6;
  int n_tot_exp = n_units * n_layers * n_cycles;

  printf("thread\tt_tot\n");
  for (int t = 0; t < core_nprocs; t++) {
    NetInTask* tsk = netin_tasks[t];
    printf("%d\t%d\n", t, tsk->t_tot);
  }


  printf("procs, %d, \tunits, %d, \tweights, %g, \tcycles, %d, \tcon_trav, %g, \tsecs, %g, \tMcon/sec, %g, \tn_tot (exp/act), %d, %d\n",
          core_nprocs, n_units, n_wts, n_cycles, n_con_trav, tot_time, con_trav_sec, n_tot_exp, n_tot);

  DeleteThreads();
  DeleteNet();
}

