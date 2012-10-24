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

class Subversion {
	svn::Apr apr;
	svn::Context *context;
	svn::Client client;
	svn::ContextListener *contextListener;
	svn::Pool pool;
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
			const bool getAll, const bool update,
			const bool noIgnore, const bool ignoreExternals );
	bool AuthSetup();
};

}
/* namespace mysvn */
#endif /* Subversion_H_ */

