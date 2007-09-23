#!/bin/csh -f
# this shell script is passed one or more epoch log files,
# and calls the epoch_log_analysis.proj project on each in turn,
# sending the results to the log file name plus a ".dt" suffix

foreach i ($*)
    echo "================================================================="
    echo $i
    emergent -nogui -ni -p epoch_log_analysis.proj logfile=$i >& $i.dt
    cat $i.dt
end
