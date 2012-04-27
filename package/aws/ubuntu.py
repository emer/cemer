#!/usr/bin/env python

import urllib

class UbuntuAmi(object):
  """Represents a particular Ubuntu AMI image"""

  _ami_map = dict()

  def __init__(self, distro, region, arch, storage='instance-store'):
    # Store as a tuple so it can be used in the dictionary.
    self.platform = (distro, region, arch, storage)
    if arch == 'i386':
      self.bits = 32
    elif arch == 'amd64':
      self.bits = 64
    else:
      self.bits = 0

  def get_ami(self):
    try:
      ami = UbuntuAmi._ami_map[self.platform]
    except KeyError:
      UbuntuAmi._lookup_ami(self.platform)
      ami = UbuntuAmi._ami_map[self.platform]
    return ami

  # Canonical provides Ubuntu images that can be used on AWS EC2,
  # details here: https://help.ubuntu.com/community/UEC/Images
  # The files are structured like so:
  # 0     1       2     3         4   5     6         7             8               9
  # natty desktop daily 20120409  ebs amd64 us-west-1 ami-291e456c  aki-9ba0f1de    paravirtual

  @staticmethod
  def _lookup_ami(platform):
    distro = platform[0]
    url = 'http://cloud-images.ubuntu.com/query/%s/desktop/daily.current.txt' % distro
    print 'Getting', url
    file = urllib.urlopen(url)
    lines = file.readlines()
    for line in lines:
      UbuntuAmi._add_ami(line)

  @staticmethod
  def _add_ami(line):
    words = line.split()
    distro = words[0]
    region = words[6]
    arch = words[5]
    storage = words[4]
    ami = words[7]
    platform = (distro, region, arch, storage)
    UbuntuAmi._ami_map[platform] = ami
