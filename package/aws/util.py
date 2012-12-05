#!/usr/bin/env python

import os, subprocess, time

def get_ssh_options(ssh_key=None, quiet=False):
  # Don't bother asking the user if they recognize the key fingerprint.
  opts = ['-o', 'StrictHostKeyChecking=no']

  # Don't pollute 'known_hosts' with Amazon IPs that may never be revisited.
  home = os.environ['HOME']
  opts += ['-o', 'UserKnownHostsFile=%s/.ssh/known_hosts_amazon' % home]

  # Add the option to use a particular identity key.
  if ssh_key is not None:
    opts += ['-i', ssh_key]

  # Add the quiet option if requested.
  if quiet:
    opts += ['-q']

  return opts

def attempt_cmd(cmd, timeout_secs=60, delay=10):
  print ' '.join(cmd)
  total_delay = 0
  while total_delay < timeout_secs:
    # check_output would be better here, but that's python 2.7.
    retval = subprocess.call(cmd)
    if 0 == retval:
      break
    print 'ERROR: command failed, will retry in %d seconds' % delay
    time.sleep(delay)
    total_delay += delay
  else:
    raise IOError, 'command failed, retries exhausted'

def scp(filename, remote_user, remote_ip, remote_dir, ssh_key=None, quiet=False):
  """Use scp to copy a file to a remote host"""

  opts = get_ssh_options(ssh_key, quiet)
  cmd = ['scp'] + opts + \
        [filename, '%s@%s:%s' % (remote_user, remote_ip, remote_dir)]
  attempt_cmd(cmd)

def ssh(command, remote_user, remote_ip, ssh_key=None):
  """Use ssh to execute a command on the remote host"""

  opts = get_ssh_options(ssh_key)
  cmd = ['ssh'] + opts + \
        ['%s@%s' % (remote_user, remote_ip), command]
  attempt_cmd(cmd)

def wget(url, outfile=None):
  """Use wget to retrieve a URL"""

  if not outfile: outfile = '/dev/null'
  cmd = ['wget', '-q', '-O', outfile, url]
  attempt_cmd(cmd)
