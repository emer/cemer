#ifndef NB_NETSTRU_H
#define NB_NETSTRU_H


#include "nb_util.h"

#include <QtCore/QString>

// switches and optional subswitches
//#define USE_INT_IDX // true to use indirect ints, else use ptrs
#define WT_RECV 0 // owned by RecvCons
#define WT_SEND 1 // owned by SendCons
#define WT_CONN 2 // is baked into Connection

#define WT_IN WT_SEND

// template, subswitches, and catch
#if (WT_IN == WT_CONN)
#elif (WT_IN == WT_RECV)
#elif (WT_IN == WT_SEND)
//# define PWT_IN_CONN // put a ptr to wt in Connection, instead of using a PtrList 
#else
# error("WT_IN not set")
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
class Projection;
class Layer;
class Network;
class NetEngine;
class NetTask;
typedef taList<NetTask>		NetTaskList;
typedef taList<Projection>		PrjnList;
typedef taPtrList<Projection>		PrjnPtrList;

// note: classes of same-name are similar in structure to
// the corresponding Emergent/Leabra classes


/* Conns are allocated by the recv Projection and owned by the receiving unit
   (via a RecvCon entry per projection.) Wts are owned by
   the "congruent" XxxxCons object for the algorithm:
   RecvCons for recv-based algos (ex. Bp)
   SendCons for send-based algos (ex. Leabra)
   
   The wts in a congruent XxxxCons object are ordered by
   Connection index (i.e., in the same order.) Therefore,
   access to the wt is done via the same
   index as used to access the Con
   
   The incongruent object has a list of pointers that
   correspond to the target Connection/wt in the congruent
   (target) list. Therefore, the sender-based Netin
   process must use an indirect lookup for the targets.
   
   The benefit, is that the Recv-based routines 
   access the wt directly, and it avoids an additional
   index in the Connection obj itself
   
   NOTE: Connection objects are never created individually, only in
   big honkin arrays -- we allocate those using malloc and
   zero them then, so we don't have (slow) ctors
  
*/
class Connection {
  // one connection between units
public:
  float 	dwt;	// delta-weight
};

class BpCon: public Connection {
public:
  float		pdw;
};

class LeabraCon: public Connection {
public:
  float		pdw;
  float		sravg;	// #NO_SAVE average of sender and receiver activation 
};

typedef taPtrList<Connection>	ConPtrList; // points to any subclass of Connection
typedef taPtrList<Unit>		UnitPtrList;
typedef taList<Unit>		UnitList;
typedef taArray<float*>		floatPtrList;

class ConsBase {
public:
  Unit**		units;
  Projection*		prjn; // receiving prjn
  int			size; // number of connections
  
  virtual bool		ownsWts() const {return false;}
  
  inline Unit*		Un(int i) {return units[i];}
  virtual Connection*	Cn(int i) = 0; // generic access to the actual type SLOW!!!
  virtual float&	Wt(int i) = 0; // generic access to the wt SLOW!!!
  virtual void		SetWt(float* /*pwt*/, int /*i*/) {} //NOTE: noop if ownsWts!
  
  void			setSize(int i, Projection* prjn) // cons/wts are alloced on the prjn
    {if (i == size) return; setSize_impl(i, prjn); size = i;}
  
  ConsBase();
  virtual ~ConsBase();
protected:
  virtual void		setSize_impl(int, Projection* prjn);
};

class RecvCons: public ConsBase {
INHERITED(ConsBase)
public:
  Layer*	send_lay; // sending layer
  int		send_idx;
  float		dwt_mean; // used by many algos
  
  
  RecvCons();
  ~RecvCons() {}
};

class RecvCons_recv_impl: public RecvCons {
INHERITED(RecvCons)
public:
  float*		wts; // flat array -- ReadOnly!!! you must access safely!!!
  
  virtual bool		ownsWts() const {return true;}
  override float&	Wt(int i) {return wts[i];} // SLOW!!!!

  RecvCons_recv_impl() {wts = NULL;}
  ~RecvCons_recv_impl() {}
protected:
  override void		setSize_impl(int i, Projection* prjn);
};

class RecvCons_send_impl: public RecvCons {
INHERITED(RecvCons)
public:
  float**		pwts; // flat array -- ReadOnly!!! you must access safely!!!
  
  override float&	Wt(int i) {return *(pwts[i]);} // SLOW!!!!
  override void		SetWt(float* pwt, int i) {pwts[i] = pwt;}

  RecvCons_send_impl() {pwts = NULL;}
  ~RecvCons_send_impl();
protected:
  override void		setSize_impl(int i, Projection* prjn);
};

/* The wts array pointers are actually subpointers
  into the Network master array (allocated globally
  during Build) and cannot be individually replaced or altered.
*/
template<class CN>
class RecvCons_recv: public RecvCons_recv_impl {
INHERITED(RecvCons_recv_impl)
public:
  CN*			cons; // flat array -- ReadOnly!!! you must access safely!!!
  
  override Connection*	Cn(int i) {return &(cons[i]);} // SLOW generic access

