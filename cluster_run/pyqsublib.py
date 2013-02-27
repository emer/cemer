#!/usr/bin/python
from subprocess import call,Popen,PIPE
import os, time, re, string

DEBUG = False

###################################################################################
class ClusterJobManager:
    '''Generic cluster job manager - abstract base class'''
    def __init__( self, verbose=False, quick=True ):
        self.verbose = verbose
        self.quick = quick
        self.job_id = 0
        self.subCmd = ["qsub -h"]
        self.script_filename = os.getcwd() + "/JOB.TMP.SH"
        self.script_shell = "/bin/bash"
        self.job_launcher = "mpirun" # hardcode program name so fails gracefully
        self.last_err = ""
    def enableVerbose(self):
        self.verbose = True
    def disableVerbose(self):
        self.verbose = False
    def isVerbose(self):
        return self.verbose
    def enableQuick(self):
        self.quick = True
    def disableQuick(self):
        self.quick = False
    def isQuick(self):
        return self.quick
    def jobID(self):
        return self.job_id
    def jobLauncher(self):
        return self.job_launcher
    def scriptFileName(self):
        return self.script_filename
    def setScriptFileName(self, name):
        # check for valid filename
        if name == "" or name == None:
            self.last_err = "%s is not a valid filename" % name
            return False
        else:
            self.script_filename = name
            return True
    def generateJobScript( self, job ):
        assert 0, "generateJobScript must be implemented!"
    def submitJob( self, job ):
        assert 0, "submitJob must be implemented!"
    def checkSubmission( self, job ):
        assert 0, "checkSubmission must be implemented!"
    def validateJob(self, job):
        if False == isinstance(job, ClusterJob):
            self.last_err = "invalid job type: job of class %s, is not of type %s" % (job.__class__, ClusterJob)
            return False
        else:
            return True
    def lastErr( self ):
        return "error: " + self.last_err
    def usage( self ):
        assert 0, "usage must be implemented!"


