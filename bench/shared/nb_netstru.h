#ifndef NB_NETSTRU_H
#define NB_NETSTRU_H


#include "nb_util.h"

#include <QtCore/QString>


class Conn;
class RecvCons;
class SendCons;
class ConSpec;
class UnitSpec;
class Unit;
class Layer;
class Network;
class NetEngine;
class NetTask;
typedef taList<NetTask>		NetTaskList;

// note: classes of same-name are similar in structure to
// the corresponding Emergent/Leabra classes


/* Conns and wts are owned by the receiving unit
   (via a RecvCon entry per projection.)
   
   The wts in a RecvCon object are ordered by
   Conn index (i.e., in the same order.) Therefore,
   access to the RecvCon wt is done via the same
   index as used to access the Con
   
   The SendCon object has a list of indexes that
   correspond to the target Conn/wt in the RecvCon
   (target) list. Therefore, the sender-based Netin
   process must use an indirect lookup for the targets.
   
   The benefit, is that the Recv-based routines 
   access the wt directly, and it avoids an additional
   index in the Conn obj itself
  
*/
class Conn {
  // one connection between units
public:
  
  float 	dwt;	// delta-weight
  float		pdw;
  float		sravg;	// #NO_SAVE average of sender and receiver activation 
  //Conn() {dwt = pdw = sravg = 0.0f;}
};

typedef taArray<Conn>	ConArray;
typedef taPtrList<Conn>	ConPtrList; // SendCons
typedef taPtrList<Unit>		UnitPtrList;
typedef taList<Unit>		UnitList;

class ConsBase {
public:
  int_Array		units; // global indexes of the units
  int			size; // number of connections
  void			setSize(int i) 
    {if (i == size) return; setSize_impl(i); size = i;}
  
  inline int		Uni(int i) {return units.el[i];}
  
  virtual ~ConsBase() {size = 0;}
protected:
  virtual void		setSize_impl(int);
};

/* The wts and cons array pointers are actually subpointers
  into the Network master arrays. These are allocated globally
  during Build, and cannot be individually replaced or altered.
*/
class RecvCons: public ConsBase {
INHERITED(ConsBase)
public:
  float*		wts; // flat array -- ReadOnly!!! you must access safely!!!
  Conn*			cons; // flat array -- ReadOnly!!! you must access safely!!!
  float			dwt_mean;
  int			wti_base; // base index for the wts in net->g_wts
  
  inline float&		Wt(int i) {return wts[i];}
  inline Conn*		Cn(int i) {return &(cons[i]);}
  
  int		send_idx;
  Layer*	send_lay; // sending layer

  RecvCons();
  ~RecvCons() {}
protected:
};

typedef taList<RecvCons>	RecvCons_List;

/* The cons contains *global* array indexes
*/
class SendCons: public ConsBase {
INHERITED(ConsBase)
public:
  int_Array		cons; // global indexes of the cons/wts
  
  inline int		Cni(int i) {return cons.el[i];}
  inline int		Wti(int i) {return cons.el[i];} // note: same as Cni
  
  int		recv_idx;
  Layer*	recv_lay; // receiving layer
  
  SendCons() { recv_idx = -1; recv_lay = NULL;}
  ~SendCons() {}
protected:
  override void		setSize_impl(int i);
};

typedef taList<SendCons>	SendCons_List;

class ConSpec {
public:
  float		cur_lrate;
  float		norm_pct;
  MinMax	wt_limits;
  void 		Compute_Weights_CtCAL(Network* net, RecvCons* cg, Unit* ru);
  void 		Compute_SRAvg(Network* net, Unit* ru);
  void 		Compute_Weights(Network* net, Unit* ru);
  
  int	dummy[8];
  ConSpec();
};


class UnitSpec {
public:
  int	dummy[8];
  
  UnitSpec();
};


class Unit {
  // a simple unit
public:
  RecvCons_List	recv;
  SendCons_List send;
  int		uni; // flat index
  float		act_avg;
  int		task_id; // which task will process this guy
  int		n_recv_cons;
  ConSpec*	cs;
  UnitSpec*	spec;
  bool		do_delta; // for sender and recv_smart
  int		n_con_calc; // total number of con calcs done for this unit
  
//  inline float&	act() {return g_acts[uni];}
//  inline float&	net() {return g_nets[uni];}
  
  void		CalcDelta(); // sets delta if we should do a delta
  
  Unit();
  ~Unit();
protected:
  int		my_rand; // random value
  char 		dummy[200]; // add cruft to make it more like leabra ;)
};

class Layer {
public:
  Network*	net;
  int		un_to_idx; // circular guy we use to pick next target unit
  
