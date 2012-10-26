// svncpp
#include <apr.hpp>
#include <context.hpp>
#include <client.hpp>
#include <wc.hpp>

// subversion
#include <Subversion.h>

//#include <qt4/QtCore/qdir.h>

int main() {
	std::string svnPath = "/home/houman/Desktop/wc/";
	mysvn::Subversion svnapi(svnPath);
	//svnapi.mkdir("test6");
	/*
	 // CHECKOUT
	 const char * moduleName = "/home/houman/Desktop/svncopy/";
	 const svn::Path destPath = "/home/houman/Desktop/svncopy15";
	 long int svn_revnum_t = 1;
	 svn::Revision revision(svn_revnum_t);
	 bool recurse = true;
	 bool ignoreExternals = false;
	 svn::Revision pegRevision(svn_revnum_t);
	 svnapi.Checkout(moduleName, destPath, revision, recurse, ignoreExternals,
	 pegRevision);
	 */

	/*
	// STATUS
	const char * path = "/home/houman/Desktop/wc";
	const bool descend = false;
	const bool getAll = false;
	const bool update = false;
	const bool noIgnore = false;
	const bool ignoreExternals = false;
	svn::StatusEntries entries = svnapi.Status(path, descend, getAll, update,
			noIgnore, ignoreExternals);
	svn::StatusEntries::const_iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		const svn::Path modifiedPath((*it).path());
		std::cout << modifiedPath.path() << std::endl;
	}
	*/


	 // COMMIT
	 const char * p = "/home/houman/Desktop/wc/dir2";
	 svn::Path path(p);
	 svn::PathVector pathVector;
	 pathVector.push_back(path);
	 svn::Targets targets(pathVector);
	 const char * message = "commit message.";
	 std::cout << svnapi.Commit(targets, message, true, false);


	/*
	 const char * p = "/home/houman/Desktop/wc/";
	 svn::Path path(p);
	 std::cout << svnapi.Update(p, svn::Revision::HEAD, true, false);
	 */

	//QDir("Folder").exists();

	return 0;
}

