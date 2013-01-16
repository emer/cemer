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

#include "taDataLink.h"
#include <taTaskThread>
#include <MemberDef>
#include <String_PArray>
#include <TypeDef>

#include <DataChangedReason>
#include <taMisc>

const KeyString taDataLink::key_name("name");

taDataLink::taDataLink(void* data_, taDataLink* &link_ref_)
{
  // save data items reference, and set us in it
  m_data = data_;
  m_link_ref = &link_ref_;
  link_ref_ = this;
  m_dbu_cnt = 0;
}

taDataLink::~taDataLink() {
  *m_link_ref = NULL; //note: m_link_ref is always valid, because the constructor passed it by reference
  if (clients.size > 0) {
    taMisc::DebugInfo("taDataLink::~taDataLink: clients.size not zero!");
  }
}

bool taDataLink::AddDataClient(IDataLinkClient* dlc) {
  if (!clients.AddUnique(dlc)) return false; // already added
  return dlc->AddDataLink(this);
}

void taDataLink::DataDestroying() { //note: linklist will automatically remove us
  IDataLinkClient* dlc;
  while (clients.size > 0) {
    //NOTE: client could destroy, so we have to remove it now
    dlc = clients.Pop();
    dlc->RemoveDataLink(this);
    //NOTE: client can still refer to us, but must do so through the ref we pass it
    dlc->DataLinkDestroying(this);
  }
}

void taDataLink::DoNotify(int dcr, void* op1_, void* op2_) {
#if !defined(NO_TA_BASE)
 // check for dispatch in a thread, which is Very Bad(TM)!!!
  if (!taTaskThread::inMainThread()) {
    taMisc::DebugInfo("A non-main thread has caused a taDataLink::DoNotify -- not allowed, notify will not be sent");
    return;
  }
#endif
  for (int i = 0; i < clients.size; ++i) {
    IDataLinkClient* dlc = clients.FastEl(i);
    if ((dcr == DCR_REBUILD_VIEWS) && !dlc->isDataView()) continue;
    if (dlc->ignoreDataChanged())
      dlc->IgnoredDataChanged(this, dcr, op1_, op2_);
    else
      dlc->DataDataChanged(this, dcr, op1_, op2_);
  }
}

// set this to emit debug messages for the following code..
// #define DATA_DATA_DEBUG 1

