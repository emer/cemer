#include "ThreadedAllReduce.h"

#ifdef DMEM_COMPILE

void * thread_recv(void * data) {
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    size_t data_recved = 0;
    ssize_t n;
    float * recv_data;
    struct recv_thread_struct * recv_struct;

    recv_struct = (struct recv_thread_struct *) data;
    recv_data = (float *)calloc(recv_struct->len, sizeof(float));

    if (!recv_struct->connectedS[recv_struct->tag]) {
        recv_struct->serverconnfds[recv_struct->tag] = accept(recv_struct->serverfds[recv_struct->tag], 
                           (struct sockaddr *) &cli_addr, 
                           &clilen);
        if (recv_struct->serverconnfds[recv_struct->tag] < 0) 
            printf("P%i: ERROR on accept\n", recv_struct->dmem_proc);
        recv_struct->connectedS[recv_struct->tag] = 1;
    }

    for (int j = 0; j < (recv_struct->dmem_nprocs - 1); j++) {
        data_recved = 0;
        while (data_recved < (recv_struct->len * sizeof(float))) {
            n = read(recv_struct->serverconnfds[recv_struct->tag],((void *)recv_data) + data_recved,recv_struct->len * sizeof(float) - data_recved);
            if (n < 0) {
                printf("Failed to read data %s\n", strerror(errno));
                break;
            }
            data_recved += n;
        }
        for (size_t i = 0; i < recv_struct->len; i++) {
            recv_struct->recv_buff[((recv_struct->dmem_proc - j - 1 + recv_struct->dmem_nprocs) % recv_struct->dmem_nprocs)*recv_struct->len + i] += recv_data[i];
        }
        
        pthread_barrier_wait(&(recv_struct->barrier));
    }

    for (int j = 0; j < (recv_struct->dmem_nprocs - 1); j++) {
        data_recved = 0;
        while (data_recved < (recv_struct->len * sizeof(float))) {
            n = read(recv_struct->serverconnfds[recv_struct->tag],((void *)recv_data) + data_recved,recv_struct->len * sizeof(float) - data_recved);
            if (n < 0) {
                printf("P%i: ERROR! Failed to read data %s\n", recv_struct->dmem_proc, strerror(errno));
                break;
            }
            data_recved += n;
        }
        for (size_t i = 0; i < recv_struct->len; i++) {
            recv_struct->recv_buff[((recv_struct->dmem_proc - j + recv_struct->dmem_nprocs) % recv_struct->dmem_nprocs)*recv_struct->len + i] = recv_data[i];
        }

        pthread_barrier_wait(&(recv_struct->barrier));   
    }

    free(recv_data);
    return NULL;
}

void * thread_spawn(void* data) { 
    struct spawn_thread_struct * thread_data = (struct spawn_thread_struct *)data; 
 
    thread_data->obj_ctx->allreduce(thread_data->send_buff, thread_data->recv_buff, thread_data->len, thread_data->tag);
    return NULL;
} 

