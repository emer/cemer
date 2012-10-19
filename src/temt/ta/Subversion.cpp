/*
 * Subversion.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: houman
 */

#include "Subversion.h"

namespace mysvn {

Subversion::Subversion(std::string svnPath) {
	this->svnPath = svnPath;

	//svn::Context *context = new svn::Context("~/.subversion");	// no need to specify the subversion configuration file as if it's not set svncpp will find the default one
	this->context = new svn::Context();
	this->client.setContext(context);
}

Subversion::~Subversion() {
}

int Subversion::mkdir(std::string name) {

	std::string fullpath = this->svnPath + name;
	try {
		client.mkdir(fullpath);
	} catch (const svn::ClientException &e) {
		std::cout << "Caught exception: " << e.message() << std::endl;
	}
	return 1;	// success
}

/* INPUT:
 * moduleName 			name of the module to checkout.
 * destPath 			destination directory for checkout.
 * revision 			the revision number to checkout. If the number is -1 then it will checkout the latest revision.
 * recurse 				whether you want it to checkout files recursively.
 * ignore_externals 	whether you want get external resources too.
 * peg_revision 		peg revision to checkout, by default current.
 *
 * OUTPUT:
 * the revision checked out
 */
long int Subversion::checkout(const char* moduleName, const svn::Path destPath,
		svn::Revision revision, bool recurse, bool ignoreExternals,
		const svn::Revision pegRevision) {
	long int checkedOutRev;
	try {
		checkedOutRev = client.checkout(moduleName, destPath, revision, recurse,
				ignoreExternals, pegRevision);
	} catch (const svn::ClientException &e) {
		std::cout << "Caught exception: " << e.message() << std::endl;
	}
	return checkedOutRev;
}

long int Subversion::commit(const svn::Targets & targets, const char * message,
		bool recurse, bool keepLocks = false) {
	long int checkedOutRev = 0;
	try {
		checkedOutRev = client.commit(targets, message, recurse, keepLocks);
	} catch (const svn::ClientException &e) {
		std::cout << "Caught exception: " << e.message() << std::endl;
	}
	return checkedOutRev;
}
} /* namespace mysvn */
