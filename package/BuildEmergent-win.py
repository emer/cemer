# This script attempts to download and install all the tools needed
# to build emergent, checkout the source from Subversion, build it,
# and create an installer.
# NOTE: if you commit a new version of this file to svn, make sure to
# update the file on the ftp site as well

import os, re, subprocess, sys, urllib, webbrowser, _winreg
from zipfile import ZipFile

def quit(exit_val):
	os.system('color 07')
	response = raw_input("\n\nPress enter to close the window.\n")
	sys.exit(exit_val)

def setEnvVar(name, value, reg, env):
	try:
		key = _winreg.OpenKey(reg, env, 0, _winreg.KEY_ALL_ACCESS)
		_winreg.SetValueEx(key, name, 0, _winreg.REG_EXPAND_SZ, value)
	except WindowsError:
		pass
	_winreg.CloseKey(key)

def getEnvVar(name, reg, env):
	try:
		key = _winreg.OpenKey(reg, env, 0, _winreg.KEY_READ)
		value, type = _winreg.QueryValueEx(key, name)
	except WindowsError:
		_winreg.CloseKey(key)
		return ""
	_winreg.CloseKey(key)
	return value

def getUserEnvVar(name):
	return getEnvVar(name, _winreg.HKEY_CURRENT_USER, 'Environment')

def getSysEnvVar(name):
	env = 'SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment'
	return getEnvVar(name, _winreg.HKEY_LOCAL_MACHINE, env)

def setUserEnvVar(name, value):
	return setEnvVar(name, value, _winreg.HKEY_CURRENT_USER, 'Environment')

def setSysEnvVar(name, value):
	env = 'SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment'
	return setEnvVar(name, value, _winreg.HKEY_LOCAL_MACHINE, env)


try:
	progfiles = set([os.environ['ProgramFiles(x86)'], os.environ['ProgramFiles'], os.environ['ProgramW6432']])
	isWin64 = True
except KeyError:
	progfiles = set([os.environ['ProgramFiles']])
	isWin64 = False

def makeDir(dir):
	try:
		os.makedirs(dir)
	except os.error:
		pass

devtool_dir = 'C:/src/devtools'
makeDir(devtool_dir)

def fixPath(path):
	return re.sub(r'/', r'\\', path)

def fileExists(file):
	if os.path.isfile(file):
		print "  Detected " + fixPath(file)
		return True
	return False

def dirExists(dir):
	if os.path.isdir(dir):
		print "  Detected " + fixPath(dir)
		return True
	return False

def findInProgFiles(path):
	for prefix in progfiles:
		file = os.path.join(prefix, path)
		if fileExists(file) or dirExists(file):
			return file # True
	return False

def getUrl(url):
	try:
		file = os.path.basename(url)
		file = os.path.join(devtool_dir, file)
		if not fileExists(file):
			print "Downloading ", url
			urllib.urlretrieve(url, file)
	except Exception:
		os.remove(file)
		print "\nERROR: could not download file from: " + url
		print "\nAborting."
		quit(1)
	return file

def getOptions(default):
	if default == "":                      return " [y/n/q/a]: "
	elif default == "y" or default == "Y": return " [Y/n/q/a]: "
	elif default == "n" or default == "N": return " [y/N/q/a]: "
	else:                                  return " [y/n/Q/a]: "

yes_to_all = False
def process_response(response):
	global yes_to_all
	if response[0] == "a" or response[0] == "A":
		yes_to_all = True
		return True
	if response[0] == "q" or response[0] == "Q":
		print "\nQuitting at user request."
		quit(0)
	return response[0] == "y" or response[0] == "Y"

def askUser(question, default="Y"):
	if yes_to_all and default == "Y": return True
	while True:
		response = raw_input(question + getOptions(default))
		if response == "": response = default
		if response != "": break
	return process_response(response)

def get_svn_rev(question):
	while True:
		response = raw_input(question + getOptions(""))
		if response != "": break
	if process_response(response):
		return "HEAD"
	if response[0] == "n" or response[0] == "N":
		return -1
	return response

