#!/usr/bin/env python
import os, subprocess, ConfigParser

def make_dir(dir):
  try: os.makedirs(dir)
  except os.error: pass

# Don't have subprocess.check_output in Python 2.6.  This will have to do.
def check_output(cmd):
  return subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]

# Create a directory that will hold all working copy directories.
wc_root = 'emergent-cluster-runs'
config_filename = os.path.join(wc_root, 'config.ini')
make_dir(wc_root)

# Read cached configuration data from an .ini file.
config = ConfigParser.RawConfigParser()
repo_section = 'repo_names'
user_section = 'user_data'
if os.path.isfile(config_filename):
  config.read(config_filename)
else:
  # File will be written out later; for now just create an empty object.
  config.add_section(repo_section)
  config.add_section(user_section)

# Get the user's Subversion username (defaults to cached value or $USER).
username_field = 'username'
try: username = config.get(user_section, username_field)
except: username = os.environ['USER']

# Allow the user to choose a different username.
other_username = raw_input('Enter your Subversion username: [%s] ' % username)
if other_username: username = other_username

# Cache the username in the config object.
config.set(user_section, username_field, username)

# Get the list of repos to choose from.
repos = sorted(config.items(repo_section))
if repos:
  print '\nChoose a repository to monitor:'
  print '  0. New repo...'
  i = 1
  for (repo_name, repo_url) in repos:
    print '  %d. %s = %s' % (i, repo_name, repo_url)
    i += 1
  print ''
  while True:
    choice = raw_input('Your choice: ')
    choice = int(choice)
    if choice < i: break
else:
  choice = 0

# Get details of the chosen repo (name, URL).
if choice == 0:
  repo_name = raw_input('\nEnter name of new repository: ')
  repo_url  = raw_input(  'Enter URL of new repository:  ')
  config.set(repo_section, repo_name, repo_url)
else:
  (repo_name, repo_url) = repos[int(choice) - 1]

# Write any changes (username, new repo) to the config file.
with open(config_filename, 'wb') as f:
  config.write(f)

print '\nUpdating repo "%s" at: %s ...' %(repo_name, repo_url)

# Do initial checkout/update interactively, so user can enter credentials
# if needed, then re-run script with nohup and non-interactive calls, so
# output can be sent to a log file.

repo_dir = os.path.join(wc_root, repo_name)
if os.path.isdir(repo_dir):
  subprocess.call(['svn', 'up', '--username', username, '--force', '--accept', 'theirs-full', repo_url, repo_dir])
else:
  subprocess.call(['svn', 'co', '--username', username, repo_url, repo_dir])

matches = []
for root, dirnames, filenames in os.walk('src'):
  for filename in fnmatch.filter(filenames, '*.c'):
      matches.append(os.path.join(root, filename))


#def main():
#  if len(sys.argv) > 3:
#    distro = sys.argv[1]
#    bits = int(sys.argv[2])
#    rev = int(sys.argv[3])
#    print 'Building distro=%s, bits=%s, rev=%s' % (distro, bits, rev)
#    build(distro, bits, rev)
#  else:
#    print 'Need to specify distro, bits, and rev.  For example:'
#    print '  %s lucid 32 5999\n' % sys.argv[0]
#    distro = raw_input('Enter distro: ')
#    bits = int(raw_input('Enter bits: '))
#    rev = int(raw_input('Enter rev: '))
#    build(distro, bits, rev)
#
#if __name__ == '__main__':
#  try:
#    main()
#  except KeyboardInterrupt:
#    print '\n\nQuitting at user request (Ctrl-C).'
#  except Exception:
#    traceback.print_exc()
#    raw_input('\nException occurred; press enter to quit')
