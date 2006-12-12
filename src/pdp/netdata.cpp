// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// netdata.cc


#include "netdata.h"
#include "ta_geometry.h"

//////////////////////
//   LayerDataEl    //
//////////////////////

void LayerDataEl::Initialize() {
  net_target = LAYER;
  data_cols = NULL;
  column = NULL;
  layer_group = NULL;
}

void LayerDataEl::Destroy() {
  CutLinks();
}

void LayerDataEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(column) {
    chan_name = column->name;
    taBase::SetPointer((taBase**)&column, NULL); // reset as soon as used -- just a temp guy!
  }
  if(!data) {
    taBase::SetPointer((taBase**)&data_cols, NULL);
  }
  if(layer) {
    layer_name = layer->name;
    layer = NULL;		// smart ref
  }
  if(!network) {
    taBase::SetPointer((taBase**)&layer_group, NULL);
  }
  if(!chan_name.empty() && layer_name.empty()) {
    layer_name = chan_name;
  }
  if(chan_name.empty() && !layer_name.empty()) {
    chan_name = layer_name;
  }
}

String LayerDataEl::GetDisplayName() const {
  String rval = "data chan: " + chan_name;
  rval += " net: " + GetTypeDef()->GetEnumString("NetTarget", net_target);
  if(net_target == LAYER) {
    rval += " " + layer_name;
  }
  return rval;
}

void LayerDataEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  // these data/network things are set by parent prior to this being called (hopefully)
  if(!data || ! network) return;
  if(chan_name.empty()) {
    if(!quiet) taMisc::CheckError("LayerDataEl Error: chan_name is empty");
    rval =  false;
  }
  if(GetChanIdx(data) < 0) {
    if(!quiet) taMisc::CheckError("LayerDataEl Error: channel/column named",
			     chan_name, "not found in data:", data->name);
    rval =  false;
  }
  if(net_target == LAYER) {
    Layer* lay = (Layer*)network->layers.FindLeafName(layer_name);
    if(!lay) {
      if(!quiet) taMisc::CheckError("LayerDataEl Error: cannot find layer named:",
				    layer_name, "in network:", network->name);
      rval =  false;
      return; // fatal
    }
  }
}

void LayerDataEl::SetDataNetwork(DataBlock* db, Network* net) {
  data = db;
  if(db && db->InheritsFrom(&TA_DataTable))
    taBase::SetPointer((taBase**)&data_cols, &((DataTable*)db)->data);
  else
    taBase::SetPointer((taBase**)&data_cols, NULL);
  network = net;
  if(net)
    taBase::SetPointer((taBase**)&layer_group, &(network->layers));
  else
    taBase::SetPointer((taBase**)&layer_group, NULL);
}

//////////////////////////
//  LayerDataEl_List	//
//////////////////////////

void LayerDataEl_List::SetDataNetwork(DataBlock* db, Network* net) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    it->SetDataNetwork(db, net);
  }
}