def inst_msvs_2008():
	while not findInProgFiles('Microsoft Visual Studio 9.0/VC/bin/cl.exe'):
		print "\nYou need to install Visual C++ 2008 Express Edition.  Make sure to get"
		print "the 2008 edition, not 2010!  Download the installer from the website"
		print "that will be opened, then run it.  Accept default installation options."
		print "Don't forget to register it within 30 days!"
		print "After installation is complete, return to this script."
		if askUser("\nReady to download and install Visual C++?"):
			webbrowser.open_new_tab('http://www.microsoft.com/express/Downloads/#Visual_Studio_2008_Express_Downloads')
			print "\nOnce Visual C++ has been installed, press enter to continue"
			response = raw_input("")

def inst_msvs_2010():
	while not findInProgFiles('Microsoft Visual Studio 10.0/VC/bin/cl.exe'):
		print "\nYou need to install Visual C++ 2010 Express Edition.  Make sure to get"
		print "the 2010 edition, not 2008!  Download the installer from the website"
		print "that will be opened, then run it.  Accept default installation options."
		print "Don't forget to register it within 30 days!"
		print "After installation is complete, return to this script."
		if askUser("\nReady to download and install Visual C++?"):
			webbrowser.open_new_tab('http://www.microsoft.com/express/Downloads/#Visual_Studio_2010_Express_Downloads')
			print "\nOnce Visual C++ has been installed, press enter to continue"
			response = raw_input("")

def inst_msvs_2010_sp1():
	msvs2010sp1_installed_file = os.path.join(devtool_dir, "msvs2010sp1_has_been_installed.txt")
	if not fileExists(msvs2010sp1_installed_file):
		print "\nYou need to update VS2010 to Service Pack 1.  Download the installer from"
		print "the website that will be opened, then run it.  It will download about 600MB"
		print "of extra stuff and take a while to install."
		print "After installation is complete, return to this script."
		if askUser("\nReady to download and install VS2010 SP1?"):
			webbrowser.open_new_tab('http://www.microsoft.com/download/en/details.aspx?id=23691')
			print "\nWait for VS2010 SP1 to finish downloading and installing."
			if askUser("\nDid VS2010 SP1 install successfully?", default=''):
				f = open(msvs2010sp1_installed_file, 'w')
				f.write('Microsoft Visual Studio 2010 Service Pack 1 has been installed,')
				f.write('according to the user.  If that is not so, delete this file.')
				f.close()
				if askUser("Were you prompted to reboot?", default=''):
					print "\nPlease reboot now and then re-run this script"
					quit(0)
				else:
					print "\nContinuing to build..."
				return
		# User chose not to install, or installation failed.
		print "\nWithout VS2010 SP1, 64-bit builds may not build correctly."
		print "Continuing to build, but you will be prompted again next time.\n"

setenv_cmd = ""
def inst_win_sdk():
	global setenv_cmd
	while True:
		setenv_cmd = findInProgFiles('Microsoft SDKs/Windows/v7.1/bin/setenv.cmd')
		if setenv_cmd: break
		print "\nIn addition to MSVC++ 2010, you also need to install the Windows SDK"
		print "in order to build 64-bit versions of emergent.  Click the 'Install Now'"
		print "link on the website that will be opened, then run it.  Accept defaults."
		print "After installation is complete, return to this script."
		if askUser("\nReady to download and install Windows SDK?"):
			webbrowser.open_new_tab('http://msdn.microsoft.com/en-us/windowsserver/bb980924.aspx')
			print "\nOnce Windows SDK has been installed, press enter to continue"
			response = raw_input("")

