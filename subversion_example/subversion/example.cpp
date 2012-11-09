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

int testSvnMakeDir()
{
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

int checkout(const char *url, const char *working_copy_path, int rev = -1)
{
  // Set up APR and a pool -- eventually we'll move this somewhere
  // more global.
  apr_initialize();
  apr_pool_t * pool = svn_pool_create(0);

  // Out parameter -- the value of the revision checked out from the
  // repository.
  svn_revnum_t result_rev;

  // URL isn't a path.  Even if the repo is on local disk and accessed
  // with file:///, that's still a URL, not a path.
  //url = svn_path_canonicalize(url, pool);

  // Canonicalize the path.
  working_copy_path = svn_path_canonicalize(working_copy_path, pool);

  // We don't want to use peg revisions, so set to unspecified.
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_unspecified;

  // Set the revision number, if provided.  Otherwise get HEAD revision.
  svn_opt_revision_t revision;
  if (rev < 0) {
    revision.kind = svn_opt_revision_head;
  }
  else {
    revision.kind = svn_opt_revision_number;
    revision.value.number = rev;
  }

  // Get all files.
  svn_depth_t depth = svn_depth_infinity;

  // Set advanced options we don't care about.
  svn_boolean_t ignore_externals = false;
  svn_boolean_t allow_unver_obstructions = true;

  // TODO: Are we going to use kdesvncpp stuff or roll our own?
  // If we're just using this as a way to get an initialized value for
  // svn_client_ctx_t then that's not good.
  svn::Context * context = new svn::Context();
  svn_client_ctx_t * ctx = context->ctx();

  // Don't need to wrap in try/catch because it's just a C call.
  // (C doesn't have exceptions).  Although, if the callbacks to
  // whatever svn::Context provides throw exceptions then we have
  // a problem.  Let's see if we can avoid using kdesvncpp directly.
  try {
    svn_error_t * err =
      svn_client_checkout3(
        &result_rev, // out param
        url,
        working_copy_path,
        peg_revision,
        revision,
        depth,
        ignore_externals,
        allow_unver_obstructions,
        ctx,
        pool);

    // Check for error.
    if (err) {
      // TODO: Hook this into taMisc::Warning()
      std::cout << "Subversion error: " << err->message << std::endl;
      svn_error_clear(err);
      return -1;
    }
  }
  catch (svn::ClientException &e) {
    std::cout << e.message() << std::endl;
  }

  // Clean up the pool and APR context.
  // TODO: this should be more global.
  svn_pool_destroy(pool);
  apr_terminate();
}

int main()
{
  //testSvnMakeDir();

  const char *url = "http://grey.colorado.edu/svn/emergent/emergent/trunk/package";
  const char *path = "/home/houman/Desktop/";

  int rev = checkout(url, path);

  if (rev < 0) {
    std::cout << "Error checking out code\n  from: " << url
      << "\n  to: " << path
      << "\ncheckout() returned " << rev << std::endl;
  }
  else {
    std::cout << "Checked out revision: " << rev << std::endl;
  }

  return 0;
}