  RecvCons_recv() {cons = NULL;}
  ~RecvCons_recv() {cons = NULL;}
protected:
  void 			setSize_impl(int i, Projection* prjn) {
    inherited::setSize_impl(i, prjn);
    cons = crealloc(cons, size, i);
  }
};

template<class CN>
class RecvCons_send: public RecvCons_send_impl {
INHERITED(RecvCons_send_impl)
public:
  CN*			cons; // flat array -- ReadOnly!!! you must access safely!!!
  
  override Connection*	Cn(int i) {return &(cons[i]);} // SLOW generic access

  RecvCons_send() {cons = NULL;}
  ~RecvCons_send() {cons = NULL;}
protected:
  void 			setSize_impl(int i, Projection* prjn) {
    inherited::setSize_impl(i, prjn);
    cons = crealloc(cons, size, i);
  }
};

typedef taPtrList<RecvCons>	RecvConsPtrList;


class SendCons: public ConsBase {
INHERITED(ConsBase)
public:
  int			recv_idx;
  Layer*		recv_lay; // receiving layer
  virtual void		SetCn(Connection* cn, int i) = 0;
  
  SendCons();
  ~SendCons() {}
};

class SendCons_send_impl: public SendCons {
INHERITED(SendCons)
public:
  float*		wts; // flat array -- ReadOnly!!! you must access safely!!!
  
  virtual bool		ownsWts() const {return true;}
  override float&	Wt(int i) {return wts[i];} // SLOW!!!!
  
  SendCons_send_impl() {wts = NULL;}
  ~SendCons_send_impl() {wts = NULL;}
protected:
  override void		setSize_impl(int i, Projection* prjn);
};

class SendCons_recv_impl: public SendCons {
INHERITED(SendCons)
public:
  float**		pwts; // flat array -- ReadOnly!!! you must access safely!!!
  
  override float&	Wt(int i) {return *(pwts[i]);} // SLOW!!!!
  override void		SetWt(float* pwt, int i) {pwts[i] = pwt;}
  
  SendCons_recv_impl();
  ~SendCons_recv_impl();
protected:
  override void		setSize_impl(int i, Projection* prjn);
};

template<class CN>
class SendCons_send: public SendCons_send_impl {
INHERITED(SendCons_send_impl)
public:
  CN**			cons;
  
  override Connection*	Cn(int i) {return cons[i];} // SLOW virtual
  override void		SetCn(Connection* cn, int i) // cn must be of type CN!!!
    {cons[i] = (CN*)cn;}

  SendCons_send() {cons = NULL;}
  ~SendCons_send() {cons = NULL;}
protected:
  void 			setSize_impl(int i, Projection* prjn) {
    inherited::setSize_impl(i, prjn);
    cons = crealloc(cons, size, i);
  }
};

template<class CN>
class SendCons_recv: public SendCons_recv_impl {
INHERITED(SendCons_recv_impl)
public:
  CN**			cons;
  
  override Connection*	Cn(int i) {return cons[i];} // SLOW virtual
  override void		SetCn(Connection* cn, int i) // cn must be of type CN!!!
    {cons[i] = (CN*)cn;}

  SendCons_recv() {cons = NULL;}
  ~SendCons_recv() {cons = NULL;}
protected:
  void 			setSize_impl(int i, Projection* prjn) {
    inherited::setSize_impl(i, prjn);
    cons = crealloc(cons, size, i);
  }
};

// ALGO Specific Guys

// Bp -- recv-based

class BpSendCons: public SendCons_recv<BpCon> {
INHERITED(SendCons_recv<BpCon>)
public:
  
  BpSendCons() {}
  ~BpSendCons() {}
};

class BpRecvCons: public RecvCons_recv<BpCon> {
INHERITED(RecvCons_recv<BpCon>)
public:
  
  BpRecvCons() {}
  ~BpRecvCons() {}
};


// Leabra -- send-based

class LeabraSendCons: public SendCons_send<LeabraCon> {
INHERITED(SendCons_send<LeabraCon>)
public:
  
  LeabraSendCons() {}
  ~LeabraSendCons() {}
};

class LeabraRecvCons: public RecvCons_send<LeabraCon> {
INHERITED(RecvCons_send<LeabraCon>)
public:
  
  LeabraRecvCons() {}
  ~LeabraRecvCons() {}
};



typedef taPtrList<SendCons>	SendConsPtrList;

class ConSpec {
public:
  float			cur_lrate;
  float			norm_pct;
  MinMax		wt_limits;
  
  virtual size_t	GetConSize() const = 0; // size of the Con
  virtual RecvCons*	NewRecvCons() const = 0;
  virtual SendCons*	NewSendCons() const = 0;  
  virtual void 		Compute_Weights(Network* net, Unit* ru) = 0;
  
  int	dummy[8];
  ConSpec();
  virtual ~ConSpec() {}
};