void taDataLink::DataDataChanged(int dcr, void* op1_, void* op2_) {
/*
  m_dbu_cnt = 0: idle state
  m_dbu_cnt < 0: in a DATA_UPDATE context
  m_dbu_cnt > 0: in a STRUCT_UPDATE context

  If we only ever issue DATA BEGIN/ENDs, we remain in DATA state;
  If we ever issue a STRUCT BEGIN, we get forced into STRUCT state,
    and any subsequent DATA commands get interpreted as STRUCT.

  We try to suppress unnecessary guys.
  If we start with STRUCT or DATA and do all the same (no different)
  then we only issue the first and last; but if DATA->STRUCT,
  then we also need to issue the STRUCT, however to maintain
  balance, we issue a semi-spurious DATA END, so we have equal
  numbers of + and -; NOTE: This situation is very unlikely to
  actually occur, since Struct and Data ops are typically mutually
  exclusive, and even then, the most likely is DATA ops nested inside
  a STRUCT update (not the other way around.)

    we need to send out all further STRUCT ops, and the final DATA one
*/
  bool send_iu = false; // set true if we should send a synthetic ITEM_UPDATED
  bool suppress = false; // set it if we should supress forwarding
  bool dummy_end = false;
  //we translate the NoDirty guy, since it is only for the sender's use
  if (dcr == DCR_ITEM_UPDATED_ND) {
    dcr = DCR_ITEM_UPDATED;
  }
  if (dcr == DCR_STRUCT_UPDATE_BEGIN) { // forces us to be in struct state
    // only forward the first one (ex some clients do a reset step)
    // OR the first one where DATA->STRUCT
    suppress = (m_dbu_cnt > 0); // send if first, or we were in DATA state
    if (m_dbu_cnt < 0) { // switch state if in DATA state
      m_dbu_cnt = -m_dbu_cnt;
      dummy_end = true;
    }
    ++m_dbu_cnt;
#ifdef DATA_DATA_DEBUG
    taMisc::Info((String)(int)this, "stru beg:", String(m_dbu_cnt));
#endif
  }
  else if (dcr == DCR_DATA_UPDATE_BEGIN) {
    suppress = (m_dbu_cnt != 0);
    if (m_dbu_cnt > 0) ++m_dbu_cnt; // stay in STRUCT state if STRUCT state
    else               --m_dbu_cnt;
#ifdef DATA_DATA_DEBUG
    taMisc::Info((String)(int)this, "data beg:", String(m_dbu_cnt));
#endif
  }
  else if ((dcr == DCR_STRUCT_UPDATE_END) || (dcr == DCR_DATA_UPDATE_END)) {
#ifdef DATA_DATA_DEBUG
    bool was_stru = false;      // debug only
    if(dcr == DCR_STRUCT_UPDATE_END)
      was_stru = true;
#endif
    if (m_dbu_cnt < 0) {
      ++m_dbu_cnt;
    } else if (m_dbu_cnt > 0) {
      --m_dbu_cnt;
      dcr = DCR_STRUCT_UPDATE_END; // force to be struct end, in case we notify
    }
    // this situation might theoretically arise if some updating action
    // mid-update causes a link to an item to get created, which will then
    // not have been tracking all the BEGINs -- but it should be safe
    // to just keep letting them happen, doing gui updates, which should be harmless
#ifdef DEBUG // just make sure these are harmless, and don't scare users...
    else {
      taMisc::Warning("Datalink for object name:", GetName(),
                      "unexpectedly received a DATA or STRUCT END (cnt was 0)");
    }
#endif
#ifdef DATA_DATA_DEBUG
    if(was_stru)
      taMisc::Info((String)(int)this, "stru end:", String(m_dbu_cnt));
    else
      taMisc::Info((String)(int)this, "data end:", String(m_dbu_cnt));
#endif
    // at the end, also send a IU
    if (m_dbu_cnt == 0) {
      if (dcr == DCR_DATA_UPDATE_END) { // just turn it into an IU
        //NOTE: clients who count (ex taDataView) must detect this implicit
        // DATA_UPDATE_END as occurring when:
        // State=DATA, Count=1
        dcr = DCR_ITEM_UPDATED;
#ifdef DATA_DATA_DEBUG
        taMisc::Info((String)(int)this, "cvt to iu:", String(m_dbu_cnt));
#endif
      }
      else {// otherwise, we send both
        send_iu = true;
      }
    } else suppress = true;
  }
  else if (dcr == DCR_ITEM_UPDATED) {
    // if we are already updating, then ignore IUs, since we'll send one eventually
    if (m_dbu_cnt != 0) suppress = true;
  }
  else if (dcr < DCR_UPDATE_VIEWS) {
    // if we are already updating, then ignore IUs, since we'll send one eventually
    if (m_dbu_cnt != 0) suppress = true;
  }

  if (!suppress) {
#ifdef DATA_DATA_DEBUG
    taMisc::Info((String)(int)this, "sending:", String(dcr));
#endif
    DoNotify(dcr, op1_, op2_);
  }
  if (dummy_end)
    DoNotify(DCR_DATA_UPDATE_END, NULL, NULL);
  if (send_iu)
    DoNotify(DCR_ITEM_UPDATED, NULL, NULL);
}

String taDataLink::GetDisplayName() const {
  MemberDef* md = GetDataMemberDef();
  if (md) return md->name;
  else    return GetName();
}

TypeDef* taDataLink::GetTypeDef() const {
#if !defined(TA_NO_GUI) && !defined(NO_TA_BASE)
  return &TA_taDataLink;
#else
  return NULL;
#endif
}

bool taDataLink::RemoveDataClient(IDataLinkClient* dlc) {
  //WARNING: dlc calls this in its destructor, therefore 'dlc' is IDataLinkClient virtual
  // version, therefore, RemoveDataClient may NOT use any IDataLinkClient virtual methods
  // that are intended to be overloaded
  // exception: IMultiDataLinkClient calls from its own destructor, so its virtuals are ok
  dlc->RemoveDataLink(this);
  return clients.RemoveEl(dlc);
}

String& taDataLink::ListClients(String& strm, int indent) const {
  String_PArray nms;
  nms.Alloc(clients.size);
  for(int i=0; i<clients.size; i++) {
    IDataLinkClient* dlc = clients.FastEl(i);
    TypeDef* dlc_typ = dlc->GetTypeDef();
    if (dlc_typ) {
      nms.Add(dlc_typ->name);
    }
  }
  taMisc::FancyPrintList(strm, nms, indent+1);
  taMisc::IndentString(strm, indent);
  return strm;
}
