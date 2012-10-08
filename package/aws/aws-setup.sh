#!/bin/bash
set -e

cd ~

# Tell apt-get not to bother us with post-install interactive configuration.
# (Why must a mail server be installed anyway???)
export DEBIAN_FRONTEND=noninteractive 

# Tell ssh how to connect to grey.
echo 'Host grey.colorado.edu' >> ~/.ssh/config
echo 'IdentityFile ~/.ssh/ec2grey' >> ~/.ssh/config
echo 'StrictHostKeyChecking no' >> ~/.ssh/config

# Add the emergent repository so we can get libquarter0 from it.
REPONAME=`lsb_release -cs`
echo "deb http://grey.colorado.edu/ubuntu $REPONAME latest" > emergent.list
sudo mv emergent.list /etc/apt/sources.list.d/

# Install subversion and libquarter0.
sudo apt-get update
sudo apt-get -q -y install subversion
sudo apt-get -q -y --allow-unauthenticated install libquarter0

# Tell subversion to store passwords plaintext.
mkdir -p ~/.subversion
echo '' >> ~/.subversion/servers
echo '[global]' >> ~/.subversion/servers
echo 'store-plaintext-passwords = yes' >> ~/.subversion/servers

# Get the package directory.
svn checkout --username anonymous --password emergent http://grey.colorado.edu/svn/emergent/emergent/trunk/package ~/package
cd ~/package

# Check if a revision was given on the command line.
REV="$1"
if [ -z $REV ]; then REV="HEAD"; fi

# Build it!
./BuildEmergent-ubuntu.sh $REV trunk N