# After installing VC++ 2010 Express and Windows SDK, you *still* don't have
# everything you need to compile a 64-bit app.  Need to create vcvars64.bat.
amd64_dir = ""
def create_vcvars64_bat():
	global amd64_dir
	amd64_dir = findInProgFiles('Microsoft Visual Studio 10.0/VC/bin/amd64')
	if not amd64_dir:
		print "\nCould not find amd64 directory.  Windows SDK not installed correctly?"
		print "\nAborting."
		quit(1)
	vcvars64_bat = fixPath(os.path.join(amd64_dir, "vcvars64.bat"))
	while not fileExists(vcvars64_bat):
		print "\nNeed to create vcvars64.bat file, since it is missing (as expected)."
		print "A Windows security dialog will appear -- click Yes on it."
		if askUser('\nOK to create "' + vcvars64_bat + '"?'):
			# Have to make a temp batch file to be run as admin to create the
			# batch file we actually care about.  "runas" is kinda like sudo.
			make_vcvars64_bat = os.path.join(devtool_dir, "make_vcvars64.bat")
			f = open(make_vcvars64_bat, 'w')
			f.write('echo call "' + fixPath(setenv_cmd) + '" /x64 > "' + vcvars64_bat + '"')
			f.close()
			os.startfile(make_vcvars64_bat, "runas")
			# Without this prompt, python won't notice the file creation.
			print "After clicking Yes on the Windows security dialog, hit enter in this window."
			response = raw_input("")

msvs = 0
def inst_msvs(ver):
	global msvs
	if ver == 2008:
		inst_msvs_2008()
		msvs = 2008
	elif ver == 2010:
		inst_msvs_2010()
		inst_win_sdk()
		inst_msvs_2010_sp1()
		create_vcvars64_bat()
		msvs = 2010
	else:
		raise "Bad MSVS version"

def inst_nsis():
	while not findInProgFiles('NSIS/NSIS.exe'):
		print "\nYou need to install NSIS.  A website will be opened that will prompt"
		print "you to save a file.  Save it and run it to install NSIS.  Accept default"
		print "installation options.  After installation is complete, return to this script."
		if askUser("\nReady to download and install NSIS?"):
			webbrowser.open_new_tab('http://prdownloads.sourceforge.net/nsis/nsis-2.46-setup.exe?download')
			print "\nOnce NSIS has been installed, press enter to continue"
			response = raw_input("")

qt_dir = 'C:/Qt/4.6.2'
def inst_qt():
	while not fileExists(qt_dir + '/bin/moc.exe'):
		print "\nYou need to install Qt.  This can be downloaded for you to install."
		if askUser("\nReady to download and install Qt?"):
			print "\nDownloading Qt.  When the download completes, installation will begin."
			print "Accept default installation options.  Return to this script when complete."
			print "\nTHIS FILE IS HUGE, be patient while it downloads....."
			# TBD: is this ok for 64-bit msvs2010 compiles also?
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/qt-win-opensource-4.6.2-vs2008.exe")
			os.system(file)
			print "\nOnce Qt has been installed, press enter to continue"
			response = raw_input("")

def inst_svn():
	while not findInProgFiles('SlikSvn/bin/svn.exe'):
		print "\nYou need to install a Subversion client (svn).  This can be downloaded"
		print "for you to install."
		if askUser("\nReady to download and install Subversion?"):
			print "\nDownloading Slik Subversion client.  When the download completes,"
			print "installation will begin.  Accept default installation options."
			print "Return to this script when complete."
			url = "http://www.sliksvn.com/pub/Slik-Subversion-1.6.15-win32.msi"
			if isWin64:
				url = "http://www.sliksvn.com/pub/Slik-Subversion-1.6.15-x64.msi"
			file = getUrl(url)
			os.system(file)
			print "\nOnce Subversion has been installed, press enter to continue"
			response = raw_input("")

cmake_exe = ""
def inst_cmake():
	global cmake_exe
	while True:
		cmake_exe = findInProgFiles('CMake 2.8/bin/cmake.exe')
		if cmake_exe: break
		print "\nYou need to install CMake.  This can be downloaded for you to install."
		if askUser("\nReady to download and install CMake?"):
			print "\nDownloading CMake.  When the download completes, installation will begin."
			print "Accept default installation options.  Return to this script when complete."
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/cmake-2.8.1-win32-x86.exe")
			os.system(file)
			print "\nOnce CMake has been installed, press enter to continue"
			response = raw_input("")

