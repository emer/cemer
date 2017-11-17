// Copyright 2017, Regents of the University of Colorado,
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

#include "NetMonItem.h"
#include <Network>
#include <Projection>
#include <Layer>
#include <MemberDef>
#include <UserDataItemBase>
#include <taMath_float>
#include <taMath_double>
#include <DataSelectSpec>
#include <taDataProc>
#include <DataTable>
#include <NetMonitor>
#include <Average>
#include <Completions>

#include <taMisc>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(NetMonItem);

taTypeDef_Of(UserDataItem);

void NetMonItem::Initialize() {
  off = false;
  error = false;
  computed = false;
  object_type = NULL;
  monitor = NULL;
  variable = "act";
  name_style = AUTO_NAME;
  max_name_len = 6;
  options = NO_INPUT;
  agg.op = Aggregate::NONE;
  data_agg = false;
  select_rows = false;
  val_type = VT_FLOAT;
  matrix = false;
  cell_num  = 0;
}

void NetMonItem::InitLinks() {
  inherited::InitLinks();
  taBase::Own(val_specs,this);
  taBase::Own(agg_specs,this);
  taBase::Own(ptrs,this);
  taBase::Own(agg,this);
  taBase::Own(pre_proc_1,this);
  taBase::Own(pre_proc_2,this);
  taBase::Own(pre_proc_3,this);
  taBase::Own(object, this);
  taBase::Own(matrix_geom, this);
  taBase::Own(data_src,this);
  taBase::Own(agg_col,this);
  taBase::Own(select_spec,this);
}

void NetMonItem::CutLinks() {
  agg_col.CutLinks();
  select_spec.CutLinks();
  data_src.CutLinks();
  pre_proc_3.CutLinks();
  pre_proc_2.CutLinks();
  pre_proc_1.CutLinks();
  agg.CutLinks();
  ResetMonVals();
  val_specs.CutLinks();
  agg_specs.CutLinks();
  object.CutLinks();
  object_type = NULL;
  monitor = NULL;
  inherited::CutLinks();
}

void NetMonItem::Copy_(const NetMonItem& cp) {
  ResetMonVals(); // won't be valid anymore
  off = cp.off;
  computed = cp.computed;
  object_type = cp.object_type;
  object = cp.object; // ptr only
  variable = cp.variable;
  var_label = cp.var_label;
  name_style = cp.name_style;
  max_name_len = cp.max_name_len;
  options = cp.options;
  val_type = cp.val_type;
  matrix = cp.matrix;
  matrix_geom = cp.matrix_geom;
  data_agg = cp.data_agg;
  data_src = cp.data_src;
  agg_col = cp.agg_col;
  select_rows = cp.select_rows;
  select_spec = cp.select_spec;
  agg = cp.agg;
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
  
  if (!monitor) {
    NetMonItem_List* monitor_list = dynamic_cast<NetMonItem_List*>(this->owner);
    if (monitor_list) {  // if it was a "copy here" the owner won't be a NetMonItem_List
      monitor = dynamic_cast<NetMonitor*>(monitor_list->owner);
    }
  }
}

void NetMonItem::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(off) return;

  CheckError(error, quiet, rval, "NetMonItem named:", name, "has an error:",
             error_msg);

  if(!computed) {
    CheckError(!owner, quiet, rval, "NetMonItem named:", name, "has no owner");
    CheckError(!object, quiet, rval, "object is NULL");
    CheckError(variable.empty(), quiet, rval,"variable is empty");
  }
  else {
    if(data_agg) {
      bool bad = false;
      if(CheckError(!data_src, quiet, rval, "data_src is NULL")) bad = true;
      if(CheckError(agg_col.col_name.empty(), quiet, rval, "agg_col.col_name is empty")) bad = true;
      if(!bad) {
        DataCol* dc = data_src->FindColName(agg_col.col_name);
        if(!CheckError(!dc, quiet, rval, "agg_col col_name:", agg_col.col_name,
                       "not found in data table:", data_src->name)) {
          CheckError(!dc->isNumeric() || (dc->valType() == taBase::VT_BYTE), quiet, rval,
                     "agg_col col_name:", agg_col.col_name,
                     "is not a proper numeric type (float, double, int)");

          if(dc->isMatrix() && matrix) {
            CheckError(dc->cell_size() != matrix_geom.Product(), quiet, rval,
                       "geometry of data_src agg source column:", dc->name,
                       "is not same as mon destination matrix");
          }
        }
        if(select_rows) {
          DataCol* sc = data_src->FindColName(select_spec.col_name);
          CheckError(!sc, quiet, rval, "select_spec column:", agg_col.col_name,
                     "not found in data table:", data_src->name);
        }
      }
    }
  }
}

String NetMonItem::GetAutoName(const String& obj_nm) {
  String rval;
  if(agg.op != Aggregate::NONE) {
    rval = agg.GetAggName() + "_";
    rval.downcase();
  }
  if(obj_nm.nonempty()) {
    rval += obj_nm + "_";
  }
  rval += (var_label.empty() ? variable : var_label);
  return rval;
}

void NetMonItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (off) {
    return; // all stuff will run once turned on...
  }
  
  // sets for old projects that didn't have this member
  if (!monitor) {
    NetMonItem_List* monitor_list = dynamic_cast<NetMonItem_List*>(this->owner);
    if (monitor_list) {
      monitor = dynamic_cast<NetMonitor*>(monitor_list->owner);
    }
  }
  
  if(computed) {
    name_style = MY_NAME;
    object = NULL;              // never have an obj for computed guy
    if(data_agg) {
      agg_col.SetDataTable(data_src);
      select_spec.SetDataTable(data_src);
      if(agg_col.col_name.nonempty() && (name.empty() || name.contains("NetMonItem"))) {
        SetName(agg_col.col_name); // use this as a default..
      }
    }
    else {
      data_src = NULL;
      agg_col.SetDataTable(NULL);
      select_rows = false;
      select_spec.SetDataTable(NULL);
    }
  }
  else {
    data_agg = false;           // make sure not displayed
    select_rows = false;
    data_src = NULL;
  }

  if(!owner) return;
  if(!object) return;
  //  object_type = object->GetTypeDef(); // not a good idea -- prevents changing !!
  if(variable.empty()) return;

  if(variable.contains("ungp_data")) {
    variable.gsub("ungp_data", "ungp");
    taMisc::Info("updated variable containing 'ungp_data' to 'ungp' to access new UnGpState variables");
  }
  if(variable.contains("ct_cycle")) {
    variable.gsub("ct_cycle", "cycle");
    taMisc::Info("updated variable containing 'ct_cycle' to new name: 'cycle'");
  }
  if(variable.contains("minus_cycles")) {
    variable.gsub("minus_cycles", "rt_cycles");
    taMisc::Info("updated variable containing 'minus_cycles' to new name: 'rt_cycles'");
  }
  if(variable.contains("phase_no")) {
    variable.gsub("phase_no", "quarter");
    taMisc::Info("updated variable containing 'phase_no' to new name: 'quarter'");
  }
  
  if (!taMisc::is_loading) {
    if(name_style == MY_NAME) {
      if(!computed) {
        if(name.empty()) {
          SetName(GetAutoName(GetObjName(object)));
        }
      }
    }
    else {                      // AUTO_NAME = always update!
      SetName(GetAutoName(GetObjName(object)));
    }
    name = taMisc::StringCVar(name);            // keep it clean for css var names
    ScanObject();
  }
}

void NetMonItem::UpdateAfterMove_impl(taBase* old_owner) {
  inherited::UpdateAfterMove_impl(old_owner);
  NetMonItem_List* old_monitor_list = dynamic_cast<NetMonItem_List*>(old_owner);
  NetMonItem_List* new_monitor_list = dynamic_cast<NetMonItem_List*>(owner);
  if (!new_monitor_list) {  // if it was a "move here" the owner won't be a NetMonItem_List - don't proceed
    return;
  }
  NetMonitor* old_monitor = dynamic_cast<NetMonitor*>(old_monitor_list->owner);
  NetMonitor* new_monitor = dynamic_cast<NetMonitor*>(new_monitor_list->owner);
  Network* old_network = old_monitor->network;
  Network* new_network = new_monitor->network;
  if (old_network != new_network) {
    if (object_type && object.ptr()) {
      taBase* new_guy = UpdatePointers_NewPar_FindNew(object.ptr(), old_network, new_network);
      if(new_guy && new_guy->InheritsFrom(object_type)) {
        object = new_guy;
      }
    }
  }
}

// similar to code in NetMonitor for when the network is changed at the monitor level
void NetMonItem::UpdatePointersAfterCopy_impl(const taBase& cp) {
  inherited::UpdatePointersAfterCopy_impl(cp);
  const NetMonItem* src_mon_item = dynamic_cast<const NetMonItem*>(&cp);
  if (object_type && object.ptr() && monitor != NULL && src_mon_item->monitor != NULL) {
    taBase* new_guy = UpdatePointers_NewPar_FindNew(object.ptr(), src_mon_item->monitor->network, monitor->network);
    if(new_guy && new_guy->InheritsFrom(object_type)) {
      object = new_guy;
    }
  }
}

String NetMonItem::GetObjName(taBase* obj) {
  if (!obj) return _nilString;

  // cases where default name is not what we want:
  if(obj->InheritsFrom(TA_Projection)) {
    Projection* prjn = (Projection*)obj;
    if(prjn->layer) {
      return taMisc::StringMaxLen(prjn->layer->name, max_name_len) + "_" + prjn->name;
    }
  }
  else if(obj->InheritsFrom(&TA_Network)) { // special case
    return _nilString;                      // just use the var
  }

  // go with the default name (display name takes care of lots of the logic already)
  String nm = obj->GetDisplayName();
  if(nm.contains('.')) {
    nm = taMisc::StringMaxLen(nm.before('.'), max_name_len) + "_" +
      taMisc::StringMaxLen(nm.after('.'), max_name_len);
  }
  else if(nm.contains(' ')) {
    nm = taMisc::StringMaxLen(nm.before(' '), max_name_len);
  }
  else {
    nm = taMisc::StringMaxLen(nm, max_name_len);
  }
  return nm;
}

String NetMonItem::GetColName(taBase* obj, int col_idx, String obj_nm) {
  if(obj && obj_nm.empty())
    obj_nm = GetObjName(obj);
  String rval;
  if(name_style == MY_NAME) {
    if(col_idx == 0)
      rval = name;
    else
      rval = name + "_" + String(col_idx);
    rval = taMisc::StringCVar(rval); // keep it clean for css var names
    TestWarning(val_specs.FindNameIdx(rval) >= 0, "NetMonItem::GetColName",
                   "Monitor item with custom name:", name,
                "conflicts with another monitor item of the same name -- please rename one of them!");
  }
  else {
    rval = GetAutoName(obj_nm);
    rval = taMisc::StringCVar(rval); // keep it clean for css var names
    if(obj) {                        // if we have an object we can try to make an unambig name
      if(val_specs.FindNameIdx(rval) >= 0) {   // NOTE: N^2 kind of thing, but done infrequently so ok..
        for(max_name_len++; max_name_len < 50; max_name_len++) { // hard coded max here..
          obj_nm = GetObjName(obj);
          rval = GetAutoName(obj_nm);
          rval = taMisc::StringCVar(rval); // keep it clean for css var names
          if(val_specs.FindNameIdx(rval) < 0) {
            break;                // safe!
          }
        }
        TestWarning(max_name_len >= 50, "NetMonItem::GetColName",
                    "Monitor item:",name,"with auto-name of:",rval,
                    "is conflicting with another item of the same name, despite an attempt to increase the max_name_len up to 50 -- either manually incease further or fix underlying naming conflict");
      }
    }
  }
  return rval;
}

DataColSpec* NetMonItem::AddMatrixCol(const String& valname, ValType vt,
                                      const MatrixGeom* geom)
{
  cell_num = 0;
  DataColSpec* cs;
  if(!computed && (agg.op != Aggregate::NONE)) {
    AddScalarCol(valname, vt);
    cs = (DataColSpec*)agg_specs.New(1, &TA_DataColSpec); // add to agg_specs!
  }
  else {
    // usual..
    cs = (DataColSpec*)val_specs.New(1, &TA_DataColSpec);
  }
  cs->SetName(valname);
  cs->val_type = vt;
  if (geom) {
    cs->SetCellGeomN(*geom);
  }
  return cs;
}

