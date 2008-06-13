#include "nb_util.h"
#include "nb_netstru.h"

#include <QtCore/QString>
#include <QtCore/QCoreApplication>

int main(int argc, char* argv[]) {
  return Nb::main(argc, argv);
}
  
int Nb::main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  
  if(argc < 4) {
    printf("must have min 3 args:\n"
      "\t<n_units>\tnumber of units in each of the layers\n"
      "\t<n_cycles>\tnumber of cycles\n"
      "\t<n_procs>\tnumber of cores or procs (0=fast single-threaded model)\n"
      "optional positional params -- none can be skipped: \n"
      "\t<n_lays>\tnumber of layers (min 2, def=3, max 128)\n"
      "\t<n_cons>\tnumber of cons per unit-projection (def=n_units)\n"
      "\t<send_act>\tpercent avg activation level (def = 10)\n"
      "optional commands: \n"
      "\t-header\t output a header line\n"
      "\t-log=1/0(def)\tlog/do-not-log optional values to ptest_core.log\n"
      "\t-nibble=n\tnibble: 0(def)=none, 1=on, 2=auto (> .20 left)\n"
      "\t-act=0\tdo not calculate activation\n"
      "-algo=n (def=0) is one of the following:\n"
      "\t 0 receiver-based\n"
      "\t 1 sender-based -- clashes allowed \n"
    );
    return 1;
  }

#ifdef USE_RECV_SMART
  net.recv_smart = true;
#endif  
  bool use_log_file = false;

  srand(56);			// always start with the same seed!

  n_units = (int)strtol(argv[1], NULL, 0);
  n_cycles = (int)strtol(argv[2], NULL, 0);
  NetEngine::n_procs = (int)strtol(argv[3], NULL, 0);
  if (NetEngine::n_procs <= 0) {
    single = true;
    NetEngine::n_procs = 1;
    Nb::net.SetEngine(new NetEngine); // just the default
  } else {
    single = false;
    if (NetEngine::n_procs > NetEngine::core_max_nprocs) 
      NetEngine::n_procs = NetEngine::core_max_nprocs;
    Nb::net.SetEngine(new ThreadNetEngine); // just the default
  }
  
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
  
  int tsend_act = 10; // def
  if ((argc > 6) && (*argv[6] != '-')) {
    tsend_act = (int)strtol(argv[6], NULL, 0);
    if (tsend_act < 0)
      tsend_act = 0;
    else if (tsend_act > 100)
      tsend_act = 100;
  }
  send_act = (int)(0x10000 * (tsend_act / 100.0f));
  
  
  // switch params
  for (int arg = 4; arg < argc; arg++) {
    QString targ(argv[arg]);
#ifdef USE_SAFE
    if (targ == "-safe=1")
      safe = true;
    if (targ == "-safe=0")
      safe = false;
#endif
    if (targ == "-header")
      hdr = true;
    if (targ.startsWith("-algo=")) {
      NetEngine::algo = targ.remove("-algo=").toInt();
      continue;
    }
    if (NetEngine::algo == NetEngine::RECV_SMART)
      Network::recv_smart = true;
      
    if (targ == "-log=1")
      use_log_file = true;
    if (targ == "-log=0")
      use_log_file = false;
    if (targ == "-act=0")
      calc_act = false;
    if (targ.startsWith("-nibble=")) {
      nibble_mode = targ.remove("-nibble=").toInt();
      continue;
    }
    //TODO: any more
  }
  
  net.Initialize();
  net.Build();

  FILE* logfile = NULL;
  if (use_log_file) {
    logfile = fopen("ptest_core.log", "w");
    fprintf(logfile,"cyc\ttot_act\n");
  }

  TimeUsed time_used;	
  time_used.Start();

  // this is the key computation loop
  for(int cyc = 0; cyc < n_cycles; cyc++) {
    net.ComputeNets();
    if (calc_act) {
      float tot_act = net.ComputeActs();
      if(use_log_file)
        fprintf(logfile,"%d\t%g\n", cyc, tot_act);
    }
  }

  time_used.Stop();
  if (use_log_file) {
    fclose(logfile);
    logfile = NULL;
  }


//     time_used.OutString(stdout);
//     printf("\n");

  double tot_time = time_used.s_used;

  double n_wts = n_layers * n_units * n_cons * 2.0;   // bidirectional connected layers
  // cons travelled will depend on activation percent, so we calc exactly
  double n_con_trav = n_tot * (n_cons * 2.0);
  double con_trav_sec = ((double)n_con_trav / tot_time) / 1.0e6;
  // but effective is based on raw numbers
  double n_eff_con_trav = n_layers * n_units * n_cycles * (n_cons * 2.0);
  double eff_con_trav_sec = ((double)n_eff_con_trav / tot_time) / 1.0e6;
  
  // note: always make/extend the thread log
  net.engine->Log(hdr);

  if (hdr)
  printf("eMcon\tMcon\tsnd_act\tprocs\tlayers\tunits\tcons\tweights\tcycles\tcon_trav\tsecs\tn_tot\talgo\tnibble\n");
  if (single) NetEngine::n_procs = 0;
  printf("%g\t%g\t%d\t%d\t%d\t%d\t%d\t%g\t%d\t%g\t%g\t%d\t%d\t%d\n",
    eff_con_trav_sec, con_trav_sec, tsend_act, NetEngine::n_procs, n_layers, n_units, n_cons, n_wts, n_cycles, n_con_trav, tot_time, n_tot, NetEngine::algo, nibble_mode);

  net.SetEngine(NULL); // controlled delete
  return 0;
}