  UnitList	units;
  
  
  void		ConnectFrom(Layer* lay_fm); // connect from me to
  void 		ConnectTo(Layer* lay_to) {lay_to->ConnectFrom(this);}
  
  Layer();
  ~Layer();
};

typedef taList<Layer>	LayerList;

class Network {
public:
  static bool		recv_based; // for recv-based algos

// global lists accessors for them
// Units -- all indexes are commensurable
  int			n_units_flat; // number of global units (acts, nets, etc.)
  Unit**		g_units; // global units 
  float*		g_acts; // global acts
  float*		g_nets; // global nets

  
  // Conns
  float*		g_wts; // global weights
  Conn*			g_cons;
  static int		g_next_wti; // next wti, only valid during Build
  
  int			cycle;
  
  
  NetEngine*		engine;
  LayerList		layers;
  
  
  
  void		Build(); 
  void 		ComputeNets();
  float 	ComputeActs();
  void		Compute_SRAvg();
  void		Compute_Weights();
  
  double	GetNTot(); // get total from all units
  
  void		SetEngine(NetEngine* engine);
  virtual void	Initialize(); // call after ctor, and engine set
  
  void		Cycle(int n_cycles = 1); // do one net cycle
  
  Network();
  virtual ~Network();
protected:
  UnitPtrList 		units_flat;	// all units, flattened
  float_Array		acts_flat; // global acts
  float_Array		nets_flat; // global nets
  
  float_Array		wts_flat; // global wts
  ConArray		cons_flat; // global cons
  
  void			PartitionUnits_RoundRobin(); // for recv, and send-clash
  //void		PartitionUnits_SendClash(); 
};

// macros for initializing and updating proc iter vars

//#define UNIT_STRIDE
#ifdef UNIT_STRIDE

#define PROC_VAR_INIT(g_u, task) \
  g_u = task

#define PROC_VAR_LOOP(my_u, g_u, total) \
  for ( \
    int my_u = AtomicFetchAdd(&g_u, NetEngine::n_procs); \
    my_u < total; \
    my_u = AtomicFetchAdd(&g_u, NetEngine::n_procs))
#else

#define PROC_VAR_INIT(g_u, task) \
  g_u = (task) * NetEngine::proc_stride

#define PROC_VAR_LOOP(my_u, g_u, total) \
  for ( \
    int my_u = AtomicFetchAdd(&g_u, NetEngine::n_stride); \
    my_u < total; \
    my_u = (++my_u & NetEngine::proc_stride_mask) ? my_u : AtomicFetchAdd(&g_u, NetEngine::n_stride))
#endif

class NetEngine { // prototype, and is also the default type (0-procs)
public:
  enum Algo { // note: not exhaustive...
    RECV	= 0,
//  RECV_SMART  = 1,	// OBS ignores senders under threshold
    
    SEND_CLASH  = 2, // sender, where writes can clash (or for 1 proc)
    SEND_ARRAY  = 3, // sender, using an array of nets to avoid clashes
#ifdef NB_CUDA
    RECV_CUDA	= 8,
    
    SEND_CUDA   = 10,
#endif

  // flags
    SEND_FLAG	= 2,
    CUDA_FLAG	= 8,
  };
  
  static int 	algo; // the algorithm number
  static NetTaskList net_tasks; // only n_procs created
  
// Chunking and allocating of data, for multi-tasking
  static const int core_max_nprocs = 32; // maximum number of processors!
  static const int 	proc_stride = 8; // items done each iter in loop (must be 2^n)
  static const int 	proc_stride_mask = proc_stride-1; // mask of proc_stride
  static int 		n_procs; // total number of processors/processes
  static int 		n_stride; // n_procs * proc_stride
  
  Network*	net; // owning net, set automatically
  
  virtual void		Initialize(); // called after creation, usually override impl
  virtual void		OnBuild() {} 
  
  // generally don't override:
  void 			ComputeNets();
  virtual float 	ComputeActs();
  virtual void		Compute_SRAvg();
  virtual void		Compute_Weights();
  
  virtual void 		Log(bool hdr); // save a log file
  
  NetEngine() {net = NULL;}
  virtual ~NetEngine();
protected:
  virtual void		Initialize_impl(); // override this
  virtual void 		DoProc(int proc_id);
  virtual void 		ComputeNets_impl(); // inner overridable part
};


class ThreadNetEngine: public NetEngine {
INHERITED(NetEngine) 
public:
  static QThread* threads[core_max_nprocs]; // only n_procs-1 created, none for [0] (main thread)
  
  override void		OnBuild(); 
    
