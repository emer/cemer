#ifndef NB_NETSTRU_H
#define NB_NETSTRU_H


#include "nb_util.h"

#include <QtCore/QString>

// switches and optional subswitches
//#define UN_IN_CON // true if target unit is in con
//#define USE_V_CON // use slow access to Cn -- for benching
#define CON_RECV 0 // owned by RecvCons
#define CON_SEND 1 // owned by SendCons

#define CON_IN CON_SEND

// template, subswitches, and catch
#if (CON_IN == CON_RECV)
//# define PWT_IN_SEND // put a ptr to wt in SendCons 
#elif (CON_IN == CON_SEND)
#else
# error("CON_IN not set")
#endif

// for SEND_ARRAY, whether to use the asymmetrical version
//#define SEND_ARY_ASYM

// optional subs
class Connection;
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
   Connection index (i.e., in the same order.) Therefore,
   access to the RecvCon wt is done via the same
   index as used to access the Con
   
   The SendCon object has a list of indexes that
   correspond to the target Connection/wt in the RecvCon
   (target) list. Therefore, the sender-based Netin
   process must use an indirect lookup for the targets.
   
   The benefit, is that the Recv-based routines 
   access the wt directly, and it avoids an additional
   index in the Connection obj itself
  
*/
class Connection {
  // one connection between units
public:
#ifdef UN_IN_CON
  Unit*		un; // target unit
#endif
  float		wt;
  float 	dwt;	// delta-weight
  float		pdw;
  float		sravg;	// #NO_SAVE average of sender and receiver activation 
  //Connection() {dwt = pdw = sravg = 0.0f;}
};

typedef taArray<Connection>		ConArray;
typedef taPtrList<Connection>		ConPtrList; // SendCons
typedef taPtrList<Unit>		UnitPtrList;
typedef taList<Unit>		UnitList;


class ConsBase {
public:
  static Connection*	InitCon(SendCons* snd, RecvCons* rcv, 
    Unit* un_fm, Unit* un_to, int i_fm, int i_to, float wt);

#ifdef UN_IN_CON
  virtual Unit*		Un(int i) = 0;
#else
  Unit**		units;
  inline Unit*		Un(int i) {return units[i];}
#endif
  virtual Connection*	V_Cn(int i) = 0; // slow virtual Cn
  int			size; // number of connections
  void			setSize(int i) 
    {if (i == size) return; setSize_impl(i); size = i;}
  
    
  ConsBase();
  virtual ~ConsBase();
protected:
  virtual void		setSize_impl(int);
};

/* The wts array pointers are actually subpointers
  into the Network master array (allocated globally
  during Build) and cannot be individually replaced or altered.
*/
class RecvCons: public ConsBase {
INHERITED(ConsBase)
public:
#if (CON_IN == CON_RECV)
  Connection*		cons; // flat array -- ReadOnly!!! you must access safely!!!
  inline Connection*	Cn(int i) {return &(cons[i]);}
  override Connection*	V_Cn(int i) {return &(cons[i]);}
  inline float&		Wt(int i) {return cons[i].wt;} // SLOW!
# ifdef UN_IN_CON
  override Unit*	Un(int i) {return cons[i].un;} // SLOW!
# endif
#elif (CON_IN == CON_SEND)
  Connection**		cons; // flat array -- ReadOnly!!! you must access safely!!!
  inline Connection*	Cn(int i) {return cons[i];}
  override Connection*	V_Cn(int i) {return cons[i];}
  void			SetCn(Connection* cn, int i) {cons[i] = cn;}
  inline float&		Wt(int i) {return cons[i]->wt;} // SLOW!
# ifdef UN_IN_CON
  Unit**		units;
  inline Unit*		Un(int i) {return units[i];}
# endif
#else
# error("CON_IN not defined")
#endif
  Layer*		send_lay; // sending layer
  int			send_idx;
  float			dwt_mean;

  RecvCons();
  ~RecvCons();
protected:
  override void		setSize_impl(int i);
};

typedef taList<RecvCons>	RecvCons_List;

