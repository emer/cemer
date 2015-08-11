#!/bin/bash  
logger "Calling resume.sh" $@ 
echo "Calling resume.sh" $@

#Slurm can provide names in its compacted form, e.g. ec1[0-9]
#so we need to expand it out into a separate list of hosts

HOSTNAMES=`scontrol show hostnames $@`

#Make sure our VPN tunnel is open and working...
ping -c 3 -W 1 172.31.1.1

IFS=$'\n'
for host in $HOSTNAMES
do
  echo "Starting " $host
  logger "Starting " $host
  python /etc/slurm-llnl/launch_ec2_instance.py $host
done