DataColSpec* NetMonItem::AddScalarCol(const String& valname, ValType vt) {
  cell_num = 0;//maybe should be 1!
  DataColSpec* cs = (DataColSpec*)val_specs.New(1, &TA_DataColSpec);
  cs->SetName(valname);
  cs->val_type = vt;
  return cs;
}

DataColSpec* NetMonItem::AddScalarCol_Agg(const String& valname, ValType vt) {
  cell_num = 0;//maybe should be 1!
  DataColSpec* cs = (DataColSpec*)agg_specs.New(1, &TA_DataColSpec);
  cs->SetName(valname);
  cs->val_type = vt;
  return cs;
}

const KeyString  NetMonItem::key_obj_name("obj_name");
const KeyString  NetMonItem::key_obj_type("obj_type");
const KeyString  NetMonItem::key_obj_var("obj_var");

String NetMonItem::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_obj_name) return (object) ? object->GetName() : _nilString;
  else if (key == key_obj_type) return (object) ? object->GetTypeDef()->name : _nilString;
  else if (key == key_obj_var) return variable;
  else return inherited::GetColText(key, itm_idx);
}

void NetMonItem::ResetMonVals() {
  val_specs.RemoveAll();
  agg_specs.RemoveAll();
  ptrs.Reset();
  members.RemoveAll();
}

bool NetMonItem::MonError(bool test, const String& fun_name,
                          const String& a, const String& b, const String& c, const String& d,
                          const String& e, const String& f, const String& g, const String& h) {
  if(!test) return false;
  error = true;
  error_msg = taMisc::SuperCat(fun_name, a, b, c, d, e, f, g, h);
  return true;
}

void NetMonItem::ScanObject() {
  error = false;
  error_msg = "";
  ResetMonVals();
  if(computed) {
    if(matrix)
      AddMatrixCol(name, val_type, &matrix_geom);
    else
      AddScalarCol(name, val_type);
    return;
  }

  if (!object) return;

  if (object->InheritsFrom(&TA_Layer)) {
    ScanObject_Layer((Layer*)object.ptr(), variable);
  }
  else if (object->InheritsFrom(&TA_Projection)) {
    ScanObject_Projection((Projection*)object.ptr(), variable);
  }
  else if (object->InheritsFrom(&TA_Projection_Group)) {
    ScanObject_ProjectionGroup((Projection_Group*)object.ptr(), variable);
  }
  else if (object->InheritsFrom(&TA_Network)) {
    ScanObject_Network((Network*)object.ptr(), variable);
  }
  else {
    // could be any type of object.ptr()
    ScanObject_InObject(object.ptr(), variable, object.ptr());
  }
}

bool NetMonItem::ScanObject_InUserData(taBase* obj, String var,
  taBase* name_obj)
{
  String key;
  if (var.contains('.')) {
    // udi should be a complex container
    key = var.before('.');
    var = var.after(".");
  }
  else {
    // udi should be a simple value
    key = var;
    var = _nilString;
  }
  UserDataItemBase* udi = obj->GetUserDataItem(key);
  if(MonError(!udi,"ScanObject_InUserData",
                "UserDataItem: ", key, " not found"))
    return true; //no mon, but we did handle it

  // note: we test for UserDataItem type, not isSimple because
  // we need to link to the value member
  if (udi->InheritsFrom(&TA_UserDataItem)) {
    if(MonError(var.nonempty(),"ScanObject_InUserData",
      "UserDataItem: ", key, " expected to be simple; can't resolve remaining var: ", var))
      return true; //no mon, but we did handle it
    MemberDef* md = udi->FindMemberName("value"); // should exist!
    if(MonError(!md,"ScanObject_InUserData",
      "unexpected: member 'value' supposed to exist"))
      return true; //no mon, but we did handle it
    if (name_obj) {
      String valname = GetColName(name_obj, val_specs.size);
      if(udi->valueAsVariant().type() == Variant::T_Int) {
	AddScalarCol(valname, VT_INT);
	if(agg.op != Aggregate::NONE) {
	  AddScalarCol_Agg(valname, VT_FLOAT); // add the agg guy just to keep it consistent
	}
      }
      else if(udi->valueAsVariant().isNumeric()) { // use float for all other numeric
	AddScalarCol(valname, VT_FLOAT);
	if(agg.op != Aggregate::NONE) {
	  AddScalarCol_Agg(valname, VT_FLOAT); // add the agg guy just to keep it consistent
	}
      }
      else {
	AddScalarCol(valname, VT_VARIANT);
	if(agg.op != Aggregate::NONE) {
	  AddScalarCol_Agg(valname, VT_VARIANT); // add the agg guy just to keep it consistent
	}
      }
    }
    // if not adding a column, it is part of a pre-allocated matrix; just add vars
    ptrs.Add(udi);
    members.Link(md);
    return true;
  }
  else {
    if(MonError(var.empty(),"ScanObject_InUserData",
      "UserDataItem: ", key, " expected to be non-simple; additional .xxx member resolution required after item key"))
      return true; //no mon, but we did handle it
    // descend...
    return ScanObject_InObject(udi, var, name_obj);
  }
  return false; // compiler food
}