LayerDataEl* LayerDataEl_List::FindChanName(const String& cnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if(it->chan_name == cnm) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeChanName(const String& cnm) {
  LayerDataEl* ld = FindChanName(cnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->chan_name = cnm;
  return ld;
}

LayerDataEl* LayerDataEl_List::FindLayerName(const String& lnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if(it->layer_name == lnm) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeLayerName(const String& lnm) {
  LayerDataEl* ld = FindLayerName(lnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->layer_name = lnm;
  return ld;
}

LayerDataEl* LayerDataEl_List::FindLayerData(const String& cnm, const String& lnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if((it->layer_name == lnm) && (it->chan_name == cnm)) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeLayerData(const String& cnm, const String& lnm) {
  LayerDataEl* ld = FindLayerData(cnm, lnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->chan_name = cnm;
  ld->layer_name = lnm;
  return ld;
}


//////////////////////
//   LayerWriterEl    //
//////////////////////

void LayerWriterEl::Initialize() {
  use_layer_type = true;
  ext_flags = Unit::NO_EXTERNAL;
  noise.type = Random::NONE;
  noise.mean = 0.0f;
  noise.var = 0.5f;
}

void LayerWriterEl::Destroy() {
  CutLinks();
}

void LayerWriterEl::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!network) return;
  if(net_target == LAYER) {
    Layer* lay = (Layer*)network->layers.FindLeafName(layer_name);
    if(!lay) return;		// already checked in parent
    if(lay->layer_type == Layer::HIDDEN) {
      taMisc::Warning("Warning: LayerwWriterEl:", chan_name, "for layer", layer_name,
		      "layer_type is HIDDEN -- not appropriate for writing to (by default).  Turn use_layer_type off and set appropriate ext_flags if this is intentional.");
    }
    // todo: do this check
    // if(data->dims() == 4 && layer->uses_groups())
//     if(offs.x != 0 || offs.y != 0) {
//       taMisc::Error("Layer::ApplyInputData: cannot have offsets for 4d data to unit groups");
//     }
  }
}

String LayerWriterEl::GetDisplayName() const {
  String rval = inherited::GetDisplayName();
  // todo: could add some expert stuff..
  return rval;
}

// note: we always do the lookup by name every time -- it just doesn't cost
// that much and it makes everything so much simpler!
bool LayerWriterEl::ApplyInputData(DataBlock* db, Network* net) {
  if(!db || !net) return false;
  int chan_idx = db->GetSourceChannelByName(chan_name);
  if(chan_idx < 0) return false;
  if(net_target == TRIAL_NAME) {
    net->trial_name = db->GetData(chan_idx);
    return true;
  }
  else if(net_target == GROUP_NAME) {
    net->group_name = db->GetData(chan_idx);
    return true;
  }
  // LAYER
  Layer* lay = (Layer*)net->layers.FindLeafName(layer_name);
  if(!lay) return false;
  taMatrixPtr mat(db->GetMatrixData(chan_idx)); //note: refs mat
  if(!mat) {
    taMisc::Warning("LayerWriterEl::ApplyInputData: could not get matrix data from channel:",
		    chan_name, "in data:",db->name);
    return false;
  }
  if(use_layer_type) {
    if(lay->layer_type == Layer::INPUT)
      ext_flags = Unit::EXT;
    else if(lay->layer_type == Layer::TARGET)
      ext_flags = Unit::TARG;
    else
      ext_flags = Unit::COMP;
  }

  // we only apply target data in TRAIN mode
  if((net->train_mode != Network::TRAIN) && (ext_flags & Unit::TARG))
    return true;
  // get the data as a slice -- therefore, frame is always 0
  lay->ApplyInputData(mat, ext_flags, &noise, &offset);
  // mat unrefs at this point, or on exit from routine
  return true;
}

//////////////////////////
//  	LayerWriter	//
//////////////////////////

void LayerWriter::Initialize() {
  layer_data.SetBaseType(&TA_LayerWriterEl);
}

void LayerWriter::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  layer_data.SetDataNetwork(data, network);
}

void LayerWriter::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!data) {
    if(!quiet) taMisc::CheckError("LayerWriter Error: data is NULL");
    rval =  false;
  }
  if(!network) {
    if(!quiet) taMisc::CheckError("LayerWriter Error: network is NULL");
    rval =  false;
  }
}

void LayerWriter::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  layer_data.SetDataNetwork(data, network); // make sure it has ptrs first!
  layer_data.CheckConfig(quiet, rval);
}

String LayerWriter::GetDisplayName() const {
  String rval = name;
  if(data) rval += " fm data: " + data->name;
  if(network) rval += " to net: " + network->name;
  return rval;
}

void LayerWriter::SetDataNetwork(DataBlock* db, Network* net) {
  data = db;
  network = net;
  layer_data.SetDataNetwork(data, network);
}

void LayerWriter::AutoConfig(bool reset_existing) {
  if(!data || !network) {
    taMisc::Error("Either the data or the network variables are not set -- please set them and try again!");
    return;
  }
  if(reset_existing) layer_data.Reset();
  Layer* lay;
  taLeafItr itr;
  FOR_ITR_EL(Layer, lay, network->layers., itr) {
    if(lay->layer_type == Layer::HIDDEN) continue;
    int chan_idx = data->GetSourceChannelByName(lay->name);
    if (chan_idx < 0) {
      taMisc::Warning("LayerWriter AutoConfig: did not find channel/data column for layer named:", lay->name, "of type:", TA_Layer.GetEnumString("LayerType", lay->layer_type));
      continue;	// not found
    }
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeLayerData(lay->name, lay->name);
    lrw->SetDataNetwork(data, network);
    lrw->DataChanged(DCR_ITEM_UPDATED);
  }
  int nm_idx = data->GetSourceChannelByName("Name");
  if(nm_idx >= 0) {
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeChanName("Name");
    lrw->net_target = LayerDataEl::TRIAL_NAME;
  }
  int gp_idx = data->GetSourceChannelByName("Group");
  if(gp_idx >= 0) {
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeChanName("Group");
    lrw->net_target = LayerDataEl::GROUP_NAME;
  }
}

