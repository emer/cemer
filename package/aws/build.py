#!/usr/bin/env python

"""
Script to build emergent using Amazon Web Services (AWS).

To launch all builds simultaneously:
  rev=5751
  cd /c/src/emergent/package/aws
  rm ~/.ssh/known_hosts_amazon
  for distro in lucid maverick natty oneiric precise quantal; do
    for bits in 32 64; do
      mintty /c/src/emergent/package/aws/build.py $distro $bits $rev &
    done
  done

To keep passwords out of the python code, some configuration files
had to be created:
  $ cat ~/.boto
  [Credentials]
  aws_access_key_id = <SECRET>
  aws_secret_access_key = <SECRET>

  [Boto]
  debug = 1

  $ cat ~/.wgetrc
  http_user = <SECRET>
  http_password = <SECRET>

Additionally, two key-pairs were created:
  ec2grey
    This script copies the private key to each EC2 instance.
    The public key was manually appended to authorized_keys on grey.

  eco.pem
    This key pair was generated from the AWS console.  The public key is
    automatically put in the authorized_keys file on each EC2 instance.
    The private key goes on the machine (i.e., my laptop) that connects
    to the EC2 instances.
"""

import os, sys, traceback
from ec2 import Ec2Instance
from grey import GreyAccess
from ubuntu import UbuntuAmi
import util

def build(distro, bits, rev):
  """Build emergent for the given Ubuntu distribution and architecture,
  using the given svn revision number"""

  # Work in the US-West-2 region (Oregon).
  region_string = 'us-west-2'

  # Get the AMI ID for the specified Ubuntu instance.
  arch = 'i386' if bits == 32 else 'amd64'
  ubuntu_ami = UbuntuAmi(distro, region_string, arch, 'instance-store')
  ami_string = ubuntu_ami.get_ami()
  desc = '%s%d' % (distro, bits)

  # Spawn the Ubuntu instance.
  instance = Ec2Instance(ami_string, region_string, desc=desc)
  instance.start_instance('eco', 'eco')

  # Once it's running, give it ssh access to grey.
  if instance.wait_for_running():
    ip = instance.get_ip()
    if ip:
      # TODO: get username from environment variable / config file.
      # (Really should fix this in all the build scripts)
      grey = GreyAccess('dpfurlani', distro, bits, ip)
      grey.update_ip()

      # Use the eco.pem identity (private key) to connect to the EC2 instance
      # as the 'ubuntu' user; use scp to transfer the ec2grey private key to
      # the EC2 instance so it can authenticate ssh connections to grey using
      # it (since grey has the corresponding public key).
      try:
        home = os.environ['HOME']
        remote_user = 'ubuntu'
        ssh_key = '%s/.ssh/eco.pem' % home
        grey_access_key = '%s/.ssh/ec2grey' % home
        util.scp(grey_access_key, remote_user, ip, '/home/ubuntu/.ssh/', ssh_key)

        # Put a setup script on the machine.
        scriptname = 'aws-setup.sh'
        script = os.path.join(os.getcwd(), scriptname)
        util.scp(script, remote_user, ip, '/home/ubuntu/', ssh_key)

        # chmod the script.
        command = 'chmod 755 /home/ubuntu/%s' % scriptname
        util.ssh(command, remote_user, ip, ssh_key)

        # Execute the script.
        command = '/home/ubuntu/%s %s' % (scriptname, rev)
        util.ssh(command, remote_user, ip, ssh_key)

      except KeyboardInterrupt:
        print 'Keyboard Interrupt (ctrl-c)'

      except IOError, e:
        print 'Failed: %s' % e

      finally:
        # TODO: maybe move this below the prompting?  Benefit of having it here
        # is we don't run the meter.  Drawback is if something fails (e.g., scp
        # of the .deb files) we have no recourse to manually ssh in to the EC2
        # instance and retry.

        # Goodbye!
        grey.release_ip()
        instance.terminate()

      # Check that everything went OK before destroying the instance.
      while True:
        answer = raw_input('Type "yes" to terminate this instance: ')
        # Verify to prevent accidental terminal input from terminating.
        if answer == 'yes':
          break

def main():
  if len(sys.argv) > 3:
    distro = sys.argv[1]
    bits = int(sys.argv[2])
    rev = int(sys.argv[3])
    print 'Building distro=%s, bits=%s, rev=%s' % (distro, bits, rev)
    build(distro, bits, rev)
  else:
    print 'Need to specify distro, bits, and rev.  For example:'
    print '  %s lucid 32 5999\n' % sys.argv[0]
    distro = raw_input('Enter distro: ')
    bits = int(raw_input('Enter bits: '))
    rev = int(raw_input('Enter rev: '))
    build(distro, bits, rev)

if __name__ == '__main__':
  try:
    main()
  except Exception:
    traceback.print_exc()
    raw_input('\nException occurred; press enter to quit')
