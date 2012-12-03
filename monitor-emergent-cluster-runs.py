#!/usr/bin/env python
import os, subprocess, ConfigParser

def make_dir(dir):
    try: os.makedirs(dir)
    except os.error: pass

# Don't have subprocess.check_output in Python 2.6.  This will have to do.
def check_output(cmd):
    return subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]

class ClusterConfig(object):
    def __init__(self, wc_root):
        # Ensure the working copy root directory exists.
        self.wc_root = wc_root
        make_dir(self.wc_root)

        # Read cached configuration data from an .ini file, if present.
        self.config_filename = os.path.join(self.wc_root, 'config.ini')
        self.config = ConfigParser.RawConfigParser()
        self.repo_section = 'repo_names'
        self.user_section = 'user_data'

        if os.path.isfile(self.config_filename):
            self.config.read(self.config_filename)
        else:
            # File will be written out later; for now just create an empty
            # object.
            self.config.add_section(self.repo_section)
            self.config.add_section(self.user_section)

    def get_username(self):
        # Get the user's Subversion username (defaults to cached value or
        # $USER).
        username_field = 'username'
        try: self.username = self.config.get(self.user_section, username_field)
        except: self.username = os.environ['USER']

        # Allow the user to choose a different username.
        other_username = raw_input(
          'Enter your Subversion username: [%s] ' % self.username)
        if other_username: self.username = other_username

        # Cache the username in the config object.
        self.config.set(self.user_section, username_field, self.username)

        return self.username

    def choose_repo(self):
        # Get the list of repos to choose from.
        repos = sorted(self.config.items(self.repo_section))
        if repos:
            print '\nChoose a repository to monitor:'
            print '  0. New repo...'
            for i, (name, url) in enumerate(repos, start=1):
                print '  %d. %s = %s' % (i, name, url)

            print ''
            while True:
                try:
                  choice = int(raw_input('Your choice: '))
                  if choice <= i: break
                except ValueError: pass

            if choice > 0:
                return repos[int(choice) - 1]

        repo_name = raw_input('\nEnter name of new repository: ')
        repo_url  = raw_input(  'Enter URL of new repository:  ')
        self.config.set(self.repo_section, repo_name, repo_url)
        return (repo_name, repo_url)

    def write_config(self):
        # Write any changes (username, new repo) to the config file.
        with open(self.config_filename, 'wb') as f:
            self.config.write(f)

def main():
    wc_root = 'emergent-cluster-runs'
    config = ClusterConfig(wc_root)
    username = config.get_username()
    repo_name, repo_url = config.choose_repo()
    config.write_config()
    #print '\nUpdating repo "%s" at: %s ...' %(repo_name, repo_url)

    # Do initial checkout/update interactively, so user can enter
    # credentials if needed (which will then be cached).
    repo_dir = os.path.join(wc_root, repo_name)
    if os.path.isdir(repo_dir):
        cmd = ['svn', 'up', '--username', username,
               '--force', '--accept', 'theirs-full', repo_dir]
    else:
        cmd = ['svn', 'co', '--username', username, repo_url, repo_dir]

    # Run command and make sure the directory exists afterwards.
    subprocess.call(cmd)
    if not os.path.isdir(repo_dir):
        print '\nCheckout failed, aborting.'
        return

    # Now do an 'svn info' to get the starting revision.
    svn_info = check_output(['svn', 'info', repo_dir])
    rev_line = [line for line in svn_info.splitlines()
                     if line[:9] == "Revision:"]
    revision = int(rev_line[0][10:])
    #print 'Found revision %s' % revision

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print '\n\nQuitting at user request (Ctrl-C).'
