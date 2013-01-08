// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef IDataLinkClient_h
#define IDataLinkClient_h 1

// parent includes:
#include <IDataLinkProxy>

// member includes:

// declare all other types mentioned but not required to include:

/* ****WARNING****
  Do NOT put logging (ie calls to taMisc::Warning, etc.) in any data link mgt
  code because it can get routed through the pager or other buffering mechanism
  which may trigger event loop calls, which can cause deletions, mid-routine
*/

// Mixin interface for Node that uses datalinks, ex. tree node or Inventor node

class TA_API IDataLinkClient : public virtual IDataLinkProxy {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
  // this is the interface available to the host data object
friend class taDataLink;
public:
#ifndef TA_NO_GUI
  inline taiDataLink*   link() const {return (taiDataLink*)m_link;}
#else
  inline taDataLink*    link() const {return m_link;}
#endif
  inline taDataLink*    link_() const {return m_link;}
  virtual bool          ignoreDataChanged() const {return false;}
    // hidden guys can ignore changes (but they always get Destroyed)
  virtual bool          isDataView() const {return false;} // true for dataviews
  virtual void          DataLinkDestroying(taDataLink* dl) = 0; // called by DataLink when destroying; it will remove datalink ref in dlc upon return
  virtual void          DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) = 0; //
  virtual void          IgnoredDataChanged(taDataLink* dl, int dcr,
    void* op1, void* op2) {} // called instead if ignoreDataChanged was true

  IDataLinkClient() {m_link = NULL;}
  ~IDataLinkClient();
protected:
  virtual bool          AddDataLink(taDataLink* dl);
    // #IGNORE true if added, false if already set (usually a bug); overridden in Multi
  virtual bool          RemoveDataLink(taDataLink* dl);
    // #IGNORE true if removed, false if not (likely not a bug, just redunancy) overridden in Multi

  taDataLink*           m_link; // NOTE: will always be a taiDataLink or subclass
};

#endif // IDataLinkClient_h
