#include <stdio.h>

#include <stdlib.h>
#include <mpi.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include "ThreadedAllReduce.h"

static int nItems2 = 140*1024*1024;
//static int nItems2 = 12;
static float * data2_send;
static float * data2_recv;
static float * data2_recv_thr;
static int nThreads = 8;
static int reps = 1;
static MPI_Comm * thread_comms;

static int dmem_nprocs, dmem_proc;

ThreadedAllReduce * myreduce;

int do_threaded = 1;


void * thread_routine(void* data) {
    int thr_no = *((int *)data);

    for (int i = 0; i < reps; i++) {
        MPI_Allreduce(&(data2_send[thr_no*(nItems2/nThreads)]), &(data2_recv_thr[thr_no*(nItems2/nThreads)]), nItems2 / nThreads, MPI_FLOAT, MPI_SUM, thread_comms[thr_no]);
    }
}

void * thread_routine2(void* data) {
    int thr_no = *((int *)data);

    for (int i = 0; i < reps; i++) {
        myreduce->allreduce(&(data2_send[thr_no*(nItems2/nThreads)]), &(data2_recv_thr[thr_no*(nItems2/nThreads)]), nItems2 / nThreads, thr_no);
    }
}

int main(int argc, char ** argv) {
    int thread_level_provided;
    int nItems = 10;
    int broken;
    float * data_send = (float *)calloc(nItems, sizeof(float));
    float * data_recv = (float *)calloc(nItems, sizeof(float));
    double timer1s;
    double timer1e;
    void * value_ptr;
    char proc_name[255];
    int proc_name_len = 255;

    struct timeval time; 
    gettimeofday(&time,NULL);

    // microsecond has 1 000 000
    // Assuming you did not need quite that accuracy
    // Also do not assume the system clock has that accuracy.
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
    
    pthread_t * thread_ids;
    thread_ids = (pthread_t *)calloc(nThreads, sizeof(pthread_t));
    int * thread_data = (int *)calloc(nThreads,sizeof(int));
    thread_comms = (MPI_Comm *)calloc(nThreads, sizeof(MPI_Comm));
    
    for (int i = 0; i < nItems; i++)
        data_send[i] = i * 2.5;

    MPI_Init_thread(&argc, (char***)&argv, MPI_THREAD_MULTIPLE, &thread_level_provided);
    MPI_Comm_size(MPI_COMM_WORLD, &dmem_nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &dmem_proc);
    MPI_Get_processor_name((char*)&proc_name, &proc_name_len);
    MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    printf("We have initialized MPI with %i on %s=%i with thread level %i\n",dmem_nprocs, proc_name, dmem_proc, thread_level_provided);

    myreduce = ThreadedAllReduce::getSingleton(MPI_COMM_WORLD, nThreads);
    
    data2_send = (float *)calloc(nItems2, sizeof(float));
    data2_recv = (float *)calloc(nItems2, sizeof(float));
    data2_recv_thr = (float *)calloc(nItems2, sizeof(float));
    
    for (int i = 0; i < nItems2; i++)
        data2_send[i] = i * 2.5 + rand();
    
    //Make sure all MPI processes have completed initialisation and are ready before starting the benchmarking
    MPI_Barrier( MPI_COMM_WORLD );
    timer1s = MPI_Wtime();
    for (int i = 0; i < reps; i++)
        MPI_Allreduce(data2_send, data2_recv, nItems2, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
    timer1e = MPI_Wtime();

    printf("We did the MPI AllReduce in %f seconds at a rate of %f Gbit/s\n", (timer1e - timer1s), ((nItems2*sizeof(float)*8.0*reps)/(timer1e - timer1s) / 1024.0 / 1024.0 / 1024.0));

    /**
       Test if we can use multi-threading with the standard MPI libraries.
    **/
    if (0) {
        timer1s = MPI_Wtime();
        for (int j = 0; j < nThreads; j++) {
            //printf("Creating thread %i\n", j);
            thread_data[j] = j;
            MPI_Comm_dup(MPI_COMM_WORLD, &(thread_comms[j]));
            pthread_create(&(thread_ids[j]), NULL, &thread_routine, (void *) &thread_data[j]);
        }
        
        
        for (int j = 0; j < nThreads; j++) {
            pthread_join(thread_ids[j], &value_ptr);
        }
        
        timer1e = MPI_Wtime();
        
        printf("We did the threaded API AllReduce in %f seconds at a rate of %f Gbit/s\n", (timer1e - timer1s), ((nItems2*sizeof(float)*8.0*reps)/(timer1e - timer1s) / 1024.0 / 1024.0 / 1024.0));
        
        broken = 0;
        for (int i = 0; i < nItems2; i++) {
            if (fabs(data2_recv[i] - data2_recv_thr[i]) > fabs(data2_recv[i] *0.00001))
                broken++;
        }
        printf("\n %i values were different!\n", broken);
    }

    /**
       Test our own AllReduce implementation in single threaded mode
    **/
    //Make sure all MPI processes have completed initialisation and are ready before starting the benchmarking
    MPI_Barrier( MPI_COMM_WORLD );
    timer1s = MPI_Wtime();
    for (int i = 0; i < reps; i++)
        myreduce->allreduce(data2_send, data2_recv_thr, nItems2, 0);
    timer1e = MPI_Wtime();
    
    broken = 0; 
    for (int i = 0; i < nItems2; i++) {
        if (fabs(data2_recv[i] - data2_recv_thr[i]) > fabs(data2_recv[i] *0.00001))
            broken++; 
    } 
    if (dmem_proc == 0) {
        printf("\nP%i: %i out of %i values were different!\n", dmem_proc, broken, nItems2);

        printf("P%i: We did our own single threaded reduce in %f seconds at a rate of %f Gbit/s\n", dmem_proc, (timer1e - timer1s), ((nItems2*sizeof(float)*8.0*reps)/(timer1e - timer1s) / 1024.0 / 1024.0 / 1024.0));
    }

    
    /**
       Test our own AllReduce implementation in threaded mode
    **/
    bzero((char *) data2_recv_thr, nItems2*sizeof(float));
    //Make sure all MPI processes have completed initialisation and are ready before starting the benchmarking
    MPI_Barrier( MPI_COMM_WORLD );
    timer1s = MPI_Wtime();
    for (int i = 0; i < reps; i++)
        myreduce->allreduce(data2_send, data2_recv_thr, nItems2);
    timer1e = MPI_Wtime();
    
    broken = 0; 
    for (int i = 0; i < nItems2; i++) { 
        if (fabs(data2_recv[i] - data2_recv_thr[i]) > fabs(data2_recv[i] *0.00001)) {
            broken++; 
            if (dmem_proc == 0)
                printf("%i: %f != %f => %f", i, data2_recv[i], data2_recv_thr[i], fabs(data2_recv[i] - data2_recv_thr[i]));
        }
    }
    if (dmem_proc == 0) {
        printf("\nP%i: %i out of %i values were different!\n", dmem_proc, broken, nItems2);

        printf("P%i: We did the threaded-1 self reduce in %f seconds at a rate of %f Gbit/s\n", dmem_proc, (timer1e - timer1s), ((nItems2*sizeof(float)*8.0*reps)/(timer1e - timer1s) / 1024.0 / 1024.0 / 1024.0));
    }

    /*********************************************************************************************************************************/
    delete myreduce;
    MPI_Finalize();


    free(data_send);
    free(data_recv);
    free(data2_send);
    free(data2_recv);
    free(thread_data);
    free(thread_ids);

}
