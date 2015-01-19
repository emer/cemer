#!/usr/bin/env python
import sys
import cluster_run_mon_lib as runmon
# get all the base code -- this file should be on the python path or 
# (more simply) just copied over to same directory where this runs
# suggest putting both in ~/cluster_run_<cluster_name>

#############################################################################
# STANDARD USER CONFIGURABLE PARAMETERS

runmon.submit_mode = "ec2_compute"

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