bool NetMonItem::ScanObject_InObject(taBase* obj, String var, taBase* name_obj, bool err_not_found) {
  if (!obj) return false;
  MemberDef* md = NULL;

  // first, try the recursive end, look for terminal member in ourself
  if(var.startsWith('.'))
    var = var.after('.');
  if (var.contains('.')) {
    if(var.startsWith("user_data.")) {
      return ScanObject_InUserData(obj, var.after("."), name_obj);
    }

    String path = var.before('.',-1);
    taBase* mbr = obj->FindFromPath(path, md);
    if(!mbr) {
      if(MonError(err_not_found, "ScanObject_InObject",
                   "path to variable not found in parent object, path:",
                   path, "var: ", var, "parent:", obj->DisplayPath())) {
        return true; //no mon, but we did handle it
      }
      return false;             // silent fail bounces back
    }

    String membname = var.after('.',-1);
    return ScanObject_InObject(mbr, membname, name_obj);
  }
  else {
    md = obj->FindMemberName(var);
    if (md) {
      // special case for Average objects -- they automatically get their avg member
      if(md->type->IsActualTaBase() && md->type->InheritsFrom(&TA_Average) &&
         !md->type->IsAnyPtr()) {
        Average* ths = (Average*) md->GetOff((void*)obj);
        if (ths) {
          // the line below was finding the wrong FindMember method - maybe something to do with ta and overloaded methods - specific to Windows
          md = ths->FindMemberName("avg");
          obj = ths;              // obj is now this..
        }
      }
      if(md && name_obj) {
        String valname = GetColName(name_obj, val_specs.size);
        ValType vt = ValTypeForType(md->type);
        AddScalarCol(valname, vt);
        if(agg.op != Aggregate::NONE) {
          AddScalarCol_Agg(valname, vt); // add the agg guy just to keep it consistent
        }
      }
      // if not adding a column, it is part of a pre-allocated matrix; just add vars
      ptrs.Add(obj);
      members.Link(md);
      return true;
    }
  }
  return false;
}

bool NetMonItem::ScanObject_InNonTAObject(void* obj, TypeDef* typ, String var, taBase* name_obj,
                                          bool err_not_found) {
  if (!obj) return false;

  // first, try the recursive end, look for terminal member in ourself
  if(var.startsWith('.'))
    var = var.after('.');

  TypeDef* own_td = typ;
  int net_base_off = 0;
  ta_memb_ptr net_mbr_off;
  MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off, var, false);
  if(!md) {
    MonError(err_not_found, "ScanObject_InNonTAObject", 
             "Variable not found:", var, "in object of type:", typ->name);
    return false;
  }
  if(md && name_obj) {
    String valname = GetColName(name_obj, val_specs.size);
    ValType vt = ValTypeForType(md->type);
    AddScalarCol(valname, vt);
    if(agg.op != Aggregate::NONE) {
      AddScalarCol_Agg(valname, vt); // add the agg guy just to keep it consistent
    }
  }
  // if not adding a column, it is part of a pre-allocated matrix; just add vars
  ptrs.Add(obj);
  members.Link(md);
  return true;
}

void NetMonItem::ScanObject_Network(Network* net, String var) {
  if(var.startsWith("layers.") || var.startsWith(".layers.")) {
    var = var.after("layers.");
    ScanObject_Network_Layers(net, var);
    return;
  }
  if(var.startsWith("prjns.") || var.startsWith(".prjns.")) {
    ScanObject_Network_Layers(net, var);
    return;
  }
  if(var.startsWith("projections.") || var.startsWith(".projections.")) {
    ScanObject_Network_Layers(net, var);
    return;
  }

  if (ScanObject_InObject(net, var, net, false)) return; // false = test

  ScanObject_Network_Layers(net, var);
}

void NetMonItem::ScanObject_Network_Layers(Network* net, String var) {
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if(lay->lesioned()) continue;
    switch(lay->layer_type) {
    case Layer::INPUT:
      if(!HasMonOption(INPUT_LAYERS)) continue;
      break;
    case Layer::HIDDEN:
      if(!HasMonOption(HIDDEN_LAYERS)) continue;
      break;
    case Layer::OUTPUT:
      if(!HasMonOption(OUTPUT_LAYERS)) continue;
      break;
    case Layer::TARGET:
      if(!HasMonOption(TARGET_LAYERS)) continue;
      break;
    }
    ScanObject_Layer(lay, var);
  }
}

