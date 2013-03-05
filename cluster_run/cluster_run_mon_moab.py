#!/usr/bin/env python
import cluster_run_mon_lib as runmon
# get all the base code -- this file should be on the python path or 
# (more simply) just copied over to same directory where this runs
# suggest putting both in ~/cluster_run_<cluster_name>

#############################################################################
# STANDARD USER CONFIGURABLE PARAMETERS

# name of queue -- used for a few things -- replace with actual!
clust_queue = "janus-short"

# full path to single processor job submission script
# STRONGLY recommend using the pyqsub based commands avail in 
# emergent/cluster_run/ directory (where this script lives as well)
#
# the sp_qsub_cmd takes args of <n_threads> <run_time> <full_command>
cluster_run_mon_lib.sp_qsub_cmd = 'sp_qsub_q'
# can add an automatic -q <queue> arg here to specify a queue
# in general it is best to have a different script for each queue
# because the emergent preferences have relevant settings for them
# sp_qsub_args = ""
cluster_run_mon_lib.sp_qsub_args = "-q " + clust_queue

# the dm_qsub_cmd takes args of <mpi_nodes> <n_threads> <run_time> <full_command>
cluster_run_mon_lib.dm_qsub_cmd = 'dm_qsub_q'
# cluster_run_mon_lib.dm_qsub_args = ""
cluster_run_mon_lib.dm_qsub_args = "-q " + clust_queue

# it is essential that these scripts return the cluster job number in the format
# created: JOB.<jobid>.sh -- we parse that return val to get the jobid to monitor
# further (you can of course do this in some other way if necessary)

# qstat-like command -- for quering a specific job_no 
# sge = qstat -j <job_no>
# moab = qstat <job_no>
# job_no will automatically be appended to end of command
cluster_run_mon_lib.qstat_cmd = "qstat"
cluster_run_mon_lib.qstat_args = ""  # here is where you put the -j if needed

# parser for qstat output -- output is different so need diff parsers
# options are sge for sge, moab for moab/torque
cluster_run_mon_lib.qstat_parser = "moab"

# qdel-like command -- for killing a job
# killjob is a special command that also deletes the JOB.* files -- see pykilljob
# be sure to use the _f version that does not prompt!
# in emergent/cluster_run directory
# job_no will automatically be appended to end of command
cluster_run_mon_lib.qdel_cmd = "killjob_f"
cluster_run_mon_lib.qdel_args = ""

# showq-like command -- this should return overall status of all users jobs
# and general info on status of cluster
# moab = showq 
# pyshowq for SGE (checked into emergent/cluster_run showq)
cluster_run_mon_lib.showq_cmd = "showq"
cluster_run_mon_lib.showq_args = ""
# parser function to use for showq output -- complex enough that this is most
# efficient way to do it
# options are pyshowq, moab
cluster_run_mon_lib.showq_parser = "moab"

# number of runtime minutes during which the script will continue to update the 
# output info from the job (job_out, dat_files)
cluster_run_mon_lib.job_update_window = 3

# set to true for more debugging info
cluster_run_mon_lib.debug = False
# cluster_run_mon_lib.debug = True

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
