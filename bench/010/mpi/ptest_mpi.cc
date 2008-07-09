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

// mpi version (dmem = distributed memory is generic handle for this)
#include <mpi.h>
int dmem_proc;			// mpi processor for this job
int dmem_nprocs;		// total number of processors
const int dmem_max_nprocs = 1024; // maximum number of processors!
const int dmem_max_share = 20000; // hack for non-dynamic array object for dmem type info

class DMemShareVar {
  // definition of one single variable (FLOAT, DOUBLE, INT) that is shared across dmem procs
public:
  MPI_Datatype	mpi_type;	// mpi's type for this variable
  int		n_vars;		// total number of variables
  int		max_per_proc;	// maximum number of vars per any one proc
  
  void*		addrs[dmem_max_share];	// addresses for each item to be shared
  int		local_proc[dmem_max_share]; // which proc each guy is local to

  int		n_local[dmem_max_nprocs]; // number of local variables for this proc
  int		recv_idx[dmem_max_nprocs];  // starting indicies into addrs_recv list
  void*		addrs_recv[dmem_max_share]; // addresses in recv format (max_per_proc * nprocs; 000..111..222...)

  float		send_vals[dmem_max_share]; // contiguous array of values, for actual communication (one for each primitive type in real app, with dynamic alloc)
  float		recv_vals[dmem_max_share]; // contiguous array of values, for actual communication (one for each primitive type in real app, with dynamic alloc)

  void	UpdateVar();		// call this after updating the variable info
  void 	SyncVar();		// synchronize variable across procs
};

// in real app, multiple dmemsharevar's can be combined into an aggregate type that
// is shared as a unit across procs

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

  // mpi stuff
  int	dmem_local_proc;	// processor on which this unit is local
};

// network configuration information

const int n_layers = 2;
int n_units;			// number of units per layer
int n_cycles;			// number of cycles of updating
Unit* layers[n_layers];		// layers = arrays of units

// construct the network

