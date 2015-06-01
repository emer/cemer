#ifdef DMEM_COMPILE

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#ifndef ThreadedAllReduce_h 
#define ThreadedAllReduce_h 1


#include <stdlib.h>
#include <mpi.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>



/**
   Implement a replacement for the MPI_Allreduce function.
   For some reason the MPI libraries often give sub-optimal performance by default.
   This assumes we are doing a reduction with the + operator on float arrays. We further assume
   that we are doing this on very large arrays, so optimizing for network bandwidth is the most important
   aspect.
   This implementation uses elements of the algorithm described in http://www.sciencedirect.com/science/article/pii/S0743731508001767
   This algorithm uses a virtual ring between MPI processors to pass information around the ring to do the all reduce.
   However, we don't yet deal with anything other than a flat network topology (all MPI processors are connected directly to the same
   n-to-n fabric with no network contention between two links with distinct nodes) and thus the mapping from MPI processor rank to place
   in the ring is a one-to-one mapping. **/

class ThreadedAllReduce {

    
private:
    char * dmem_proc_names;
    int dmem_nprocs, dmem_proc;
    MPI_Comm mpi_ctx;
    int initialized;

    int nThreads;
    pthread_t * thread_ids;

    int base_port;
    static const int defaultnThreads = 6;


    int      * serverfds;  //Server socket file descriptor for each connection thread
    int      * serverconnfds; //Connected incomming socket file descripter for each thread
    int      * clientfds; //Outgoi#include "reduce_replacement.hpp"ng socket file descriptor for each connection thread
    int      * connectedS; //If server socket has a client connection
    int      * connectedC; //If the client socket is connected
    struct sockaddr_in * serv_addrs; // #IGNORE Connection information to connect to our respective "server"

    ThreadedAllReduce(MPI_Comm mpi_ctx, int nThreads, char * node_prefix, char * node_suffix);

public:
    static ThreadedAllReduce * getSingleton(MPI_Comm mpi_ctx, int nThreads = defaultnThreads, char * node_prefix = NULL, char * node_suffix = NULL);
    ~ThreadedAllReduce();
    void allreduce(float * src, float * dst, size_t len, int tag);
    void allreduce(float * src, float * dst, size_t len);
};

struct recv_thread_struct {
    int tag;
    size_t len;
    float * recv_buff;
    pthread_barrier_t barrier;

    int      * serverfds;  //Server socket file descriptor for each connection thread
    int      * serverconnfds; //Connected incomming socket file descripter for each thread
    int      * connectedS; //If server socket has a client connection
    
    int dmem_proc, dmem_nprocs;
};

struct spawn_thread_struct {
    int tag;
    size_t len;
    float * recv_buff;
    float * send_buff;
    
    int dmem_proc, dmem_nprocs;
    ThreadedAllReduce * obj_ctx;
};



#endif
#endif 