######################################################################################################
class SGEJobManager( ClusterJobManager ):
    '''Cluster job manager interface for Grid Engine (SGE)'''
    def __init__(self, verbose=False, quick=False):
        ClusterJobManager.__init__(self, verbose, quick)
        #if os.environ.get("SGE_ROOT") == None:
        #    assert 0, "$SGE_ROOT not found, cannot use Grid Engine!"

    def generateJobScript( self, job ):
        if False == ClusterJobManager.validateJob(self, job): 
            return False

        if job.isThreaded() and (job.numCores() !=1 ) and (job.numCores()%2 != 0):
            self.last_err = "threaded jobs under SGE must be of 1,2,4,6, or 8 cores"
            return False
        
        file= open(self.script_filename,"w")
        file.write("#!/bin/sh\n")
        if job.isArrayJob():
            self.subCmd.append("-b y")
        file.write("#$ -S " + self.script_shell + "\n")
        self.subCmd.append("-S " + self.script_shell)
        file.write("#$ -cwd\n")
        self.subCmd.append("-cwd")
        file.write("#$ -j y\n")
        self.subCmd.append("-j y")
        file.write("#$ -R y\n")
        self.subCmd.append("-R y")

        if job.memorySize() != None:
            file.write("#$ -l h_vmem=%s\n" % job.memorySize() )
            self.subCmd.append("-l h_vmem=%s" % job.memorySize() )

        # set queue
        if job.queueName() == '':
            #SGE currently has just one queue 'all.q'
            job.setQueue('all.q')
        file.write("#$ -q %s\n" % job.queueName() )
        self.subCmd.append("-q %s" % job.queueName() )
                
        file.write("#$ -l h_rt=%s\n" % job.wallTime() ) 
        self.subCmd.append("-l h_rt=%s" % job.wallTime())
        if job.isThreaded():
            file.write("#$ -pe threaded_mpi_%d %d\n" % (job.numCores(), job.numCores()*job.numNodes()))
            self.subCmd.append("-pe threaded_mpi_%d %d" % (job.numCores(), job.numCores()*job.numNodes()))
        else :       
            file.write("#$ -pe mpi %d\n" % (job.numCores()*job.numNodes()) )
            self.subCmd.append("-pe mpi %d" % (job.numCores()*job.numNodes()))
        if job.isArrayJob():
                file.write("#$ -t %d-%d:%d\n" % (int(job.taskStart()), int(job.numTasks()), int(job.taskStep()) ))
                self.subCmd.append("-t %d-%d:%d" % (job.taskStart(), job.numTasks(), job.taskStep() ) )
                file.write("#$ -tc %d\n" % job.taskThrottle() )
                self.subCmd.append("-tc %d" % job.taskThrottle())
                file.write("TASK_ID=$SGE_TASK_ID\n")
                    
        file.write("export DISPLAY=:0.0\n")
        file.write("\n")
     
        file.write(". /usr/local/Modules/3.2.6/init/bash\n")
	file.write("module load tools/sge mpi2/ib/openmpi-1.3.2-gcc.sge\n")
	file.write("\n")

        file.write("## generate a proper hostfile for SGE\n")
        file.write("mkdir -p ${SGE_O_WORKDIR}/.tempfiles/$$\n")
	if job.isThreaded():
		file.write("/usr/local/bin/pehostfile2machinefile $PE_HOSTFILE | sort -u > ${SGE_O_WORKDIR}/.tempfiles/$$/hostfile\n")
	else :
		file.write("/usr/local/bin/pehostfile2machinefile $PE_HOSTFILE > ${SGE_O_WORKDIR}/.tempfiles/$$/hostfile\n")
        if self.verbose: 
            file.write("TOTPROCS=$(( $(cat $PE_HOSTFILE | wc -l) * $(head -n 1 $PE_HOSTFILE | awk '{print $2}') ))\n")
            file.write('echo "TOTPROCS: $TOTPROCS"\n')
            file.write("TOTNODES=`wc -l ${SGE_O_WORKDIR}/.tempfiles/$$/hostfile | awk '{print $1}'`\n")
            file.write('echo "TOTNODES: $TOTNODES"\n')
        file.write("\n")

        if job.isThreaded():
            file.write("%s -np %d -machinefile ${SGE_O_WORKDIR}/.tempfiles/$$/hostfile %s \n" % (self.job_launcher, job.numNodes(), job.userCmd()))
        else :       
            file.write("%s -np %d -machinefile ${SGE_O_WORKDIR}/.tempfiles/$$/hostfile %s \n" % (self.job_launcher, job.numNodes()*job.numCores(), job.userCmd()))

        file.write("\n")
        file.write("# remove our temp hostfile\n")
        file.write("rm -rf ${SGE_O_WORKDIR}/.tempfiles/$$\n")
        file.close()
        return True

    def submitJob( self, job ):
        '''Sumbits the job to the SGE resource manager'''
        if False == ClusterJobManager.validateJob(self, job): 
            return False

        if DEBUG:
            print " ".join(self.subCmd)
        
        #os.popen("chmod a+x " + self.script_filename, "r")
        cmd = ['chmod','a+x', self.script_filename]
        if DEBUG:
            print cmd
        Popen(cmd)

        # now submitting as a binary job - so must pass options via command line since embedded options are ignored for binary jobs
        cmd = " ".join(self.subCmd) + " " + self.script_filename
        if DEBUG:
            print cmd
        stdoutstr = Popen(cmd, shell=True, stdout=PIPE).communicate()[0]
        self.job_id = re.search('([0-9]+)', stdoutstr).group()

        cmd = ['qalter','-o','JOB.' + self.job_id + '.out', '-N', 'JOB.' + self.job_id + '.sh', self.job_id]
        if DEBUG:
            print cmd
        Popen(cmd)
        
        cmd = ['qalter', '-h', 'U', self.job_id]
        if DEBUG:
            print cmd
        Popen(cmd)

        if job.isArrayJob():
            cmd = ['cp', self.script_filename, 'JOB.' + self.job_id + '.sh']
            if DEBUG:
                print cmd
            Popen(cmd)
        else:
            cmd = ['mv', self.script_filename, 'JOB.' + self.job_id + '.sh']
            if DEBUG:
                print cmd
            Popen(cmd)
        print "created: JOB.%s.sh" % (self.job_id)
        if not self.quick:
            time.sleep(5)
            print os.popen('qstat | grep -A1 ^job-ID', 'r').read()
            print os.popen('qstat | grep ' + self.job_id, 'r').read()
            print os.popen('cat JOB.' + self.job_id + '.out', 'r').read()
        return True

    def checkSubmission( self, job ):
        print "checking SGE job submission..."
        
