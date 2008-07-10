#ifndef NB_NETSTRU_H
#define NB_NETSTRU_H


#include "nb_util.h"

#include <QtCore/QString>

// switches and optional subswitches

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
typedef taPtrList<Connection>	ConnectionPtrList; // note: typically pts to subclass
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
  float		wt;
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

class FunkyLeabraCon: public LeabraCon {
public:
  float		funky; // an interneural deconfabulating ratecon modulator 
};

typedef taPtrList<Connection>	ConPtrList; // points to any subclass of Connection
typedef taPtrList<Unit>		UnitPtrList;
typedef taList<Unit>		UnitList;
typedef taArray<float*>		floatPtrList;

class ConsBase {
public:
  static Connection*	InitCon(SendCons* snd, RecvCons* rcv, 
    Unit* un_fm, Unit* un_to, int i_fm, int i_to, float wt);
  
  Unit*			own_un; // my owner, for fast access
  Unit**		units;
  Projection*		prjn; // receiving prjn
  int			size; // number of connections
  
  virtual bool		ownsCons() const {return false;}
  
  inline Unit*		Un(int i) {return units[i];}
  virtual Connection*	V_Cn(int i) = 0; // generic access to the actual type SLOW!!!
  
  void			setSize(int i) // cons/wts are alloced on the prjn
    {if (i == size) return; setSize_impl(i); size = i;}
  
  ConsBase();
  virtual ~ConsBase();
protected:
  virtual void		setSize_impl(int);
  virtual void		SetCn(Connection*, int) {} // only impl'ed by non-owner
};

class RecvCons: public ConsBase {
INHERITED(ConsBase)
public:
  int		send_idx;
  Layer*	send_lay; // sending layer
  float		dwt_mean; // used by many algos
  
  RecvCons();
  ~RecvCons() {}
};

class RecvCons_recv_impl: public RecvCons {
INHERITED(RecvCons)
public:
  
  override bool		ownsCons() const {return true;}
  virtual Connection*	V_Cns() = 0; // SLOW!!!!

  RecvCons_recv_impl() {}
  ~RecvCons_recv_impl() {}
};

class RecvCons_send_impl: public RecvCons {
INHERITED(RecvCons)
public:
  
  virtual Connection**	V_Cns() = 0; // SLOW!!!!

  RecvCons_send_impl() {}
  ~RecvCons_send_impl() {}
protected:
  override void		SetCn(Connection* cn, int i) 
    {V_Cns()[i] = cn;}
};

/* The templated guys are useful in the templated ConSpec routines.
  Note that it is ok to cast to a templated version, even if the 
  code further inherits from the template.
*/
template<class CN>
class RecvCons_recv: public RecvCons_recv_impl {
INHERITED(RecvCons_recv_impl)
public:
  CN*			cons; // flat array -- ReadOnly!!! you must access safely!!!
  
  override Connection*	V_Cn(int i) {return &(cons[i]);} // SLOW generic access
  override Connection*	V_Cns() {return cons;} // SLOW!!!!

  RecvCons_recv() {cons = NULL;}
  ~RecvCons_recv() {cons = NULL;}
protected:
  void 			setSize_impl(int i);
};

template<class CN>
class RecvCons_send: public RecvCons_send_impl {
INHERITED(RecvCons_send_impl)
public:
  CN**			cons; // flat array -- ReadOnly!!! you must access safely!!!
  
  override Connection*	V_Cn(int i) {return cons[i];} // SLOW generic access
  override Connection**	V_Cns() {return (Connection**)cons;} // SLOW!!!!

  RecvCons_send() {cons = NULL;}
  ~RecvCons_send() {cons = NULL;}
protected:
  void 			setSize_impl(int i);
};

typedef taPtrList<RecvCons>	RecvConsPtrList;


class SendCons: public ConsBase {
INHERITED(ConsBase)
public:
  int			recv_idx;
  Layer*		recv_lay; // receiving layer
  
  SendCons();
  ~SendCons() {}
};

typedef taPtrList<SendCons>	SendConsPtrList;

class SendCons_send_impl: public SendCons {
INHERITED(SendCons)
public:

  virtual bool		ownsCons() const {return true;}
  virtual Connection*	V_Cns() = 0; // SLOW!!!!
  
  SendCons_send_impl() {}
  ~SendCons_send_impl() {}
protected:
};

typedef taPtrList<SendCons_send_impl>	SendCons_send_implPtrList;

class SendCons_recv_impl: public SendCons {
INHERITED(SendCons)
public:
  
  virtual Connection**	V_Cns() = 0; // SLOW!!!!
  
  SendCons_recv_impl() {}
  ~SendCons_recv_impl() {}
protected:
  override void		SetCn(Connection* cn, int i) 
    {V_Cns()[i] = cn;}
};

template<class CN>
class SendCons_send: public SendCons_send_impl {
INHERITED(SendCons_send_impl)
public:
  CN*			cons;
  
