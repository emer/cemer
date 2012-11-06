//============================================================================
// Name        : main.cpp
// Author      : Houman
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <apr.h>
#include <apr_xlate.h>
#include <apr_hash.h>
#include <apr_errno.h>
#include <apr_pools.h>
#include <apr_file_io.h>
#include <apr_general.h>

#include <svn_client.h>
#include <svn_repos.h>
#include <svn_fs.h>
#include <svn_pools.h>
#include <svn_error.h>
#include <svn_path.h>

#include "kdevsvncpp/client.hpp"
#include "kdevsvncpp/context.hpp"

int testSvnMakeDir() {

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

	return 0;
}

int testSvnCheckOut() {

}

int main() {
	//testSvnMakeDir();
	return 0;
}