##########################################################################################################################
class PBSJobManager( ClusterJobManager ):
    '''Cluster job manager interface for Torque/Maui'''
    def __init__(self, verbose=False, quick=False):
        ClusterJobManager.__init__(self, verbose, quick)
        #with open(os.devnull, 'w') as fp:
        #    if call(["which","pbsnodes"], stdout=fp) != 0:
        #        assert 0, "ERROR: pbsnodes not found...be sure proper dotkit/module is loaded in your environment"
    def generateJobScript( self, job ):
        if False == ClusterJobManager.validateJob(self, job): 
            return False

        file = open(self.script_filename,"w")
        file.write("#!/bin/sh\n")
        file.write("#PBS -S " + self.script_shell + "\n")
        self.subCmd.append("-S " + self.script_shell)
        file.write("#PBS -j oe\n")
        self.subCmd.append("-j oe")
        file.write("#PBS -l walltime=%s\n" % job.wallTime() )
        self.subCmd.append("-l walltime=%s" % job.wallTime())
        file.write("#PBS -l nodes=%d:ppn=%d\n" % (job.numNodes(), job.numCores() ) )
        self.subCmd.append("-l nodes=%d:ppn=%d" % (job.numNodes(), job.numCores() ))
        file.write("#PBS -l naccesspolicy=uniqueuser\n")
        self.subCmd.append("-l naccesspolicy=uniqueuser")
        if job.memorySize() != None:
            file.write("#PBS -l pmem=%s\n" % job.memorySize() )
            self.subCmd.append("-l pmem=%s" % job.memorySize() )
            
        # determine queue...user argument overrides
        if job.queueName() == '':
            hours = int(job.wallTime().split(":")[0])
            totCores = job.numNodes() * job.numCores()
            if job.memorySize() != None and int(job.memorySize()[:-2]) >= 24:
                if hours <= 120 and totCores <= 32:
                    job.setQueue('himem')
                else:
                    assert 0, "Himem node limited to 32 cores and walltime < 120 hours"
            elif hours <= 1 and totCores <= 512:
                job.setQueue('janus-debug')
            elif hours <= 4 and totCores <= 1024:
                job.setQueue('janus-short')
            elif hours <= 24 and totCores <= 1024:
                job.setQueue('janus-normal')
            elif hours <= 24 and totCores > 1024 and totCores <= 5120:
                job.setQueue('jaunus-wide')
            elif hours <=168 and totCores <= 512:
                job.setQueue('janus-long')
            else:
                job.setQueue('batch')
        file.write("#PBS -q %s\n" % job.queueName() )
        self.subCmd.append("-q %s" % job.queueName() )
            

        if job.isArrayJob():
            file.write("#PBS -t %d-%d%%%d\n" % (job.taskStart(), job.numTasks(), job.taskThrottle() ))
            file.write("TASK_ID=$PBS_ARRAYID\n")

        file.write("\n")
        file.write("# need to 'use' proper dotkits\n")
        file.write(". /curc/tools/utils/dkinit\n")
        #file.write("reuse .openmpi-1.4.3_ib\n")
        file.write("reuse /projects/oreillyr/bin/emergent\n")

        #file.write("export DISPLAY=:0.0\n\n")
        file.write("# change to working dir\n")
        file.write("cd $PBS_O_WORKDIR\n")
        file.write("\n")

        # LIKELY TO REMOVE ON RC/JANUS
        file.write("# make a proper job hostfile\n")
        file.write("mkdir -p ${PBS_O_WORKDIR}/.tempfiles/$$\n")
        file.write("cat $PBS_NODEFILE > ${PBS_O_WORKDIR}/.tempfiles/$$/orig_hostfile\n")
        file.write("cat $PBS_NODEFILE | sort -u > ${PBS_O_WORKDIR}/.tempfiles/$$/hostfile\n")
        if self.verbose: 
            file.write("##totprocs\n")
            file.write("TOTPROCS=`wc -l $PBS_NODEFILE | awk '{print $1}'`\n")
            file.write('echo "TOTPROCS: $TOTPROCS"\n')
            file.write("TOTNODES=`wc -l ${PBS_O_WORKDIR}/.tempfiles/$$/hostfile | awk '{print $1}'`\n")
            file.write('echo "TOTNODES: $TOTNODES"\n')
        file.write("\n")

        if job.isThreaded():
            file.write("%s -np %d -machinefile ${PBS_O_WORKDIR}/.tempfiles/$$/hostfile %s \n" % (self.job_launcher, job.numNodes(), job.userCmd()))
        else :       
            file.write("%s -np %d -machinefile ${PBS_O_WORKDIR}/.tempfiles/$$/hostfile %s \n" % (self.job_launcher, job.numNodes()*job.numCores(), job.userCmd()))
    
        file.write("\n")
        file.write("# we're done...so remove the hostfile\n")
        file.write("rm -rf ${PBS_O_WORKDIR}/.tempfiles/$$\n")
        file.close()
        return True

    def submitJob( self, job ):
        if False == ClusterJobManager.validateJob(self, job): 
            return False

        cmd = ['chmod','a+x', self.script_filename]
        if DEBUG:
            print cmd
        Popen(cmd)
        cmd = ['qsub', '-h', self.script_filename]
        if DEBUG:
            print cmd
        stdoutstr = Popen(cmd,stdout=PIPE).communicate()[0]
        
        self.job_id = re.search('([0-9]+)', stdoutstr).group()
        cmd = ['mv',self.script_filename,'JOB.'+ self.job_id + '.sh']
        if DEBUG:
            print cmd
        Popen(cmd)
        if job.isArrayJob():
            cmd =['qalter', '-o', 'JOB.'+ self.job_id +'.out', '-N', 'JOB.'+self.job_id+'.sh', self.job_id+'[]']
            if DEBUG:
                print cmd
            Popen(cmd)
            time.sleep(5)
            cmd = ['qrls', self.job_id+'[]']
            if DEBUG:
                print cmd
            Popen(cmd)
        else:
            cmd = ['qalter', '-o', 'JOB.'+ self.job_id + '.out', '-N', 'JOB.' + self.job_id + '.sh', self.job_id]
            if DEBUG:
                print cmd
            Popen(cmd)
            time.sleep(5)
            cmd = ['qrls', self.job_id]
            if DEBUG:
                print cmd
            Popen(cmd)
        print "created: JOB.%s.sh" % (self.job_id)
    
        # checkjob doesn't seem to work well on RC
        #if not self.quick:
            #time.sleep(5)
            #if job.isArrayJob():
            #    print Popen(['checkjob', self.job_id+'[]', '|','grep','-A1','Allocated'],stdout=PIPE).communicate()[0]
            #else:
            #    print Popen(['checkjob', self.job_id, '|', 'grep', '-A1', 'Allocated'],stdout=PIPE).communicate()[0]
            #print Popen(['cat', 'JOB.' + self.job_id + '.out'],stdout=PIPE).communicate()[0]
    
        return True

    def checkSubmission( self, job ):
        print "checking PBS job submission..."

