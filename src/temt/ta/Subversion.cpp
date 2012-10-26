/*
 * Subversion.cpp
 *
 *  Created on: Oct 17, 2012
 *      Author: houman
 */

#include "Subversion.h"

namespace mysvn {




EmergentSvnContextListener::EmergentSvnContextListener(Subversion *pSvn)
  : pSvn(pSvn)
{
}

EmergentSvnContextListener~EmergentSvnContextListener()
{
}

bool
EmergentSvnContextListener::contextGetLogin(const std::string & realm,
                  std::string & username,
                  std::string & password,
                  bool & maySave)
{
	return false;
}

void
EmergentSvnContextListener::contextNotify(const char *path,
                svn_wc_notify_action_t action,
                svn_node_kind_t kind,
                const char *mime_type,
                svn_wc_notify_state_t content_state,
                svn_wc_notify_state_t prop_state,
                svn_revnum_t revision)
{
}

bool
EmergentSvnContextListener::contextCancel()
{
	return false;
}

bool
EmergentSvnContextListener::contextGetLogMessage(std::string & msg)
{
	return false;
}

svn::SslServerTrustAnswer
EmergentSvnContextListener::contextSslServerTrustPrompt(const SslServerTrustData & data,
                              apr_uint32_t & acceptedFailures)
{
	static svn::SslServerTrustAnswer tmp;
	return tmp;
}

bool
EmergentSvnContextListener::contextSslClientCertPrompt(std::string & certFile)
{
	return false;
}

bool
EmergentSvnContextListener::contextSslClientCertPwPrompt(std::string & password,
                               const std::string & realm,
                               bool & maySave)
{
return false;
}



Subversion::Subversion(std::string svnPath)
  : context(new svn::Context)
  , client()
  , pool()
  , contextListener(new EmergentSvnContextListener)
  , svnPath(svnPath)
{
	//this->context = new svn::Context("~/.subversion");	// no need to specify the subversion configuration file as if it's not set svncpp will find the default one
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
		bool recurse = true, bool keepLocks = false) {
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

svn_revnum_t Subversion::Update(const svn::Path & path,
		const svn::Revision & revision, bool recurse = true,
		bool ignore_externals = false) {
	svn_revnum_t rev = 0;
	try {
		svn::Pool pool;
		rev = client.update(path, revision, recurse, ignore_externals);
	} catch (const svn::ClientException &e) {
		std::cout << "Caught exception: " << e.message() << std::endl;
	}
	return rev;
}



} /* namespace mysvn */
