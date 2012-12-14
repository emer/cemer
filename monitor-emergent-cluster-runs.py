#!/usr/bin/env python
import datetime, os, re, subprocess, sys, time, traceback, ConfigParser
import xml.etree.ElementTree as ET

def make_dir(dir):
    try: os.makedirs(dir)
    except os.error: pass

# Don't have subprocess.check_output in Python 2.6.  This will have to do.
def check_output(cmd):
    return subprocess.Popen(cmd, stdout=subprocess.PIPE).communicate()[0]

#############################################################################

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
        try:    value = self.config.get(section, field)
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

    def get_clustername(self):
        # Get the name of this cluster.
        return self.prompt_for_field(
            self.user_section, 'clustername',
            'Enter the name of this cluster:', os.environ['HOSTNAME'])

    def get_poll_interval(self):
        # Get the amount of time between polling the subversion server.
        return self.prompt_for_int_field(
            self.user_section, 'poll_interval',
            'Enter the polling interval, in seconds:')

    def get_check_user(self):
        # Should the script only start jobs committed by the selected user?
        check = self.prompt_for_field(
            self.user_section, 'check_user',
            'Only run jobs you committed?', 'Yes')
        return check and check[0] in 'yY'

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

#############################################################################

class SubversionPoller(object):
    def __init__(self, username, repo_dir, repo_url, delay, check_user):
        self.username   = username
        self.repo_dir   = repo_dir
        self.repo_url   = repo_url
        self.delay      = delay
        self.check_user = check_user

        # The repo directory only includes the working copy root and
        # repo_name (as defined by the user and stored in config.ini).
        # It does not include the project name, hence the '/[^/]+/'.
        # The RE captures the whole (relative) filename to be retrieved
        # later in a m.group(1) call.
        esc_repo_dir = re.escape(self.repo_dir)
        self.re_comp = re.compile(
            r'^\s*[AUGR]\s+(%s/[^/]+/submit/.*)' % esc_repo_dir)

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
        revision, author = self._get_commit_info(self.repo_dir)
        return revision

    def poll(self, func, nohup_file=''):
        # Enter the loop to check for updates to job submission files.
        print '\nPolling the Subversion server every %d seconds ' \
              '(hit Ctrl-C to quit) ...' % self.delay
        while True:
            # If running in background and the file was deleted, then exit.
            if nohup_file and not os.path.isfile(nohup_file):
                break

            sys.stdout.write('.') # TODO: remove, or create spinner...
            sys.stdout.flush()

            submit_files = self._check_for_updates()
            if submit_files: print ''
            for filename in submit_files:
                try:
                    rev, author = self._get_commit_info(filename)
                    if self.check_user and author != self.username:
                        print 'Ignoring job submitted by user %s:' % author
                        print '  File: %s' % filename
                    else:
                        if os.path.basename(filename) == 'jobs_submit.dat':
                            self._start_jobs(filename, rev)
                        else:
                            func(filename, rev)
                except:
                    traceback.print_exc()
                    print '\nCaught exception trying to parse job ' \
                          'submission file'
                    print 'Continuing to poll the Subversion server ' \
                          '(hit Ctrl-C to quit) ...'
            time.sleep(self.delay)

    def _check_for_updates(self):
        cmd = ['svn', 'up', '--username', self.username, '--force',
               '--accept', 'theirs-full', '--non-interactive', self.repo_dir]
        svn_update = check_output(cmd)

        # 'svn up' does not allow an '--xml' parameter, so we need to
        # scrape the textual output it produces.
        # The 'm' MatchObject iterates over a list of length 1.
        submit_files = [m.group(1) for l in svn_update.splitlines()
                                   for m in [self.re_comp.match(l)] if m]
        return submit_files

    def _get_commit_info(self, filename):
        """Get the commit revision and author for the given filename."""

        cmd = ['svn', 'info', '--xml', filename]
        svn_info = check_output(cmd)

        root = ET.fromstring(svn_info)
        try:    revision = root.getiterator('commit')[0].attrib['revision']
        except: revision = '0'

        try:    author = root.getiterator('author')[0].text
        except: author = ''

        return (revision, author)

    def _start_jobs(self, filename, rev):
        # TODO:
        # 1. read the jobs_submit.dat DataTable
        # 2. start jobs
        # 3. write the jobs_running.dat DataTable
        # 4. commit the running table to svn.

        # Columns in jobs_running:
        # model_svn = str(rev)
        # submit_svn = str(rev)
        # submit_job = str(row number)
        # tag = '_'.join((submit_svn, submit_job))
        # status = 'SUBMITTED'
        # # Start job using scheduler.  Capture stdout+stderr to job_out_file.
        # job_out_file = tag + '.out'
        # job_out = first 2048 chars of job_out_file.
        # job_no = job number assigned by scheduler, scraped from job_out_file.
        # dat_files = committed results files matching tag_*.out
        # command, notes, repo_url, cluster, queue, run_time, ram_gb,
        #   n_threads, mpi_nodes = copied from jobs_submit.dat
        pass