void NetMonItem::ScanObject_Layer(Layer* lay, String var) {
  // check for projection monitor
  if(var.startsWith('.'))
    var = var.after('.');
  if(var.contains('.')) {
    if(var.startsWith("units[")) {
      ScanObject_LayerUnits(lay, var);
      return;
    }
    if(var.startsWith("ungp[")) {
      ScanObject_LayerUnGp(lay, var);
      return;
    }

    String subvar = var.before('.');
    if(subvar.endsWith("projections") || subvar.endsWith("prjns")) {
      ScanObject_ProjectionGroup(&lay->projections, var.after('.'));
      return;
    }
    if((subvar == "r") || (subvar == "s")) {
      ScanObject_LayerCons(lay, var);
      return;
    }
  }

  if (ScanObject_InObject(lay, var, lay, false)) return; // false = just test, no error
  if(!lay->own_net) return;

  bool on_unit = CheckVarOnUnit(var, lay->own_net);
  if(MonError(!on_unit, "ScanObject_Layer", "variable not found on layer or unit, and is not r.* or s.* connection variable:", var)) {
    return;
  }

  // go get unit vars
  NetworkState_cpp* net_state = lay->GetValidNetState();
  if(!net_state) return;
  
  MatrixGeom geom;
  if(lay->unit_groups) {
    if(lay->gp_geom.n_not_xy || lay->un_geom.n_not_xy)
      geom.SetGeom(1, lay->n_units);       // irregular: flatten!
    else
      geom.SetGeom(4, lay->un_geom.x, lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
  }
  else {
    if(lay->un_geom.n_not_xy)
      geom.SetGeom(1, lay->n_units);       // irregular: flatten!
    else
      geom.SetGeom(2, lay->un_geom.x, lay->un_geom.y);
  }
  String valname = GetColName(lay, val_specs.size);
  AddMatrixCol(valname, VT_FLOAT, &geom);
  String un_obj_nm = GetObjName(lay);
  for (int i = 0; i < lay->n_units_built; ++i) {
    UnitState_cpp* un = lay->GetUnitState(net_state, i);
    if(un) {
      ScanObject_Unit(un, var, un_obj_nm + "[" + String(i) + "]", net_state);
    }
  }
}

void NetMonItem::ScanObject_LayerUnits(Layer* lay, String var) {
  NetworkState_cpp* net_state = lay->GetValidNetState();
  if(!net_state) return;
  
  String range2;
  String range1 = var.between('[', ']');
  String rmdr = var.after(']');
  if(rmdr.contains('[')) {
    range2 = rmdr.between('[', ']');
    rmdr = rmdr.after(']');
  }
  if(rmdr.startsWith('.')) rmdr = rmdr.after('.');

  bool on_unit = CheckVarOnUnit(rmdr, lay->own_net);
  if(MonError(!on_unit, "ScanObject_Layer", "variable not found on layer or unit, and is not r.* or s.* connection variable:", rmdr)) {
    return;
  }
  
  String valname = GetColName(lay, val_specs.size);
  MatrixGeom geom;

  String un_obj_nm = GetObjName(lay);

  String un_range;
  String gp_range;
  if(range2.nonempty()) {
    un_range = range2;
    gp_range = range1;
  }
  else {
    un_range = range1;
  }
  
  int unidx1 = 0; 
  int unidx2 = 0; 
  if(un_range.contains('-')) {
    unidx1 = (int)un_range.before('-');
    unidx2 = (int)un_range.after('-');
    if(unidx2 < 0)
      unidx2 = lay->un_geom_n-1;
  }
  else {
    unidx1 = (int)un_range;
    unidx2 = unidx1;
  }

  int gpidx1 = 0; 
  int gpidx2 = 0; 
  if(gp_range.contains('-')) {
    gpidx1 = (int)gp_range.before('-');
    gpidx2 = (int)gp_range.after('-');
    if(gpidx2 < 0)
      gpidx2 = lay->n_ungps-1;
  }
  else {
    gpidx1 = (int)gp_range;
    gpidx2 = gpidx1;
  }
  
  gpidx1 = MIN(gpidx1, lay->n_ungps-1);
  gpidx2 = MIN(gpidx2, lay->n_ungps-1);
  int ngp = 1+gpidx2-gpidx1;
  unidx1 = MIN(unidx1, lay->un_geom_n-1);
  unidx2 = MIN(unidx2, lay->un_geom_n-1);
  int nun = 1+unidx2-unidx1;
  
  
  if(gp_range.nonempty()) { // group case
    if(ngp > 1 && nun > 1) {
      geom.SetGeom(2, nun, ngp);
      AddMatrixCol(valname, VT_FLOAT, &geom);
    }
    else if(ngp > 1) {
      geom.SetGeom(1, ngp);
      AddMatrixCol(valname, VT_FLOAT, &geom);
    }
    else if(nun > 1) {
      geom.SetGeom(1, nun);
      AddMatrixCol(valname, VT_FLOAT, &geom);
    }
    else {
      AddScalarCol(valname, VT_FLOAT);
    }
    for (int gi = gpidx1; gi <= gpidx2; ++gi) {
      for (int i = unidx1; i <= unidx2; ++i) {
        UnitState_cpp* unit = lay->GetUnitStateGpUnIdx(net_state, gi, i);
        String un_nm = un_obj_nm + "[" + String(gi) + "][" + String(i) + "]";
        ScanObject_Unit(unit, rmdr, un_nm, net_state);
      }
    }
  }
  else {                        // just unit idxs
    if(nun > 1) {
      geom.SetGeom(1, nun);
      AddMatrixCol(valname, VT_FLOAT, &geom);
    }
    else {
      AddScalarCol(valname, VT_FLOAT);
    }
    for (int i = unidx1; i <= unidx2; ++i) {
      UnitState_cpp* unit = lay->GetUnitState(net_state, i);
      String un_nm = un_obj_nm + "[" + String(i) + "]";
      ScanObject_Unit(unit, rmdr, un_nm, net_state);
    }
  }
}

void NetMonItem::ScanObject_LayerUnGp(Layer* lay, String var) {
  NetworkState_cpp* net_state = lay->GetValidNetState();
  if(!net_state) return;
  
  TypeDef* ungp_typ = lay->own_net->UnGpStateType();
  
  String range = var.between('[', ']');
  String rmdr = var.after(']');
  if(rmdr.startsWith('.')) rmdr = rmdr.after('.');

  String valname = GetColName(lay, val_specs.size);
  MatrixGeom geom;

  int gpidx1 = 0; 
  int gpidx2 = 0; 
  if(range.contains('-')) {
    gpidx1 = (int)range.before('-');
    gpidx2 = (int)range.after('-');
    if(gpidx2 < 0)              // -1 is layer un gp but not in a range
      gpidx2 = lay->n_ungps-1; 
  }
  else {
    gpidx1 = (int)range;
    gpidx2 = gpidx1;
  }

  gpidx1 = MIN(gpidx1, lay->n_ungps-1);
  gpidx2 = MIN(gpidx2, lay->n_ungps-1);
  int ngp = 1+gpidx2-gpidx1;

  if(ngp > 1) {
    geom.SetGeom(1, ngp);
    AddMatrixCol(valname, VT_FLOAT, &geom);
  }
  else {
    AddScalarCol(valname, VT_FLOAT);
  }
  for (int i = gpidx1; i <= gpidx2; ++i) {
    UnGpState_cpp* ungp = lay->GetUnGpState(net_state, i);
    ScanObject_InNonTAObject(ungp, ungp_typ, rmdr, NULL, true); // true = err not found
  }
}

void NetMonItem::ScanObject_LayerCons(Layer* lay, String var) {
  if(!lay->own_net->IsBuiltIntact()) // no-can-do
    return;
  String subvar = var.before('.');
  if(subvar == "r") {
    for(int i=0;i<lay->projections.size; i++) {
      Projection* prjn = lay->projections[i];
      if(prjn->MainNotActive()) continue;
      ScanObject_PrjnCons(prjn, var);
    }
  }
  else {                        // must be s
    for(int i=0;i<lay->send_prjns.size; i++) {
      Projection* prjn = lay->send_prjns[i];
      if(prjn->MainNotActive()) continue;
      ScanObject_PrjnCons(prjn, var);
    }
  }
}

void NetMonItem::ScanObject_PrjnCons(Projection* prj, String var) {
  if(prj->MainNotActive()) return;
  Layer* lay = NULL;
  String subvar = var.before('.');
  bool recv = true;
  if(subvar == "r") lay = prj->layer;
  else { lay = prj->from; recv = false; }
  String convar = var.after('.');
  MemberDef* con_md = prj->con_type->members.FindNameR(convar);
  if(!con_md) return;           // can't find that var!
  NetworkState_cpp* net = lay->GetValidNetState();
  if(!net) return;

  PrjnState_cpp* prjn = prj->GetPrjnState(net);
  
  // always create a 4dimensional matrix: 1st 2 are units, 2nd 2 are cons
  taVector2i lay_geom;
  if(lay->unit_groups) {
    if(lay->gp_geom.n_not_xy || lay->un_geom.n_not_xy) {
      lay_geom.x = lay->n_units;
      lay_geom.y = 1;
    }
    else {
      lay_geom = lay->un_geom * lay->gp_geom;
    }
  }
  else {
    if(lay->un_geom.n_not_xy) {
      lay_geom.x = lay->n_units;
      lay_geom.y = 1;
    }
    else {
      lay_geom = lay->un_geom;
    }
  }

  // find the geometry span of the cons
  taVector2i con_geom_max;
  taVector2i con_geom_min(INT_MAX, INT_MAX);
  for(int ui = 0; ui < lay->n_units_built; ui++) {
    UNIT_STATE* u = lay->GetUnitState(net, ui);
    if(recv) {
      ConState_cpp* cg = u->RecvConStatePrjn(net, prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        UnitState_cpp* su = cg->UnState(j,net);
        if(!su) continue;
        taVector2i upos;
        upos.SetXY(su->pos_x, su->pos_y);
        con_geom_max.Max(upos);
        con_geom_min.Min(upos);
      }
    }
    else {                      // send
      ConState_cpp* cg = u->SendConStatePrjn(net, prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        UnitState_cpp* su = cg->UnState(j,net);
        if(!su) continue;
        taVector2i upos;
        upos.SetXY(su->pos_x, su->pos_y);
        con_geom_max.Max(upos);
        con_geom_min.Min(upos);
      }
    }
  }
  con_geom_max += 1;            // add one for sizing
  if((con_geom_min.x == INT_MAX) || (con_geom_min.y == INT_MAX))
    con_geom_min = 0;
  taVector2i con_geom = con_geom_max - con_geom_min;
  int n_cons = con_geom.Product();
  MatrixGeom geom;
  geom.SetGeom(4, con_geom.x, con_geom.y, lay_geom.x, lay_geom.y);
  String valname = GetColName(prj, val_specs.size);
  AddMatrixCol(valname, VT_FLOAT, &geom);

  // now get all the vals
  for(int ui = 0; ui < lay->n_units_built; ui++) {
    UNIT_STATE* u = lay->GetUnitState(net, ui);
    int st_idx = ptrs.size;
    for(int j=0;j<n_cons;j++) { // add blanks -- set them later
      ptrs.Add(NULL); members.Link(con_md);
    }
    if(recv) {
      ConState_cpp* cg = u->RecvConStatePrjn(net, prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        UnitState_cpp* su = cg->UnState(j,net);
        if(!su) continue;
        taVector2i upos;
        upos.SetXY(su->pos_x, su->pos_y);
        upos -= con_geom_min;
        int idx = upos.y * con_geom.x + upos.x;
        ptrs[st_idx + idx] = &(cg->Cn(j,con_md->idx,net)); // set the ptr
      }
    }
    else {                      // send
      ConState_cpp* cg = u->SendConStatePrjn(net, prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        UnitState_cpp* su = cg->UnState(j,net);
        if(!su) continue;
        taVector2i upos;
        upos.SetXY(su->pos_x, su->pos_y);
        upos -= con_geom_min;
        int idx = upos.y * con_geom.x + upos.x;
        ptrs[st_idx + idx] = &(cg->Cn(j,con_md->idx,net)); // set the ptr
      }
    }
  }
}

void NetMonItem::ScanObject_ProjectionGroup(Projection_Group* pg, String var) {
  if (ScanObject_InObject(pg, var, pg, false)) return; // false = test

  for (int i = 0; i < pg->size; i++) {
    if (Projection* prjn = pg->FastEl(i)) {
      if(prjn->MainIsActive()) {
        ScanObject_Projection(prjn, var);
      }
    }
  }
}

void NetMonItem::ScanObject_Projection(Projection* prjn, String var) {
  if(prjn->MainNotActive()) return;

  if(var.startsWith("r.") || var.startsWith("s.")) {
    ScanObject_PrjnCons(prjn, var);
  }
  else {
    ScanObject_InObject(prjn, var, prjn);
  }
}


bool NetMonItem::CheckVarOnUnit(String var, Network* net) {
  if(!net)
    return false;

  TypeDef* unit_typ = net->UnitStateType();

  if(var.startsWith('.'))
    var = var.after('.');

  if(var.startsWith("r.") || var.startsWith("s.")) {
    return true;                // can't really check for sure so just assume..
  }
  
  TypeDef* own_td = unit_typ;
  int net_base_off = 0;
  ta_memb_ptr net_mbr_off;
  MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, net_mbr_off, var, false);
  if(!md) return false;
  return true;
}

