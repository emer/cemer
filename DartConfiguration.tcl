# This file is configured by CMake automatically as DartConfiguration.tcl
# If you choose not to use CMake, this file may be hand configured, by
# filling in the required variables.


# Configuration directories and files
SourceDirectory: /Users/mingus/emergent
BuildDirectory: /Users/mingus/emergent

# Site is something like machine.domain, i.e. pragmatic.crd
Site: mightyme.local

# Build name is osname-revision-compiler, i.e. Linux-2.4.2-2smp-c++
BuildName: Darwin-c++

# Submission information
IsCDash: TRUE
DropSite: grey.colorado.edu
DropLocation: /CDash/submit.php?project=Emergent
DropSiteUser: 
DropSitePassword: 
DropSiteMode: 
DropMethod: http
TriggerSite: http://grey.colorado.edu/cgi-bin/Submit-Random-TestingResults.cgi
ScpCommand: /usr/bin/scp

# Dashboard start time
NightlyStartTime: 04:00:00 EST

# Commands for the build/test/submit cycle
ConfigureCommand: "/usr/local/bin/cmake" "/Users/mingus/emergent"
MakeCommand: /usr/bin/make -i

# CVS options
# Default is "-d -P -A"
CVSCommand: /usr/bin/cvs
CVSUpdateOptions: -d -A -P

# Subversion options
SVNCommand: /usr/local/bin/svn
SVNUpdateOptions: 

# Generic update command
UpdateCommand: /usr/local/bin/svn
UpdateOptions: 
UpdateType: svn

# Dynamic analisys and coverage
PurifyCommand: 
ValgrindCommand: 
ValgrindCommandOptions: 
MemoryCheckCommand: MEMORYCHECK_COMMAND-NOTFOUND
MemoryCheckCommandOptions: 
MemoryCheckSuppressionFile: 
CoverageCommand: /usr/bin/gcov

# Testing options
# TimeOut is the amount of time in seconds to wait for processes
# to complete during testing.  After TimeOut seconds, the
# process will be summaily terminated.
# Currently set to 25 minutes
TimeOut: 1500
