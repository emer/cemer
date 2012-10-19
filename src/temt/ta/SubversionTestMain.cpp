#include <apr.hpp>
#include <context.hpp>
#include <client.hpp>
#include <wc.hpp>
#include <iostream>
#include <Subversion.h>

//using namespace svn;
using namespace mysvn;
using namespace std;
int main() {
	string svnPath = "/home/houman/Desktop/svncopy/";
	mysvn::Subversion svnapi(svnPath);
	//svnapi.mkdir("test6");
	/*
	 const char * moduleName = "/home/houman/Desktop/svncopy/";
	 const svn::Path destPath = "/home/houman/Desktop/svncopy15";
	 long int svn_revnum_t = 1;
	 svn::Revision revision(svn_revnum_t);
	 bool recurse = true;
	 bool ignoreExternals = false;
	 svn::Revision pegRevision(svn_revnum_t);
	 svnapi.checkout(moduleName, destPath, revision, recurse, ignoreExternals,
	 pegRevision);

	 */

	const char * t = "/home/houman/Desktop/svncopy/txt.txt";
	svn::Path path(t);
	svn::PathVector pathVector;
	pathVector.push_back(path);
	//pathVector.push_back(path);
	svn::Targets targets(pathVector);
	std::cout << svnapi.commit(targets, "sdfsdf", false, false);
	return 0;
}
