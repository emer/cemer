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
#include <MemberDef>
#include <UserDataItemBase>
#include <taMath_float>
#include <taMath_double>
#include <DataSelectSpec>
#include <taDataProc>
#include <DataTable>
#include <NetMonitor>
#include <Average>

#include <taMisc>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(NetMonItem);

taTypeDef_Of(UserDataItem);

void NetMonItem::Initialize() {
  off = false;
  computed = false;
  object_type = NULL;
  lookup_var = NULL;
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
  lookup_var = NULL;
  monitor = NULL;
  inherited::CutLinks();
}

void NetMonItem::Copy_(const NetMonItem& cp) {
  ResetMonVals(); // won't be valid anymore
  off = cp.off;
  computed = cp.computed;
  object_type = cp.object_type;
  object = cp.object; // ptr only
  lookup_var = cp.lookup_var;
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

String NetMonItem::GetAutoName(taBase* obj) {
  if(!obj) return "";
  String rval;
  if(agg.op != Aggregate::NONE) {
    rval = agg.GetAggName() + "_";
    rval.downcase();
  }
  if(obj->InheritsFrom(&TA_Network)) { // special case
    rval += (var_label.empty() ? variable : var_label);
  }
  else {
    rval += GetObjName(obj) + "_" + (var_label.empty() ? variable : var_label);
  }
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
  if(lookup_var) {
    variable = lookup_var->name;
    lookup_var = NULL;
  }
  if(variable.empty()) return;

  if (!taMisc::is_loading) {
    if(name_style == MY_NAME) {
      if(!computed) {
        if(name.empty()) {
          SetName(GetAutoName(object));
        }
      }
    }
    else {                      // AUTO_NAME = always update!
      SetName(GetAutoName(object));
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
  else if(obj->InheritsFrom(TA_Unit)) {
    Unit* u = (Unit*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if(lay) {
      return GetObjName(lay) + "[" + String(u->GetMyLeafIndex()) + "]";
    }
  }
  else if (obj->InheritsFrom(TA_Unit_Group)) {
    Unit_Group* ug = (Unit_Group*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if(lay) {
      String nm = GetObjName(lay);
      if(ug->owner == lay) return nm; // synoymous with layer
      int idx = ug->idx;
      if(idx >= 0)
        return nm + "_gp_" + String(idx) + "_";
    }
  }
  else if (obj->InheritsFrom(TA_ConGroup)) {
    ConGroup* cg = (ConGroup*)obj;
    if(cg->IsRecv()) {
      if(cg->prjn && cg->prjn->from.ptr()) {
        Unit* un = GET_OWNER(obj, Unit);
        if (un) {
          return GetObjName(un) + "_r_" + taMisc::StringMaxLen(cg->prjn->from->name, max_name_len);
        }
      }
    }
    else {
      if(cg->prjn && cg->prjn->layer) {
        Unit* un = GET_OWNER(obj, Unit);
        if (un) {
          return GetObjName(un) + "_s_" + taMisc::StringMaxLen(cg->prjn->layer->name, max_name_len);
        }
      }
    }
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

String NetMonItem::GetColName(taBase* obj, int col_idx) {
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
    rval = GetAutoName(obj);
    rval = taMisc::StringCVar(rval); // keep it clean for css var names
    if(val_specs.FindNameIdx(rval) >= 0) {   // NOTE: N^2 kind of thing, but done infrequently so ok..
      for(max_name_len++; max_name_len < 50; max_name_len++) { // hard coded max here..
        rval = GetAutoName(obj);
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

void NetMonItem::ScanObject() {
  // TODO: what about orphaned columns in the sink?????
  ResetMonVals();
  if(computed) {
    if(matrix)
      AddMatrixCol(name, val_type, &matrix_geom);
    else
      AddScalarCol(name, val_type);
    return;
  }

  if (!object) return;

  if (object->InheritsFrom(&TA_Unit))
    ScanObject_Unit((Unit*)object.ptr(), variable);
  else if (object->InheritsFrom(&TA_Layer))
    ScanObject_Layer((Layer*)object.ptr(), variable);
  else if (object->InheritsFrom(&TA_Unit_Group))
    ScanObject_UnitGroup((Unit_Group*)object.ptr(), variable);
  else if (object->InheritsFrom(&TA_Projection))
    ScanObject_Projection((Projection*)object.ptr(), variable);
  else if (object->InheritsFrom(&TA_Projection_Group))
    ScanObject_ProjectionGroup((Projection_Group*)object.ptr(), variable);
  else if (object->InheritsFrom(&TA_Network))
    ScanObject_Network((Network*)object.ptr(), variable);
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
  if(TestError(!udi,"ScanObject_InUserData",
                "UserDataItem: ", key, " not found"))
    return true; //no mon, but we did handle it

  // note: we test for UserDataItem type, not isSimple because
  // we need to link to the value member
  if (udi->InheritsFrom(&TA_UserDataItem)) {
    if(TestError(var.nonempty(),"ScanObject_InUserData",
      "UserDataItem: ", key, " expected to be simple; can't resolve remaining var: ", var))
      return true; //no mon, but we did handle it
    MemberDef* md = udi->FindMemberName("value"); // should exist!
    if(TestError(!md,"ScanObject_InUserData",
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
    if(TestError(var.empty(),"ScanObject_InUserData",
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
      if(TestError(err_not_found, "ScanObject_InObject",
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
    else if(obj->InheritsFrom(&TA_Unit)) { // special case for UnitVars
      Unit* un = (Unit*)obj;
        // don't attempt to scan if network not set or intact..
      if(!un->lesioned() && un->own_net()->IsBuiltIntact()) {
        md = un->own_net()->unit_vars_built->members.FindName(var);
        if(md) {
          if(name_obj) {
            String valname = GetColName(name_obj, val_specs.size);
            ValType vt = ValTypeForType(md->type);
            AddScalarCol(valname, vt);
            if(agg.op != Aggregate::NONE) {
              AddScalarCol_Agg(valname, vt); // add the agg guy just to keep it consistent
            }
          }
          // if not adding a column, it is part of a pre-allocated matrix; just add vars
          ptrs.Add(un->GetUnitVars());
          members.Link(md);
          return true;
        }
      }
    }
  }
  return false;
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

  // we now know it must be a regular unit variable (or invalid); do that
  MatrixGeom geom;
  if(lay->unit_groups) {
    if(lay->gp_geom.n_not_xy || lay->un_geom.n_not_xy)
      geom.SetGeom(1, lay->units.leaves);       // irregular: flatten!
    else
      geom.SetGeom(4, lay->un_geom.x, lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
  }
  else {
    if(lay->un_geom.n_not_xy)
      geom.SetGeom(1, lay->units.leaves);       // irregular: flatten!
    else
      geom.SetGeom(2, lay->un_geom.x, lay->un_geom.y);
  }
  String valname = GetColName(lay, val_specs.size);
  AddMatrixCol(valname, VT_FLOAT, &geom);
  if (geom.dims() == 1) {
    for (int i = 0; i < lay->units.leaves; ++i) {
      ScanObject_InObject(lay->units.Leaf(i), var, NULL); // don't make a col
    }
  }
  else if(geom.dims() == 2) {
    taVector2i c;
    for (c.y = 0; c.y < lay->un_geom.y; ++c.y) {
      for (c.x = 0; c.x < lay->un_geom.x; ++c.x) {
        Unit* u = lay->UnitAtCoord(c); // NULL if odd size or not built
        if(u && !u->lesioned())
          ScanObject_InObject(u, var, NULL); // don't make a col
      }
    }
  }
  else if(geom.dims() == 4) {
    taVector2i gc;
    for (gc.y = 0; gc.y < lay->gp_geom.y; ++gc.y) {
      for (gc.x = 0; gc.x < lay->gp_geom.x; ++gc.x) {
        taVector2i c;
        for (c.y = 0; c.y < lay->un_geom.y; ++c.y) {
          for (c.x = 0; c.x < lay->un_geom.x; ++c.x) {
            Unit* u = lay->UnitAtGpCoord(gc, c);
            if(u && !u->lesioned())
              ScanObject_InObject(u, var, NULL); // don't make a col
          }
        }
      }
    }
  }
}

void NetMonItem::ScanObject_LayerUnits(Layer* lay, String var) {
  if(!lay->own_net->IsBuiltIntact()) // no-can-do
    return;
  
  String range2;
  String range1 = var.between('[', ']');
  String rmdr = var.after(']');
  if(rmdr.contains('[')) {
    range2 = rmdr.between('[', ']');
    rmdr = rmdr.after(']');
  }
  if(rmdr.startsWith('.')) rmdr = rmdr.after('.');

  String valname = GetColName(lay, val_specs.size);
  MatrixGeom geom;

  if(range2.nonempty()) { // group case
    if(range1.contains('-')) {
      int gpidx1 = (int)range1.before('-');
      int gpidx2 = (int)range1.after('-');

      if(range2.contains('-')) {
        int unidx1 = (int)range2.before('-');
        int unidx2 = (int)range2.after('-');
        geom.SetGeom(2, 1+unidx2-unidx1, 1+gpidx2-gpidx1);
        AddMatrixCol(valname, VT_FLOAT, &geom);
        for (int gi = gpidx1; gi <= lay->units.gp.size && gi <= gpidx2; ++gi) {
          Unit_Group* gp = (Unit_Group*)lay->units.gp.SafeEl(gi);
          if(!gp) break;
          for (int i = unidx1; i < gp->size && i <= unidx2; ++i) {
            ScanObject_InObject(gp->SafeEl(i), rmdr, NULL); // don't make a col
          }
        }
      }
      else {
        int idx = (int)range2;
        geom.SetGeom(1, 1+gpidx2-gpidx1);
        AddMatrixCol(valname, VT_FLOAT, &geom);
        for (int gi = gpidx1; gi <= lay->units.gp.size && gi <= gpidx2; ++gi) {
          Unit_Group* gp = (Unit_Group*)lay->units.gp.SafeEl(gi);
          if(!gp) break;
          ScanObject_InObject(gp->SafeEl(idx), rmdr, NULL); // don't make a col
        }
      }
    }
    else {
      int gpidx = (int)range1;
      Unit_Group* gp = (Unit_Group*)lay->units.gp.SafeEl(gpidx);
      if(!gp) return;
      if(range2.contains('-')) {
        int unidx1 = (int)range2.before('-');
        int unidx2 = (int)range2.after('-');
        geom.SetGeom(1, 1+unidx2-unidx1);
        AddMatrixCol(valname, VT_FLOAT, &geom);
        for (int i = unidx1; i < gp->size && i <= unidx2; ++i) {
          ScanObject_InObject(gp->SafeEl(i), rmdr, NULL); // don't make a col
        }
      }
      else {
        int idx = (int)range2;
        AddScalarCol(valname, VT_FLOAT);
        ScanObject_InObject(gp->SafeEl(idx), rmdr, NULL); // don't make a col
      }
    }
  }
  else {                        // just unit idxs
    if(range1.contains('-')) {
      int idx1 = (int)range1.before('-');
      int idx2 = (int)range1.after('-');
      geom.SetGeom(1, 1+idx2-idx1);
      AddMatrixCol(valname, VT_FLOAT, &geom);
      for (int i = idx1; i < lay->units.leaves && i <= idx2; ++i) {
        ScanObject_InObject(lay->units.Leaf(i), rmdr, NULL); // don't make a col
      }
    }
    else {
      int idx = (int)range1;
      String valnm = GetColName(lay, val_specs.size);
      AddScalarCol(valnm, VT_FLOAT);
      ScanObject_InObject(lay->units.Leaf(idx), rmdr, NULL); // don't make a col
    }
  }
}

void NetMonItem::ScanObject_LayerCons(Layer* lay, String var) {
  if(!lay->own_net->IsBuiltIntact()) // no-can-do
    return;
  String subvar = var.before('.');
  if(subvar == "r") {
    for(int i=0;i<lay->projections.size; i++) {
      Projection* prjn = lay->projections[i];
      if(prjn->NotActive()) continue;
      ScanObject_PrjnCons(prjn, var);
    }
  }
  else {                        // must be s
    for(int i=0;i<lay->send_prjns.size; i++) {
      Projection* prjn = lay->send_prjns[i];
      if(prjn->NotActive()) continue;
      ScanObject_PrjnCons(prjn, var);
    }
  }
}

void NetMonItem::ScanObject_PrjnCons(Projection* prjn, String var) {
  if(prjn->NotActive()) return;
  Layer* lay = NULL;
  String subvar = var.before('.');
  bool recv = true;
  if(subvar == "r") lay = prjn->layer;
  else { lay = prjn->from; recv = false; }
  String convar = var.after('.');
  MemberDef* con_md = prjn->con_type->members.FindNameR(convar);
  if(!con_md) return;           // can't find that var!
  Network* net = lay->own_net;

  if(!net->IsBuiltIntact()) // no-can-do
    return;
  
  // always create a 4dimensional matrix: 1st 2 are units, 2nd 2 are cons
  taVector2i lay_geom;
  if(lay->unit_groups) {
    if(lay->gp_geom.n_not_xy || lay->un_geom.n_not_xy) {
      lay_geom.x = lay->units.leaves;
      lay_geom.y = 1;
    }
    else {
      lay_geom = lay->un_geom * lay->gp_geom;
    }
  }
  else {
    if(lay->un_geom.n_not_xy) {
      lay_geom.x = lay->units.leaves;
      lay_geom.y = 1;
    }
    else {
      lay_geom = lay->un_geom;
    }
  }

  // find the geometry span of the cons
  taVector2i con_geom_max;
  taVector2i con_geom_min(INT_MAX, INT_MAX);
  FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
    if(recv) {
      ConGroup* cg = u->RecvConGroupPrjn(prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        Unit* su = cg->Un(j,net);
        if(!su) continue;
        taVector2i upos;  su->LayerLogPos(upos);
        con_geom_max.Max(upos);
        con_geom_min.Min(upos);
      }
    }
    else {                      // send
      ConGroup* cg = u->SendConGroupPrjn(prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        Unit* su = cg->Un(j,net);
        if(!su) continue;
        taVector2i upos; su->LayerLogPos(upos);
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
  String valname = GetColName(prjn, val_specs.size);
  AddMatrixCol(valname, VT_FLOAT, &geom);

  // now get all the vals
  FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
    int st_idx = ptrs.size;
    for(int j=0;j<n_cons;j++) { // add blanks -- set them later
      ptrs.Add(NULL); members.Link(con_md);
    }
    if(recv) {
      ConGroup* cg = u->RecvConGroupPrjn(prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        Unit* su = cg->Un(j,net);
        if(!su) continue;
        taVector2i upos;  su->LayerLogPos(upos);
        upos -= con_geom_min;
        int idx = upos.y * con_geom.x + upos.x;
        ptrs[st_idx + idx] = &(cg->Cn(j,con_md->idx,net)); // set the ptr
      }
    }
    else {                      // send
      ConGroup* cg = u->SendConGroupPrjn(prjn);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
        Unit* su = cg->Un(j,net);
        if(!su) continue;
        taVector2i upos;  su->LayerLogPos(upos);
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
      if(prjn->IsActive()) {
        ScanObject_Projection(prjn, var);
      }
    }
  }
}

void NetMonItem::ScanObject_Projection(Projection* prjn, String var) {
  if(prjn->NotActive()) return;

  if(var.startsWith("r.") || var.startsWith("s.")) {
    ScanObject_PrjnCons(prjn, var);
  }
  else {
    ScanObject_InObject(prjn, var, prjn);
  }
}

void NetMonItem::ScanObject_UnitGroup(Unit_Group* ug, String var) {
  // check for projection monitor
  if(var.startsWith('.'))
    var = var.after('.');
  if(var.contains('.')) {
    String subvar = var.before('.');
    if(TestError((subvar == "projections") || (subvar == "prjns"), "ScanObject_UnitGroup",
                 "cannot monitor projections group from UnitGroup object")) {
      return;
    }
    // todo: could do this but is it really needed??  would need to pass ug pointer to
    // a special version of PrjnCons fun
    if(TestError((subvar == "r") || (subvar == "s"), "ScanObject_UnitGroup",
                 "cannot monitor connection weights from UnitGroup object")) {
      return;
    }
  }

  if (ScanObject_InObject(ug, var, ug, false)) return; // false = test

  if(!ug->own_lay->own_net->IsBuiltIntact()) // no-can-do
    return;
  
  // we now know it must be a regular unit variable (or invalid); do that
  MatrixGeom geom;
  if(ug->own_lay->un_geom.n_not_xy)
    geom.SetGeom(1, ug->size);  // irregular: flatten!
  else
    geom.SetGeom(2, ug->own_lay->un_geom.x, ug->own_lay->un_geom.y);

  String valname = GetColName(ug, val_specs.size);
  AddMatrixCol(valname, VT_FLOAT, &geom);
  if(geom.dims() == 1) {
    for(int i = 0; i < ug->size; i++) {
      ScanObject_InObject(ug->FastEl(i), var, NULL); // don't make a col
    }
  }
  else {
    taVector2i c;
    for (c.y = 0; c.y < ug->own_lay->un_geom.y; ++c.y) {
      for (c.x = 0; c.x < ug->own_lay->un_geom.x; ++c.x) {
        Unit* u = ug->UnitAtCoord(c); // NULL if odd size or not built
        if(u)
          ScanObject_InObject(u, var, NULL); // don't make a col
      }
    }
  }
}

void NetMonItem::ScanObject_Unit(Unit* u, String var) {
  if(u->lesioned()) return;
  if(!u->own_net()->IsBuiltIntact()) // no-can-do
    return;
  if(ScanObject_InObject(u, var, u, false)) return; // false = test

  // otherwise, we only grok the special s. and r. indicating conns
  if (!var.contains('.')) return;
  String subvar = var.before('.');
  String convar = var.after('.');
  if (subvar=="r") {
    for(int i=0;i<u->NRecvConGps();i++)
      ScanObject_RecvCons(u->RecvConGroup(i), convar);
  }
  else {                        // must be s
    for(int i=0;i<u->NSendConGps();i++)
      ScanObject_SendCons(u->SendConGroup(i), convar);
  }
}

void NetMonItem::ScanObject_RecvCons(ConGroup* cg, String var) {
  if(!cg || !cg->IsActive()) return;
  MemberDef* con_md = cg->ConType()->members.FindNameR(var);
  if(!con_md) return;           // can't find that var!

  Network* net = cg->prjn->layer->own_net;
  if(!net->IsBuiltIntact()) // no-can-do
    return;

  // find the geometry span of the cons
  taVector2i con_geom_max;
  taVector2i con_geom_min(INT_MAX, INT_MAX);
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j,net);
    if(!su) continue;
    taVector2i upos;  su->LayerLogPos(upos);
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
  String valname = GetColName(cg->OwnUn(net), val_specs.size);
  AddMatrixCol(valname, VT_FLOAT, &geom);

  for(int j=0;j<n_cons;j++) {   // add blanks -- set them later
    ptrs.Add(NULL); members.Link(con_md);
  }
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j,net);
    if(!su) continue;
    taVector2i upos;  su->LayerLogPos(upos);
    upos -= con_geom_min;
    int idx = upos.y * con_geom.x + upos.x;
    ptrs[idx] = &(cg->Cn(j, con_md->idx, net));      // set the ptr
  }
}

void NetMonItem::ScanObject_SendCons(ConGroup* cg, String var) {
  if(!cg || !cg->IsActive()) return;
  MemberDef* con_md = cg->ConType()->members.FindNameR(var);
  if(!con_md) return;           // can't find that var!

  Network* net = cg->prjn->layer->own_net;
  if(!net->IsBuiltIntact()) // no-can-do
    return;

  // find the geometry span of the cons
  taVector2i con_geom_max;
  taVector2i con_geom_min(INT_MAX, INT_MAX);
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j,net);
    if(!su) continue;
    taVector2i upos;  su->LayerLogPos(upos);
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
  String valname = GetColName(cg->OwnUn(net), val_specs.size);
  AddMatrixCol(valname, VT_FLOAT, &geom);

  for(int j=0;j<n_cons;j++) {   // add blanks -- set them later
    ptrs.Add(NULL); members.Link(con_md);
  }
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j,net);
    if(!su) continue;
    taVector2i upos;  su->LayerLogPos(upos);
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

//  DataTable sel_out(false);
  
  taProject* proj = GetMyProj();
  DataTable* sel_out = proj->GetNewAnalysisDataTable("sel_out", true);

  
  bool use_sel_out = false;     // use sel_out instead of data_src

  if(select_rows && select_spec.col_name.nonempty()) {
    DataSelectSpec selspec(false);
    selspec.ops.Link(&select_spec);
    use_sel_out = taDataProc::SelectRows(sel_out, data_src, &selspec);
    if(TestWarning(sel_out->rows == 0, "GetMonVals_DataAgg",
               "select rows did not match any rows -- reverting to full data table"))
      use_sel_out = false;
  }

  DataColSpec* vcs = val_specs.FastEl(0);
  DataCol* dc;
  if(use_sel_out)
    dc = sel_out->FindColName(agg_col.col_name);
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
    taBase::UnRef(dmat);
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

String_Array*  NetMonItem::StringFieldLookupForCompleter(const String& cur_txt, int cur_pos,
                                                         const String& mbr_name, int& new_pos) {
  if (!monitor) {
    UpdateAfterEdit_impl(); // do this to get the pointer to NetMonitor set
  }
    
  monitor->var_completer_list.Reset();
  
  MemberSpace mbr_space = object_type->members;
  for (int i = 0; i < mbr_space.size; ++i) {
    MemberDef* mbr_def = mbr_space.FastEl(i);
    monitor->var_completer_list.Add(mbr_def->name);
  }
  return &monitor->var_completer_list;
}