bool LayerWriter::ApplyInputData() {
  if(!data || !network) return false;
  bool rval = true;
  for (int i = 0; i < layer_data.size; ++i) {
    LayerWriterEl* lw = (LayerWriterEl*)layer_data.FastEl(i);
    bool nval = lw->ApplyInputData(data, network);
    if(!nval) rval = false;
  }
  return rval;
}

/*TODO
//////////////////////
//   LayerReader    //
//////////////////////

void LayerReader::Initialize() {
}

void LayerReader::Destroy() {
  CutLinks();
}

void LayerReader::InitLinks(){
  inherited::InitLinks();
  //TODO:
}

void LayerReader::CutLinks() {
  //TODO:
  inherited::CutLinks();
}

void LayerReader::Copy_(const LayerReader& cp) {
  //TODO: need to copy source info
}

//////////////////////////
//  LayerReader_List	//
//////////////////////////

void LayerReader_List::AutoConfig_impl(DataBlock* db, Network* net,
  bool freshen, Layer::LayerType lt) 
{
  if (!freshen) Reset();
  Layer* lay;
  taLeafItr itr;
  FOR_ITR_EL(Layer, lay, net->layers., itr) {
    //note: we only look for any lt flags, not all of them
    if (!(lay->layer_type & lt)) continue;
    int chan = db->GetSinkChannelIndexByName(lay->name);
    if (chan < 0) continue;
    // find matching existing, or make new
    LayerReader* lrw = NULL;
    if (freshen) 
      lrw = (LayerReader*)FindByDataAndLayer(db, lay);
    if (!lrw) {
      lrw = (LayerReader*)New(1);
      SET_POINTER(lrw->data, db);
      SET_POINTER(lrw->layer, lay);
    }
    //TODO: set additional props
    lrw->DataChanged(DCR_ITEM_UPDATED);
  }
}
*/

//////////////////////////////////////////////////////////////////////////////
//  NetMonItem
//////////////////////////////////////////////////////////////////////////////

void NetMonItem::Initialize() {
  object_type = NULL;
  member_var = NULL;
  variable = "act";
  name_style = AUTO_NAME;
  max_name_len = 6;
  cell_num  = 0;
}

void NetMonItem::InitLinks() {
  inherited::InitLinks();
  taBase::Own(val_specs,this);
  taBase::Own(ptrs,this);
  taBase::Own(pre_proc_1,this);
  taBase::Own(pre_proc_2,this);
  taBase::Own(pre_proc_3,this);
  taBase::Own(object, this);
}

void NetMonItem::CutLinks() {
  pre_proc_3.CutLinks();
  pre_proc_2.CutLinks();
  pre_proc_1.CutLinks();
  ResetMonVals();
  val_specs.CutLinks();
  object.CutLinks();
  object_type = NULL;
  member_var = NULL;
  inherited::CutLinks();
}

void NetMonItem::Copy_(const NetMonItem& cp) {
  ResetMonVals(); // won't be valid anymore
  object = cp.object; // ptr only
  object_type = cp.object_type;
  member_var = cp.member_var;
  variable = cp.variable;
  var_label = cp.var_label;
  name_style = cp.name_style;
  max_name_len = cp.max_name_len;
  pre_proc_1 = cp.pre_proc_1;
  pre_proc_2 = cp.pre_proc_2;
  pre_proc_3 = cp.pre_proc_3;
}

void NetMonItem::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  if(!owner) {
    if(!quiet) taMisc::CheckError("NetMonItem named:", name, "has no owner");
    rval =  false;
  }
  if(!object) {
    if(!quiet) taMisc::CheckError("NetMonItem named:", name, "path:", GetPath(),
			     "object is NULL");
    rval =  false;
  }
  if(variable.empty()) {
    if(!quiet) taMisc::CheckError("NetMonItem named:", name, "path:", GetPath(),
			     "variable is empty");
    rval =  false;
  }
}

void NetMonItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();

  if(!owner) return;
  if(!object) return;
  object_type = object->GetTypeDef();
  if(member_var) {
    variable = member_var->name;
    member_var = NULL;
  }
  if(variable.empty()) return;
  
  if (!taMisc::is_loading) {
    if(name_style == MY_NAME) {
      if(name.empty() || name.contains(GetTypeDef()->name)) {
	name = GetObjName(object) + "_" + (var_label.empty() ? variable : var_label);
      }
    }
    else {			// AUTO_NAME = always update!
      name = GetObjName(object) + "_" + (var_label.empty() ? variable : var_label);
    }
    name = taMisc::StringCVar(name);		// keep it clean for css var names
    ScanObject();
  }
}

