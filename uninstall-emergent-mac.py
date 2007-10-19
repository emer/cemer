#!/usr/bin/env python
# Emergent uninstaller for OSX. Modified quite a bit from the qt-uninstaller.py. GPL.. 

# Usage instructions: Just call this with the name (but not path) of
# the package receipt.  The package receipt should be located in
# /Library/Receipts. This script should remove any named package that
# exists in that directory, and does some special stuff if the package
# is determined to be emergent.

# TODO: Detect all emergent package receipts in /Library/Receipts and uninstall them

import commands, sys, os, shutil, glob, dircache

trace = True # Make this false to be quiet
justTest = False # Make this false to actually remove

def recursiveDelete(dirname):
    files = dircache.listdir(dirname)
    for file in files:
        path = os.path.join (dirname, file)
        if os.path.isdir(path):
            recursiveDelete(path)
        else:
            print 'Removing file: "%s"' % path
            retval = os.unlink(path)

    print 'Removing directory:', dirname
    os.rmdir(dirname)

#Remove the files in the list
def removeFiles(fileList):
    directories = []
    for file in fileList:
        file = file[1:]
        if not os.path.islink(file) and os.path.isdir(file):
            directories.append(file)
        elif len(file) > 0:
            if os.path.exists(file) or os.path.islink(file):
                if trace:
                    print "remove file: " + file
                if not justTest:
                    os.remove(file) 
            else:
                print "file: %s does not exist, skipping" % file

    # Now remove any empty directories
    directories.reverse()
    for dir in directories:
        if (os.path.exists(dir)) and len(os.listdir(dir)) == 0:
            if trace:
                print "remove dir: " + dir
            if not justTest:
                os.rmdir(dir)
        elif trace and os.path.exists(dir):
            print "NOT removing " + dir

def removePackage(package):
    realPackageName = "/Library/Receipts/" + package[0]
    print "Removing " + realPackageName
    bomLocation = os.path.join(realPackageName, "Contents/Archive.bom")
    if os.path.exists(realPackageName) and os.path.isdir(realPackageName):
        fileList = commands.getoutput("/usr/bin/lsbom -f -p f -d -l " + bomLocation).split()
        if len(fileList) > 0:
            removeFiles(fileList)
            shutil.rmtree(realPackageName)
    else:
        print "Emergent is not installed!"

if os.getuid() != 0:
    print sys.argv[0] + ": This script must be run as root or with sudo, exiting now."
    sys.exit(-1)

# Take the package name on the command-line. E.g., "Emergent-4.0.8_universal.pkg"
if len(sys.argv) > 1:
    package = sys.argv[1:]
else:
    print "Error: You must supply a (single) complete package name, e.g., Emergent-4.0.8_universal.pkg"
    sys.exit(0)

removePackage(package)

# Hack: The Qt directories don't seem to be completely removed by the above. So let's just take care of that...

#if package[0].lower().find("emergent"):
for dir in os.listdir("/Library/Frameworks"):
    if dir[0:2] == "Qt":
        recursiveDelete("/Library/Frameworks/" + dir)
