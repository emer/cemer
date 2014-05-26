// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "NetMonitor.h"
#include <DataTable>
#include <Network>
#include <Layer>
#include <Projection>

#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(NetMonitor);

void NetMonitor::Initialize() {
  rmv_orphan_cols = true;
}

void NetMonitor::InitLinks() {
  inherited::InitLinks();
  taBase::Own(items, this);
  taBase::Own(data, this);
  taBase::Own(network, this);
}

void NetMonitor::CutLinks() {
  data.CutLinks();
  network.CutLinks();
  items.CutLinks();
  inherited::CutLinks();
}

void NetMonitor::Copy_(const NetMonitor& cp) {
  items = cp.items;
  rmv_orphan_cols = cp.rmv_orphan_cols;
  data = cp.data; //warning: generates a UAE, but we ignore it
  network = cp.network;
}

void NetMonitor::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!data, quiet, rval, "data is NULL");
  CheckError(!network, quiet, rval,"network is NULL");
}

void NetMonitor::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  items.CheckConfig(quiet, rval);
}

void NetMonitor::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading || taMisc::is_duplicating) {
    prev_network = network;
    return;
  }
  UpdateNetworkPtrs();          // updates prev_network..
  UpdateDataTable();
}

String NetMonitor::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(data) rval += " to data: " + data->name;
  return rval;
}

NetMonItem* NetMonitor::AddNetwork(Network* net, const String& variable) {
  return AddObject(net, variable);
}

NetMonItem* NetMonitor::AddLayer(Layer* lay, const String& variable) {
  return AddObject(lay, variable);
}

NetMonItem* NetMonitor::AddProjection(Projection* prj, const String& variable) {
  return AddObject(prj, variable);
}

NetMonItem* NetMonitor::AddUnitGroup(Unit_Group* ug, const String& variable) {
  return AddObject(ug, variable);
}

NetMonItem* NetMonitor::AddUnit(Unit* un, const String& variable) {
  return AddObject(un, variable);
}

NetMonItem* NetMonitor::AddBlank() {
  return (NetMonItem*)items.New_gui(1);             // gui version
}

NetMonItem* NetMonitor::AddObject(taBase* obj, const String& variable) {
  // check for exact obj/variable already there, otherwise add one
  NetMonItem* nmi;
  for (int i = 0; i < items.size; ++i) {
    nmi = items.FastEl(i);
    if ((nmi->object.ptr() == obj) && (nmi->variable == variable))
      return nmi;
  }
  nmi = (NetMonItem*)items.New_gui(1, &TA_NetMonItem); // use gui to update
  nmi->SetMonVals(obj, variable);
  return nmi;
}

NetMonItem* NetMonitor::AddNetMax() {
  NetMonItem* nmi = AddObject(network, "avg_netin.max");
  nmi->var_label = "netmax";
  nmi->UpdateAfterEdit();
  return nmi;
}

NetMonItem* NetMonitor::AddNetRel() {
  NetMonItem* nmi = AddObject(network, "prjns.avg_netin_rel");
  nmi->var_label = "netrel";
  nmi->UpdateAfterEdit();
  return nmi;
}

NetMonItem* NetMonitor::AddLayActAvg() {
  NetMonItem* nmi = AddObject(network, "acts_m_avg.avg");
  nmi->var_label = "avg_act";
  nmi->UpdateAfterEdit();
  return nmi;
}

void NetMonitor::RemoveMonitors() {
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    nmi->ResetMonVals();
  }
}

void NetMonitor::SetDataTable(DataTable* dt) {
  if(data.ptr() == dt) return;
  data = dt; // note: auto does UAE
}

void NetMonitor::SetNetwork(Network* net) {
//   if(network.ptr() == net) return;
  network = net;
  UpdateNetworkPtrs();
}

void NetMonitor::UpdateNetworkPtrs() {
  if(network == prev_network) return; // already done
  if(!network) return;
  items.UpdatePointers_NewParType(&TA_Network, network);
  // this should now be redundant, right?
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    if (nmi->object_type) {
      if(nmi->object_type->InheritsFrom(&TA_Network) && nmi->object.ptr() != network) {
        nmi->object = network;
      }
    }
  }
  if(prev_network) {
    UpdateNetworkPtrs_NewPar(prev_network, network);
  }
  prev_network = network;       // update now that we've changed everything
}

void NetMonitor::UpdateNetworkPtrs_NewPar(taBase* old_net, taBase* new_net) {
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    if (nmi->object_type && nmi->object.ptr()) {
      taBase* new_guy = UpdatePointers_NewPar_FindNew(nmi->object.ptr(), old_net, new_net);
      if(new_guy && new_guy->InheritsFrom(nmi->object_type)) {
        nmi->object = new_guy;
      }
    }
  }
}

void NetMonitor::SetDataNetwork(DataTable* dt, Network* net) {
  SetNetwork(net);
  SetDataTable(dt);
}

void NetMonitor::UpdateDataTable(bool reset_first) {
  if (!data) return;
  data->StructUpdate(true);
  if(reset_first)
    data->ResetData();
  ScanAllObjects();
  CollectAllSpecs();
  all_specs.UpdateDataTableCols(data, rmv_orphan_cols);
  data->StructUpdate(false);
}

void NetMonitor::ScanAllObjects() {
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    if (!nmi->off)
      nmi->ScanObject();
  }
}

void NetMonitor::CollectAllSpecs() {
  all_specs.Reset();
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    nmi->CollectAllSpecs(this);
  }
}

void NetMonitor::GetMonVals() {
  if(TestError(!data, "GetMonVals", "data pointer not set!"))
    return;
  if(TestError(!data->WriteAvailable(), "GetMonVals", "Cannot write to data -- maybe need to set WriteItem to point to row to write to?"))
    return;
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    if (!nmi->off)
      nmi->GetMonVals(data);
  }
}

