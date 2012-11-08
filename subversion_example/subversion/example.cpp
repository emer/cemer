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
#include <client.hpp>
#include <context.hpp>
#include <context_listener.hpp>

#include <iostream>

int testSvnMakeDir() {
	/*	old stuff
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

int testSvnCheckOut() {
	apr_initialize();
	apr_pool_t * pool = svn_pool_create(0);

	svn_revnum_t * result_rev = new svn_revnum_t();
	const char * URL = svn_path_canonicalize("file:///home/houman/Desktop/svn",
			pool);
	const char * path = svn_path_canonicalize("/home/houman/Desktop/", pool);
	const svn_opt_revision_t * peg_revision = new svn_opt_revision_t();
	const svn_opt_revision_t * revision = new svn_opt_revision_t();
	svn_boolean_t ignore_externals = false;
	svn_boolean_t allow_unver_obstructions = true;
	svn::Context * context = new svn::Context();
	svn_client_ctx_t * ctx = context->ctx();
	try {
		/*
		 * Checkout a working copy from a repository.
		 * Parameters:
		 * [out]result_rev
		 * 		If non-NULL, the value of the revision checked out from the repository.
		 * [in]	URL	The repository URL of the checkout source.
		 * [in]	path	The root of the new working copy.
		 * [in]	peg_revision	The peg revision.
		 * [in]	revision	The operative revision.
		 * [in]	depth	The depth of the operation. If svn_depth_unknown, then behave as if for svn_depth_infinity, except in the case of resuming a previous checkout of path (i.e., updating), in which case use the depth of the existing working copy.
		 * [in]	ignore_externals
		 * 		If TRUE, don't process externals definitions as part of this operation.
		 * [in]	allow_unver_obstructions
		 * 		If TRUE, then tolerate existing unversioned items that obstruct incoming paths. Only obstructions of the same type (file or dir) as the added item are tolerated. The text of obstructing files is left as-is, effectively treating it as a user modification after the checkout. Working properties of obstructing items are set equal to the base properties.
		 * 		If FALSE, then abort if there are any unversioned obstructing items.
		 * [in]	ctx	The standard client context, used for authentication and notification.
		 * [in]	pool	Used for any temporary allocation.
		 */
		svn_error_t * err = svn_client_checkout3(result_rev, URL, path,
				peg_revision, revision, svn_depth_infinity, ignore_externals,
				allow_unver_obstructions, ctx, pool);
		//std::cout << err->message << std::endl;
	} catch (svn::ClientException &e) {
		std::cout << e.message() << std::endl;
	}
	svn_pool_destroy(pool);
	apr_terminate();
}

int main() {
	//testSvnMakeDir();
	testSvnCheckOut();
	std::cout << "DONE!";
	return 0;
}
