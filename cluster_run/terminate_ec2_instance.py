#!/usr/bin/env python

import sys, subprocess, re, time, bsddb3, json, syslog


################################
# These variables are user specific and need to be adjusted to the users EC2 account
################################
basedir="/var/lib/slurm-llnl/"

ec2_region = "us-west-2"
ec2_user = ""
ec2_key = ""
################################
################################

syslog.syslog("Test syslog")

print basedir + 'ec2instances.db'
db =  bsddb3.db.DB()
db.set_cachesize(1,0)
db.open(basedir + 'ec2instances.db', None, bsddb3.db.DB_HASH, bsddb3.db.DB_CREATE)
#db = bsddb.hashopen(basedir + 'ec2instances.db', 'c')

if (sys.argv[1] in db):
    syslog.syslog("Instance launched, terminating it " + sys.argv[1])
    server_info = json.loads(db[sys.argv[1]])
    syslog.syslog(str(server_info))

else:
    print "No instance registered under this name, ignoring"
    syslog.syslog("No instance registered under this name (" + sys.argv[1] + "), ignoring")
    db.close()
    exit()


ec2launch = "ec2-terminate-instances --region " + ec2_region + " -O " + ec2_user + " -W " + ec2_key + " " + server_info[unicode("instance_name")]
cmdsub = ec2launch.split()
result = subprocess.check_output(cmdsub)
syslog.syslog(result)

db.sync()
del db[sys.argv[1]]
db.sync()
db.close()
