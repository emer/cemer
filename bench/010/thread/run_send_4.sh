#!/bin/sh

# this is a script file for running various standard configurations of
# nb_thread using same params as leabra_bench.proj
# use this file for 2-core systems

# <n_units> <n_cycles>  <n_procs>  <n_lays>  <n_cons>(=def) <send_act>  


# Sender-based, clobber

./nb_thread 25 5000 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 5000 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 25 5000 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 5000 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}

./nb_thread 100 1000 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1000 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 100 1000 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1000 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}

./nb_thread 625 100 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 100 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 625 100 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 100 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}

./nb_thread 1000 50 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 50 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 1000 50 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 50 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}


# Sender-based, array

./nb_thread 25 5000 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 5000 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 25 5000 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 5000 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}

./nb_thread 100 1000 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1000 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 100 1000 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1000 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}

./nb_thread 625 100 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 100 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 625 100 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 100 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}

./nb_thread 1000 50 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 50 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 1000 50 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 50 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