#####################################################################################

class ClusterJob:
    '''Generic cluster job - abstract base class'''
    # @TODO determine proper node/core limits
    MAX_NODES = 5120
    MAX_CORES = 12
    def __init__( self ):
        self.tasks = 0
        self.start = 1
        self.step = 1
        self.throttle = 1
        self.threaded = False
        self.nodes = 1
        self.cores = 1
        self.time = "24:00:00"
        self.queue = None
        self.memory = None
        self.user_cmd = "hostname"
        self.last_err =""
    def isThreaded(self):
        return self.threaded
    def enableThreaded( self ):
        self.threaded = True
    def disableThreaded( self ):
        self.threaded = False
    def isArrayJob(self):
        if self.tasks > 0:
            return True
        else:
            return False
    def numTasks( self ):
        return self.tasks
    def setNumTasks( self, n ):
        self.tasks = n
    def setTaskStart( self, n ):
        self.start = n
    def taskStart(self):
        return self.start
    def setTaskThrottle( self, n):
        self.throttle = n
    def taskThrottle(self):
        return self.throttle
    def setTaskStep( self, n):
        self.step = n
    def taskStep(self):
        return self.step
    def numNodes(self):
        return self.nodes
    def setNumNodes( self, n ):
        # nodes must be > 0 and < MAX_NODES
        if n > 0 and n <= ClusterJob.MAX_NODES:
            self.nodes = n
        else:
            self.last_err = "nodes must be > 0 and < %d" % ClusterJob.MAX_NODES
            return False
        return True
    def numCores(self):
        return self.cores
    def setNumCores( self, c ):
        # cores must be > 0 and < MAX_CORES
        if c > 0 and c <= ClusterJob.MAX_CORES:
            self.cores = c
        else:
            self.last_err = "cores must be > 0 and < %d" % ClusterJob.MAX_CORES
            return False
        return True
    def wallTime(self):
        return self.time
    def setWallTime( self, t ): 
        # check that it's of format: Nd|m|h (e.g. 2d or 12h or 35m)
        wt = t.lower()
        if ( wt.endswith("d") and int(wt[:-1]) > 0 ):
            self.time =""+str(24*int(wt[:-1]))+":00:00"
        elif ( wt.endswith("h") and int(wt[:-1]) > 0 ):
            self.time =""+wt[:-1]+":00:00"
        elif ( wt.endswith("m") and int(wt[:-1]) > 0 ):
            self.time ="00:"+wt[:-1]+":00"
        else:
            self.last_err = "Unsupported walltime format: %s" % t
            return False
        return True
    def setQueue( self, q):
        self.queue = q
    def queueName(self):
        if self.queue == None:
            return ''
        else:
            return self.queue
    def setMemory( self, mem):
        # check that it's of format: Nmb|gb (e.g. 4mb or 4gb)
        m = mem.lower()
        if m.endswith('mb') or m.endswith('gb') or m.isdigit():
            self.memory = m
        else:
            self.last_err = "Unsupported memory format: %s" % mem
            return False
        return True
    def memorySize( self ):
        return self.memory
    def userCmd(self):
        return self.user_cmd
    def setUserCmd( self, cmd ):
         # only check that path to user requested program exists
        if "" == os.popen("which " + cmd[0],"r").read()[:-1]:
            self.last_err = "cannot locate user program: %s" % cmd[0]
            return False
        else:
            self.user_cmd = string.join( cmd, " ")
            return True
    def lastErr( self ):
        return "error: " + self.last_err
    def usage( self ):
        assert 0, "usage must be implemented!"
    