void MakeNet() {
  layers[0] = new Unit[n_units];
  layers[1] = new Unit[n_units];

  for(int i=0;i<n_units;i++) {
    Unit& un1 = layers[0][i];
    Unit& un2 = layers[1][i];

    // random initial activations [0..1]
    un1.act = (float)rand() / RAND_MAX;
    un2.act = (float)rand() / RAND_MAX;

    // mpi initialization: assign mpi process number
    un1.dmem_local_proc = i % dmem_nprocs;
    un2.dmem_local_proc = i % dmem_nprocs;

    if(un1.dmem_local_proc != dmem_proc) { // don't allocate if not local
      // but we need to keep the random numbers the same as for a single-proc job
      for(int j=0;j<n_units;j++) {
	rand();
	rand();
      }
      continue;
    }

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

void ComputeNets() {
  // compute net input = activation times weights
  // this is the "inner loop" that takes all the time!
  for(int l=0;l<n_layers;l++) {
    for(int i=0;i<n_units;i++) {
      Unit& un = layers[l][i];
      un.net = 0.0f;

      if(un.dmem_local_proc != dmem_proc) continue; // don't run if not local

      for(int j=0;j<n_units;j++) {
	un.net += un.recv_wts[j].wt * un.recv_wts[j].su->act;
      }
    }
  }
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
// mpi/dmem code

DMemShareVar dmem_net_var;	// netinput variable shared across all procs

void DMem_MakeNetType() {
  dmem_net_var.mpi_type = MPI_FLOAT;
  int tot_cnt = 0;
  for(int l=0;l<n_layers;l++) {
    for(int i=0;i<n_units;i++, tot_cnt++) {
      Unit& un = layers[l][i];
      dmem_net_var.addrs[tot_cnt] = (void*)&(un.net);
      dmem_net_var.local_proc[tot_cnt] = un.dmem_local_proc;
    }
  }
  dmem_net_var.n_vars = tot_cnt;
  dmem_net_var.UpdateVar();
}

void DMemShareVar::UpdateVar() {
  // initialize counts
  for(int i=0;i<dmem_nprocs;i++) {
    n_local[i] = 0;
  }

  for(int i=0;i<n_vars;i++) {
    n_local[local_proc[i]]++;	// increment counts
  }

  // find max
  max_per_proc = 0;
  for(int i=0;i<dmem_nprocs;i++) {
    if(n_local[i] > max_per_proc) max_per_proc = n_local[i];
  }

  static int proc_ctr[dmem_max_nprocs];
  for(int i=0;i<dmem_nprocs;i++) {
    proc_ctr[i] = 0;
    recv_idx[i] = max_per_proc * i;
  }

  // allocate the addrs_recv array (max_per_proc * nprocs; 000..111..222...)
  for(int i=0;i<n_vars;i++) {
    int lproc = local_proc[i];
    addrs_recv[recv_idx[lproc] + proc_ctr[lproc]] = addrs[i];
    proc_ctr[lproc]++;
  }
}

// what we want is an allgather
// need to have a separate buffer for getting the results: needs to be memory-mapped
// as an array of all the same elements.  i.e., recv_type and send_type need to be the
// same for all procs!  as do the counts!

// 0: send: 0000
// 1: send: 1111
// 2: send: 2222
// all recv: 0000 1111 2222

void DMemShareVar::SyncVar() {
  // basic computation here is to send all of my stuff to all other nodes
  // and for them to send all of their stuff to me
  
  // first copy our values into the send array
  int my_idx = recv_idx[dmem_proc];
  int my_n = n_local[dmem_proc];
  int i;
  for(i=0;i<my_n;i++, my_idx++) {
    send_vals[i] = *((float*)addrs_recv[my_idx]); // switch on type in real fun
  }
  
  MPI_Allgather(send_vals, my_n, mpi_type,
		recv_vals, max_per_proc, mpi_type, MPI_COMM_WORLD);

  // now copy stuff back out
  for(int proc=0;proc<dmem_nprocs;proc++) {
    if(proc == dmem_proc) continue; // done!
    int p_idx = recv_idx[proc];
    for(int i=0;i<n_local[proc];i++, p_idx++) {
      *((float*)addrs_recv[p_idx]) = recv_vals[p_idx]; // switch on type in real fun
    }
  }
}

void SyncNets() {
  dmem_net_var.SyncVar();
}

int main(int argc, char* argv[]) {
  if(argc < 2) {
    printf("must have 2 args:\n\t<n_units>\tnumber of units in each of 2 layers\n");
    printf("\t<n_cycles>\tnumber of cycles\n");
    return 1;
  }

  bool use_log_file = false;

  srand(56);			// always start with the same seed!

  n_units = (int)strtol(argv[1], NULL, 0);
  n_cycles = (int)strtol(argv[2], NULL, 0);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &dmem_nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &dmem_proc);

  MakeNet();

  DMem_MakeNetType();

  FILE* logfile;
  if(use_log_file && dmem_proc == 0)
    logfile = fopen("ptest_mpi.log", "w");

  TimeUsed time_used;	time_used.rec = true;
  TimeUsed start;  	start.rec = true;
  start.GetTimes();

  // this is the key computation loop
  for(int cyc = 0; cyc < n_cycles; cyc++) {
    ComputeNets();
    SyncNets();
    float tot_act = ComputeActs();
    if(use_log_file && dmem_proc == 0)
      fprintf(logfile,"%d\t%g\n", cyc, tot_act);
  }

  time_used.GetUsed(start);

  if(dmem_proc == 0) {
    if(use_log_file)
      fclose(logfile);

//     time_used.OutString(stdout);
//     printf("\n");

    double tot_time = time_used.tot / 1.0e2;

    double n_wts = n_units * n_units * 2.0f;   // 2 fully connected layers
    double n_con_trav = n_wts * n_cycles;
    double con_trav_sec = ((double)n_con_trav / tot_time) / 1.0e6;

    printf("procs, %d, \tunits, %d, \tweights, %g, \tcycles, %d, \tcon_trav, %g, \tsecs, %g, \tMcon/sec, %g\n",
	   dmem_nprocs, n_units, n_wts, n_cycles, n_con_trav, tot_time, con_trav_sec);
  }

  DeleteNet();
  MPI_Finalize();
}

