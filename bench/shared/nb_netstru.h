#ifndef NB_NETSTRU_H
#define NB_NETSTRU_H


#include "nb_util.h"

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
#ifdef USE_RECV_SMART
  bool		do_delta; // for recv_smart
#endif
  float act;			// activation value
  float net;			// net input value
  Connection* send_wts;		// sending weights
  int		task_id; // which task will process this guy
  Unit_List	targs;
  ConSpec*	cs;
  char dummy[16]; // bump a bit, to spread these guys out
  
  Connection* 	Cn(int i) const { return &(send_wts[i]); }
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
  static int	un_to_idx; // circular guy we globally use to pick next target unit
  
  Unit_List	units;
  
  void		Connect(Layer* to); // connect from me to
  
  Layer();
  ~Layer();
};

typedef taPtrList<Layer>	Layer_List;

class Network {
public:
  static bool	recv_based; // true for recv, false for send
  static bool	recv_smart; // fort recv-based, does the smart recv algo
  
  Unit_List 	units_flat;		// layers = arrays of units
  Layer_List	layers;
  
  void		Build(); 
  void 		ComputeNets();
  float 	ComputeActs();
  
  Network();
  ~Network();
};

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
  static void 	Send_Netin_0(Unit* su); // shared by 0 and N
  virtual void	Send_Netin() = 0; // NetIn
  
// Recv_Netin
  static void 	Recv_Netin_0(Unit* ru); // shared by 0 and N
  virtual void	Recv_Netin(); // default is the _0 version
  
  
// ComputeAct  
  static void ComputeAct_inner(Unit* un);
  float		my_act;
  virtual void	ComputeAct(); // default does it globally
  
  NetTask();
};

void NetTask::Send_Netin_inner_0(float cn_wt, float* ru_net, float su_act_eff) {
  *ru_net += su_act_eff * cn_wt;
  //tru_net = *ru_net + su_act_eff * cn_wt;
}


class NetTask_0: public NetTask {
// for single threaded approach -- optimum for that
public:
  void		Send_Netin();
//  void		ComputeAct();
};

class NetTask_N: public NetTask {
// for N threaded approach -- optimum for that
public:
  void		Send_Netin();
  void		Recv_Netin();
  void		ComputeAct();
};

class Nb { // global catchall
public:
  static int n_procs;		// total number of processors
  static const int core_max_nprocs = 32; // maximum number of processors!
  static QThread* threads[core_max_nprocs]; // only n_procs-1 created, none for [0] (main thread)
  static NetTask* netin_tasks[core_max_nprocs]; // only n_procs created
  
  static int n_layers;
  static int n_units;			// number of units per layer
  static int n_cons; // number of cons per unit
  static int n_units_flat; // total number of units (flattened, all layers)
  static int n_cycles;			// number of cycles of updating
  static Network net;		// global network 
  static int n_tot; // total units (reality check)
  static float tot_act; // check on tot act
  static int send_act; // send activation, as a fraction of 2^16 
  static int this_rand; // assigned a new random value each cycle, to let us randomize unit acts
  static bool nibble; // setting false disables nibbling and adds sync to loop
  static bool single; // true for single thread mode, to compare against nprocs=1
  static bool calc_act;
  
private:
  Nb();
  ~Nb();
};

#endif