String NetMonItem::GetObjName(TAPtr obj) {
  if (!obj) return _nilString;

  // cases where default name is not what we want:

  if(obj->InheritsFrom(TA_Projection)) {
    Projection* prjn = (Projection*)obj;
    if(prjn->from && prjn->layer) {
      return taMisc::StringMaxLen(prjn->layer->name, max_name_len) + "_Fm_" + 
	taMisc::StringMaxLen(prjn->from->name, max_name_len);
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
      int idx = lay->units.gp.FindEl(ug);
      if(idx >= 0)
	return nm + "_gp_" + String(idx) + "_";
    }
  }
  else if (obj->InheritsFrom(TA_RecvCons)) {
    RecvCons* cg = (RecvCons*)obj;
    if(cg->prjn && cg->prjn->from) {
      Unit* un = GET_OWNER(obj, Unit);
      if (un) {
	return GetObjName(un) + "_r_" + taMisc::StringMaxLen(cg->prjn->from->name, max_name_len);
      }
    }
  }
  else if (obj->InheritsFrom(TA_SendCons)) {
    SendCons* cg = (SendCons*)obj;
    if(cg->prjn && cg->prjn->layer) {
      Unit* un = GET_OWNER(obj, Unit);
      if (un) {
	return GetObjName(un) + "_s_" + taMisc::StringMaxLen(cg->prjn->layer->name, max_name_len);
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

String NetMonItem::GetChanName(taBase* obj, int col_idx) {
  String base_nm;
  if(name_style == MY_NAME) {
    if(col_idx == 0)
      base_nm = name;
    else
      base_nm = name + "_" + String(col_idx);
  }
  else {
    if(obj->InheritsFrom(&TA_Network)) // special case
      return (var_label.empty() ? variable : var_label);
    base_nm = GetObjName(obj);
  }
  String rval = base_nm + "_" + (var_label.empty() ? variable : var_label);
  rval = taMisc::StringCVar(rval); // keep it clean for css var names
  return rval;
}

MatrixChannelSpec* NetMonItem::AddMatrixChan(const String& valname, ValType vt,
					     const MatrixGeom* geom) 
{
  cell_num = 0;
  MatrixChannelSpec* cs = (MatrixChannelSpec*)val_specs.New(1, &TA_MatrixChannelSpec);
  cs->SetName(valname);
  cs->val_type = vt;
  cs->uses_cell_names = false;	// not!
  if (geom) {
    cs->cell_geom = *geom;
  } else {
    cs->cell_names.SetGeom(1, 0); //dynamic -- note: not used!
  }
  cs->UpdateAfterEdit();
  return cs;
}

ChannelSpec* NetMonItem::AddScalarChan(const String& valname, ValType vt) {
  cell_num = 0;//maybe should be 1!
  ChannelSpec* cs = (ChannelSpec*)val_specs.New(1, &TA_ChannelSpec);
  cs->SetName(valname);
  cs->val_type = vt;
  cs->UpdateAfterEdit();
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
  ptrs.Reset();
  members.RemoveAll();
}

void NetMonItem::ScanObject() {
  // TODO: what about orphaned columns in the sink?????
  ResetMonVals();
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

bool NetMonItem::ScanObject_InObject(taBase* obj, String var, taBase* name_obj) {
  if (!obj) return false; 
  MemberDef* md = NULL;
  
  // first, try the recursive end, look for terminal member in ourself
  if (var.contains('.')) {
    String membname = var.before('.');
    md = obj->FindMember(membname);
    //note: if memb not found, then we assume it is in an iterated subobj...
    if (!md) return false;
    
    if (!md->type->InheritsFrom(&TA_taBase)) {
      taMisc::Error("NetMonitor can only monitor taBase objects, not: ",
		    md->type->name, " var: ", var);
      return true; //no mon, but we did handle it
    }
    // we can only handle embedded objs and ptrs to objs
    TAPtr ths = NULL;
    if (md->type->ptr == 0)
      ths = (TAPtr) md->GetOff((void*)obj);
    else if (md->type->ptr == 1)
      ths = *((TAPtr*)md->GetOff((void*)obj));
    else {
      taMisc::Error("NetMonitor can only handle embedded taBase objects"
        " or ptrs to them, not level=",
        String(md->type->ptr), " var: ", var);
      return true; //no mon, but we did handle it
    }
    // because we found the subobj, we deref the var and invoke ourself recursively
    var = var.after('.');
    return ScanObject_InObject(ths, var, name_obj);
  }
  else {
    md = obj->FindMember(var);
    if (md) {
      if(name_obj) {
	String valname = GetChanName(name_obj, val_specs.size);
	ValType vt = ValTypeForType(md->type);
        AddScalarChan(valname, vt);
      }
      // if not adding a column, it is part of a pre-allocated matrix; just add vars
      ptrs.Add(obj);
      members.Link(md);
      return true;
    }
  }
  return false;
}

void NetMonItem::ScanObject_Network(Network* net, String var) {
  if (ScanObject_InObject(net, var, net)) return;

  taLeafItr itr;
  Layer* lay;
  FOR_ITR_EL(Layer, lay, net->layers., itr)
    ScanObject_Layer(lay, var);
}

void NetMonItem::ScanObject_Layer(Layer* lay, String var) {
  // check for projection monitor
  if(var.contains('.')) {
    String subvar = var.before('.');
    if((subvar == "projections") || (subvar == "prjns")) {
      ScanObject_ProjectionGroup(&lay->projections, var.after('.'));
      return;
    }
    if((subvar == "r") || (subvar == "s")) {
      ScanObject_LayerCons(lay, var);
      return;
    }
  }

  if (ScanObject_InObject(lay, var, lay)) return;

  // we now know it must be a regular unit variable (or invalid); do that
  MatrixGeom geom;
  if(lay->unit_groups) {
    if(lay->gp_geom.n_not_xy || lay->un_geom.n_not_xy)
      geom.SetGeom(1, lay->units.leaves);	// irregular: flatten!
    else
      geom.SetGeom(4, lay->un_geom.x, lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
  } else {
    if(lay->un_geom.n_not_xy)
      geom.SetGeom(1, lay->units.leaves);	// irregular: flatten!
    else
      geom.SetGeom(2, lay->un_geom.x, lay->un_geom.y);
  }
  String valname = GetChanName(lay, val_specs.size);
  AddMatrixChan(valname, VT_FLOAT, &geom);
  if (geom.size == 1) {
    for (int i = 0; i < lay->units.leaves; ++i) {
      ScanObject_InObject(lay->units.Leaf(i), var, NULL); // don't make a col
    }
  } else if(geom.size == 2) {
    TwoDCoord c;
    for (c.y = 0; c.y < lay->un_geom.y; ++c.y) {
      for (c.x = 0; c.x < lay->un_geom.x; ++c.x) {
        Unit* u = lay->FindUnitFmCoord(c); // NULL if odd size or not built
        if(u) 
	  ScanObject_InObject(u, var, NULL); // don't make a col
      }
    }
  } else if(geom.size == 4) {
    TwoDCoord gc;
    for (gc.y = 0; gc.y < lay->gp_geom.y; ++gc.y) {
      for (gc.x = 0; gc.x < lay->gp_geom.x; ++gc.x) {
	TwoDCoord c;
	for (c.y = 0; c.y < lay->un_geom.y; ++c.y) {
	  for (c.x = 0; c.x < lay->un_geom.x; ++c.x) {
	    Unit* u = lay->FindUnitFmGpCoord(gc, c);
	    if(u) 
	      ScanObject_InObject(u, var, NULL); // don't make a col
	  }
	}
      }
    }
  }
}

void NetMonItem::ScanObject_LayerCons(Layer* lay, String var) {
  String subvar = var.before('.');
  if(subvar == "r") {
    for(int i=0;i<lay->projections.size; i++) {
      Projection* prjn = lay->projections[i];
      ScanObject_PrjnCons(prjn, var);
    }
  }
  else {			// must be s
    for(int i=0;i<lay->send_prjns.size; i++) {
      Projection* prjn = lay->send_prjns[i];
      ScanObject_PrjnCons(prjn, var);
    }
  }
}

void NetMonItem::ScanObject_PrjnCons(Projection* prjn, String var) {
  if(!prjn->from || !prjn->layer) return;
  Layer* lay = NULL;
  String subvar = var.before('.');
  bool recv = true;
  if(subvar == "r") lay = prjn->layer;
  else { lay = prjn->from; recv = false; }
  String convar = var.after('.');
  MemberDef* con_md = prjn->con_type->members.FindNameR(convar);
  if(!con_md) return;		// can't find that var!

  // always create a 4dimensional matrix: 1st 2 are units, 2nd 2 are cons
  TwoDCoord lay_geom;
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
  TwoDCoord con_geom_max;
  TwoDCoord con_geom_min(INT_MAX, INT_MAX);
  taLeafItr uitr;
  Unit* u;
  FOR_ITR_EL(Unit, u, lay->units., uitr) {
    if(recv) {
      RecvCons* cg = u->recv.SafeEl(prjn->recv_idx);
      if(!cg) continue;
      for(int j=0; j<cg->cons.size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos = su->GetMyAbsPos();
	con_geom_max.Max(upos);
	con_geom_min.Min(upos);
      }
    }
    else {			// send
      SendCons* cg = u->send.SafeEl(prjn->send_idx);
      if(!cg) continue;
      for(int j=0; j<cg->cons.size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos = su->GetMyAbsPos();
	con_geom_max.Max(upos);
	con_geom_min.Min(upos);
      }
    }
  }
  con_geom_max += 1;		// add one for sizing
  if((con_geom_min.x == INT_MAX) || (con_geom_min.y == INT_MAX))
    con_geom_min = 0;
  TwoDCoord con_geom = con_geom_max - con_geom_min;
  int n_cons = con_geom.Product();
  MatrixGeom geom;
  geom.SetGeom(4, con_geom.x, con_geom.y, lay_geom.x, lay_geom.y);
  String valname = GetChanName(prjn, val_specs.size);
  AddMatrixChan(valname, VT_FLOAT, &geom);

  // now get all the vals
  FOR_ITR_EL(Unit, u, lay->units., uitr) {
    int st_idx = ptrs.size;
    for(int j=0;j<n_cons;j++) {	// add blanks -- set them later
      ptrs.Add(NULL); members.Link(con_md);
    }
    if(recv) {
      RecvCons* cg = u->recv.SafeEl(prjn->recv_idx);
      if(!cg) continue;
      for(int j=0; j<cg->cons.size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos = su->GetMyAbsPos() - con_geom_min;
	int idx = upos.y * con_geom.x + upos.x;
	ptrs[st_idx + idx] = cg->Cn(j);	// set the ptr
      }
    }
    else {			// send
      SendCons* cg = u->send.SafeEl(prjn->send_idx);
      if(!cg) continue;
      for(int j=0; j<cg->cons.size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos = su->GetMyAbsPos() - con_geom_min;
	int idx = upos.y * con_geom.x + upos.x;
	ptrs[st_idx + idx] = cg->Cn(j);	// set the ptr
      }
    }
  }
}

void NetMonItem::ScanObject_ProjectionGroup(Projection_Group* pg, String var) {
  if (ScanObject_InObject(pg, var, pg)) return;
  
  for(int i=0;i<pg->size;i++) {
    Projection* prjn = pg->FastEl(i);
    ScanObject_Projection(prjn, var);
  }
}

void NetMonItem::ScanObject_Projection(Projection* prjn, String var) {
  if (ScanObject_InObject(prjn, var, prjn)) return;
  
  Layer* lay = NULL;
  if (var.before('.') == "r") lay = prjn->layer;
  else if (var.before('.') == "s") lay = prjn->from;
  if (lay == NULL) {
    taMisc::Error("NetMonItem Projection does not have layer's set or",
		   "selected var does not apply to connections");
    return;
  }
  ScanObject_PrjnCons(prjn, var);
}

void NetMonItem::ScanObject_UnitGroup(Unit_Group* ug, String var) {
  // check for projection monitor
  if(var.contains('.')) {
    String subvar = var.before('.');
    if((subvar == "projections") || (subvar == "prjns")) {
      taMisc::Error("NetMonItem: cannot monitor projections group from UnitGroup object");
      return;
    }
    if((subvar == "r") || (subvar == "s")) {
      // todo: could do this but is it really needed??  would need to pass ug pointer to
      // a special version of PrjnCons fun
      taMisc::Error("NetMonItem: cannot monitor connection weights from UnitGroup object");
      return;
    }
  }

  if (ScanObject_InObject(ug, var, ug)) return;

  // we now know it must be a regular unit variable (or invalid); do that
  MatrixGeom geom;
  if(ug->geom.n_not_xy)
    geom.SetGeom(1, ug->size);	// irregular: flatten!
  else
    geom.SetGeom(2, ug->geom.x, ug->geom.y);
  
  String valname = GetChanName(ug, val_specs.size);
  AddMatrixChan(valname, VT_FLOAT, &geom);
  if(geom.size == 1) {
    for(int i = 0; i < ug->size; i++) {
      ScanObject_InObject(ug->FastEl(i), var, NULL); // don't make a col
    }
  }
  else {
    TwoDCoord c;
    for (c.y = 0; c.y < ug->geom.y; ++c.y) {
      for (c.x = 0; c.x < ug->geom.x; ++c.x) {
        Unit* u = ug->FindUnitFmCoord(c); // NULL if odd size or not built
        if(u) 
	  ScanObject_InObject(u, var, NULL); // don't make a col
      }
    }
  }
}

void NetMonItem::ScanObject_Unit(Unit* u, String var) {
  if(ScanObject_InObject(u, var, u)) return;
  
  // otherwise, we only grok the special s. and r. indicating conns
  if (!var.contains('.')) return;
  String subvar = var.before('.');
  String convar = var.after('.');
  if (subvar=="r") { 
    for(int i=0;i<u->recv.size;i++)
      ScanObject_RecvCons(u->recv[i], convar);
  }
  else {			// must be s
    for(int i=0;i<u->send.size;i++)
      ScanObject_SendCons(u->send[i], convar);
  }
}

void NetMonItem::ScanObject_RecvCons(RecvCons* cg, String var) {
  if(!cg || !cg->prjn) return;
  MemberDef* con_md = cg->con_type->members.FindNameR(var);
  if(!con_md) return;		// can't find that var!

  // find the geometry span of the cons
  TwoDCoord con_geom_max;
  TwoDCoord con_geom_min(INT_MAX, INT_MAX);
  for(int j=0; j<cg->cons.size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos = su->GetMyAbsPos();
    con_geom_max.Max(upos);
    con_geom_min.Min(upos);
  }
  con_geom_max += 1;		// add one for sizing
  if((con_geom_min.x == INT_MAX) || (con_geom_min.y == INT_MAX))
    con_geom_min = 0;
  TwoDCoord con_geom = con_geom_max - con_geom_min;
  int n_cons = con_geom.Product();
  MatrixGeom geom;
  geom.SetGeom(2, con_geom.x, con_geom.y);
  String valname = GetChanName(cg, val_specs.size);
  AddMatrixChan(valname, VT_FLOAT, &geom);

  for(int j=0;j<n_cons;j++) {	// add blanks -- set them later
    ptrs.Add(NULL); members.Link(con_md);
  }
  for(int j=0; j<cg->cons.size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos = su->GetMyAbsPos() - con_geom_min;
    int idx = upos.y * con_geom.x + upos.x;
    ptrs[idx] = cg->Cn(j);	// set the ptr
  }
}

void NetMonItem::ScanObject_SendCons(SendCons* cg, String var) {
  if(!cg || !cg->prjn) return;
  MemberDef* con_md = cg->con_type->members.FindNameR(var);
  if(!con_md) return;		// can't find that var!

  // find the geometry span of the cons
  TwoDCoord con_geom_max;
  TwoDCoord con_geom_min(INT_MAX, INT_MAX);
  for(int j=0; j<cg->cons.size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos = su->GetMyAbsPos();
    con_geom_max.Max(upos);
    con_geom_min.Min(upos);
  }
  con_geom_max += 1;		// add one for sizing
  if((con_geom_min.x == INT_MAX) || (con_geom_min.y == INT_MAX))
    con_geom_min = 0;
  TwoDCoord con_geom = con_geom_max - con_geom_min;
  int n_cons = con_geom.Product();
  MatrixGeom geom;
  geom.SetGeom(2, con_geom.x, con_geom.y);
  String valname = GetChanName(cg, val_specs.size);
  AddMatrixChan(valname, VT_FLOAT, &geom);

  for(int j=0;j<n_cons;j++) {	// add blanks -- set them later
    ptrs.Add(NULL); members.Link(con_md);
  }
  for(int j=0; j<cg->cons.size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos = su->GetMyAbsPos() - con_geom_min;
    int idx = upos.y * con_geom.x + upos.x;
    ptrs[idx] = cg->Cn(j);	// set the ptr
  }
}

void NetMonItem::SetMonVals(TAPtr obj, const String& var) {
  if ((object == obj) && (variable == var)) return; 
  object = obj;
  variable = var;
  UpdateAfterEdit();
}

void NetMonItem::SmartRef_DataDestroying(taSmartRef* ref, taBase* obj) {
  ResetMonVals();
  inherited::SmartRef_DataDestroying(ref, obj);//does UAE
}

void NetMonItem::SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
    int dcr, void* op1_, void* op2_) 
{
//NOTE: we get these here when script running and any member vars are updated --
// don't update objects -- instead, we may want to use a RefList for the objects,
// and thus detect deletion there.
 // ScanObject();
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
  rval = md->GetValVar(obj);
  // pre-process.. 
  // note: NONE op leaves Variant in same format, otherwise converted to float
  pre_proc_3.EvaluateVar(pre_proc_2.EvaluateVar(pre_proc_1.EvaluateVar(rval)));
  return true;
}

void NetMonItem::GetMonVals(DataBlock* db) {
  if ((!db) || variable.empty())  return;

  int mon = 0; 
  //note: there should always be the exact same number of mons as items to set,
  // but in case of mismatch, the GetMonVal will return Invalid,
  Variant mbval;
  for (int ch = 0; ch < val_specs.size; ++ch) {
    ChannelSpec* cs = val_specs.FastEl(ch);
    if (cs->isMatrix()) {
      int vals = cs->cellGeom().Product();
      taMatrix* mat = db->GetSinkMatrix(cs->chan_num); // pre-ref'ed
      if (mat) {
        for (int j = 0; j < vals; ++j) {
          GetMonVal(mon++, mbval); // note: we don't care if not set, ie invalid
          mat->SetFmVar_Flat(mbval, j);
        }
        taBase::UnRef(mat);
      }
    } else { // scalar
      GetMonVal(mon++, mbval);
      db->SetData(mbval, cs->chan_num);
    }
  }
}


//////////////////////////
//  NetMonItem_List	//
//////////////////////////

String NetMonItem_List::GetColHeading(const KeyString& key) const {
  static String col_obj("Object Name");
  static String col_typ("Object Type");
  static String col_var("Variable");
  
  if (key == NetMonItem::key_obj_name) return col_obj;
  else if (key == NetMonItem::key_obj_type) return col_typ;
  else if (key == NetMonItem::key_obj_var) return col_var;
  else return inherited::GetColHeading(key); 
}


const KeyString NetMonItem_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return NetMonItem::key_obj_name;
  case 1: return NetMonItem::key_obj_type;
  case 2: return NetMonItem::key_obj_var;
  default: return _nilKeyString;
  }
}

//////////////////////////
//  NetMonitor		//
//////////////////////////

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
  if(!data) {
    if(!quiet) taMisc::CheckError("NetMonitor named:", name, "path:", GetPath(),
				  "data is NULL");
    rval = false;
  }
  if(!network) {
    if(!quiet) taMisc::CheckError("NetMonitor named:", name, "path:", GetPath(),
				  "network is NULL");
    rval = false;
  }
}

void NetMonitor::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  items.CheckConfig(quiet, rval);
}

void NetMonitor::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading || taMisc::is_duplicating) return;
  UpdateNetworkPtrs();
  UpdateMonitors();
}