use_jom = False
jom_exe = ""
jom_dir = os.path.join(devtool_dir, "jom")
def inst_jom():
	global use_jom, jom_exe
	jom_exe = os.path.join(jom_dir, "jom.exe")
	if fileExists(jom_exe):
		use_jom = True
	else:
		print "\nYou should install Jom.  This can be downloaded and installed for you."
		if askUser("\nReady to download and install Jom?"):
			print "\nDownloading Jom..."
			file = getUrl("ftp://ftp.qt.nokia.com/jom/jom.zip")
			zip = ZipFile(file)
			zip.extractall(jom_dir)
		if fileExists(jom_exe):
			use_jom = True

# Check if current directory is an emergent source directory.
# If so, try to build there.
emer_src = fixPath('C:/src/emergent')
def is_emer_src_path(path):
	global emer_src
	loc = path.find("emergent")
	if loc == -1: return False
	slash = path.find("\\", loc)
	if slash == -1: slash = path.find("/", loc)
	if slash > 0: path = path[:slash]
	if path != emer_src:
		if askUser("\nBuild in this emergent source directory: " + path + " ?"):
			emer_src = path
	return True

def get_emer_src_path():
	if is_emer_src_path(os.getcwd()) or is_emer_src_path(sys.argv[0]):
		pass
	print "\nWill build in emergent source directory: " + emer_src

# Ask user if they want to build Debug or Release.
build_debug = False
def ask_debug_or_release():
	global build_debug
	done = False
	while not done:
		if build_debug:
			# Debug used to be yellow-on-black, but "vcvarsall.bat x64"
			# coincidentally uses that same scheme, so now this is blue.
			os.system('color 0b')
			if askUser("\nMake DEBUG build? (say no for release)"): done = True
			else: build_debug = False
		else:
			os.system('color 07')
			if askUser("\nMake RELEASE build? (say no for debug)"): done = True
			else: build_debug = True
	# Debug and Release versions can share the same source files, so no need for this:
	#if build_debug:
	#	emer_src += "-dbg"

# Ask user if they want 32-bit or 64-bit build.
build_64bit = True
def ask_32_or_64():
	global build_64bit
	done = False
	if msvs == 2008:
		build_64bit = False
		print "\nMSVS 2008 only supports 32-bit builds."
		if not askUser("Make 32-bit build?"): quit(0)
		done = True
	while not done:
		if build_64bit:
			if askUser("\nMake 64-bit build? (say no for 32)"): done = True
			else: build_64bit = False
		else:
			if askUser("\nMake 32-bit build? (say no for 64)"): done = True
			else: build_64bit = True

# Get the extension to add to directories to differentiate msvs2008 vs 2010, 32 vs 64, and debug.
def get_dir_extension_args(is_64, is_dbg):
	ext = '-msvs' + str(msvs)
	ext += "-64" if is_64 else "-32"
	if is_dbg: ext += "-dbg"
	return ext

def get_dir_extension():
	return get_dir_extension_args(build_64bit, build_debug)

def inst_emer_src():
	if not dirExists(emer_src + '/.svn'):
		print "\nYou need to checkout the emergent source code from subversion."
		print "This can be done for you."
		svn_rev = get_svn_rev("\nOK to get the latest emergent source from Subversion? (or enter svn rev)")
		if (svn_rev > 0):
			makeDir(emer_src)
			svnclient = findInProgFiles('SlikSvn/bin/svn.exe')
			if not svnclient:
				print "Can't continue because SVN client needs to be installed!  Quitting."
				quit(1)

			response = raw_input("\nEnter your username (blank for anonymous) [anonymous]: ")
			cmd = '"' + svnclient + '" checkout -r ' + svn_rev
			if response == "" or response == "anonymous":
				cmd += " --username anonymous --password emergent"
			else:
				cmd += " --username " + response
			cmd += " http://grey.colorado.edu/svn/emergent/emergent/trunk " + emer_src
			print "\nChecking out emergent source code..."
			print cmd
			os.system(cmd)
		else:
			print "Can't continue because no source available!  Quitting."
			quit(1)
	else:
		# Found .svn folder, make sure it's up to date
		print "\nYou probably want to make sure the emergent source is up to date."
		svn_rev = get_svn_rev("\nOK to get the latest emergent source from Subversion? (or enter svn rev)")
		if (svn_rev > 0):
			svnclient = findInProgFiles('SlikSvn/bin/svn.exe')
			if not svnclient:
				print "Can't continue because SVN client needs to be installed!  Quitting."
				quit(1)
			cmd = '"' + svnclient + '" update -r ' + svn_rev + ' ' + emer_src
			print "\nUpdating emergent source code..."
			print cmd
			os.system(cmd)

