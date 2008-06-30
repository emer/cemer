#ifndef NB_NETSTRU_H
#define NB_NETSTRU_H


#include "nb_util.h"

#include <QtCore/QString>

// Now: actual neural network code
// units compute net = sum_j weight_j * act_j for all units that send input into them
// then share this net across nodes
// and then compute activations, and repeat.

class Conn;
class RecvCons;
class ConSpec;
class UnitSpec;
class Unit;
class Layer;
class Network;
class NetEngine;
class NetTask;
typedef taList<NetTask>		NetTaskList;

// note: some of these classes are same-name and similar in structure to
// the corresponding Emergent/Leabra classes

// Accessor macros

#ifdef FLAT_WT
# define CN_WT(cn) (Network::wts[cn->wti])
#else
# define CN_WT(cn) (cn->wt)
#endif

class Conn {
  // one connection between units
public:
  static float*		g_wts; // global weights
  static int		g_next_wti; // next wti
  
  int	 	wti;	// index of weight value
  inline float&	wt() const {return g_wts[wti];}
  float 	dwt;	// delta-weight
  float		pdw;
  float		sravg;	// #NO_SAVE average of sender and receiver activation 
};

typedef taArray<Conn>	ConArray;
typedef taPtrList<Conn>	ConPtrList; // SendCons
typedef taPtrList<Unit>		UnitPtrList;
typedef taList<Unit>		UnitList;

class ConsBase {
public:
  virtual ConArray*	GetConArray() {return NULL;}
  
  int_Array		units; // global indexes of the units
  
  virtual ~ConsBase() {}
};

class RecvCons: public ConsBase {
public:
  float		dwt_mean;
#ifdef SEND_CONS
  ConPtrList	cons;
  inline Conn*	Cn(int i) {return (Conn*)cons.el[i];}
#else
  ConArray	cons;
  // #NO_FIND #NO_SAVE #CAT_Structure the array of connections, in index correspondence with units
  inline Conn*	Cn(int i) {return &(cons.el[i]);}
  override ConArray*	GetConArray() {return &cons;}
#endif
  int		send_idx;
  Layer*	send_lay; // sending layer

  RecvCons();
  ~RecvCons() {}
};

typedef taList<RecvCons>	RecvCons_List;


class SendCons: public ConsBase {
public:
#ifdef SEND_CONS
  ConArray	cons;
  // #NO_FIND #NO_SAVE #CAT_Structure the array of connections, in index correspondence with units
  override ConArray*	GetConArray() {return &cons;}
  inline Conn*	Cn(int i) {return &(cons.el[i]);}
#else
  ConPtrList	cons;
  // #NO_FIND #NO_SAVE #CAT_Structure list of pointers to receiving connections, in index correspondence with units;
  inline Conn*	Cn(int i) {return (Conn*)cons.el[i];}
#endif
  
  int		recv_idx;
  Layer*	recv_lay; // receiving layer
  
  SendCons() { recv_idx = -1; recv_lay = NULL;}
  ~SendCons() {}
};

typedef taList<SendCons>	SendCons_List;

class ConSpec {
public:
  float		cur_lrate;
  float		norm_pct;
  MinMax	wt_limits;
#ifndef SEND_CONS
  void 		Compute_Weights_CtCAL(RecvCons* cg, Unit* ru);
#endif
  void 		Compute_SRAvg(Unit* ru);
  void 		Compute_Weights(Unit* ru);
    // note: does nothing for SEND_CONS
  
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
  static float*		g_acts; // global acts
  static float*		g_nets; // global nets

//  float 	act;			// activation value index
//  float 	net;			// net input value
  float		act_avg;
  
  RecvCons_List	recv;
  SendCons_List send;
  
  int		task_id; // which task will process this guy
  int		flat_idx; // flat index
  int		n_recv_cons;
  ConSpec*	cs;
  UnitSpec*	spec;
  bool		do_delta; // for sender and recv_smart
  int		n_con_calc; // total number of con calcs done for this unit
  
  inline float&	act() {return g_acts[flat_idx];}
  inline float&	net() {return g_nets[flat_idx];}
  
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
  
#ifdef SEND_CONS
  void		ConnectFrom(Layer* lay_fm) {lay_fm->ConnectTo(this);}
  void 		ConnectTo(Layer* lay_to);
#else
  void		ConnectFrom(Layer* lay_fm); // connect from me to
  void 		ConnectTo(Layer* lay_to) {lay_to->ConnectFrom(this);}
#endif
  