void NetMonItem::ScanObject_Unit(UnitState_cpp* u, String var, String obj_nm, NetworkState_cpp* net) {
  if(!u || u->lesioned()) return;
  if(!net->IsBuiltIntact()) // no-can-do
    return;

  TypeDef* unit_typ = ((Network*)net->net_owner)->UnitStateType();
  if(ScanObject_InNonTAObject(u, unit_typ, var, NULL, false)) return; // false = no err

  // otherwise, we only grok the special s. and r. indicating conns
  if (!var.contains('.')) return;
  String subvar = var.before('.');
  String convar = var.after('.');
  if (subvar=="r") {
    int ncg = u->NRecvConGps(net);
    for(int i=0;i<ncg;i++) {
      ConState_cpp* cg = u->RecvConState(net, i);
      LayerState_cpp* fmlay = cg->GetSendLayer(net);
      String cg_nm = obj_nm + "_r_" + taMisc::StringMaxLen(fmlay->layer_name, max_name_len);
      ScanObject_RecvCons(cg, convar, cg_nm, net);
    }
  }
  else {                        // must be s
    int ncg = u->NSendConGps(net);
    for(int i=0;i<ncg;i++) {
      ConState_cpp* cg = u->SendConState(net, i);
      LayerState_cpp* tolay = cg->GetRecvLayer(net);
      String cg_nm = obj_nm + "_s_" + taMisc::StringMaxLen(tolay->layer_name, max_name_len);
      ScanObject_SendCons(cg, convar, cg_nm, net);
    }
  }
}