coin_dir = ""
coin_dir_32 = 'C:/Coin/3.1.3'
coin_dir_64 = 'C:/Coin/3.1.3-64'
def inst_coin():
	global coin_dir
	if build_64bit:
		coin_dir = coin_dir_64
		coin_ftp = "ftp://grey.colorado.edu/pub/emergent/Coin-3.1.3-bin-msvc9-amd64.zip"
	else:
		coin_dir = coin_dir_32
		coin_ftp = "ftp://grey.colorado.edu/pub/emergent/Coin-3.1.3-bin-msvc9.zip"

	while not fileExists(coin_dir + '/bin/coin3.dll'):
		print "\nYou need to get the Coin package.  This can be done for you."
		if askUser("\nReady to download and unzip Coin?"):
			makeDir(coin_dir)
			print "\nDownloading Coin..."
			file = getUrl(coin_ftp)
			zip = ZipFile(file)
			zip.extractall(coin_dir)

third_party_dir = ""
def inst_3rd_party():
	global third_party_dir
	third_party_dir = emer_src + '/3rdparty'
	while not fileExists(third_party_dir + '/lib/gsl.lib'):
		print "\nYou need to get the 'third party' package.  This can be done for you."
		if askUser("\nReady to download and unzip 3rd party tools?"):
			print "\nDownloading 3rd party tools..."
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/3rdparty-5.0.1-win32.zip")
			zip = ZipFile(file)
			zip.extractall(third_party_dir)

quarter_dir = 'C:/src'
quarter_sln = ''
def inst_quarter():
	global quarter_sln
	if msvs == 2008:
		quarter_sln = quarter_dir + '/Quarter/build/msvc9/quarter1.sln'
	elif msvs == 2010:
		quarter_sln = quarter_dir + '/Quarter/build/msvc10/quarter1.sln'
	while not fileExists(quarter_sln):
		print "\nYou need to get the Quarter package.  This can be done for you."
		if askUser("\nReady to download and unzip Quarter?"):
			print "\nDownloading Quarter..."
			#file = getUrl("ftp://grey.colorado.edu/pub/emergent/Quarter-latest.zip")
			file = getUrl("ftp://grey.colorado.edu/pub/emergent/Quarter-r460-msvc10-9.zip")
			zip = ZipFile(file)
			zip.extractall(quarter_dir)

def setEnvironmentVariable(name, value):
	setUserEnvVar(name, value)
	os.environ[name] = value
	print "Set " + name + "=" + value

def checkEnvironmentVariable(name, value):
	oldval = ""
	try: oldval = os.environ[name]
	except KeyError: pass
	regval = getUserEnvVar(name)
	if regval == value:
		if oldval != value:
			print "Updating " + name + " in local environment to match registry settings"
			os.environ[name] = value
		print "Environment variable OK: " + name + "=" + value
	elif regval == "" and oldval == "":
		print "Need to set environment variable: " + name + "=" + value
		if askUser("OK to set variable?"):
			setEnvironmentVariable(name, value)
	else:
		print "Need to update environment variable: " + name
		print "  Current registry value:    " + regval
		print "  Current environment value: " + oldval
		print "  New value to be set:       " + value
		if askUser("OK to change variable?"):
			setEnvironmentVariable(name, value)

