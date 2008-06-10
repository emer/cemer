#include "nb_util.h"
#include "nb_netstru.h"

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
      "\t<send_act>\tpercent (/100) avg activation level (def = 100)\n"
      "optional commands: \n"
      "\t-header\t output a header line\n"
      "\t-log=0\t(def) do not log optional values to ptest_core.log\n"
      "\t-log=1\tlog optional values to ptest_core.log\n"
      "\t-act=0\tdo not calculate activation\n"
    );
    return 1;
  }

#ifdef USE_RECV_SMART
  net.recv_based = true; net.recv_smart = true;
#endif  
  bool use_log_file = false;

  srand(56);			// always start with the same seed!

  n_units = (int)strtol(argv[1], NULL, 0);
  //TODO: parameterize this!
  Network::n_units_flat = n_units * n_layers;
  n_cycles = (int)strtol(argv[2], NULL, 0);
  n_procs = (int)strtol(argv[3], NULL, 0);
  if (n_procs <= 0) {
    single = true;
    n_procs = 1;
  } else {
    single = false;
    if (n_procs > core_max_nprocs) n_procs = core_max_nprocs;
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
#ifdef USE_SAFE
    if (strcmp(argv[arg], "-safe=1") == 0)
      safe = true;
    if (strcmp(argv[arg], "-safe=0") == 0)
      safe = false;
#endif
    if (strcmp(argv[arg], "-header") == 0)
      hdr = true;
    if (strcmp(argv[arg], "-log=1") == 0)
      use_log_file = true;
    if (strcmp(argv[arg], "-log=0") == 0)
      use_log_file = false;
    if (strcmp(argv[arg], "-act=0") == 0)
      calc_act = false;
    //TODO: any more
  }
  
  MakeThreads();
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
  
  if (use_log_file) {
    logfile = fopen("nb_thread.log", "w");
    if (hdr) fprintf(logfile,"\nthread\tt_tot\tstart lat\trun time\n");
    for (int t = 0; t < n_procs; t++) {
      NetTask* tsk = netin_tasks[t];
      QTaskThread* th = (QTaskThread*)threads[t];
      double start_lat = 0;
      double run_time = 0;
      start_lat = tsk->start_latency.s_used;
      run_time = tsk->run_time.s_used;
      fprintf(logfile,"%d\t%d\t%g\t%g\n", t, tsk->t_tot, start_lat, run_time);
    }
    fclose(logfile);
    logfile = NULL;
  }

  if (hdr)
  printf("eMcon\tMcon\tsnd_act\tprocs\tlayers\tunits\tcons\tweights\tcycles\tcon_trav\tsecs\tn_tot\n");
  if (single) n_procs = 0;
  printf("%g\t%g\t%d\t%d\t%d\t%d\t%d\t%g\t%d\t%g\t%g\t%d\n",
    eff_con_trav_sec, con_trav_sec, tsend_act, n_procs, n_layers, n_units, n_cons, n_wts, n_cycles, n_con_trav, tot_time, n_tot);

  DeleteThreads();
//  DeleteNet();
  return 0;
}