void NetMonItem::ScanObject_RecvCons(ConState_cpp* cg, String var, String obj_nm, NetworkState_cpp* net) {
  if(!cg || !cg->IsActive()) return;
  if(!net->IsBuiltIntact()) // no-can-do
    return;
  
  MemberDef* con_md = cg->ConType((Network*)net->net_owner)->members.FindNameR(var);
  if(!con_md) return;           // can't find that var!

  // find the geometry span of the cons
  taVector2i con_geom_max;
  taVector2i con_geom_min(INT_MAX, INT_MAX);
  for(int j=0; j<cg->size; ++j) {
    UnitState_cpp* su = cg->UnState(j,net);
    if(!su) continue;
    taVector2i upos;
    upos.SetXY(su->pos_x, su->pos_y);
    con_geom_max.Max(upos);
    con_geom_min.Min(upos);
  }
  con_geom_max += 1;            // add one for sizing
  if((con_geom_min.x == INT_MAX) || (con_geom_min.y == INT_MAX))
    con_geom_min = 0;
  taVector2i con_geom = con_geom_max - con_geom_min;
  int n_cons = con_geom.Product();
  MatrixGeom geom;
  geom.SetGeom(2, con_geom.x, con_geom.y);
  String valname = GetColName(NULL, val_specs.size, obj_nm);
  AddMatrixCol(valname, VT_FLOAT, &geom);

  for(int j=0;j<n_cons;j++) {   // add blanks -- set them later
    ptrs.Add(NULL); members.Link(con_md);
  }
  for(int j=0; j<cg->size; ++j) {
    UnitState_cpp* su = cg->UnState(j,net);
    if(!su) continue;
    taVector2i upos;
    upos.SetXY(su->pos_x, su->pos_y);
    upos -= con_geom_min;
    int idx = upos.y * con_geom.x + upos.x;
    ptrs[idx] = &(cg->Cn(j, con_md->idx, net));      // set the ptr
  }
}

void NetMonItem::ScanObject_SendCons(ConState_cpp* cg, String var, String obj_nm, NetworkState_cpp* net) {
  if(!cg || !cg->IsActive()) return;
  if(!net->IsBuiltIntact()) // no-can-do
    return;

  MemberDef* con_md = cg->ConType((Network*)net->net_owner)->members.FindNameR(var);
  if(!con_md) return;           // can't find that var!

  // find the geometry span of the cons
  taVector2i con_geom_max;
  taVector2i con_geom_min(INT_MAX, INT_MAX);
  for(int j=0; j<cg->size; ++j) {
    UnitState_cpp* su = cg->UnState(j,net);
    if(!su) continue;
    taVector2i upos;
    upos.SetXY(su->pos_x, su->pos_y);
    con_geom_max.Max(upos);
    con_geom_min.Min(upos);
  }
  con_geom_max += 1;            // add one for sizing
  if((con_geom_min.x == INT_MAX) || (con_geom_min.y == INT_MAX))
    con_geom_min = 0;
  taVector2i con_geom = con_geom_max - con_geom_min;
  int n_cons = con_geom.Product();
  MatrixGeom geom;
  geom.SetGeom(2, con_geom.x, con_geom.y);
  String valname = GetColName(NULL, val_specs.size, obj_nm);
  AddMatrixCol(valname, VT_FLOAT, &geom);

  for(int j=0;j<n_cons;j++) {   // add blanks -- set them later
    ptrs.Add(NULL); members.Link(con_md);
  }
  for(int j=0; j<cg->size; ++j) {
    UnitState_cpp* su = cg->UnState(j,net);
    if(!su) continue;
    taVector2i upos;
    upos.SetXY(su->pos_x, su->pos_y);
    upos -= con_geom_min;
    int idx = upos.y * con_geom.x + upos.x;
    ptrs[idx] = &(cg->Cn(j, con_md->idx, net));      // set the ptr
  }
}

void NetMonItem::SetMonVals(taBase* obj, const String& var) {
  if ((object.ptr() == obj) && (variable == var)) return;
  object = obj;
  if(object)
    object_type = object->GetTypeDef();
  variable = var;
  UpdateAfterEdit();
}

void NetMonItem::SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) {
  ResetMonVals();
  inherited::SmartRef_SigDestroying(ref, obj);//does UAE
}

void NetMonItem::SmartRef_SigEmit(taSmartRef* ref, taBase* obj,
    int sls, void* op1_, void* op2_)
{
//NOTE: we get these here when script running and any member vars are updated --
// don't update objects -- instead, we may want to use a RefList for the objects,
// and thus detect deletion there.
 // ScanObject();
}

void NetMonItem::CollectAllSpecs(NetMonitor* mon) {
  if(off) return;
  int st_idx = mon->all_specs.size;
  for(int i=0; i<val_specs.size; i++) {
    DataColSpec* ds = val_specs.FastEl(i);
    mon->all_specs.Link(ds);
    ds->col_num = st_idx + i;
  }
}

bool NetMonItem::GetMonVal(int i, Variant& rval) {
  void* obj = NULL;
  MemberDef* md = NULL;
  if (i < ptrs.size) {
    obj = ptrs.FastEl(i);
    md = members.FastEl(i);
  }
  if (!md || !obj) {
    rval = _nilVariant;
    return false;
  }
  if(md->GetOwnerType()->InheritsFrom(&TA_Connection))
    rval = *((float*)obj);      // we stored the raw float*
  else
    rval = md->GetValVar(obj);
  // pre-process..
  // note: NONE op leaves Variant in same format, otherwise converted to float
  pre_proc_3.EvaluateVar(pre_proc_2.EvaluateVar(pre_proc_1.EvaluateVar(rval)));
  return true;
}