  Layer();
  ~Layer();
protected:
  void		ConnectFrom_Recv(Layer* lay_fm); // connect from me to
  void		ConnectFrom_Send(Layer* lay_fm); // connect from me to
};

typedef taList<Layer>	LayerList;

class Network {
public:
  static bool		recv_based; // for recv-based algos
  static bool		recv_smart; // for recv-based, does the smart recv algo

// global lists and static accessors for them
  static Unit**		g_units; // global units 
  UnitPtrList 		units_flat;	// all units, flattened
  float_Array		acts_flat; // global acts
  float_Array		nets_flat; // global nets
  float_Array		wts_flat; // global wts
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
  
  Network();
  virtual ~Network();
protected:
  void		PartitionUnits_RoundRobin(); // for recv, and send-clash
  //void		PartitionUnits_SendClash(); 
};


class NetEngine { // prototype, and is also the default type (0-procs)
public:
  enum Algo { // note: not exhaustive...
    RECV	= 0,
    RECV_SMART  = 1,		// ignores senders under threshold
    
    SEND_CLASH  = 2, // sender, where writes can clash (or for 1 proc)
    SEND_ARRAY  = 3, // sender, using an array of nets to avoid clashes
#ifdef NB_CUDA
    RECV_CUDA	= 8, // NOTE: requires RECV_CONS (ie !SEND_CONS)
    
    SEND_CUDA   = 10, // NOTE: requires SEND_CONS
#endif

  // flags
    SEND_FLAG	= 2,
    CUDA_FLAG	= 8,
  };
  
  static int 	algo; // the algorithm number
  static const int core_max_nprocs = 32; // maximum number of processors!
  static int n_procs;		// total number of processors
  static NetTaskList net_tasks; // only n_procs created
  
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

typedef void (*Recv_Netin_0_t)(Unit* ru);

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
  
// All
  int		g_u; 
  int		t_tot; // shared by Xxx_Netin
  int		n_run; // for diagnostics, num times run
  
  void		run();

// Send_Netin
  inline static void Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff);
    // highly optimized inner loop
  static void 	Send_Netin_0(Unit* su); // shared by 0 and N
  virtual void	Send_Netin_Clash() {} // NetIn
  virtual void	Send_Netin_Array() {} // only used by Net_N
  
// Recv_Netin
  static 	Recv_Netin_0_t Recv_Netin_0; // set in NetTask::Initialize
  static void 	Recv_Netin_0_Dumb(Unit* ru); // shared by 0 and N
  static void 	Recv_Netin_0_Smart(Unit* ru); // shared by 0 and N
  virtual void	Recv_Netin(); // default is the _0 version
  
  
// ComputeAct  
  static float ComputeAct_inner(int uni);
  float		my_act;
  virtual void	ComputeAct(); // default does it globally
  
// Weights
  virtual void	Compute_SRAvg(); // compat with single or threaded
//  virtual void	Compute_dWt(); // compat with single or threaded
  virtual void	Compute_Weights(); // compat with single or threaded
  
  NetTask();
};

void NetTask::Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff) {
  ru_net += su_act_eff * cn_wt;
  //tru_net = *ru_net + su_act_eff * cn_wt;
}


class NetTask_0: public NetTask {
// for single threaded approach -- optimum for that
public:
  void		Send_Netin_Clash();
//  void		ComputeAct();
};

class NetTask_N: public NetTask {
// for N threaded approach -- optimum for that
public:
  float_Array		excit; // Send_Array only
  
  void		Send_Netin_Clash();
  void		Send_Netin_Array();
  void		Recv_Netin();
  void		ComputeAct();
  
  NetTask_N() {}
};

class Nb { // global catchall and startup
public:
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
  static signed char sndcn; // if 1, uses SEND_CONS, else 0=RECV_CONS
  static bool use_log_file;
  
  TimeUsed 	time_used;
  QString 	log_suff; // suffix
  QString	log_filename;	
  
  virtual int	main(); // must be suplied in the main.cpp
  
  int		PreInitialize(); 
  void		Initialize();
  Nb(int argc_, char** argv_) {argc = argc_; argv = argv_;}
  virtual ~Nb() {}
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

