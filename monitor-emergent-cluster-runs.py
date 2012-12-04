#!/usr/bin/env python
import os, re, subprocess, sys, time, ConfigParser

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
        if os.path.isfile(self.config_filename):
            self.config.read(self.config_filename)

        # Ensure all needed sections are present (e.g., first script run).
        self.repo_section = 'repo_names'
        self.user_section = 'user_data'
        self.ensure_section_exists(self.repo_section)
        self.ensure_section_exists(self.user_section)

    def ensure_section_exists(self, section):
        if not self.config.has_section(section):
            self.config.add_section(section)

    def prompt_for_field(self, section, field, message, default=""):
        # Get the cached value (if any) otherwise use the default.
        try: value = self.config.get(section, field)
        except: value = default

        # Allow the user to choose a different value.
        prompt = message + ' '
        if value: prompt += '[%s] ' % value
        other_value = raw_input(prompt)
        if other_value: value = other_value

        # Cache the value in the config object and return it.
        self.config.set(section, field, value)
        self.write_config()
        return value

    def prompt_for_int_field(self, section, field, message, default=""):
        while True:
            try:
                return int(self.prompt_for_field(
                    section, field, message, default))
            except ValueError: pass

    def get_username(self):
        # Get the user's Subversion username (defaults to cached value or
        # $USER).
        return self.prompt_for_field(
            self.user_section, 'username',
            'Enter your Subversion username:', os.environ['USER'])

    def get_poll_interval(self):
        # Get the amount of time between polling the subversion server.
        return self.prompt_for_int_field(
            self.user_section, 'poll_interval',
            'Enter the polling interval, in seconds:')

    def get_repo_choice(self):
        return self.prompt_for_int_field(
            self.user_section, 'repo_choice', 'Your choice:')

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
                choice = self.get_repo_choice()
                if choice <= i: break

            if choice > 0:
                return repos[int(choice) - 1]

        repo_name = raw_input('\nEnter name of new repository: ')
        repo_url  = raw_input(  'Enter URL of new repository:  ')
        self.config.set(self.repo_section, repo_name, repo_url)
        self.write_config()
        return (repo_name, repo_url)

    def write_config(self):
        # Write any changes (username, new repo) to the config file.
        with open(self.config_filename, 'wb') as f:
            self.config.write(f)

class SubversionPoller(object):
    def __init__(self, username, repo_dir, repo_url, delay):
        self.username = username
        self.repo_dir = repo_dir
        self.repo_url = repo_url
        self.delay = delay

        esc_repo_dir = re.escape(self.repo_dir)
        self.re_comp = re.compile(
            r'[AUGR]\s+' + esc_repo_dir + r'/[^/]+/submit/')

    def get_initial_wc(self):
        # Either checkout or update the directory.
        if os.path.isdir(self.repo_dir):
            cmd = ['svn', 'up', '--username', self.username, '--force',
                   '--accept', 'theirs-full', self.repo_dir]
        else:
            cmd = ['svn', 'co', '--username', self.username,
                   self.repo_url, self.repo_dir]

        # Run command and make sure the directory exists afterwards.
        # This initial checkout/update is interactive so the user can
        # enter credentials if needed, which will then be cached for
        # the remainder of this script.
        subprocess.call(cmd)
        if not os.path.isdir(self.repo_dir):
            print '\nCheckout failed, aborting.'
            sys.exit(1)

        # Now do an 'svn info' (non-interactive) to get the starting rev.
        svn_info = check_output(['svn', 'info', self.repo_dir])
        rev_line = [line for line in svn_info.splitlines()
                         if line[:9] == "Revision:"]
        revision = int(rev_line[0][10:])
        #print 'Found revision %s' % revision
        return revision

    def check_for_updates(self):
        cmd = ['svn', 'up', '--username', self.username, '--force',
               '--accept', 'theirs-full', '--non-interactive', self.repo_dir]
        svn_update = check_output(cmd)
        submit_files = [line[1:].strip() for line in svn_update.splitlines()
                            if self.re_comp.match(line)]
        return submit_files

    def poll(self, func):
        # Enter the loop to check for updates to job submission files.
        print '\nPolling the Subversion server every %d seconds ' \
              '(hit Ctrl-C to quit) ...' % self.delay
        while True:
            submit_files = self.check_for_updates()
            for filename in submit_files:
                func(filename)
            time.sleep(self.delay)

def run_job_for_file(filename):
    print 'TODO: Submit job for file: %s' % filename

def main():
    # Read the config file, allow the user to add a new repo, get
    # the repo they'd like to use, and write it all back to disk.
    wc_root = 'emergent-cluster-runs'
    config = ClusterConfig(wc_root)
    username = config.get_username()
    repo_name, repo_url = config.choose_repo()
    #print '\nUpdating repo "%s" at: %s ...' %(repo_name, repo_url)

    # Checkout or update the working copy.
    # The path format matches ClusterManager::setPaths() in the C++ code.
    repo_url += '/repos/' + username
    repo_dir = os.path.join(wc_root, repo_name)
    print ''
    delay = config.get_poll_interval()
    poller = SubversionPoller(username, repo_dir, repo_url, delay)
    revision = poller.get_initial_wc()
    poller.poll(run_job_for_file) # Infinite loop.

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print '\n\nQuitting at user request (Ctrl-C).'
