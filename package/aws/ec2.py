#!/usr/bin/env python

import boto.ec2
import sys
import time

class Ec2Connection(object):
  _connection_map = dict()

  @staticmethod
  def get_connection(region):
    try:
      conn = Ec2Connection._connection_map[region]
      print 'Got connection to %s' % region
    except KeyError:
      print 'Connecting to %s' % region
      conn = boto.ec2.connect_to_region(region)
      Ec2Connection._connection_map[region] = conn
    return conn

  @staticmethod
  def status():
    print 'Current instances:'
    for region, conn in Ec2Connection._connection_map.items():
      print 'In region %s:' % region
      reservations = conn.get_all_instances()
      for res in reservations:
        print ' In reservation %s:' % res.id
        instances = res.instances
        for inst in instances:
          status = inst.update()
          ip = ''
          if status == 'running':
            ip = ' at %s' % inst.ip_address
          print '  Instance %s (%s, %s) is %s%s' % \
            (inst.id, inst.image_id, inst.instance_type, status, ip)

class Ec2Instance(object):
  """Represents an EC2 instance"""

  def __init__(self, ami, region, desc=''):
    self.ami = ami
    self.region = region
    self.desc = desc

  def start_instance(self, key, sec_group):
    conn = Ec2Connection.get_connection(self.region)
    print 'Starting instance using AMI %s (%s)' % (self.ami, self.desc)
    reservation = conn.run_instances(
      self.ami, key_name=key, security_groups=[sec_group])
    self.instance = reservation.instances[0]

  def wait_for_running(self, timeout_secs=5*60, delay=10):
    total_delay = 0

    print 'Waiting for instance to run (this may take a few minutes)'
    while total_delay < timeout_secs:
      self.instance.update()
      if self.instance.state == 'pending':
        sys.stdout.write('.')
        sys.stdout.flush()
        time.sleep(delay)
        total_delay += delay
      elif self.instance.state == 'running':
        print 'Instance %s (%s) is running at %s' % \
          (self.instance.id, self.desc, self.instance.ip_address)
        return True
      else:
        print 'Instance %s (%s) is %s' % \
          (self.instance.id, self.desc, self.instance.state)
        break

    else:
      print 'Instance %s (%s) is still pending after %ds' % \
        (self.instance.id, self.desc, total_delay)

    return False

  def get_ip(self):
    if self.instance.update() == 'running':
      return self.instance.ip_address
    return None

  def terminate(self):
    self.instance.terminate()