  //ThreadNetEngine();
  ~ThreadNetEngine();
protected:
  override void		Initialize_impl();
  void			DeleteThreads();
  void 			MakeThreads();
  override void 	DoProc(int proc_id);
  override void 	ComputeNets_impl();
  void 			ComputeNets_SendArray();
};


class NetTask: public Task {
public:
  enum Proc {
    P_Recv_Netin,
    P_Send_Netin_Clash,
    P_Send_Netin_Array,
    P_ComputeAct,
    P_ComputeSRAvg,
    P_ComputeWeights
  };
  
  Network*	net;
// All
  int		g_u; 
  int		t_tot; // shared by Xxx_Netin
  int		n_run; // for diagnostics, num times run
  
  void		run();

// Send_Netin
  inline static void Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff);
    // highly optimized inner loop
  void 		Send_Netin_0(Unit* su); // shared by 0 and N
  virtual void	Send_Netin_Clash() {} // NetIn
  virtual void	Send_Netin_Array() {} // only used by Net_N
  
// Recv_Netin
  void 		Recv_Netin_0(Unit* ru); // shared by 0 and N
  virtual void	Recv_Netin(); // default is the _0 version
  
  
// ComputeAct  
  float 	ComputeAct_inner(int uni);
  float		my_act;
  virtual void	ComputeAct(); // default does it globally
  
// Weights
  virtual void	Compute_SRAvg(); // compat with single or threaded
//  virtual void	Compute_dWt(); // compat with single or threaded
  virtual void	Compute_Weights(); // compat with single or threaded
  
  NetTask(NetEngine* engine);
};

void NetTask::Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff) {
  ru_net += su_act_eff * cn_wt;
  //tru_net = *ru_net + su_act_eff * cn_wt;
}


class NetTask_0: public NetTask {
// for single threaded approach -- optimum for that
INHERITED(NetTask)
public:
  void		Send_Netin_Clash();
//  void		ComputeAct();
  NetTask_0(NetEngine* engine): inherited(engine) {}
};

class NetTask_N: public NetTask {
// for N threaded approach -- optimum for that
INHERITED(NetTask)
public:
  float*		excit; // Send_Array only -- task0 uses actual, others are alloc'ed
  
  void		Send_Netin_Clash();
  void		Send_Netin_Array();
  void		Recv_Netin();
  void		ComputeAct();
  
  NetTask_N(NetEngine* engine): inherited(engine) {excit = NULL;}
  ~NetTask_N();
};

class Nb { // global catchall and startup
public:
  static Nb*	inst; // set when created;
  static bool hdr;
// global net params
  static int n_layers;
  static int n_units;			// number of units per layer
  static int n_cons; // number of cons per unit
  static int n_cycles;			// number of cycles of updating
  static Network net;		// global network 
  static int n_tot; // total units done each NetIn (reality check)
  static int n_prjns; // total number of prjns
  static float tot_act; // check on tot act
  static int dwt_rate; // how many cycles per dwt calc; 0=none
  static int sra_rate; // how many cycles per sravg calc; 0=none
  static int tsend_act; // as decimal percent
  static int send_act; // send activation, as a fraction of 2^16 
  static int inv_act; // inverse of activation -- can use to divide
  
  static int this_rand; // assigned a new random value each cycle, to let us randomize unit acts
  
// thread values
  
  static float nibble_thresh; // < 80%
  static signed char nibble_mode; // 0=none, 1=on, 2=auto (> 20% left)
  static signed char fast_prjn; // fast prjns directly access target unit array
  static bool single; // true for single thread mode, to compare against nprocs=1
  static bool calc_act;
  static bool sender; // sender based, else receiver-based
  static bool use_log_file;
  
  TimeUsed 	time_used;
  QString 	log_suff; // suffix
  QString	log_filename;	
  void*		act_logfile; // File*
  
  virtual int	main(); // must be suplied in the main.cpp
  
  int		PreInitialize(); 
  void		Initialize();
  Nb(int argc_, char** argv_) {argc = argc_; argv = argv_; inst = this;}
  virtual ~Nb() {inst = NULL;}
protected:
  int 		argc;
  char**	argv;
  
  QString	prompt; // displayed if user enters no options	
// various result vals, only valid at end
  double tot_time;

  double n_wts;
  double n_con_trav;
  double con_trav_sec;
  double n_eff_con_trav;
  double eff_con_trav_sec;

  virtual void		PreInitialize_impl(int& rval); 
  virtual void		ParseCmdLine(int& rval);
  virtual void		Initialize_impl() {} // extensions
  virtual void		PrintResults();
  virtual NetEngine*	CreateNetEngine(); // called after all Initialize done, 

private:
  Nb();
};

#endif