String NetMonitor::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(data) rval += " to data: " + data->name;
  return rval;
}

void NetMonitor::AddObject(TAPtr obj, const String& variable) {
  // check for exact obj/variable already there, otherwise add one
  NetMonItem* nmi;
  for (int i = 0; i < items.size; ++i) {
    nmi = items.FastEl(i);
    if ((nmi->object == obj) && (nmi->variable == variable))
      return;
  }
  nmi = (NetMonItem*)items.New(1, &TA_NetMonItem);
  nmi->SetMonVals(obj, variable);
}

void NetMonitor::RemoveMonitors() {
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    nmi->ResetMonVals();
  }
}

void NetMonitor::SetDataTable(DataTable* dt) {
  data = dt; // note: auto does UAE
}

void NetMonitor::SetNetwork(Network* net) {
  network = net;
  UpdateNetworkPtrs();
}

void NetMonitor::UpdateNetworkPtrs() {
  if(network)
    items.UpdatePointers_NewParType(&TA_Network, network);
}

void NetMonitor::SetDataNetwork(DataTable* dt, Network* net) {
  SetNetwork(net);
  SetDataTable(dt);
}

void NetMonitor::UpdateMonitors(bool reset_first) {
  if (!data) return;
  if(reset_first)
    data->Reset();
  if (rmv_orphan_cols) 
    data->MarkCols();
  // this will probably be big, so wrap the whole thing
  data->StructUpdate(true);
  // (re)scan all the objects
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    nmi->ScanObject();
    nmi->val_specs.UpdateDataBlockSchema(data);
  }
  if (rmv_orphan_cols)
    data->RemoveOrphanCols();
  data->StructUpdate(false);
}

void NetMonitor::GetMonVals() {
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    nmi->GetMonVals(data);
  }
}


