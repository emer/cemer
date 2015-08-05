#!/bin/bash  
logger "Calling suspend " $@ 
echo "Calling suspend " $@

#Slurm can provide names in its compacted form, e.g. ec1[0-9]
#so we need to expand it out into a separate list of hosts

HOSTNAMES=`scontrol show hostnames $@`

IFS=$'\n'
for host in $HOSTNAMES
do
  echo "Terminating $host"
  logger "Terminating $host"
  python terminate_ec2_instance.py $host
done