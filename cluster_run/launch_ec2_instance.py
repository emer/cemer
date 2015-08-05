#!/usr/bin/env python

import sys, subprocess, re, time, bsddb, json

################################
# These variables are user specific and need to be adjusted to the users EC2 account
################################
basedir="/etc/slurm-llnl/"

ec2_ami = "ami-436d6573"
ec2_sg = ""
ec2_sshkey = "standard_key"
ec2_region = "us-west-2"
ec2_user = ""
ec2_key = ""
################################
################################



#Test to see if the host is already launch and if not register the name in a persistend database to keep track of which slurm names fit with which ec2 instance ids
db = bsddb.hashopen(basedir + 'ec2instances.db', 'c')

if (sys.argv[1] in db):
    print "Instance already launched"
    print db[sys.argv[1]]
    del db[sys.argv[1]]
    exit()
else:
    print "Launching new instance"
    db[sys.argv[1]] = "launching"

server_info = {}

#Launch instance
ec2launch = "ec2-run-instances " + ec2_ami + " -n 1 -g " + ec2_sg + " -k " + ec2_sshkey +  " -t t2.micro --region " + ec2_region + " -O " + ec2_user + " -W " + ec2_key + " -d " + sys.argv[1]
cmdsub = ec2launch.split()
print "cmd: " + str(cmdsub)
result = subprocess.check_output(cmdsub)
print "Result: " + str(result)

#Parse information out of textual response
instancere = re.compile("^INSTANCE\s*(.*?)\s*ami-.*", re.MULTILINE)
instance = instancere.search(result)
instance_name = instance.group(1)
print instance_name

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

        break
    time.sleep(10)
