#!/usr/bin/env python

import sys, subprocess, re, time, bsddb, json


################################
# These variables are user specific and need to be adjusted to the users EC2 account
################################
basedir="/etc/slurm-llnl/"
ec2_region = "us-west-2"
ec2_user = ""
ec2_key = ""
################################
################################


db = bsddb.hashopen(basedir + 'ec2instances.db', 'c')

if (sys.argv[1] in db):
    print "Instance launched, terminating it"
    server_info = json.loads(db[sys.argv[1]])
    print server_info

else:
    print "No instance registered under this name, ignoring"
    exit()


ec2launch = "ec2-terminate-instances --region " + ec2_region + " -O " + ec2_user + " -W " + ec2_key + server_info[unicode("instance_name")]
cmdsub = ec2launch.split()
result = subprocess.check_output(cmdsub)

del db[sys.argv[1]]
