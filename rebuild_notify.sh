#!/bin/bash

svn_rev=`svn info | grep Revision:`
echo ${svn_rev}
/usr/bin/mail -s "Emergent on clusters updated to ${svn_rev}" ccnlab-logistics@grey.colorado.edu < /dev/null