ThreadedAllReduce::ThreadedAllReduce(MPI_Comm mpi_ctx, int nThreads) {
    char * proc_name;
    int proc_name_len;
    int portno;
    int port_retry;
    int error_encountered;
    int * dmem_errors_encountered;
    struct sockaddr_in serv_addr, cli_addr;

    proc_name = (char *) calloc(MPI_MAX_PROCESSOR_NAME, sizeof(char));
    thread_ids = (pthread_t *)calloc(nThreads, sizeof(pthread_t));
    this->mpi_ctx = mpi_ctx;
    this->nThreads = nThreads;
    MPI_Comm_size(mpi_ctx, &dmem_nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &dmem_proc);
    MPI_Get_processor_name((char*)proc_name, &proc_name_len);

    /** Get the list of MPI processor names to be able to open our own TCP/IP sockets too. **/
    dmem_errors_encountered = (int*)calloc(dmem_nprocs, sizeof(int));
    dmem_proc_names = (char*)calloc(dmem_nprocs, sizeof(char *)*MPI_MAX_PROCESSOR_NAME);
    printf("We are initializing our reduce context on %s with rank %i of %i\n", proc_name, dmem_proc, dmem_nprocs);

    MPI_Allgather(proc_name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, dmem_proc_names, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, mpi_ctx);

    for (port_retry = 0; port_retry < 20; port_retry++) {
        error_encountered = 0;
        if (dmem_proc == 0) {
            if (port_retry == 0) {
                for (int i = 0; i < dmem_nprocs; i++) {
                    printf("P%i: rank %i is running on %s\n", dmem_proc, i, &dmem_proc_names[MPI_MAX_PROCESSOR_NAME*i]);
                }
            }
            base_port = 16000 + (rand() % 32000);
             
            printf("Baseport is %i\n", base_port);
        }
        MPI_Bcast(&base_port, 1, MPI_INT, 0, mpi_ctx);

        /** Setup TCP/IP server (incoming) sockets */
        serverfds = (int *)calloc(nThreads, sizeof(int));
        serverconnfds = (int *)calloc(nThreads, sizeof(int));
        connectedS = (int *)calloc(nThreads, sizeof(int));
    
        for (int i = 0; i < nThreads; i++) {
            serverfds[i] = socket(AF_INET, SOCK_STREAM, 0);
            if (serverfds[i] < 0) {
                printf("P%i: ERROR opening socket (%s)\n", dmem_proc, strerror(errno));
                initialized = 0;
                error_encountered = 1;
                break;
            }
            portno = base_port + i + 20*dmem_proc;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
            if (bind(serverfds[i], (struct sockaddr *) &serv_addr,
                     sizeof(serv_addr)) < 0) {
                printf("P%i: ERROR on binding!! (%s)\n", dmem_proc,  strerror(errno));
                initialized = 0;
                error_encountered = 1;
                break;
            } 
            if (listen(serverfds[i],5) < 0) {
                printf("P%i: ERROR listening to socket (%s)\n", dmem_proc, strerror(errno));
                initialized = 0;
                error_encountered = 1;
                break;
            }
        }
        MPI_Allgather(&error_encountered,1, MPI_INT, dmem_errors_encountered, 1, MPI_INT, mpi_ctx);

        error_encountered = 0;
        for (int i = 0; i < dmem_nprocs; i++)
            if (dmem_errors_encountered[i] == 1) error_encountered = 1;
        if (error_encountered == 0) break;
    }
    if (error_encountered) {
        free(dmem_errors_encountered);
        printf("ERROR: !!!!!!!!!!! Despite multiple retries, can't obtain valid socket !!!!!!!!!!\n");
        return;
    }

    /** Setup TCP/IP client (outgoing) sockets
        For an Allreduce, we connect the processors in a ring structure.
        So processor n sends to processor n + 1 and receives from n - 1
        Each thread creates its own TCP/IP socket to its neighbour **/
    
    connectedC = (int *)calloc(nThreads, sizeof(int));
    clientfds = (int *)calloc(nThreads, sizeof(int));
    serv_addrs = (sockaddr_in *)calloc(nThreads, sizeof(struct sockaddr_in));

    struct hostent *server;
    bzero((char *) serv_addrs, nThreads*sizeof(struct sockaddr_in));

    for (int i = 0; i < nThreads; i++) {
        clientfds[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (clientfds[i] < 0) {
            printf("P%i: ERROR opening socket\n", dmem_proc);
        }
        server = gethostbyname(&dmem_proc_names[((dmem_proc + 1) % dmem_nprocs)*MPI_MAX_PROCESSOR_NAME]);
        portno = base_port + i + 20*((dmem_proc + 1) % dmem_nprocs); //connect to the port of our next neighbour
        bzero((char *) &(serv_addrs[i]), sizeof(struct sockaddr_in));
        serv_addrs[i].sin_family = AF_INET;
        bcopy((char *)server->h_addr_list[0], 
              (char *)&serv_addrs[i].sin_addr.s_addr,
              server->h_length);
        serv_addrs[i].sin_port = htons(portno);

    }

    initialized = 1;
    
}

ThreadedAllReduce::~ThreadedAllReduce() {

    for (int i = 0; i < nThreads; i++) {
        if (connectedC[i]) {
            close(clientfds[i]);
        }
        if (connectedS[i]) {
            close(serverconnfds[i]);
        }
        close(serverfds[i]);    
    }
    free(dmem_proc_names);
    free(serverfds);
    free(serverconnfds);
    free(clientfds);
    free(connectedS);
    free(connectedC);
    free(serv_addrs);
    free(thread_ids);
}

ThreadedAllReduce * ThreadedAllReduce::getSingleton(MPI_Comm mpi_ctx, int nThreads ) {
    static ThreadedAllReduce * singleton = NULL;
    if (singleton != NULL)
        return singleton;
    singleton = new ThreadedAllReduce(mpi_ctx, nThreads);
    return singleton;
}

void ThreadedAllReduce::allreduce(float * src, float * dst, size_t len, int tag) {
    pthread_t recv_threadid;
    struct recv_thread_struct recv_struct;
    size_t seg_len = len / dmem_nprocs; // For efficiency we sub-divide the length into dmem_nprocs chunks of equal size;
    
    void * value_ptr;
    int sendfd;
    size_t data_sent = 0;
    ssize_t n;


    if ((len % dmem_nprocs) != 0) {
        printf("ERROR: We can't deal with reduce buffer sizes that aren't multiples of the number of processors.\n");
        return;
    }
    
    if (initialized == 0) {
        printf("ERROR: Our all reduce context was not correctly initialized\nThis might well lead to deadlock if some MPI processors were initialized");
        return;
    }

    /** Setup parameters we pass into the receiving thread **/
    recv_struct.len = seg_len;
    recv_struct.tag = tag;
    recv_struct.recv_buff = dst;
    pthread_barrier_init(&recv_struct.barrier, NULL, 2);

    /** Copy member values to recv_struct to access them from the thread function **/
    recv_struct.serverfds = serverfds;
    recv_struct.serverconnfds = serverconnfds;
    recv_struct.connectedS = connectedS;
    
    recv_struct.dmem_proc = dmem_proc;
    recv_struct.dmem_nprocs = dmem_nprocs;

    /** We need to initialize the destination buffer with our own version of the results, so that we can then reduce values into this buffer **/
    for (size_t i = 0; i < len; i++) {
        dst[i] = src[i];
    }

    pthread_create(&recv_threadid, NULL,  &thread_recv, (void *) &recv_struct);

    /** Check if we have a connection already from a previous call to allreduce and then reuse socket **/
    if (!connectedC[tag]) {
        
        if (connect(clientfds[tag],(struct sockaddr *) &(serv_addrs[tag]),sizeof(struct sockaddr_in)) < 0) 
            printf("P%i: ERROR connecting to server: %s\n", dmem_proc, strerror(errno));
        //printf("P%i: Managed to connect to server for thread %i\n", dmem_proc, tag);
        connectedC[tag] = 1;
    } 

    /** Perform the reduce operation on segment length pieces, by sending segments around the ring and reducing our values into the respective segment.
        After dmem_nprocs - 1 forwardings around the ring, the segment has the correct reduction value. By pipelining and each processor passing on a different
        segment and any given time, we can maximize the total network bandwidth, assuming there is no congestion between the different network links between two processors.
        Therefore at the end of this stage, there is a correct copy of each segment on one of the processors. **/
    for (int j = 0; j < (dmem_nprocs - 1); j++) {
        n = 0;
        data_sent = 0;
        while (data_sent < seg_len*sizeof(float)) {
            n = write(clientfds[recv_struct.tag],((void *)(&dst[((dmem_proc - j + dmem_nprocs) % dmem_nprocs)*seg_len])) + data_sent,seg_len * sizeof(float) - data_sent);
            if (n < 0) {
                printf("Sending failed with %s\n", strerror(errno));
                break;
            }
            data_sent += n;
            //printf("Sent %li bytes for a total of %lu out of %lu on thread %i\n", n, data_sent, len*sizeof(float), tag);
        }
        /** We need to synchronize with the recev thread to make sure we have all of the information before we send it on in the next iteration **/
        pthread_barrier_wait(&(recv_struct.barrier));
    }

    /** Now that each MPI processor has a segment lengthed piece with the correct result, we need to send all of the segments once around the ring
        to ensure that every processor has the full view of the reduce. **/
    for (int j = 0; j < (dmem_nprocs - 1); j++) {
        n = 0;
        data_sent = 0;
        while (data_sent < seg_len*sizeof(float)) {
            n = write(clientfds[recv_struct.tag],((void *)(&dst[((dmem_proc + 1 - j + dmem_nprocs) % dmem_nprocs)*seg_len])) + data_sent,seg_len * sizeof(float) - data_sent);
            if (n < 0) {
                printf("P%i: ERROR! Sending failed with %s\n", dmem_proc, strerror(errno));
                break;
            }
            data_sent += n;
        }
        /** We need to synchronize with the recev thread to make sure we have all of the information before we send it on in the next iteration **/
        pthread_barrier_wait(&(recv_struct.barrier));
    }
    
    pthread_join(recv_threadid, &value_ptr);

}

/**
 * This is the version that spawns a number of threads for you and handles all of the thread stuff.
 **/
void ThreadedAllReduce::allreduce(float * src, float * dst, size_t len) {

    void * value_ptr;
    double timer1s, timer1e; 
    size_t remainder_len;
    struct spawn_thread_struct * thread_data = (struct spawn_thread_struct *)calloc(nThreads,sizeof(struct spawn_thread_struct));

    remainder_len = len % (nThreads*dmem_nprocs);
    len = len - remainder_len;

    timer1s = MPI_Wtime(); 
    
    for (int j = 0; j < nThreads; j++) { 
        thread_data[j].tag = j; 
        //myreduce->allreduce(&(data2_send[thr_no*(nItems2/nThreads)]), &(data2_recv_thr[thr_no*(nItems2/nThreads)]), nItems2 / nThreads, thr_no);
        thread_data[j].len = len / nThreads;
        thread_data[j].recv_buff = &(dst[j*len/nThreads]);
        thread_data[j].send_buff = &(src[j*len/nThreads]);
        thread_data[j].obj_ctx = this;
        pthread_create(&(thread_ids[j]), NULL,  &thread_spawn, (void *) &thread_data[j]); 
    } 
         
    for (int j = 0; j < nThreads; j++) { 
        pthread_join(thread_ids[j], &value_ptr); 
    }
    MPI_Barrier(mpi_ctx); 

    if (remainder_len != 0) {
        MPI_Allreduce(&(src[len]), &(dst[len]), remainder_len, MPI_FLOAT, MPI_SUM, mpi_ctx);
    }
         
    timer1e = MPI_Wtime();
 
    if (dmem_proc == 0) {
        printf("P%i: ThreadedAllReduce reduce in %f seconds at a rate of %f Gbit/s\n", dmem_proc, (timer1e - timer1s),
               ((len*sizeof(float)*8.0)/(timer1e - timer1s) / 1024.0 / 1024.0 / 1024.0)); 
    }
    
}

#endif DMEM_COMPILE
