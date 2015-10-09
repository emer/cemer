#!/bin/bash

# make sure we've got latest here -- not necc same as target build!
svn update

svn_rev=`svn info | grep Revision:`
echo ${svn_rev}
/usr/bin/mail -s "Emergent on clusters updated to ${svn_rev}" ccnlab-logistics@grey.colorado.edu < /dev/null
