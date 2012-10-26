/*
 * Subversion.h
 *
 *  Created on: Oct 17, 2012
 *      Author: houman
 */

#ifndef Subversion_H_
#define Subversion_H_

// svncpp
#include <apr.hpp>
#include <context.hpp>
#include <client.hpp>
#include <wc.hpp>
#include <paths.h>
#include <revision.hpp>
#include <targets.hpp>
#include <path.hpp>
#include <context_listener.hpp>
#include <status.hpp>
#include <status_selection.hpp>

#include <iostream>

namespace mysvn {

class Subversion; // fwd declare

class EmergentSvnContextListener : public svn::ContextListener
{
  Subversion *pSvn;

public:
	EmergentSvnContextListener(Subversion *pSvn); // need this??
	  virtual ~EmergentSvnContextListener(); // need this??

	  // TODO: implement all of these methods in the .cpp
	virtual bool
  contextGetLogin(const std::string & realm,
                  std::string & username,
                  std::string & password,
                  bool & maySave);

  virtual void
  contextNotify(const char *path,
                svn_wc_notify_action_t action,
                svn_node_kind_t kind,
                const char *mime_type,
                svn_wc_notify_state_t content_state,
                svn_wc_notify_state_t prop_state,
                svn_revnum_t revision);

  virtual bool
  contextCancel();

  virtual bool
  contextGetLogMessage(std::string & msg);

  virtual SslServerTrustAnswer
  contextSslServerTrustPrompt(const SslServerTrustData & data,
                              apr_uint32_t & acceptedFailures);

  virtual bool
  contextSslClientCertPrompt(std::string & certFile);

  virtual bool
  contextSslClientCertPwPrompt(std::string & password,
                               const std::string & realm,
                               bool & maySave);
};


class Subversion
{
private:
	friend EmergentSvnContextListener; // maybe need this?

	//svn::Apr apr;	// might be unnecessary
	svn::Context * context;
	svn::Client client;
	svn::Pool pool;	// might be unnecessary
	svn::ContextListener * contextListener;
	//svn::StatusSel statusSel;

	std::string svnPath;
public:
	Subversion(std::string svnPath);
	virtual ~Subversion();
	int Mkdir(std::string name);
	long int Checkout(const char* moduleName, const svn::Path destPath,
			svn::Revision revision, bool recurse, bool ignoreExternals,
			const svn::Revision pegRevision);
	int Add(std::string origin, std::string destination);
	int long Commit(const svn::Targets & targets, const char * message,
			bool recurse, bool keepLocks);
	svn::StatusEntries Status(const char * path, const bool descend,
			const bool getAll, const bool update, const bool noIgnore,
			const bool ignoreExternals);
	svn_revnum_t Update(const svn::Path & path, const svn::Revision & revision,
			bool recurse, bool ignore_externals);
	bool AuthSetup();

};

}
/* namespace mysvn */
#endif /* Subversion_H_ */