/* The cons contains *global* array indexes
*/
class SendCons: public ConsBase {
INHERITED(ConsBase)
public:
#if (CON_IN == CON_RECV)
  Connection**		cons; // flat array -- ReadOnly!!! you must access safely!!!
  inline Connection*	Cn(int i) {return cons[i];}
  override Connection*	V_Cn(int i) {return cons[i];}
  void			SetCn(Connection* cn, int i) {cons[i] = cn;}
  inline float&		Wt(int i) {return cons[i]->wt;} // SLOW!
# ifdef UN_IN_CON
  Unit**		units;
  inline Unit*		Un(int i) {return units[i];}
# endif
#elif (CON_IN == CON_SEND)
  Connection*		cons; // flat array -- ReadOnly!!! you must access safely!!!
  inline Connection*	Cn(int i) {return &(cons[i]);}
  override Connection*	V_Cn(int i) {return &(cons[i]);}
  inline float&		Wt(int i) {return cons[i].wt;} // SLOW!
# ifdef UN_IN_CON
  override Unit*	Un(int i) {return cons[i].un;} // SLOW!
# endif
#endif
  Layer*		recv_lay; // receiving layer
  int			recv_idx;
  
  SendCons();
  ~SendCons();
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
  float		act;
  float 	net;
  int		uni; // flat index
  float		act_avg;
  RecvCons_List	recv;
  SendCons_List send;
  ConSpec*	cs;
  UnitSpec*	spec;
  
  int		n_recv_cons;
  int		n_con_calc; // total number of con calcs done for this unit
  bool		do_delta; // for sender and recv_smart
  
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
  
  int			cycle;
  
  
  NetEngine*		engine;
  LayerList		layers;
  
  
  
  void		Build(); 
  void 		ComputeNets();
  float 	ComputeActs();
  void		Compute_SRAvg();
  void		Compute_Weights();
  float*	AllocWts(int sz); // alloc indicated number (from pool created in Build)
  
  double	GetNTot(); // get total from all units
  
  void		SetEngine(NetEngine* engine);
  virtual void	Initialize(); // call after ctor, and engine set
  
  void		Cycle(int n_cycles = 1); // do one net cycle
  
  Network();
  virtual ~Network();
protected:
  UnitPtrList 		units_flat;	// all units, flattened
  float_Array 		wts_flat;
  int			next_wti;
  
  void			PartitionUnits_RoundRobin(); // for recv, and send-clash
  //void		PartitionUnits_SendClash(); 
};

// macros for initializing and updating proc iter vars

//#define UNIT_STRIDE
#ifdef UNIT_STRIDE

#define PROC_VAR_LOOP(my_u, g_u, total) \
  for ( \
    int my_u = AtomicFetchAdd(&g_u, 1); \
    my_u < total; \
    my_u = AtomicFetchAdd(&g_u, 1))
#else

#define PROC_VAR_LOOP(my_u, g_u, total) \
  for ( \
    int my_u = AtomicFetchAdd(&g_u, NetEngine::proc_stride); \
    my_u < total; \
    my_u = (++my_u & NetEngine::proc_stride_mask) ? my_u : AtomicFetchAdd(&g_u, NetEngine::proc_stride))
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
  static const int 	proc_stride = 16; // items done each iter in loop (must be 2^n)
  static const int 	proc_stride_mask = proc_stride-1; // mask of proc_stride
  static int 		n_procs; // total number of processors/processes
//  static int 		n_stride; // n_procs * proc_stride
  
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
  static int	g_u;  // shared by ALL
  
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
  int		t_tot; // shared by Xxx_Netin
  int		n_run; // for diagnostics, num times run
  
  void		run();

// Send_Netin
#ifdef DEBUG
# define Send_Netin_inner_0(cn_wt, ru_net, su_act_eff) \
    ru_net += su_act_eff * cn_wt;
#else // inline
  inline static void Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff);
    // highly optimized inner loop
#endif
  void 		Send_Netin_0(Unit* su); // shared by 0 and N
  virtual void	Send_Netin_Clash() {} // NetIn
  virtual void	Send_Netin_Array() {Send_Netin_Clash();} // only used by Net_N
  
// Recv_Netin
  void 		Recv_Netin_0(Unit* ru); // shared by 0 and N
  virtual void	Recv_Netin(); // default is the _0 version
  
  
// ComputeAct  
  float 	ComputeAct_inner(Unit* un);
  float		my_act;
  virtual void	ComputeAct(); // default does it globally
  
// Weights
  virtual void	Compute_SRAvg(); // compat with single or threaded
//  virtual void	Compute_dWt(); // compat with single or threaded
  virtual void	Compute_Weights(); // compat with single or threaded
  
  NetTask(NetEngine* engine);
};

#ifndef DEBUG
void NetTask::Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff) {
  ru_net += su_act_eff * cn_wt;
}
#endif

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
  static const int	cn_in = CON_IN; // just for logging
  
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

