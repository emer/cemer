#!/usr/bin/env python
import sys
import cluster_run_mon_lib as runmon
# get all the base code -- this file should be on the python path or 
# (more simply) just copied over to same directory where this runs
# suggest putting both in ~/cluster_run_<cluster_name>

#############################################################################
# STANDARD USER CONFIGURABLE PARAMETERS

# name of queue -- used for a few things -- replace with actual!
runmon.clust_queue = "blanca-ccn"

# specifies if the cluster uses QoS or partitions to specify the queue. This is only relevant for SLURM
runmon.use_qos = True

runmon.use_cuda = False

# full path to single processor job submission script
# STRONGLY recommend using the pyqsub based commands avail in 
# emergent/cluster_run/ directory (where this script lives as well)
#
# the sp_qsub_cmd takes args of <n_threads> <run_time> <full_command>
# runmon.sp_qsub_cmd = 'pyqsub'
# runmon.sp_qsub_args = "--threaded --quick --jobtype sp_qsub_q"

# the dm_qsub_cmd takes args of <mpi_nodes> <per_node> <n_threads> <run_time> <full_command>
# runmon.dm_qsub_cmd = 'pyqsub'
# runmon.dm_qsub_args = "--threaded --quick --jobtype dm_qsub_q"

# it is essential that these scripts return the cluster job number in the format
# created: JOB.<jobid>.sh -- we parse that return val to get the jobid to monitor
# further (you can of course do this in some other way by editing code below)

# specify the job launcher command with all of its parameters. The parameters might be
# specific to the cluster environment, such as the network interface the MPI libraries should use
#job_launcher = "mpirun"
runmon.job_launcher = 'mpirun --bind-to none'
#runmon.job_launcher = 'mpirun --bind-to none --mca btl_tcp_if_include bond0'

# specify a setup script that is run at the beginning of a job on the node the job is executed,
# this allows to ensure that all of the enivronment variables, paths and other resources necessary
# on the compute node is available and correctly setup.
runmon.path_setup = "/work/ccnlab/bin/emergent_setup.sh"

# qstat-like command -- for quering a specific job_no 
# sge = qstat -j <job_no>
# moab = qstat <job_no>
# job_no will automatically be appended to end of command
runmon.qstat_cmd = "scontrol"
runmon.qstat_args = ["show", "JobId"]  # here is where you put the -j if needed

# parser for qstat output -- output is different so need diff parsers
# options are sge for sge, moab for moab/torque
runmon.qstat_parser = "slurm"

# qdel-like command -- for killing a job
# job_no will automatically be appended to end of command
runmon.qdel_cmd = "scancel"
runmon.qdel_args = ""

# showq-like command -- this should return overall status of all users jobs
# and general info on status of cluster
# moab = showq 
# pyshowq for SGE (checked into emergent/cluster_run showq)
runmon.showq_cmd = "squeue"
runmon.showq_args = ['-a', '-h', '-p', runmon.clust_queue, '-o', "%.18i %.14P %.30j %.10u %.8T %.10M %.9l %.6D %.10C %.10m %.10p %R"]
# parser function to use for showq output -- complex enough that this is most efficient way to do it
runmon.showq_parser = "slurm"

# number of runtime minutes during which the script will continue to update the 
# output info from the job (job_out, dat_files)
runmon.job_update_window = 3

# On large public cluster, there can be hundreds or even thousands of jobs running concurrently
# querying all jobs to compile the cluster_info table can be prohibitively expensive in that case
runmon.disable_cluster_info = False

# specify the email address of the user to notify of job events
runmon.mail_user = "$USER"
# specify the type of job events the user should be notified about via email
runmon.mail_type = "FAIL"

# set to true for more debugging info
runmon.debug = False
# runmon.debug = True

# END OF STANDARD USER CONFIGURABLE PARAMETERS
#############################################################################

if __name__ == '__main__':
    try:
        if len(sys.argv) == 1:
            runmon.main()
        else:
            runmon.main_background()
    except KeyboardInterrupt:
        print '\n\nQuitting at user request (Ctrl-C).'