  override Connection*	V_Cn(int i) {return &(cons[i]);} // SLOW virtual
  override Connection*	V_Cns() {return cons;}
  inline CN*		Cn(int i) {return &(cons[i]);}
  
  SendCons_send() {cons = NULL;}
  ~SendCons_send() {cons = NULL;}
protected:
  void 			setSize_impl(int i);
};

template<class CN>
class SendCons_recv: public SendCons_recv_impl {
INHERITED(SendCons_recv_impl)
public:
  CN**			cons;
  
  override Connection*	V_Cn(int i) {return cons[i];} // SLOW virtual
  override Connection**	V_Cns() {return (Connection**)cons;}
  inline CN*		Cn(int i) {return cons[i];}

  SendCons_recv() {cons = NULL;}
  ~SendCons_recv() {setSize_impl(0);}
protected:
  void 			setSize_impl(int i);
};


// ALGO Specific Guys

// Leabra -- send-based
typedef SendCons_send<LeabraCon> LeabraSendCons;
typedef RecvCons_send<LeabraCon> LeabraRecvCons;

typedef SendCons_send<FunkyLeabraCon> FunkyLeabraSendCons;
typedef RecvCons_send<FunkyLeabraCon> FunkyLeabraRecvCons;

/* why this nonsense???
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
*/

// Bp -- recv-based

typedef SendCons_recv<BpCon> BpSendCons;
typedef RecvCons_recv<BpCon> BpRecvCons;

/* again unnecessary...
class BpSendCons: public SendCons_recv<BpCon> {
INHERITED(SendCons_send<BpCon>)
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
*/


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

class ConSpec_send_impl: public ConSpec {
public:
};


class LeabraConSpec_impl: public ConSpec_send_impl {
INHERITED(ConSpec_send_impl)
public:
  inline static void Send_Netin_inner_0(float cn_wt, float& ru_net, float su_act_eff);
    // highly optimized inner loop
  template<class CN>
  void 			T_Send_Netin_0(Unit* su); // shared by 0 and N
  virtual void 		Send_Netin_0(Unit* su) = 0;

  template<class CN>
  void 			T_Send_Netin_Array(Unit* su, float* excit);
  virtual void 		Send_Netin_Array(Unit* su, float* excit) = 0;
  
  override void 	Compute_Weights(Network* net, Unit* ru);
  void 			Compute_Weights_CtCAL(Network* net, LeabraRecvCons* cg, Unit* ru);
  void 			Compute_SRAvg(Network* net, Unit* su);
};

template<class CN>
class LeabraConSpecT: public LeabraConSpec_impl {
INHERITED(LeabraConSpec_impl)
public:
  override void 	Send_Netin_0(Unit* su)
    {T_Send_Netin_0<CN>(su);}
  override void 	Send_Netin_Array(Unit* su, float* excit) 
    {T_Send_Netin_Array<CN>(su, excit);}
  
public: // i/f
  override size_t	GetConSize() const {return sizeof(CN);} // size of the Con
  override RecvCons*	NewRecvCons() const {return new RecvCons_send<CN>;}
  override SendCons*	NewSendCons() const {return new SendCons_send<CN>;}  
  
};

typedef  LeabraConSpecT<LeabraCon> LeabraConSpec;
typedef  LeabraConSpecT<FunkyLeabraCon> FunkyLeabraConSpec;

void LeabraConSpec_impl::Send_Netin_inner_0(float cn_wt, 
  float& ru_net, float su_act_eff) 
{
  ru_net += su_act_eff * cn_wt;
  //tru_net = *ru_net + su_act_eff * cn_wt;
}

class BpConSpec: public ConSpec {
INHERITED(ConSpec)
public:
  override size_t	GetConSize() const {return sizeof(BpCon);} // size of the Con
  RecvCons*		NewRecvCons() const {return new BpRecvCons;}
  SendCons*		NewSendCons() const {return new BpSendCons;}  
  override void 	Compute_Weights(Network* , Unit* /*ru*/) {} // nothing
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
  
  void			AllocConMem(int n_cons); // allocates required memory
  Unit**		AllocPUnits(int sz); // alloc indicated number (from pool)
  Connection**		AllocPCons(int sz); // alloc indicated number (from pool)
  Connection*		AllocCons(int sz); // alloc indicated number (from pool)
    // note: must cast to correct type

  Projection();
  ~Projection();
protected:
  UnitPtrList		units; // all unit ptrs
  ConnectionPtrList	pcons; // all con ptrs
  char*			cons; // conn memory
  int			next_cni;
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
  override void		Cycle_impl(); // do one net cycle
};

class BpNetwork: public Network {
INHERITED(Network)
public:
  
