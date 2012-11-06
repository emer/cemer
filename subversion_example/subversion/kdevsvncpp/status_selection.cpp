/*
 * ====================================================================
 * Copyright (c) 2002-2009 The RapidSvn Group.  All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the file GPL.txt.  
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://rapidsvn.tigris.org/.
 * ====================================================================
 */

// subversion api
#include "svn_types.h"

// apr api
#include "apr_file_info.h"

// svncpp

#include "kdevsvncpp/exception.hpp"
#include "kdevsvncpp/path.hpp"
#include "kdevsvncpp/pool.hpp"
#include "kdevsvncpp/status.hpp"
#include "kdevsvncpp/status_selection.hpp"
#include "kdevsvncpp/targets.hpp"
#include "kdevsvncpp/url.hpp"


namespace svn
{
  struct StatusSel::Data
  {
    Targets targets;
    StatusVector status;

    bool hasDirs;
    bool hasFiles;
    bool hasVersioned;
    bool hasUnversioned;
    bool hasUrl;
    bool hasLocal;

    Path emptyTarget;

    /** default constructor */
    Data() {}

    /** copy constructor */
    Data(const Data & src)
    {
      if (this != &src)
        assign(src);
    }

    /** assign new values */
    void
    assign(const Data & src)
    {
      // clear existing...
      clear();

      // ... and set from source
      StatusVector::const_iterator it;
      for (it = src.status.begin(); it != src.status.end(); it++)
      {
        push_back(*it);
      }
    }

    void
    clear()
    {
      targets.clear();
      status.clear();

      hasDirs = false;
      hasFiles = false;
      hasVersioned = false;
      hasUnversioned = false;
      hasLocal = false;
      hasUrl = false;
    }

    void
    push_back(const Status & status_)
    {
      // skip pseudo entries
      if (!status_.isSet())
        return;

      if (!status_.isVersioned())
      {
        // for an unversioned entry we do not know
        // whether it's a file or a directory so
        // we have to check using APR
        apr_finfo_t finfo;
        Pool pool;
        apr_status_t apr_status = apr_stat(
                                    &finfo, status_.path(), APR_FINFO_TYPE, pool);

        // if we get an error the file might
        // have been deleted in the meantime
        // anyhow: we dont want to display it
        if (apr_status != APR_SUCCESS)
          return;

        hasUnversioned = true;

        if (APR_DIR == finfo.filetype)
          hasDirs = true;
        else
          hasFiles = true;

      }
      else
      {
        hasVersioned = true;
        if (Url::isValid(status_.path()))
          hasUrl = true;
        else
          hasLocal = true;

        if (svn_node_dir == status_.entry().kind())
          hasDirs = true;
        else
          hasFiles = true;
      }

      // add stuff only now (because of possible apr_error
      // which causes the function to exit)
      targets.push_back(status_.path());
      status.push_back(status_);
    }
  };


  StatusSel::StatusSel()
      : m(new Data)
  {
  }

  StatusSel::StatusSel(const StatusSel & src)
      : m(new Data)
  {
    // different instance?
    if (this != &src)
      m->assign(*src.m);
  }

  StatusSel &
  StatusSel::operator = (const StatusSel & src)
  {
    if (this != &src)
    {
      delete m;
      m = new Data(*src.m);
    }

    return *this;
  }

  StatusSel::~StatusSel()
  {
    delete m;
  }

  const apr_array_header_t *
  StatusSel::array(const Pool & pool) const
  {
    return m->targets.array(pool);
  }

  const StatusVector &
  StatusSel::statusVector() const
  {
    return m->status;
  }

  const Targets &
  StatusSel::targets() const
  {
    return m->targets;
  }

  size_t
  StatusSel::size() const
  {
    return m->targets.size();
  }

  void
  StatusSel::push_back(const Status & status)
  {
    m->push_back(status);
  }

  void
  StatusSel::clear()
  {
    m->clear();
  }

  void
  StatusSel::reserve(size_t size)
  {
    m->targets.reserve(size);
    m->status.reserve(size);
  }

  StatusSel::operator const PathVector & () const
  {
    return m->targets;
  }

  const Path &
  StatusSel::target() const
  {
    if (size() > 0)
      return m->targets.targets()[0];
    else
      return m->emptyTarget;
  }

  bool
  StatusSel::hasDirs() const
  {
    return m->hasDirs;
  }

  bool
  StatusSel::hasFiles() const
  {
    return m->hasFiles;
  }

  bool
  StatusSel::hasVersioned() const
  {
    return m->hasVersioned;
  }

  bool
  StatusSel::hasUnversioned() const
  {
    return m->hasUnversioned;
  }

  bool
  StatusSel::hasLocal() const
  {
    return m->hasLocal;
  }

  bool
  StatusSel::hasUrl() const
  {
    return m->hasUrl;
  }
}

/* -----------------------------------------------------------------
 * local variables:
 * eval: (load-file "../../rapidsvn-dev.el")
 * end:
 */