class DMJob(ClusterJob):
    '''Distributed memory cluster job'''
    def __init__( self ):
        ClusterJob.__init__(self)
        self.nodes = 2
        self.cores = 2
    def setNumNodes( self, n ):
        if n*self.cores > 1:
            if ClusterJob.setNumNodes( self, n ):
                return True
            else: 
                return False 
        else:
            self.last_err ="for distributed memory job, nodes*cores must be > 1"
            return False
    def setNumCores( self, c ):
        if c*self.nodes > 1:
            if ClusterJob.setNumCores( self, c ):
                return True
            else:
                return False 
        else:
            self.last_err ="for distributed memory job, nodes*cores must be > 1"
            return False
    def usage( self ):
        return '''usage:
dm_qsub [-m memory] [-q queue] [-t 1-<tasks> [-r <limit>] ] <nodes> <ppn> <time> <cmd> 
    <memory> maximum amount of physical memory used by any single process of the job (in GB or MB)
    <queue> the name of the queue where you want your job to run. Without this the script
            will pick the appropriate queue, so only use this if you know what you're doing
    <tasks> is the number of array job tasks to run
    <limit> is how many array tasks to run simultaneously. This acts to limit
            how many tasks can run on the cluster simultaneously
    <nodes> is number of compute nodes to spread computation across
            (nodes communicate via MPI, and generally this is only useful if
            doing trial-wise MPI, not within network MPI)
     <ppn>  number of processors per node to use, where processors use threading
            to compute -- YOU MUST ALSO PASS n_threads=<ppn> as args to emergent
            or whatever other program you might be using, or have this as a default
            for emergent, 2 gives ~2x speedup, and 4 gives 3 -- above that is not
            typically useful
     <time> is #d (days) | #h (hours) | #m (minutes) and is used for
            scheduling your job -- please try to be as accurate as
            possible but don't worry about being wrong.
     <cmd>  is the usual kind of startup command to run the job
            e.g., emergent_mpi -nogui -ni -p my.proj.gz epochs=100 batches=10 n_threads=4
            (note that you MUST use _mpi version of executable!

Examples:
dm_qsub 8 4 24h emergent_mpi -nogui -ni -p my.proj.gz epochs=100 batches=10 n_threads=4
            (runs on 8 nodes, 4 procs per node, taking an estimated 24 hrs to finish)
dm_qsub 5 2 2d emergent_mpi -nogui -ni -p my.proj.gz epochs=100 batches=10 n_threads=2
            (runs on 5 nodes, 2 procs per node, taking an estimated 2 days to finish)
dm_qsub -m 4GB 5 2 2d emergent_mpi -nogui -ni -p my.proj.gz epochs=100 batches=10 n_threads=2
            (runs on 5 nodes, 2 procs per node, taking an estimated 2 days to finish and no more than 4GB of memory)
        '''
