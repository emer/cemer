#!/usr/bin/env python

import sys, subprocess, re, time, bsddb, json, syslog

################################
# These variables are user specific and need to be adjusted to the users EC2 account
################################
basedir="/var/lib/slurm-llnl/"

ec2_ami = "ami-d17278e1"
ec2_sg = ""
ec2_sshkey = "standard_key"
ec2_region = "us-west-2"
ec2_user = ""
ec2_key = ""

ec2_3xl_price = "0.10"
################################
################################

 

#Test to see if the host is already launch and if not register the name in a persistend database to keep track of which slurm names fit with which ec2 instance ids
db = bsddb.hashopen(basedir + 'ec2instances.db', 'c')

syslog.syslog(str(db.keys()))
print str(db.keys())

if (sys.argv[1] in db):
    syslog.syslog("Instance already launched " + sys.argv[1])
    print db[sys.argv[1]]
    exit()
else:
    syslog.syslog("Launching new instance " + sys.argv[1])
    db[sys.argv[1]] = "launching"
    db.sync()

server_info = {}

def launch_spot (launch_name, instance_type, spot_price):
    ec2launch = "ec2-request-spot-instances " + ec2_ami + " --price " + str(spot_price) + " -n 1 -g " + ec2_sg + " -k " + ec2_sshkey +  " -t " + instance_type + " --region " + ec2_region + " -O " + ec2_user + " -W " + ec2_key + " -d " + launch_name + " --availability-zone us-west-2b"
    cmdsub = ec2launch.split()
    print "cmd: " + str(cmdsub)
    result = subprocess.check_output(cmdsub)
    print "Result: " + str(result)

    spotre = re.compile("^SPOTINSTANCEREQUEST\s*(.*?)\s*" + str(spot_price) + ".*", re.MULTILINE)
    spot = spotre.search(result)
    spotid = spot.group(1)

    print spotid

    while (True):
        ec2spotinstance = "ec2-describe-spot-instance-requests " + spotid + " -O " + ec2_user + " -W " + ec2_key + " --region " + ec2_region
        cmdsub = ec2spotinstance.split()
        result = subprocess.check_output(cmdsub)
        #print result

        instancere = re.compile("^SPOTINSTANCEREQUEST\s*.*\s+(.+?)\s*ami-.*", re.MULTILINE)
        instance = instancere.search(result)
        instance_name = instance.group(1)
        
        if len(instance_name) > 5:
            syslog.syslog(launch_name + " -> " + instance_name)
            print "*****"
            print instance_name
            print "*******"
            print len(instance_name)
            return instance_name

def launch_instance (launch_name, instance_type):
    ec2launch = "ec2-run-instances " + ec2_ami + " -n 1 -g " + ec2_sg + " -k " + ec2_sshkey +  " -t " + instance_type + " --region " + ec2_region + " -O " + ec2_user + " -W " + ec2_key + " -d " + sys.argv[1] + " --availability-zone us-west-2b"
    cmdsub = ec2launch.split()
    print "cmd: " + str(cmdsub)
    result = subprocess.check_output(cmdsub)
    print "Result: " + str(result)
    
    #Parse information out of textual response
    instancere = re.compile("^INSTANCE\s*(.*?)\s*ami-.*", re.MULTILINE)
    instance = instancere.search(result)
    instance_name = instance.group(1)
    return instance_name

#Launch instance
if sys.argv[1].startswith("ec2-4s-"):
    instance_name = launch_spot(sys.argv[1], "c4.xlarge", 0.1)
elif sys.argv[1].startswith("ec2-4-"):
    instance_name = launch_instance(sys.argv[1], "c4.xlarge")
elif sys.argv[1].startswith("ec2-2s-"):
    instance_name = launch_spot(sys.argv[1], "c4.large", 0.05)
elif sys.argv[1].startswith("ec2-2-"):
    instance_name = launch_instance(sys.argv[1], "c4.large")
elif sys.argv[1].startswith("ec2-mpis-"):
    instance_name = launch_spot(sys.argv[1], "c4.8xlarge",0.50)
else:
    syslog.syslog("Using an instance type we don't know about, failing")
    exit(1)


server_info["instance_name"] = instance_name

#Probe instance information to determin IPs
ec2getinstance = "ec2-describe-instances -O AKIAJ4SCF5FBPXL5OITQ -W WaqVJLFaKMTLG34fB1hOoHKTxjvP628lYEdbtMm/  --region us-west-2 " + instance_name

while (True):
    cmdsub = ec2getinstance.split()
    result = subprocess.check_output(cmdsub)

    #Parse IP information out of results
    publicipre = re.compile("^NICASSOCIATION\s*(.*?)\s+", re.MULTILINE)
    publicipm = publicipre.search(result)
    privateipre = re.compile("^NIC\s*(.*?)\s+", re.MULTILINE)
    privateipm = publicipre.search(result)

    #Once we have a 
    if (publicipm):
        
        publicip = publicipm.group(1)
        publicnamere = re.compile("^INSTANCE.*?ami-.+?\s+(.*?)\s+", re.MULTILINE)
        publicnamem = publicnamere.search(result)

        privateipre = re.compile("^INSTANCE.*?" + publicip + "\s+(.*?)\s+", re.MULTILINE)
        privateipm = privateipre.search(result)
        privateip = privateipm.group(1)
        privatenamere = re.compile("^INSTANCE.*?compute.amazonaws.com\s+(.*?)\s+", re.MULTILINE)
        privatenamem = privatenamere.search(result)

        #Update slurm to reflect the correct ip addresses of these cloud instances
        scontrolcmd = "scontrol update nodename=" + sys.argv[1] + " nodeaddr=" + privateip + " nodehostname=" + privatenamem.group(1)
        cmdsub = scontrolcmd.split()
        result = subprocess.check_output(cmdsub)


        server_info["publicname"] = publicnamem.group(1)
        server_info["privatename"] = privatenamem.group(1)
        server_info["publicip"] = publicip
        server_info["privateip"] = privateip

        db[sys.argv[1]] = json.dumps(server_info)
        syslog.syslog(db[sys.argv[1]])

        break
    time.sleep(10)
