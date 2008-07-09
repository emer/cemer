@echo off

rem this is a script file for running various standard configurations of
rem nb_thread using same params as leabra_bench.proj
rem use this file for 2-core systems

rem <n_units> <n_cycles>  <n_procs>  <n_lays>  <n_cons>(=def) <send_act>  

rem Receiver-based

release\nb_thread 25 5000 0 5 0 100 -algo=0 -header %1 %2 %3 %4 %5
release\nb_thread 25 5000 1 5 0 100 -algo=0 %1 %2 %3 %4 %5
release\nb_thread 25 5000 2 5 0 100 -algo=0 %1 %2 %3 %4 %5

release\nb_thread 100 1000 0 5 0 100 -algo=0 %1 %2 %3 %4 %5
release\nb_thread 100 1000 1 5 0 100 -algo=0 %1 %2 %3 %4 %5
release\nb_thread 100 1000 2 5 0 100 -algo=0 %1 %2 %3 %4 %5

release\nb_thread 625 100 0 5 0 100 -algo=0 %1 %2 %3 %4 %5
release\nb_thread 625 100 1 5 0 100 -algo=0 %1 %2 %3 %4 %5
release\nb_thread 625 100 2 5 0 100 -algo=0 %1 %2 %3 %4 %5

release\nb_thread 1000 50 0 5 0 100 -algo=0 %1 %2 %3 %4 %5
release\nb_thread 1000 50 1 5 0 100 -algo=0 %1 %2 %3 %4 %5
release\nb_thread 1000 50 2 5 0 100 -algo=0 %1 %2 %3 %4 %5


rem Sender-based, clobber

release\nb_thread 25 5000 0 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 25 5000 1 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 25 5000 2 5 0 5 -algo=2 %1 %2 %3 %4 %5

release\nb_thread 100 1000 0 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 100 1000 1 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 100 1000 2 5 0 5 -algo=2 %1 %2 %3 %4 %5

release\nb_thread 625 100 0 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 625 100 1 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 625 100 2 5 0 5 -algo=2 %1 %2 %3 %4 %5

release\nb_thread 1000 50 0 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 1000 50 1 5 0 5 -algo=2 %1 %2 %3 %4 %5
release\nb_thread 1000 50 2 5 0 5 -algo=2 %1 %2 %3 %4 %5


rem Sender-based, array

release\nb_thread 25 5000 0 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 25 5000 1 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 25 5000 2 5 0 5 -algo=3 %1 %2 %3 %4 %5

release\nb_thread 100 1000 0 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 100 1000 1 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 100 1000 2 5 0 5 -algo=3 %1 %2 %3 %4 %5

release\nb_thread 625 100 0 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 625 100 1 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 625 100 2 5 0 5 -algo=3 %1 %2 %3 %4 %5

release\nb_thread 1000 50 0 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 1000 50 1 5 0 5 -algo=3 %1 %2 %3 %4 %5
release\nb_thread 1000 50 2 5 0 5 -algo=3 %1 %2 %3 %4 %5