void NetMonItem::GetMonVals(DataTable* db) {
  if ((!db) || variable.empty())  return;
  if(computed) {
    if(data_agg) {
      GetMonVals_DataAgg(db);
    }
    return;
  }

  if(agg.op != Aggregate::NONE) {
    GetMonVals_Agg(db);
    return;
  }

  int mon = 0;
  //note: there should always be the exact same number of mons as items to set,
  // but in case of mismatch, the GetMonVal will return Invalid,
  Variant mbval;
  for (int ch = 0; ch < val_specs.size; ++ch) {
    DataColSpec* cs = val_specs.FastEl(ch);
    if (cs->is_matrix) {
      int vals = cs->cell_geom.Product();
      taMatrixPtr mat(db->GetMatrixData(cs->col_num));
      if (mat) {
        for (int j = 0; j < vals; ++j) {
          GetMonVal(mon++, mbval); // note: we don't care if not set, ie invalid
          mat->SetFmVar_Flat(mbval, j);
        }
      }
    }
    else { // scalar
      GetMonVal(mon++, mbval);
      db->SetData(mbval, cs->col_num);
    }
  }
}

void NetMonItem::GetMonVals_Agg(DataTable* db) {
  if ((!db) || variable.empty() || computed)  return;

  if(TestError(agg_specs.size != val_specs.size, "GetMonVals_Agg",
               "internal error: agg_specs.size != val_specs.size! -- report as bug!"))
    return;

  int mon = 0;
  //note: there should always be the exact same number of mons as items to set,
  // but in case of mismatch, the GetMonVal will return Invalid,
  Variant mbval;
  for (int ch = 0; ch < val_specs.size; ++ch) {
    DataColSpec* vcs = val_specs.FastEl(ch);
    DataColSpec* acs = agg_specs.FastEl(ch);
    if (acs->is_matrix) {
      int vals = acs->cell_geom.Product();
      agg_tmp_calc.SetGeom(1,vals);
      for (int j = 0; j < vals; ++j) {
        GetMonVal(mon++, mbval); // note: we don't care if not set, ie invalid
        agg_tmp_calc.SetFmVar_Flat(mbval, j);
      }
      mbval = taMath_float::vec_aggregate(&agg_tmp_calc, agg);
      db->SetData(mbval, vcs->col_num);
    } else { // scalar
      GetMonVal(mon++, mbval);
      db->SetData(mbval, vcs->col_num);
    }
  }
}

void NetMonItem::GetMonVals_DataAgg(DataTable* db) {
  if(!db || !data_src || agg_col.col_name.empty())  return;

  if(TestError(val_specs.size != 1, "GetMonVals_DataAgg",
               "internal error: val_specs.size != 1 for computed val -- report as bug!"))
    return;

  DataTable sel_out(false);
  sel_out.OwnTempObj(); // this is ESSENTIAL for temp data tables -- otherwise cols can't access their parent table b/c owner is not set!
  
  bool use_sel_out = false;     // use sel_out instead of data_src

  if(select_rows && select_spec.col_name.nonempty()) {
    DataSelectSpec selspec(false);
    selspec.ops.Link(&select_spec);
    use_sel_out = taDataProc::SelectRows(&sel_out, data_src, &selspec);
    // if(TestWarning(sel_out.rows == 0, "GetMonVals_DataAgg",
    //            "select rows did not match any rows -- reverting to full data table"))
    //   use_sel_out = false;
  }

  DataColSpec* vcs = val_specs.FastEl(0);
  DataCol* dc;
  if(use_sel_out)
    dc = sel_out.FindColName(agg_col.col_name);
  else
    dc = data_src->FindColName(agg_col.col_name);
  if(!dc) return;               // err in checkconfig
  if(!dc->isNumeric() || (dc->valType() == taBase::VT_BYTE)) return;

  Variant mbval;
  if(!matrix || !vcs->is_matrix || !dc->is_matrix) { // no matrix
    if(dc->valType() == taBase::VT_DOUBLE) {
      mbval = taMath_double::vec_aggregate((double_Matrix*)dc->AR(), agg);
    }
    else if(dc->valType() == taBase::VT_FLOAT) {
      mbval = taMath_float::vec_aggregate((float_Matrix*)dc->AR(), agg);
    }
    else if(dc->valType() == taBase::VT_INT) {
      int_Matrix* mat = (int_Matrix*)dc->AR();
      taMath_float::vec_fm_ints(&agg_tmp_calc, mat);
      mbval = taMath_float::vec_aggregate(&agg_tmp_calc, agg);
    }
    db->SetData(mbval, vcs->col_num);
  }
  else {                        // both src and dest are matrix
    int vals = vcs->cell_geom.Product();
    vals = MIN(vals, dc->cell_size());
    taMatrixPtr dmat(db->GetMatrixData(vcs->col_num));
    if(!dmat) return;                                  // bail
    if(dc->valType() == taBase::VT_DOUBLE) {
      taMath_double::mat_frame_aggregate(&agg_tmp_calc_d, (double_Matrix*)dc->AR(), agg);
      for (int j = 0; j < vals; ++j) {
        mbval = agg_tmp_calc_d.FastEl_Flat(j);
        dmat->SetFmVar_Flat(mbval, j);
      }
    }
    else if(dc->valType() == taBase::VT_FLOAT) {
      taMath_float::mat_frame_aggregate(&agg_tmp_calc, (float_Matrix*)dc->AR(), agg);
      for (int j = 0; j < vals; ++j) {
        mbval = agg_tmp_calc.FastEl_Flat(j);
        dmat->SetFmVar_Flat(mbval, j);
      }
    }
    else if(dc->valType() == taBase::VT_INT) {
      int_Matrix* mat = (int_Matrix*)dc->AR();
      taMath_float::vec_fm_ints(&agg_tmp_calc, mat);
      taMath_float::mat_frame_aggregate(&agg_tmp_calc_2, &agg_tmp_calc, agg);
      for (int j = 0; j < vals; ++j) {
        mbval = agg_tmp_calc_2.FastEl_Flat(j);
        dmat->SetFmVar_Flat(mbval, j);
      }
    }
  }
}

void NetMonItem::SetObject(taBase* obj) {
  object = obj;
  if(obj) {
    object_type = obj->GetTypeDef();
  }
  UpdateAfterEdit();
}

void NetMonItem::ToggleOffFlag() {
  off = !off;
  SigEmitUpdated();
}

void NetMonItem::GetMemberCompletionList(const MemberDef* md, Completions& completions) {
  if (object_type) {
    MemberSpace mbr_space = object_type->members;
    for (int i = 0; i < mbr_space.size; ++i) {
      MemberDef* mbr_def = mbr_space.FastEl(i);
      completions.member_completions.Link(mbr_def);
    }
  }
}