# Quarter build outputs get copied to COINDIR as part of the build process,
# so test that output to see if this step has completed.
def compile_quarter():
	while not fileExists(coin_dir + '/lib/quarter1d.lib'):
		print "\nYou need to compile the Quarter package.  This can be done for you."
		if askUser("\nReady to compile Quarter?"):
			if msvs == 2008:
				msbuild = "C:/WINDOWS/Microsoft.NET/Framework/v3.5/MSBuild.exe"
				platform = "Win32"
			elif build_64bit:
				msbuild = "C:/Windows/Microsoft.NET/Framework64/v4.0.30319/MSBuild.exe"
				platform = "x64"
			else:
				msbuild = "C:/Windows/Microsoft.NET/Framework/v4.0.30319/MSBuild.exe"
				platform = "Win32"

			if not fileExists(msbuild):
				print "\nCould not find file: " + msbuild
				print "Quitting"
				quit(1)

			print "\nCompiling Quarter..."
			# os.system() won't work here -- because of the space after DLL??
			# Build Release and Debug libraries
			subprocess.call([msbuild, '/p:Configuration=DLL (Release)', '/p:Platform=' + platform, quarter_sln])
			subprocess.call([msbuild, '/p:Configuration=DLL (Debug)', '/p:Platform=' + platform, quarter_sln])
			print "\nWarnings are OK when building Quarter.  It should say 'Build succeeded'.\n"

def list_difference(orig, remove):
	for item in remove:
		try: orig.remove(item)
		except ValueError: pass

def fix_environment():
	checkEnvironmentVariable('COINDIR', fixPath(coin_dir))
	# TBD: is this ok for 64-bit msvs2010 compiles also?
	checkEnvironmentVariable('QMAKESPEC', 'win32-msvc2008')
	checkEnvironmentVariable('QTDIR', fixPath(qt_dir))
	checkEnvironmentVariable('EMERGENTDIR', fixPath('C:/Emergent'))

	needed_paths = [
		# TBD: need this?
		#fixPath(amd64_dir),
		fixPath(qt_dir + "/bin"),
		fixPath(coin_dir + "/bin"),
		fixPath(third_party_dir + "/bin"),
		fixPath(jom_dir),
		fixPath(os.path.dirname(cmake_exe)),
		]

	syspath = getSysEnvVar('PATH').split(';')
	orig_userpath = getUserEnvVar('PATH')
	userpath = orig_userpath.split(';')

 	# Get rid of items in the existing path for the wrong architecture
 	if build_64bit:
 		remove_paths = [
 			fixPath(coin_dir_32 + "/bin"),
 			]
	else:
 		remove_paths = [
 			fixPath(coin_dir_64 + "/bin"),
 			]

	list_difference(userpath, remove_paths)
	list_difference(syspath, remove_paths)

	# Only need to add things that aren't already in the path (if any).
	list_difference(needed_paths, syspath + userpath)
	if needed_paths:
		new_userpath_list = userpath + needed_paths
		new_userpath_str = ';'.join(new_userpath_list)
		print "\nNeed to update PATH environment variable."
		print "Old user path: " + orig_userpath
		print "New user path: " + new_userpath_str
		print "System path will not be changed."
		if askUser("\nReady to update path?"):
			setUserEnvVar('PATH', new_userpath_str)
			userpath = new_userpath_list

	wholepath = ';'.join(syspath) + ';' + ';'.join(userpath)
	cmd = os.environ['ComSpec']
	expanded_path = subprocess.check_output([cmd, '/c', '@echo ' + wholepath])
	os.environ['PATH'] = expanded_path
	print "\nPATH = " + expanded_path

