// apr
#include <apr.h>
#include <apr_errno.h>
#include <apr_pools.h>
#include <apr_general.h>

// subversion
#include <svn_client.h>
#include <svn_repos.h>
#include <svn_pools.h>
#include <svn_error.h>
#include <svn_path.h>

// kdesvn
#include <context.hpp>

#include "emergent_svn/pool.h"
#include "emergent_svn/client.h"

#include <iostream>

int testSvnMakeDir() {
	/*  old stuff
	 apr_initialize();
	 apr_pool_t * pool = svn_pool_create(0);
	 const char * path = svn_path_canonicalize("/home/houman/Desktop/wc/", pool);
	 const char * newDir = svn_path_dirname(svn_path_canonicalize("dir3", pool),
	 pool);
	 //make_new_directory(path, newDir, pool);
	 svn_commit_info_t ** commit_info_p = svn_create_commit_info(pool);
	 const apr_array_header_t * paths = "/home/houman/Desktop/wc/";
	 //svn_client_ctx_t * ctx = svn_client_create_context();
	 //svn_client_mkdir(commit_info_p, paths, ctx, pool);
	 svn_pool_destroy(pool);
	 apr_terminate();
	 */
	return 0;
}

int main() {
	//testSvnMakeDir();

	// create a pool
	Pool pool;

	//const char *url = "http://grey.colorado.edu/svn/emergent/emergent/trunk/package";
	const char *url = "file:///home/houman/Desktop/svn/";

	const char *path = "/home/houman/Desktop/wc6/dir15";

	Client client;

	/*
	int rev =  client.Checkout(pool, url, path);

	if (rev < 0) {
			std::cout << "Error checking out code\n  from: " << url << "\n  to: " << path << "\ncheckout() returned " << rev
					<< std::endl;
		} else {
			std::cout << "Checked out revision: " << rev << std::endl;
		}

	bool success = client.Update(pool, path);

	*/

	client.Mkdir(pool, path);


	pool.~Pool();

	return 0;
}