class LeabraConSpec: public ConSpec {
public:
  inline static void Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff);
    // highly optimized inner loop
  void 			Send_Netin_0(Unit* su); // shared by 0 and N

  override size_t	GetConSize() const {return sizeof(LeabraCon);} // size of the Con
  RecvCons*		NewRecvCons() const {return new LeabraRecvCons;}
  SendCons*		NewSendCons() const {return new LeabraSendCons;}  
  override void 	Compute_Weights(Network* net, Unit* ru);
  void 		Compute_Weights_CtCAL(Network* net, LeabraRecvCons* cg, Unit* ru);
  void 		Compute_SRAvg(Network* net, Unit* ru);
};

void LeabraConSpec::Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff) {
  ru_net += su_act_eff * cn_wt;
  //tru_net = *ru_net + su_act_eff * cn_wt;
}

class BpConSpec: public ConSpec {
INHERITED(ConSpec)
public:
  void 			Recv_Netin_0(Unit* ru); // shared by 0 and N
};

class UnitSpec {
public:
  int	dummy[8];
  
  UnitSpec();
};


class Unit {
  // a simple unit
public:
  RecvConsPtrList recv; // note: owned
  SendConsPtrList send; // note: owned
  
  float		act;
  float 	net;
  int		uni; // flat index
  float		act_avg;
  int		n_recv_cons;
  ConSpec*	cs;
  UnitSpec*	spec;
  bool		do_delta; // for sender and recv_smart
  int		n_con_calc; // total number of con calcs done for this unit
  
  void		CalcDelta(); // sets delta if we should do a delta
  
  Unit();
  ~Unit();
protected:
  int		my_rand; // random value
  char 		dummy[200]; // add cruft to make it more like leabra ;)
};

/* Projection
  The Projection manages the memory for the cons and wts for the connections.
  For all algos, the SendCons owns the conns.
  For sender-based algos, the SendCons will own the wts, and thus they will
  be ordered by sender.
  For receiver-based algos, the RecvCons will own the wts, and they will be
  ordered the same as the corresponding cons.
*/

class Projection {
public:
  Layer*		own_lay; // owning layer
  int			size; // number of connections
  int			con_size; // number of bytes per conn -- must be %4 bytes
  
  void			SetSize(int i); // sets number of wts/conns
  float*		AllocWts(int sz); // alloc indicated number (from pool)

  Projection();
  ~Projection();
protected:
  float*		wts; // all weights
  void*			cons; // conn memory
  float_Array 		wts_flat;
  int			next_wti;
};

class Layer {
public:
  Network*	net;
  int		un_to_idx; // circular guy we use to pick next target unit
  
  UnitList	units;
  PrjnList	prjns; // receving prjns (owned)
  PrjnPtrList	send_prjns; // sender links
  
  
  void		ConnectFrom(Layer* lay_fm); // connect from me to
  void 		ConnectTo(Layer* lay_to) {lay_to->ConnectFrom(this);}
  
  Layer();
  ~Layer();
};

typedef taList<Layer>	LayerList;

class Network {
public:
  enum NetType {
    Leabra		= 0,
    Bp			= 1,
  };
  
  static bool		recv_based; // for recv-based algos

// global lists accessors for them
// Units -- all indexes are commensurable
  ConSpec*		con_spec; // we use one global value, of type of network
  int			n_units_flat; // number of global units (acts, nets, etc.)
  Unit**		g_units; // global units 
  
  int			cycle;
  
  
  NetEngine*		engine;
  LayerList		layers;
  
  
  
  void		Build(); 
  void 		ComputeNets();
  float 	ComputeActs();
  
  double	GetNTot(); // get total from all units
  
  void		SetEngine(NetEngine* engine);
  virtual void	Initialize(); // call after ctor, and engine set
  
  virtual void	Cycle(int n_cycles = 1); // do one net cycle
  
  Network();
  virtual ~Network();
protected:
  UnitPtrList 		units_flat;	// all units, flattened
  
  virtual void		Cycle_impl(); // do one net cycle
  void			PartitionUnits_RoundRobin(); // for recv, and send-clash
  //void		PartitionUnits_SendClash(); 
};


class LeabraNetwork: public Network {
INHERITED(Network)
public:
  
  void		Compute_Weights();
  void		Compute_SRAvg();
  LeabraNetwork();
  ~LeabraNetwork();
protected:
  UnitPtrList 		units_flat;	// all units, flattened
  
  override void		Cycle_impl(); // do one net cycle
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
  virtual void	Send_Netin_Clash() {} // NetIn
  virtual void	Send_Netin_Array() {Send_Netin_Clash();} // only used by Net_N
  
// Recv_Netin
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
  static Network* net;		// global network 
  static int n_tot; // total units done each NetIn (reality check)
  static int n_prjns; // total number of prjns
  static float tot_act; // check on tot act
  static int net_type; // network type, def= Leabra
  static int dwt_rate; // how many cycles per dwt calc; 0=none
  static int sra_rate; // how many cycles per sravg calc; 0=none
  static int tsend_act; // as decimal percent
  static int send_act; // send activation, as a fraction of 2^16 
  static int inv_act; // inverse of activation -- can use to divide
  
  static int this_rand; // assigned a new random value each cycle, to let us randomize unit acts
  static const int	wt_in = WT_IN; // just for logging
  
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