def build_emergent():
	print "\nYour build environment is configured."
	if not askUser("\nReady to build?"): quit(0)

	print "\nBuilding emergent..."
	emer_build = emer_src + '/build' + get_dir_extension()
	makeDir(emer_build)

	if msvs == 2008:
		vcvarsall_bat = findInProgFiles('Microsoft Visual Studio 9.0/VC/vcvarsall.bat')
	elif msvs == 2010:
		vcvarsall_bat = findInProgFiles('Microsoft Visual Studio 10.0/VC/vcvarsall.bat')

	if not vcvarsall_bat:
		print "Can't continue because vcvarsall.bat file not found!  Quitting."
		quit(1)

	cmake_bat = emer_src + '/do_cmake.bat'
	f = open(cmake_bat, 'w')
	f.write('echo on\n')
	if build_64bit:
		f.write('call "' + fixPath(vcvarsall_bat) + '" x64\n')
	else:
		f.write('call "' + fixPath(vcvarsall_bat) + '" x86\n')
	f.write("cd /d " + fixPath(emer_build) + "\n")
	if build_debug:
		if msvs == 2008:
			f.write('"' + fixPath(cmake_exe) + '" .. -G "Visual Studio 9 2008" -DCMAKE_BUILD_TYPE=Debug\n')
		elif msvs == 2010:
			if build_64bit:
				f.write('"' + fixPath(cmake_exe) + '" .. -G "Visual Studio 10 Win64" -DCMAKE_BUILD_TYPE=Debug\n')
			else:
				f.write('"' + fixPath(cmake_exe) + '" .. -G "Visual Studio 10" -DCMAKE_BUILD_TYPE=Debug\n')
		f.write('if ERRORLEVEL 1 exit\n')
		f.write('start Emergent.sln\n')
	elif use_jom:
		f.write('"' + fixPath(cmake_exe) + '" .. -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Release\n')
		f.write('if ERRORLEVEL 1 exit\n')
		f.write('"' + fixPath(jom_exe) + '" package\n')
	else:
		f.write('"' + fixPath(cmake_exe) + '" .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release\n')
		f.write('if ERRORLEVEL 1 exit\n')
		f.write('nmake package\n')
	f.close()

	while 0 != os.system(cmake_bat):
		global yes_to_all
		yes_to_all = False
		if not askUser("\nCompilation failed.  OK to rebuild?"): quit(0)
	print "\nCompilation succeeded.\n"

def rename_package():
	version = ""
	revision = ""
	for line in open(emer_src + '/config.h'):
		if "VERSION" in line:
			version = line.split('"')[-2]
	for line in open(emer_build + '/src/temt/lib/svnrev.h'):
		if "SVN_REV" in line:
			revision = line.split(' ')[-1].rstrip()
			revision
	print "\nDetected version: " + version
	print "         svn rev: " + revision
	if len(version) > 0 and len(revision) > 0:
		win_arch_suffix = "-win32.exe"
		if build_64bit: win_arch_suffix = "-win64.exe"
		old_name = emer_build + '/emergent-' + version + '-win32.exe'
		new_name = emer_build + '/emergent-' + version + '-' + revision + win_arch_suffix
		if not os.path.isfile(old_name):
			print "\nCould not find installer: " + old_name
			quit(1)
		ok_to_overwrite = False
		if os.path.isfile(new_name):
			print "\nInstaller already exists with name: " + new_name
			if askUser("\nOK to overwrite?"):
				ok_to_overwrite = True
				os.remove(new_name)
			else:
				print "\nWill not overwrite existing installer, quitting."
				quit(0)
		print "\nWill rename installer:"
		print "  from: " + old_name
		print "  to:   " + new_name
		if ok_to_overwrite or askUser("\nOK to rename?"):
			os.rename(old_name, new_name)
			print "Installer renamed."

try:
	os.system('color 07')
	inst_msvs(2008) # 2010 Doesn't completely work yet...
	inst_nsis()
	inst_qt()
	inst_svn()
	inst_cmake()
	inst_jom()
	get_emer_src_path()
	ask_debug_or_release()
	ask_32_or_64()
	inst_emer_src()
	inst_coin()
	inst_3rd_party()
	inst_quarter()
	fix_environment()
	compile_quarter()
	build_emergent()
	if not build_debug:
		rename_package()
	quit(0)
except KeyboardInterrupt:
	print "\n\nQuitting at user request (Ctrl-C)."

