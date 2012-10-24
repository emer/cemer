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
	//this->context = new svn::Context("~/.subversion");	// no need to specify the subversion configuration file as if it's not set svncpp will find the default one
	this->context = new svn::Context();
	this->context->setListener(this->contextListener);
	this->client.setContext(context);

	//std::cout << this->statusSel.hasFiles();
	//std::cout << this->context->getListener();
	//this->contextListener = new svn::ContextListener();
}

Subversion::~Subversion() {
}

int Subversion::Mkdir(std::string name) {

	std::string fullpath = this->svnPath + name;
	try {
		client.mkdir(fullpath);
	} catch (const svn::ClientException &e) {
		std::cout << "Caught exception: " << e.message() << std::endl;
	}
	return 1;	// success
}

bool Subversion::AuthSetup() {
	return true;
}

/* INPUT:
 * moduleName 			name of the module to Checkout.
 * destPath 			destination directory for Checkout.
 * revision 			the revision number to Checkout. If the number is -1 then it will Checkout the latest revision.
 * recurse 				whether you want it to Checkout files recursively.
 * ignore_externals 	whether you want get external resources too.
 * peg_revision 		peg revision to Checkout, by default current.
 *
 * OUTPUT:
 * the revision checked out
 */
long int Subversion::Checkout(const char* moduleName, const svn::Path destPath,
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

long int Subversion::Commit(const svn::Targets & targets, const char * message,
		bool recurse, bool keepLocks = false) {
	long int checkedOutRev = 0;
	try {
		checkedOutRev = client.commit(targets, message, recurse, keepLocks);
	} catch (const svn::ClientException &e) {
		std::cout << "Caught exception: " << e.message() << std::endl;
	}
	return checkedOutRev;
}

svn::StatusEntries Subversion::Status(const char * path, const bool descend =
		false, const bool getAll = true, const bool update = false,
		const bool noIgnore = false, const bool ignoreExternals = false) {
	svn::StatusEntries statusEntries;
	try {
		statusEntries = client.status(path, descend, getAll, update, noIgnore,
				ignoreExternals);
	} catch (const svn::ClientException &e) {
		std::cout << "Caught exception: " << e.message() << std::endl;
	}
	return statusEntries;
}

} /* namespace mysvn */
