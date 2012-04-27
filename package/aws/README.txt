Python scripts to build emergent for Ubuntu using Amazon Web Services.

build.py is the main script.  If run from the command line, it prompts
for the distribution (lucid, maverick, natty, oneiric), the architecture
(32 or 64 bits), and the Subversion revision number.  These values can
also be provided on the command line or passed into build.build().

Some preparation is needed to use these scripts.
 * The scripts depend on boto v2.3.0 for interfacing with Amazon Web Services.
   Download and install this Python library so it can be imported by ec2.py.
 * Sign up for an AWS account: http://aws.amazon.com/
 * Go to the EC2 management console: https://console.aws.amazon.com/ec2/
 * Choose the region US West (Oregon), which corresponds to 'us-west-2'.
 * Create a key-pair named 'eco'.  Download the private key to ~/.ssh/eco.pem
 * Create a security group named 'eco'.  Add a rule to allow SSH.
 * The 'us-west-2' and 'eco' strings are hardcoded in build.py, if you use
   a different region or names, change the file.
 * Generate a key-pair locally: cd ~/.ssh && ssh-keygen.exe -f ec2grey
   Copy ec2grey.pub into ~/.ssh/authorized_keys on grey
 * From the AWS console, click your username in the upper-right corner, and
   choose Security Credentials.  A new page opens; under Access Keys copy
   your Access Key ID and Secret Access Key into a file ~/.boto like so:
     $ cat ~/.boto
     [Credentials]
     aws_access_key_id = <SECRET>
     aws_secret_access_key = <SECRET>
     [Boto]
     debug = 1
 * Create a file with username/password info for wget:
     $ cat ~/.wgetrc
     http_user = <SECRET>
     http_password = <SECRET>
 * This keeps passwords out of the Python code so it can be source controlled.
 * Adjust the username that's hardcoded in build.py to your username on grey.
   Leave the remote_user = 'ubuntu' alone, that one is for ssh'ing to EC2.

The aws-setup.sh script, which gets copied to each EC2 instance and run, is
nearly fully automated.  The only remaining problem is that for some reason,
a mail server gets installed, and there's a post-install configuration step
for Postfix.  If you get prompted for configuration, chose option '1' which
is no configuration.

The Python scripts were tested under Cygwin.  They will probably not work
without modification on Python for Windows, since they rely on utilities
like wget, ssh, and scp being in the path, and rely on those utilities'
configuration files.  The scripts should work on Linux but have not been
tested.

grey.py is not checked into source control.  It contains logic similar to the
opensesame script to allow the EC2 instances to scp the built packages to grey.
