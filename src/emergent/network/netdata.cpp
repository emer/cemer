// Copyright, 1995-2007, Regents of the University of Colorado,
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



// netdata.cc


#include "netdata.h"
#include "ta_geometry.h"

//////////////////////
//   LayerDataEl    //
//////////////////////

void LayerDataEl::Initialize() {
  net_target = LAYER;
  data_cols = NULL;
  col_lookup = NULL;
  layer_group = NULL;
}

void LayerDataEl::Destroy() {
  CutLinks();
}

void LayerDataEl::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(col_lookup) {
    chan_name = col_lookup->name;
    taBase::SetPointer((taBase**)&col_lookup, NULL); // reset as soon as used -- just a temp guy!
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
  CheckError(chan_name.empty(), quiet, rval,
	     "chan_name is empty");
  CheckError(GetChanIdx(data) < 0, quiet, rval,
	     "channel/column named",chan_name, "not found in data:", data->name);
  if(net_target == LAYER) {
    Layer* lay = (Layer*)network->layers.FindLeafName(layer_name);
    if(CheckError(!lay, quiet, rval,
		  "cannot find layer named:",
		  layer_name, "in network:", network->name)) {
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

LayerDataEl* LayerDataEl_List::FindMakeChanName(const String& cnm, bool& made_new) {
  made_new = false;
  LayerDataEl* ld = FindChanName(cnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->chan_name = cnm;
  made_new = true;
  return ld;
}

LayerDataEl* LayerDataEl_List::FindLayerName(const String& lnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if(it->layer_name == lnm) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeLayerName(const String& lnm, bool& made_new) {
  made_new = false;
  LayerDataEl* ld = FindLayerName(lnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->layer_name = lnm;
  made_new = true;
  return ld;
}

LayerDataEl* LayerDataEl_List::FindLayerData(const String& cnm, const String& lnm) {
  for(int i = 0; i < size; ++i) {
    LayerDataEl* it = FastEl(i);
    if((it->layer_name == lnm) && (it->chan_name == cnm)) return it;
  }
  return NULL;
}

LayerDataEl* LayerDataEl_List::FindMakeLayerData(const String& cnm, const String& lnm,
						 bool& made_new) {
  made_new = false;
  LayerDataEl* ld = FindLayerData(cnm, lnm);
  if(ld) return ld;
  ld = (LayerDataEl*)New(1);
  ld->chan_name = cnm;
  ld->layer_name = lnm;
  made_new = true;
  return ld;
}


//////////////////////
//   LayerWriterEl    //
//////////////////////

void LayerWriterEl::Initialize() {
  use_layer_type = true;
  na_by_range = false;
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
    TestWarning(lay->layer_type == Layer::HIDDEN, "CheckConfig",
      "layer_type is HIDDEN -- not appropriate for writing to (by default). Turn use_layer_type off and set appropriate ext_flags if this is intentional.");
  }
}

String LayerWriterEl::GetDisplayName() const {
  String rval = inherited::GetDisplayName();
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
  if(TestWarning(!mat, "ApplyInputData",
		 "could not get matrix data from channel:",
		 chan_name, "in data:",db->name)) {
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

  // note: always provide all data, it is up to the network to decide how to use it
  // based on train_mode
  // get the data as a slice -- therefore, frame is always 0
  lay->ApplyInputData(mat, ext_flags, &noise, &offset, na_by_range);
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
  CheckError(!data, quiet, rval, "data is NULL");
  CheckError(!network, quiet, rval,"network is NULL");
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

void LayerWriter::AutoConfig(bool remove_unused) {
  if(TestError(!data || !network, "AutoConfig",
	       "Either the data or the network variables are not set -- please set them and try again!")) {
    return;
  }

  StructUpdate(true);

  for(int i=0;i<layer_data.size;i++) {
    layer_data.FastEl(i)->ClearBaseFlag(BF_MISC1); // use this for marking usage
  }

  bool made_new;
  Layer* lay;
  taLeafItr itr;
  FOR_ITR_EL(Layer, lay, network->layers., itr) {
    if(lay->layer_type == Layer::HIDDEN) continue;
    int chan_idx = data->GetSourceChannelByName(lay->name, false);
    if(TestWarning(chan_idx < 0, "AutoConfig",
		   "did not find data column for layer named:", lay->name, "of type:", TA_Layer.GetEnumString("LayerType", lay->layer_type))) {
      continue;	// not found
    }
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeLayerData(lay->name, lay->name, made_new);
    lrw->SetDataNetwork(data, network);
    lrw->DataChanged(DCR_ITEM_UPDATED);
    lrw->SetBaseFlag(BF_MISC1);	// mark as used
  }
  int nm_idx = data->GetSourceChannelByName("Name", false);
  if(nm_idx >= 0) {
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeChanName("Name", made_new);
    lrw->net_target = LayerDataEl::TRIAL_NAME;
    lrw->DataChanged(DCR_ITEM_UPDATED);
    lrw->SetBaseFlag(BF_MISC1);	// mark as used
  }
  int gp_idx = data->GetSourceChannelByName("Group", false);
  if(gp_idx >= 0) {
    LayerWriterEl* lrw = (LayerWriterEl*)layer_data.FindMakeChanName("Group", made_new);
    lrw->net_target = LayerDataEl::GROUP_NAME;
    lrw->DataChanged(DCR_ITEM_UPDATED);
    lrw->SetBaseFlag(BF_MISC1);	// mark as used
  }

  if(remove_unused) {
    for(int i=layer_data.size-1; i>=0; i--) {
      if(!layer_data.FastEl(i)->HasBaseFlag(BF_MISC1))
	layer_data.RemoveIdx(i);
    }
  }

  StructUpdate(false);
}

bool LayerWriter::ApplyInputData() {
  if(!data || !network) return false;
  if(TestError(!data->ReadAvailable(), "ApplyInputData",
	       "data not available for reading from datatable:", data->GetName(),
	       "you must perform a ReadItem or ReadFirst/Next on data to select row to read input patterns from (std Epoch program does this in NetDataLoop)")) {
    return false;
  }
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
  off = false;
  computed = false;
  object_type = NULL;
  lookup_var = NULL;
  variable = "act";
  name_style = AUTO_NAME;
  max_name_len = 6;
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
}

void NetMonItem::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
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
  if(obj->InheritsFrom(&TA_Network)) // special case
    rval += (var_label.empty() ? variable : var_label);
  else
    rval += GetObjName(obj) + "_" + (var_label.empty() ? variable : var_label);
  return rval;
}

void NetMonItem::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (off) return; // all stuff will run once turned on...
  if(computed) {
    name_style = MY_NAME;
    object = NULL;		// never have an obj for computed guy
    if(data_agg) {
      agg_col.SetDataTable(data_src);
      select_spec.SetDataTable(data_src);
    }
    else {
      data_src = NULL;
      agg_col.SetDataTable(NULL);
      select_rows = false;
      select_spec.SetDataTable(NULL);
    }
  }
  else {
    data_agg = false;		// make sure not displayed
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
	  name = GetAutoName(object);
	}
      }
    }
    else {			// AUTO_NAME = always update!
      name = GetAutoName(object);
    }
    name = taMisc::StringCVar(name);		// keep it clean for css var names
    ScanObject();
  }
}

String NetMonItem::GetObjName(taBase* obj) {
  if (!obj) return _nilString;

  // cases where default name is not what we want:

  if(obj->InheritsFrom(TA_Projection)) {
    Projection* prjn = (Projection*)obj;
    if(prjn->from.ptr() && prjn->layer) {
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
      int idx = ug->idx;
      if(idx >= 0)
	return nm + "_gp_" + String(idx) + "_";
    }
  }
  else if (obj->InheritsFrom(TA_RecvCons)) {
    RecvCons* cg = (RecvCons*)obj;
    if(cg->prjn && cg->prjn->from.ptr()) {
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
  String rval;
  if(name_style == MY_NAME) {
    if(col_idx == 0)
      rval = name;
    else
      rval = name + "_" + String(col_idx);
    rval = taMisc::StringCVar(rval); // keep it clean for css var names
    TestWarning(val_specs.FindNameIdx(rval) >= 0, "NetMonItem::GetChanName",
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
	  break;		// safe!
	}
      }
      TestWarning(max_name_len >= 50, "NetMonItem::GetChanName",
		  "Monitor item:",name,"with auto-name of:",rval,	
		  "is conflicting with another item of the same name, despite an attempt to increase the max_name_len up to 50 -- either manually incease further or fix underlying naming conflict");
    }
  }
  return rval;
}

MatrixChannelSpec* NetMonItem::AddMatrixChan(const String& valname, ValType vt,
					     const MatrixGeom* geom) 
{
  cell_num = 0;
  MatrixChannelSpec* cs;
  if(!computed && (agg.op != Aggregate::NONE)) {
    AddScalarChan(valname, vt);
    cs = (MatrixChannelSpec*)agg_specs.New(1, &TA_MatrixChannelSpec); // add to agg_specs!
  }
  else {
    // usual..
    cs = (MatrixChannelSpec*)val_specs.New(1, &TA_MatrixChannelSpec);
  }
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

ChannelSpec* NetMonItem::AddScalarChan_Agg(const String& valname, ValType vt) {
  cell_num = 0;//maybe should be 1!
  ChannelSpec* cs = (ChannelSpec*)agg_specs.New(1, &TA_ChannelSpec);
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
  agg_specs.RemoveAll();
  ptrs.Reset();
  members.RemoveAll();
}

void NetMonItem::ScanObject() {
  // TODO: what about orphaned columns in the sink?????
  ResetMonVals();
  if(computed) {
    if(matrix)
      AddMatrixChan(name, val_type, &matrix_geom);
    else
      AddScalarChan(name, val_type);
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
  } else {
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
    MemberDef* md = udi->FindMember("value"); // should exist!
    if(TestError(!md,"ScanObject_InUserData",
      "unexpected: member 'value' supposed to exist"))
      return true; //no mon, but we did handle it
    if (name_obj) {
      String valname = GetChanName(name_obj, val_specs.size);
      AddScalarChan(valname, VT_VARIANT);
      if(agg.op != Aggregate::NONE) {
        AddScalarChan_Agg(valname, VT_VARIANT); // add the agg guy just to keep it consistent
      }
    }
    // if not adding a column, it is part of a pre-allocated matrix; just add vars
    ptrs.Add(udi);
    members.Link(md);
    return true;
      
  } else {
    if(TestError(var.empty(),"ScanObject_InUserData",
      "UserDataItem: ", key, " expected to be non-simple; additional .xxx member resolution required after item key"))
      return true; //no mon, but we did handle it
    // descend...
    return ScanObject_InObject(udi, var, name_obj);
  }
  return false; // compiler food
}

bool NetMonItem::ScanObject_InObject(taBase* obj, String var, taBase* name_obj) {
  if (!obj) return false; 
  MemberDef* md = NULL;
  
  // first, try the recursive end, look for terminal member in ourself
  if (var.contains('.')) {
    String membname = var.before('.');
    // user data is a special case
    if (membname == "user_data") {
      return ScanObject_InUserData(obj, var.after("."), name_obj);
    }
    
    md = obj->FindMember(membname);
    //note: if memb not found, then we assume it is in an iterated subobj...
    if (!md) return false;
    
    if(TestError(!md->type->InheritsFrom(&TA_taBase),"ScanObject_InObject",
		 "can only monitor taBase objects, not: ", md->type->name, " var: ", var)) {
      return true; //no mon, but we did handle it
    }
    // we can only handle embedded objs and ptrs to objs
    taBase* ths = NULL;
    if (md->type->ptr == 0)
      ths = (taBase*) md->GetOff((void*)obj);
    else if (md->type->ptr == 1)
      ths = *((taBase**)md->GetOff((void*)obj));
    else {
      TestError(true, "ScanObject_InObject", "can only handle embedded taBase objects"
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
	if(agg.op != Aggregate::NONE) {
	  AddScalarChan_Agg(valname, vt); // add the agg guy just to keep it consistent
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

void NetMonItem::ScanObject_Network(Network* net, String var) {
  if (ScanObject_InObject(net, var, net)) return;

  taLeafItr itr;
  Layer* lay;
  FOR_ITR_EL(Layer, lay, net->layers., itr) {
    if(lay->lesioned()) continue;
    ScanObject_Layer(lay, var);
  }
}

void NetMonItem::ScanObject_Layer(Layer* lay, String var) {
  // check for projection monitor
  if(var.contains('.')) {
    if(var.contains('[')) {
      ScanObject_LayerUnits(lay, var);
      return;
    }

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
  if (geom.dims() == 1) {
    for (int i = 0; i < lay->units.leaves; ++i) {
      ScanObject_InObject(lay->units.Leaf(i), var, NULL); // don't make a col
    }
  } else if(geom.dims() == 2) {
    TwoDCoord c;
    for (c.y = 0; c.y < lay->un_geom.y; ++c.y) {
      for (c.x = 0; c.x < lay->un_geom.x; ++c.x) {
        Unit* u = lay->UnitAtCoord(c); // NULL if odd size or not built
        if(u) 
	  ScanObject_InObject(u, var, NULL); // don't make a col
      }
    }
  } else if(geom.dims() == 4) {
    TwoDCoord gc;
    for (gc.y = 0; gc.y < lay->gp_geom.y; ++gc.y) {
      for (gc.x = 0; gc.x < lay->gp_geom.x; ++gc.x) {
	TwoDCoord c;
	for (c.y = 0; c.y < lay->un_geom.y; ++c.y) {
	  for (c.x = 0; c.x < lay->un_geom.x; ++c.x) {
	    Unit* u = lay->UnitAtGpCoord(gc, c);
	    if(u) 
	      ScanObject_InObject(u, var, NULL); // don't make a col
	  }
	}
      }
    }
  }
}

void NetMonItem::ScanObject_LayerUnits(Layer* lay, String var) {
  String range2;
  String range1 = var.between('[', ']');
  String rmdr = var.after(']');
  if(rmdr.contains('[')) {
    range2 = rmdr.between('[', ']');
    rmdr = rmdr.after(']');
  }
  if(rmdr.startsWith('.')) rmdr = rmdr.after('.');

  String valname = GetChanName(lay, val_specs.size);
  MatrixGeom geom;

  if(range2.nonempty()) { // group case
    if(range1.contains('-')) {
      int gpidx1 = (int)range1.before('-');
      int gpidx2 = (int)range1.after('-');

      if(range2.contains('-')) {
	int unidx1 = (int)range2.before('-');
	int unidx2 = (int)range2.after('-');
	geom.SetGeom(2, 1+unidx2-unidx1, 1+gpidx2-gpidx1);
	AddMatrixChan(valname, VT_FLOAT, &geom);
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
	AddMatrixChan(valname, VT_FLOAT, &geom);
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
	AddMatrixChan(valname, VT_FLOAT, &geom);
	for (int i = unidx1; i < gp->size && i <= unidx2; ++i) {
	  ScanObject_InObject(gp->SafeEl(i), rmdr, NULL); // don't make a col
	}
      }
      else {
	int idx = (int)range2;
	AddScalarChan(valname, VT_FLOAT);
	ScanObject_InObject(gp->SafeEl(idx), rmdr, NULL); // don't make a col
      }
    }
  }
  else {			// just unit idxs
    if(range1.contains('-')) {
      int idx1 = (int)range1.before('-');
      int idx2 = (int)range1.after('-');
      geom.SetGeom(1, 1+idx2-idx1);
      AddMatrixChan(valname, VT_FLOAT, &geom);
      for (int i = idx1; i < lay->units.leaves && i <= idx2; ++i) {
	ScanObject_InObject(lay->units.Leaf(i), rmdr, NULL); // don't make a col
      }
    }
    else {
      int idx = (int)range1;
      String valname = GetChanName(lay, val_specs.size);
      AddScalarChan(valname, VT_FLOAT);
      ScanObject_InObject(lay->units.Leaf(idx), rmdr, NULL); // don't make a col
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
      for(int j=0; j<cg->size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos;  su->LayerLogPos(upos);
	con_geom_max.Max(upos);
	con_geom_min.Min(upos);
      }
    }
    else {			// send
      SendCons* cg = u->send.SafeEl(prjn->send_idx);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos; su->LayerLogPos(upos);
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
      for(int j=0; j<cg->size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos;  su->LayerLogPos(upos);
	upos -= con_geom_min;
	int idx = upos.y * con_geom.x + upos.x;
	ptrs[st_idx + idx] = cg->Cn(j);	// set the ptr
      }
    }
    else {			// send
      SendCons* cg = u->send.SafeEl(prjn->send_idx);
      if(!cg) continue;
      for(int j=0; j<cg->size; ++j) {
	Unit* su = cg->Un(j);
	if(!su) continue;
	TwoDCoord upos;  su->LayerLogPos(upos);
	upos -= con_geom_min;
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
    if(prjn->off || prjn->from->lesioned()) continue; // skip off or lesioned
    ScanObject_Projection(prjn, var);
  }
}

void NetMonItem::ScanObject_Projection(Projection* prjn, String var) {
  if (ScanObject_InObject(prjn, var, prjn)) return;
  
  Layer* lay = NULL;
  if (var.before('.') == "r") lay = prjn->layer;
  else if (var.before('.') == "s") lay = prjn->from;
  if(TestError(!lay, "ScanObject_Projection", "projection does not have layer's set or",
	       "selected var does not apply to connections")) {
    return;
  }
  ScanObject_PrjnCons(prjn, var);
}

void NetMonItem::ScanObject_UnitGroup(Unit_Group* ug, String var) {
  // check for projection monitor
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

  if (ScanObject_InObject(ug, var, ug)) return;

  // we now know it must be a regular unit variable (or invalid); do that
  MatrixGeom geom;
  if(ug->own_lay->un_geom.n_not_xy)
    geom.SetGeom(1, ug->size);	// irregular: flatten!
  else
    geom.SetGeom(2, ug->own_lay->un_geom.x, ug->own_lay->un_geom.y);
  
  String valname = GetChanName(ug, val_specs.size);
  AddMatrixChan(valname, VT_FLOAT, &geom);
  if(geom.dims() == 1) {
    for(int i = 0; i < ug->size; i++) {
      ScanObject_InObject(ug->FastEl(i), var, NULL); // don't make a col
    }
  }
  else {
    TwoDCoord c;
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
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos;  su->LayerLogPos(upos);
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
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos;  su->LayerLogPos(upos);
    upos -= con_geom_min;
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
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos;  su->LayerLogPos(upos);
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
  for(int j=0; j<cg->size; ++j) {
    Unit* su = cg->Un(j);
    if(!su) continue;
    TwoDCoord upos;  su->LayerLogPos(upos);
    upos -= con_geom_min;
    int idx = upos.y * con_geom.x + upos.x;
    ptrs[idx] = cg->Cn(j);	// set the ptr
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

void NetMonItem::GetMonVals_Agg(DataBlock* db) {
  if ((!db) || variable.empty() || computed)  return;

  if(TestError(agg_specs.size != val_specs.size, "GetMonVals_Agg",
	       "internal error: agg_specs.size != val_specs.size! -- report as bug!"))
    return;

  int mon = 0; 
  //note: there should always be the exact same number of mons as items to set,
  // but in case of mismatch, the GetMonVal will return Invalid,
  Variant mbval;
  for (int ch = 0; ch < val_specs.size; ++ch) {
    ChannelSpec* vcs = val_specs.FastEl(ch);
    ChannelSpec* acs = agg_specs.FastEl(ch);
    if (acs->isMatrix()) {
      int vals = acs->cellGeom().Product();
      agg_tmp_calc.SetGeom(1,vals);
      for (int j = 0; j < vals; ++j) {
	GetMonVal(mon++, mbval); // note: we don't care if not set, ie invalid
	agg_tmp_calc.SetFmVar_Flat(mbval, j);
      }
      mbval = taMath_float::vec_aggregate(&agg_tmp_calc, agg);
      db->SetData(mbval, vcs->chan_num);
    } else { // scalar
      GetMonVal(mon++, mbval);
      db->SetData(mbval, vcs->chan_num);
    }
  }
}

void NetMonItem::GetMonVals_DataAgg(DataBlock* db) {
  if(!db || !data_src || agg_col.col_name.empty())  return;

  if(TestError(val_specs.size != 1, "GetMonVals_DataAgg",
	       "internal error: val_specs.size != 1 for computed val -- report as bug!"))
    return;

  DataTable sel_out(false);
  bool use_sel_out = false;	// use sel_out instead of data_src

  if(select_rows && select_spec.col_name.nonempty()) {
    DataSelectSpec selspec(false);
    selspec.ops.Link(&select_spec);
    use_sel_out = taDataProc::SelectRows(&sel_out, data_src, &selspec);
    if(TestWarning(sel_out.rows == 0, "GetMonVals_DataAgg",
	       "select rows did not match any rows -- reverting to full data table"))
      use_sel_out = false;
  }

  ChannelSpec* vcs = val_specs.FastEl(0);
  DataCol* dc;
  if(use_sel_out)
    dc = sel_out.FindColName(agg_col.col_name);
  else
    dc = data_src->FindColName(agg_col.col_name);
  if(!dc) return;		// err in checkconfig
  if(!dc->isNumeric() || (dc->valType() == taBase::VT_BYTE)) return;

  Variant mbval;
  if(!matrix || !vcs->isMatrix() || !dc->isMatrix()) { // no matrix
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
    db->SetData(mbval, vcs->chan_num);
  }
  else {			// both src and dest are matrix
    int vals = vcs->cellGeom().Product();
    vals = MIN(vals, dc->cell_size());
    taMatrix* dmat = db->GetSinkMatrix(vcs->chan_num); // pre-ref'ed
    if(!dmat) return;				       // bail
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
  CheckError(!data, quiet, rval, "data is NULL");
  CheckError(!network, quiet, rval,"network is NULL");
}

void NetMonitor::CheckChildConfig_impl(bool quiet, bool& rval) {
  inherited::CheckChildConfig_impl(quiet, rval);
  items.CheckConfig(quiet, rval);
}

void NetMonitor::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading || taMisc::is_duplicating) return;
  UpdateNetworkPtrs();
  UpdateDataTable();
}

String NetMonitor::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(data) rval += " to data: " + data->name;
  return rval;
}

void NetMonitor::AddBlank() {
  items.New_gui(1);		// gui version
}

void NetMonitor::AddObject(taBase* obj, const String& variable) {
  // check for exact obj/variable already there, otherwise add one
  NetMonItem* nmi;
  for (int i = 0; i < items.size; ++i) {
    nmi = items.FastEl(i);
    if ((nmi->object.ptr() == obj) && (nmi->variable == variable))
      return;
  }
  nmi = (NetMonItem*)items.New_gui(1, &TA_NetMonItem); // use gui to update
  nmi->SetMonVals(obj, variable);
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
  ResetDataTableCols();		// this calls updatedatatable(false) too -- always make a clean update
//   UpdateDataTable(false);	// re-cache pointers after network setting
}

void NetMonitor::UpdateNetworkPtrs() {
  if(network) {
    items.UpdatePointers_NewParType(&TA_Network, network);
    for (int i = 0; i < items.size; ++i) {
      NetMonItem* nmi = items.FastEl(i);
      if(nmi->object_type && nmi->object_type->InheritsFrom(&TA_Network)
	 && nmi->object.ptr() != network) {
	nmi->object = network;
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
  if (rmv_orphan_cols) 
    data->MarkCols();
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    if (!nmi->off)
      nmi->ScanObject();
  }
  if (rmv_orphan_cols)
    data->RemoveOrphanCols(); // note: will remove 'off' items
  for (int i = 0; i < items.size; ++i) {
    NetMonItem* nmi = items.FastEl(i);
    if (!nmi->off)
      nmi->val_specs.UpdateDataBlockSchema(data);
  }
  data->StructUpdate(false);
}

void NetMonitor::ResetDataTableCols() {
  if (!data) return;
  data->StructUpdate(true);
  data->RemoveAllCols();
  UpdateDataTable(false);
  data->StructUpdate(false);
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



//////////////////////////
//  NetMonitor		//
//////////////////////////

void ActBasedRF::Initialize() {
  norm_mode = NORM_TRG_UNIT_RF_LAY;
  threshold = .5f;
}

void ActBasedRF::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void ActBasedRF::CheckThisConfig_impl(bool quiet, bool& rval) {
  inherited::CheckThisConfig_impl(quiet, rval);
  CheckError(!rf_data, quiet, rval, "rf_data is NULL");
  CheckError(!network, quiet, rval,"network is NULL");
  CheckError(!trg_layer, quiet, rval,"trg_layer is NULL");
}

String ActBasedRF::GetDisplayName() const {
  String rval = name;
  if(network) rval += " fm net: " + network->name;
  if(rf_data) rval += " to data: " + rf_data->name;
  if(trg_layer) rval += " trg lay: " + trg_layer->name;
  return rval;
}

void ActBasedRF::ConfigDataTable(DataTable* dt, Network* net) {
  dt->StructUpdate(true);
  dt->Reset();			// nuke cols -- ensure matching
  int rows = trg_layer->units.leaves;
  int idx;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, net->layers., li) {
    if(lay->lesioned()) continue;
    DataCol* da;
    if(lay->unit_groups)
      da = dt->FindMakeColName(lay->name, idx, VT_FLOAT, 4, lay->un_geom.x,
			  lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
    else
      da = dt->FindMakeColName(lay->name, idx, VT_FLOAT, 2, lay->un_geom.x,
			  lay->un_geom.y);
  }
  dt->EnforceRows(rows);
  dt->StructUpdate(false);
}

void ActBasedRF::InitData() {
  if(!network || !rf_data || !trg_layer) return;
  ConfigDataTable(rf_data, network);
  ConfigDataTable(&sum_data, network);
  wt_array.SetGeom(1, trg_layer->units.leaves);

  for(int i=0;i<rf_data->data.size; i++) {
    DataCol* da = rf_data->data.FastEl(i);
    ((float_Matrix*)da->AR())->InitVals();
  }
  for(int i=0;i<sum_data.data.size; i++) {
    DataCol* da = sum_data.data.FastEl(i);
    ((float_Matrix*)da->AR())->InitVals();
  }
  wt_array.InitVals();
  rf_data->UpdateAllViews();
}

void ActBasedRF::InitAll(DataTable* dt, Network* net, Layer* tlay) {
  rf_data = dt;
  network = net;
  trg_layer = tlay;

  if(!network || !rf_data || !trg_layer) return;
  rf_data->ResetData();
  sum_data.ResetData();
  InitData();
}

bool ActBasedRF::IncrementSums() {
  if(!network || !rf_data || !trg_layer) return false;

  int idx;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, network->layers., li) {
    if(lay->lesioned() || lay->Iconified()) continue;
    DataCol* sum_da = NULL;
    if(lay->unit_groups) {
      sum_da = sum_data.FindMakeColName(lay->name, idx, VT_FLOAT, 4, lay->un_geom.x,
					lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
    }
    else {
      sum_da = sum_data.FindMakeColName(lay->name, idx, VT_FLOAT, 2, lay->un_geom.x,
					lay->un_geom.y);
    }

    int tidx = 0;
    Unit* tu;
    taLeafItr tui;
    for(tu = trg_layer->units.FirstEl(tui); tu; tu = trg_layer->units.NextEl(tui), tidx++) {
      float tact = fabsf(tu->act);
      if(tact < threshold) continue; // not this time!

      wt_array.FastEl(tidx) += tact;

      float_Matrix* sum_mat = (float_Matrix*)sum_da->GetValAsMatrix(tidx);
      taBase::Ref(sum_mat);
      int sidx = 0;
      Unit* su;
      taLeafItr sui;
      for(su = lay->units.FirstEl(sui); su; su = lay->units.NextEl(sui), sidx++) {
	sum_mat->FastEl(sidx) += tu->act * su->act;
      }
      taBase::unRefDone(sum_mat);
    }
  }
  return true;
}

bool ActBasedRF::ComputeRF() {
  if(!rf_data) return false;
  if(TestError(rf_data->data.size != sum_data.data.size ||
	       sum_data.rows != wt_array.size, "ComputeRF", "data tables not the same size, do InitData and re-run!"))
    return false;

  // first, do the normalization
  for(int i=0;i<rf_data->data.size; i++) {
    DataCol* rf_da = rf_data->data.FastEl(i);
    DataCol* sum_da = sum_data.data.FastEl(i);
    
    for(int r=0;r<rf_data->rows; r++) {
      float wt = wt_array.FastEl(r);
      float sc = 1.0f;
      if(wt > 0.0f)
	sc = 1.0f / wt;
      float_Matrix* rf_mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
      float_Matrix* sum_mat = (float_Matrix*)sum_da->GetValAsMatrix(r);
      taBase::Ref(rf_mat);
      taBase::Ref(sum_mat);
      
      for(int j=0;j<rf_mat->size; j++) {
	rf_mat->FastEl(j) = sum_mat->FastEl(j) * sc;
      }

      taBase::unRefDone(rf_mat);
      taBase::unRefDone(sum_mat);
    }
  }

  int idx;
  
  switch(norm_mode) {
  case NO_NORM:
    break;
  case NORM_TRG_UNIT: {
    // per row, across cols
    for(int r=0;r<rf_data->rows; r++) {
      float max_val = 0.0f;
      for(int i=0;i<rf_data->data.size; i++) {
	DataCol* rf_da = rf_data->data.FastEl(i);
	float_Matrix* mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
	taBase::Ref(mat);
	float mx = taMath_float::vec_abs_max(mat, idx);
	max_val = MAX(mx, max_val);
	taBase::unRefDone(mat);
      }
      float sc = 1.0f;
      if(max_val > 0.0f)
	sc = 1.0f / max_val;
      for(int i=0;i<rf_data->data.size; i++) {
	DataCol* rf_da = rf_data->data.FastEl(i);
	float_Matrix* mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
	taBase::Ref(mat);
	taMath_float::vec_mult_scalar(mat, sc);
	taBase::unRefDone(mat);
      }
    }
    break;
  }
  case NORM_TRG_LAYER: {
    // over entire set of data
    float max_val = 0.0f;
    for(int i=0;i<rf_data->data.size; i++) {
      DataCol* rf_da = rf_data->data.FastEl(i);
      float mx = taMath_float::vec_abs_max((float_Matrix*)rf_da->AR(), idx);
      max_val = MAX(mx, max_val);
    }
    float sc = 1.0f;
    if(max_val > 0.0f)
      sc = 1.0f / max_val;
    for(int i=0;i<rf_data->data.size; i++) {
      DataCol* rf_da = rf_data->data.FastEl(i);
      taMath_float::vec_mult_scalar((float_Matrix*)rf_da->AR(), sc);
    }
    break;
  }
  case NORM_RF_LAY: {
    // per column
    for(int i=0;i<rf_data->data.size; i++) {
      DataCol* rf_da = rf_data->data.FastEl(i);
      taMath_float::vec_norm_abs_max((float_Matrix*)rf_da->AR());
    }
    break;
  }
  case NORM_TRG_UNIT_RF_LAY: {
    // per cell
    for(int r=0;r<rf_data->rows; r++) {
      for(int i=0;i<rf_data->data.size; i++) {
	DataCol* rf_da = rf_data->data.FastEl(i);
	float_Matrix* mat = (float_Matrix*)rf_da->GetValAsMatrix(r);
	taBase::Ref(mat);
	taMath_float::vec_norm_abs_max(mat);
	taBase::unRefDone(mat);
      }
    }
    break;
  }
  }

  rf_data->UpdateAllViews();
  return true;
}

bool ActBasedRF::CopyRFtoNetWtPrjn(int trg_unit_no) {
  if(!network || !rf_data || !trg_layer) return false;

  if(TestError(trg_unit_no >= rf_data->rows, "CopyRFtoNetWtPrjn", "trg_unit_no is greater than number of target units"))
    return false;

  int idx;
  Layer* lay;
  taLeafItr li;
  FOR_ITR_EL(Layer, lay, network->layers., li) {
    if(lay->lesioned() || lay->Iconified()) continue;
    DataCol* rf_da = NULL;
    if(lay->unit_groups) {
      rf_da = rf_data->FindMakeColName(lay->name, idx, VT_FLOAT, 4, lay->un_geom.x,
					lay->un_geom.y, lay->gp_geom.x, lay->gp_geom.y);
    }
    else {
      rf_da = rf_data->FindMakeColName(lay->name, idx, VT_FLOAT, 2, lay->un_geom.x,
					lay->un_geom.y);
    }

    int mx = MAX(lay->units.leaves, rf_da->cell_size());

    for(int i=0;i<mx; i++) {
      Unit* u = lay->units.Leaf(i);
      float rfv = rf_da->GetValAsFloatM(trg_unit_no, i);
      u->wt_prjn = rfv;
    }
  }
  network->UpdateAllViews();
  return true;
}
