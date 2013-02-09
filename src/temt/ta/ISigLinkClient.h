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

#ifndef ISigLinkClient_h
#define ISigLinkClient_h 1

// parent includes:
#include <ISigLinkProxy>

// member includes:

// declare all other types mentioned but not required to include:
class taiSigLink; //
class taSigLink;  //

/* ****WARNING****
  Do NOT put logging (ie calls to taMisc::Warning, etc.) in any data link mgt
  code because it can get routed through the pager or other buffering mechanism
  which may trigger event loop calls, which can cause deletions, mid-routine
*/

// Mixin interface for Node that uses siglinks, ex. tree node or Inventor node


taTypeDef_Of(ISigLinkClient);

class TA_API ISigLinkClient : public virtual ISigLinkProxy {
  //#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE this is the interface available to the host data object
friend class taSigLink;
public:
#ifndef TA_NO_GUI
  inline taiSigLink*   link() const {return (taiSigLink*)m_link;}
#else
  inline taSigLink*    link() const {return m_link;}
#endif
  inline taSigLink*    link_() const {return m_link;}
  virtual bool          ignoreSigEmit() const {return false;}
    // hidden guys can ignore changes (but they always get Destroyed)
  virtual bool          isDataView() const {return false;} // true for dataviews
  virtual void          SigLinkDestroying(taSigLink* dl) = 0; // called by SigLink when destroying; it will remove siglink ref in dlc upon return
  virtual void          SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) = 0;
  // this is how we receive a signal from the siglink
  virtual void          IgnoredSigEmit(taSigLink* dl, int sls,
    void* op1, void* op2) {} // called instead if ignoreSigEmit was true

  ISigLinkClient() {m_link = NULL;}
  ~ISigLinkClient();
protected:
  virtual bool          AddSigLink(taSigLink* dl);
    // #IGNORE true if added, false if already set (usually a bug); overridden in Multi
  virtual bool          RemoveSigLink(taSigLink* dl);
    // #IGNORE true if removed, false if not (likely not a bug, just redunancy) overridden in Multi

  taSigLink*           m_link; // NOTE: will always be a taiSigLink or subclass
};

#endif // ISigLinkClient_h
