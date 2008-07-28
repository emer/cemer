#!/bin/sh

# this is a script file for running various standard configurations of
# nb_thread using same params as leabra_bench.proj
# use this file for 2-core systems

# <n_units> <n_cycles>  <n_procs>  <n_lays>  <n_cons>(=def) <send_act>  

# Receiver-based

./nb_thread 25 4970 0 5 0 100 -algo=0 -header ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 4970 1 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 25 4970 2 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 4970 4 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}

./nb_thread 100 1065 0 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1065 1 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 100 1065 2 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1065 4 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}

./nb_thread 625 142 0 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 142 1 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 625 142 2 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 142 4 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}

./nb_thread 1000 71 0 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 71 1 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 1000 71 2 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 71 4 5 0 100 -algo=0  ${1} ${2} ${3} ${4} ${5}


# Sender-based, clobber

./nb_thread 25 4970 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 4970 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 25 4970 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 4970 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}

./nb_thread 100 1065 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1065 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 100 1065 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1065 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}

./nb_thread 625 142 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 142 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 625 142 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 142 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}

./nb_thread 1000 71 0 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 71 1 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 1000 71 2 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 71 4 5 0 5 -algo=2  ${1} ${2} ${3} ${4} ${5}


# Sender-based, array

./nb_thread 25 4970 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 4970 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 25 4970 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 25 4970 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}

./nb_thread 100 1065 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1065 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 100 1065 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 100 1065 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}

./nb_thread 625 142 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 142 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 625 142 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 625 142 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}

./nb_thread 1000 71 0 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 71 1 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5} 
./nb_thread 1000 71 2 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
./nb_thread 1000 71 4 5 0 5 -algo=3  ${1} ${2} ${3} ${4} ${5}
