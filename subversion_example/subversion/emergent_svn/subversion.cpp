/*
 * client.cpp
 *
 *  Created on: Nov 9, 2012
 *      Author: houman
 */

#include "subversion.h"

Subversion::Subversion() {
	// TODO Auto-generated constructor stub

}

Subversion::~Subversion() {
	// TODO Auto-generated destructor stub
}

/*
 * Pool: pool
 * const char *url: the url to the repository to checkout from
 * const char *working_copy_path: the path at which the checked out working copy will be saved
 * int rev: the revision to checkout - DEFAULT: the head revision
 *
 * OUTPUT:
 * the value of the revision checked out from the repository
 */
int Subversion::Checkout(Pool pool, const char *url, const char *working_copy_path, int rev) {

	// Out parameter -- the value of the revision checked out from the repository.
	svn_revnum_t result_rev;

	// Canonicalize the path.
	working_copy_path = svn_path_canonicalize(working_copy_path, pool.pool());

	// We don't want to use peg revisions, so set to unspecified.
	svn_opt_revision_t peg_revision; // = new svn_opt_revision_t();
	peg_revision.kind = svn_opt_revision_unspecified;

	// Set the revision number, if provided. Otherwise get HEAD revision.
	svn_opt_revision_t revision; // = new svn_opt_revision_t();
	if (rev < 0) {
		revision.kind = svn_opt_revision_head;
	} else {
		revision.kind = svn_opt_revision_number;
		revision.value.number = rev;
	}

	// Get all files.
	svn_depth_t depth = svn_depth_infinity;

	// Set advanced options we don't care about.
	svn_boolean_t ignore_externals = false;
	svn_boolean_t allow_unver_obstructions = true;

	// TODO: implement our own context
	svn::Context context;
	svn_client_ctx_t *ctx = context.ctx();

	svn_error_t *err = svn_client_checkout3(
			&result_rev, // out param
			url, working_copy_path, &peg_revision, &revision, depth, ignore_externals, allow_unver_obstructions, ctx,
			pool.pool());

	// Check for error.
	if (err) {
		// TODO: Hook this into taMisc::Warning()
		std::cout << "Subversion error: " << err->message << std::endl;
		svn_error_clear(err);
		return -1;
	}
	return result_rev;
}

/*
 * Pool: pool
 * const char *path: the path to update
 * int rev: the revision to get - DEFAULT: the head revision
 *
 * OUTPUT:
 * true: successfully updated
 * false: error
 */
bool Subversion::Update(Pool pool, const char * path, int rev) {

	// Out parameter -- the value of the revision checked out from the repository.
	apr_array_header_t *result_revs;

	// create an array containing a single element which is the input path to be updated
	apr_array_header_t *paths = apr_array_make(pool.pool(), 1, sizeof(const char *));
	path = svn_path_canonicalize(path, pool.pool());
	(*((const char **) apr_array_push(paths))) = path;

	// Set the revision number, if provided. Otherwise get HEAD revision.
	svn_opt_revision_t revision;
	if (rev < 0) {
		revision.kind = svn_opt_revision_head;
	} else {
		revision.kind = svn_opt_revision_number;
		revision.value.number = rev;
	}

	// Get all files.
	svn_depth_t depth = svn_depth_infinity;

	// Set advanced options we don't care about.
	svn_boolean_t ignore_externals = false;
	svn_boolean_t allow_unver_obstructions = true;
	svn_boolean_t depth_is_sticky = true;

	// TODO: implement our own context
	svn::Context context;
	svn_client_ctx_t *ctx = context.ctx();

	svn_error_t *err = svn_client_update3(&result_revs, paths, &revision, depth, depth_is_sticky, ignore_externals,
			allow_unver_obstructions, ctx, pool.pool());

	// Check for error.
	if (err) {
		// TODO: Hook this into taMisc::Warning()
		std::cout << "Subversion error: " << err->message << std::endl;
		svn_error_clear(err);
		return false;
	}

	/*	the following loop provides a vector containing all updated revision numbers
	 *  I commented it out since we don't need it.
	 std::vector<svn_revnum_t> rev_nums;
	 int i;
	 for (i = 0; i < result_revs->nelts; i++) {
	 svn_revnum_t rev_num = APR_ARRAY_IDX(result_revs, i, svn_revnum_t) ;
	 rev_nums.push_back(rev_num);
	 }
	 */

	return true;
}

bool Subversion::Mkdir(Pool pool, const char *path) {

	// won't be used unless we make an immediate commit after adding files (by setting revprop_table)
	svn_commit_info_t *commit_info_p = svn_create_commit_info(pool.pool());

	// create an array containing a single path to be created
	apr_array_header_t *paths = apr_array_make(pool.pool(), 1, sizeof(const char *));
	path = svn_path_canonicalize(path, pool.pool());
	(*((const char **) apr_array_push(paths))) = path;

	// create any non-existent parent directories
	svn_boolean_t make_parents = true;

	// we need to set revprop_table to a non-null if we wanna make an immediate commit after adding files
	// svn_client_propget3 can be used to create an apr_hash_t
	const apr_hash_t *revprop_table = NULL;

	// TODO: implement our own context
	svn::Context context;
	svn_client_ctx_t *ctx = context.ctx();

	svn_error_t *err = svn_client_mkdir3(&commit_info_p, paths, make_parents, revprop_table, ctx, pool.pool());

	// Check for error.
	if (err) {
		// TODO: Hook this into taMisc::Warning()
		std::cout << "Subversion error: " << err->message << std::endl;
		svn_error_clear(err);
		return false;
	}

	return true;

}

int Subversion::Commit(Pool pool, const char *path, const char *message) {

	//svn_commit_info_t *commit_info_p = svn_create_commit_info(pool.pool());
	svn_commit_info_t *commit_info_p = NULL;

	// create an array containing a single element which is the path path to be committed
	apr_array_header_t *paths = apr_array_make(pool.pool(), 1, sizeof(const char *));
	path = svn_path_canonicalize(path, pool.pool());
	(*((const char **) apr_array_push(paths))) = path;

	// commit changes to the children of the paths
	svn_depth_t depth = svn_depth_infinity;

	// unlock paths in the repository after commit
	svn_boolean_t keep_locks = false;

	// no need to changelist filtering
	svn_boolean_t keep_changelists = false;
	const apr_array_header_t *changelists = apr_array_make(pool.pool(), 0, sizeof(const char *));

	const apr_hash_t *revprop_table = NULL;

	// TODO: implement our own context
	svn::Context context;
	context.setLogMessage("message");
	svn_client_ctx_t *ctx = context.ctx();


	// to be used if we need to load the log message from a file
	//const char *tmp_file = NULL;
	//(*svn_client_get_commit_log3_t)(&message, tmp_file, paths, ctx->log_msg_baton3, pool.pool());

	svn_error_t *err = svn_client_commit4(&commit_info_p, paths, depth, keep_locks, keep_changelists, changelists,
			revprop_table, ctx, pool.pool());

	// Check for error.
	if (err) {
		// TODO: Hook this into taMisc::Warning()
		std::cout << "Subversion error: " << err->message << std::endl;
		svn_error_clear(err);

		return false;
	}
	return 1; // TODO: return the new revision
}