class SPJob(ClusterJob):
    '''Single processor cluster job'''
    def setNumNodes( self, n ):
        if n == 1:
            self.nodes = n
            return True
        else:
            self.last_err ="nodes must = 1 for single processor jobs"
            return False    
    def setNumCores( self, c ):
        if c == 1 or self.threaded:
            if ClusterJob.setNumCores( self, c ):
                return True
            else:
                return False
        else:
            self.last_err ="cores must = 1 for single processor non-threaded jobs"
            return False
    def usage( self ):
        return '''usage:
sp_qsub [-m memory] [-q queue] [-t 1-<tasks> [-r <limit>] ] <procs> <time> <cmd>  
    <memory> maximum amount of physical memory used by any single process of the job (in GB or MB)
    <queue> the name of the queue where you want your job to run. Without this the script
            will pick the appropriate queue, so only use this if you know what you're doing
    <tasks> is the number of array job tasks to run
    <limit> is how many array tasks to run simultaneously. This acts to limit
            how many tasks can run on the cluster simultaneously
   <procs>  number of processors (on one node) to use, where processors use threading
            to compute -- YOU MUST ALSO PASS n_threads=<ppn> as args to emergent
            or whatever other program you might be using, or have this as a default
            for emergent, 2 gives ~2x speedup, and 4 gives 3 -- above that is not
            typically useful
     <time> is #d (days) | #h (hours) | #m (minutes) and is used for
            scheduling your job -- please try to be as accurate as
            possible but don't worry about being wrong.
     <cmd>  is the usual kind of startup command to run the job
            e.g., emergent -nogui -ni -p my.proj.gz epochs=100 batches=10 tag=_run2

Examples:
sp_qsub 2 30m emergent -nogui -ni -p my.proj.gz epochs=100 batches=10 tag=_run3
            (runs something taking an estimated 30 minutes,
sp_qsub 100h emergent -nogui -ni -p monster.proj.gz epochs=100 batches=10 tag=_run4
            (runs something taking an estimated 3 days on 4 procs!)
sp_qsub -m 3GB 2 30m emergent -nogui -ni -p my.proj.gz epochs=100 batches=10 tag=_run3
            (runs something taking an estimated 30 minutes, and no more than 3GB of memory
        '''
#######################################################################################################
