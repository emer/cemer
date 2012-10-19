/*
 * Subversion.h
 *
 *  Created on: Oct 17, 2012
 *      Author: houman
 */

#ifndef Subversion_H_
#define Subversion_H_

#include <apr.hpp>
#include <context.hpp>
#include <client.hpp>
#include <wc.hpp>
#include <paths.h>
#include <revision.hpp>
#include <targets.hpp>
#include <path.hpp>

#include <iostream>

namespace mysvn {

class Subversion {
	svn::Context *context;
	svn::Client client;

	std::string svnPath;
public:
	Subversion(std::string svnPath);
	virtual ~Subversion();
	int mkdir(std::string name);
	long int checkout(const char* moduleName, const svn::Path destPath,
			svn::Revision revision, bool recurse, bool ignoreExternals,
			const svn::Revision pegRevision);
	int add(std::string origin, std::string destination);
	int long commit(const svn::Targets & targets, const char * message,
			bool recurse, bool keepLocks);
};

}
/* namespace mysvn */
#endif /* Subversion_H_ */