  BpNetwork();
  ~BpNetwork();
protected:
//  override void		Cycle_impl(); // do one net cycle
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

// Taskable functions have the following signature
// typically, they are static object functions that take an instance
// as the pointer obj

typedef void (*TaskFun_t)(NetTask* task, void* inst);

class NetTask: public Task {
public:
  static void T_Send_Netin_Clash(NetTask* nt, void* )
    {nt->Send_Netin_Clash();}
  static void T_Send_Netin_Array(NetTask* nt, void* ) 
    {nt->Send_Netin_Array();}
  static void T_Recv_Netin(NetTask* nt, void* )
    {nt->Recv_Netin();}
  static void T_ComputeAct(NetTask* nt, void* )
    {nt->ComputeAct();}
  static void T_Compute_SRAvg(NetTask* nt, void* )
    {nt->Compute_SRAvg();}
  static void T_Compute_Weights(NetTask* nt, void* )
    {nt->Compute_Weights();}




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
  
  void		SetTaskFun(TaskFun_t tf, void* inst);
  
  NetTask(NetEngine* engine);
protected:
  TaskFun_t 		TaskFun;
  void* 		inst;
};


class NetEngine { // prototype, and is also the default type (0-procs)
public:
  enum Algo { // note: not exhaustive...
    RECV	= 0,
//  RECV_SMART  = 1,	// OBS ignores senders under threshold
    
    SEND_CLASH  = 2, // sender, where writes can clash (or for 1 proc)
    SEND_ARRAY  = 3, // sender, using an array of nets to avoid clashes
//    SEND_PRJN   = 6, // sender, using target-disjoint arrays
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
  virtual void		OnBuild_Pre() {} // before net built
  virtual void		OnBuild_Post() {} // after net built
  
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
  virtual void 		DoProc(TaskFun_t tf, void* inst = NULL);
  virtual void 		ComputeNets_impl(); // inner overridable part
};


class ThreadNetEngine: public NetEngine {
INHERITED(NetEngine) 
public:
  static QThread* threads[core_max_nprocs]; // only n_procs-1 created, none for [0] (main thread)
  SendCons_send_implPtrList*	send_cons; // [n_procs] -- each list has disjoint targets
  
//  override void		OnBuild_Pre(); 
  override void		OnBuild_Post(); 
    
  ThreadNetEngine();
  ~ThreadNetEngine();
protected:
  override void		Initialize_impl();
  void			DeleteThreads();
  void 			MakeThreads();
  override void 	DoProc(TaskFun_t tf, void* inst = NULL);
  override void 	ComputeNets_impl();
  void 			ComputeNets_SendArray();
  void 			ComputeNets_SendPrjn();
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
  
// thread values
  
  static float nibble_thresh; // < 80%
  static signed char nibble_mode; // 0=none, 1=on, 2=auto (> 20% left)
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


/* Templates
*/

template<class CN>
void RecvCons_recv<CN>::setSize_impl(int i) {
  inherited::setSize_impl(i);
  cons = (CN*)prjn->AllocCons(i);
}

template<class CN>
void RecvCons_send<CN>::setSize_impl(int i) {
  inherited::setSize_impl(i);
  cons = (CN**)prjn->AllocPCons(i);
}

template<class CN>
void SendCons_send<CN>::setSize_impl(int i) {
  inherited::setSize_impl(i);
  cons = (CN*)prjn->AllocCons(i);
}

template<class CN>
void SendCons_recv<CN>::setSize_impl(int i) {
  inherited::setSize_impl(i);
  cons = (CN**)prjn->AllocPCons(i);
}

template<class CN>
void LeabraConSpec_impl::T_Send_Netin_0(Unit* su) {
  const float su_act_eff = su->act;
  for (int j = 0; j < su->send.size; ++j) {
    SendCons_send<CN>* send_gp = (SendCons_send<CN>*)su->send[j];
    const int send_sz = send_gp->size;
    su->n_con_calc += send_sz;
    Unit** units = send_gp->units; // unit pointer
    CN* cons = send_gp->cons; // the cons themselves!
    for (int i=0; i < send_sz; i++) {
      //const int targ_i = unis[i];
      Send_Netin_inner_0(cons[i].wt, units[i]->net, su_act_eff);
    }
  }
}

template<class CN>
void LeabraConSpec_impl::T_Send_Netin_Array(Unit* su, float* excit) {
  float su_act_eff = su->act;
  for (int j = 0; j < su->send.size; ++j) {
    SendCons_send<CN>* send_gp = (SendCons_send<CN>*)su->send[j];
    const int send_sz = send_gp->size;
    su->n_con_calc += send_sz;
    Unit** units = send_gp->units; // unit pointer
    CN* cons = send_gp->cons; 
    for (int i=0; i < send_sz; i++) {
      //const int targ_i = uns[i];
      LeabraConSpec::Send_Netin_inner_0(cons[i].wt, excit[units[i]->uni], su_act_eff);
    }
  }
}

#endif