#############################################################################

def run_job_for_file(filename, rev):
    print 'TODO: Submit job for revision %s of file: %s' % (rev, filename)
    params = ConfigParser.RawConfigParser()
    params.read(filename)
    general_section = 'GENERAL_PARAMS'
    param_list = params.get(general_section, 'parameters').split(',')
    for p in param_list:
        min_val = params.getfloat(p, 'min_val')
        max_val = params.getfloat(p, 'max_val')
        incr    = params.getfloat(p, 'incr')
        print '  %s = %f .. %f step %f' % (p, min_val, max_val, incr)

#############################################################################

# If the user chooses to run in the background, this file will be created.
# The polling loop will exit if the file is subsequently deleted.  The user
# may delete the file manually, or simply re-running this script will delete
# the file.
nohup_filename = 'nohup-running-monitor-emergent-cluster-runs.txt'

def main():
    # Delete the nohup file, if it exists.
    if os.path.isfile(nohup_filename):
        print 'Removing nohup file: %s' % nohup_filename
        os.remove(nohup_filename)
        print 'The background script should stop at its next poll interval.'
        print 'Hit Ctrl-C at any time if you just want to quit.\n'

    # Read the config file, allow the user to add a new repo, get
    # the repo they'd like to use, and write it all back to disk.
    wc_root = 'emergent-cluster-runs'
    config = ClusterConfig(wc_root)
    username = config.get_username()
    clustername = config.get_clustername()
    repo_name, repo_url = config.choose_repo()

    # Checkout or update the working copy.
    # The path format matches ClusterManager::setPaths() in the C++ code.
    repo_url += '/' + clustername + '/' + username
    repo_dir = os.path.join(wc_root, repo_name)
    print ''
    delay = config.get_poll_interval()
    check_user = config.get_check_user()

    poller = SubversionPoller(username, repo_dir, repo_url, delay, check_user)
    revision = poller.get_initial_wc()

    run_nohup = raw_input('\nRun in the background using nohup? [Y/n] ')
    if not run_nohup or run_nohup in 'yY':
        cmd = ['nohup', sys.argv[0], username, repo_dir, repo_url, str(delay),
               str(check_user)]
        subprocess.Popen(cmd)
        time.sleep(1) # Give it a chance to start.
        print 'Running in the background.  Re-run script to stop.'
    else:
        poller.poll(run_job_for_file) # Infinite loop.

def main_background():
    print '\nStarting background run at %s' % datetime.datetime.now()

    username    = sys.argv[1]
    repo_dir    = sys.argv[2]
    repo_url    = sys.argv[3]
    delay       = int(sys.argv[4])
    check_user  = sys.argv[5] == 'True'

    poller = SubversionPoller(username, repo_dir, repo_url, delay, check_user)
    with open(nohup_filename, 'w') as f:
        f.write('delete this file to stop the backgrounded script.')
    poller.poll(run_job_for_file, nohup_filename) # Infinite loop.

    print '\nStopping background run at %s' % datetime.datetime.now()

#############################################################################

if __name__ == '__main__':
    try:
        if len(sys.argv) == 1:
            main()
        else:
            main_background()
    except KeyboardInterrupt:
        print '\n\nQuitting at user request (Ctrl-C).'
