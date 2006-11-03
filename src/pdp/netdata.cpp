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

void LayerDataEl::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
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
  if((net->context != Network::TRAIN) && (ext_flags & Unit::TARG))
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

void LayerWriter::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
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



//////////////////////////
//  NetMonItem		//
//////////////////////////

/* Data Type valus

Object Names

if object has a name, then return up to first 4 chars
else:
  Unit:
    * start with first 4 chars of LayerName
    * add linear index of Unit in its UnitGroup
    ex. inpu[23]
  UnitGroup:
    * start with first 4 chars of LayerName
    * add '.un'
    * if not root UG, then at its linear index in root
    ex.:
      outp.un   (for root UG)
      outp.un[2]  (for a nested UG)
  any other type:
    'no_nm' (literal)
  
Unit
  Single Variable (no .)
    * one mon
    * name like: unit.variable
    ex. myun.act
  Compound Variable
    s.* or r.* -- translate to 'recv.' and 'send.'
    if var is ConnectionGroup (recv or send)
      Scan the ConnectionGroup using the name after 
    else, allow 1 more level, ex. bias.X

ConnectionGroup
   For each CG:
     * get the MemberDef of el_type, for varname
     * get owning Unit u of CG
     * part name: ObjName(u) + [CG index in CG root]
     For each Connection:
       * link ptr, add MemberDef (same for all)
       * name = partname + [con idx]. + varname
       
Projection
   if member found based on variable, 
     then link that in
   else, required to be a Layer, either s. or r.:
   if r.* then use lay = proj.layer
   if s.* then use lay = proj.from
   * scan all Units in root UG of that layer
   
Layer
   Look for variable in Layer
   if variable is a x.y look for X in layer
   else, assume it is Unit variable
     * ScanUnits of Layer for the variable
     * if found, add GEOM X and Y opts to first dataitem
      
UnitGroup
   Look for variable in UG
   if variable is a x.y look for X in UG
   else, assume it is Unit variable
     * ScanUnits of UG for the variable
     * if found, add GEOM X and Y (of UG) opts to first dataitem
  
  
X: some member name
UNnm -- derived Unit name

 
Net Obj		Variable ex	Value Dimensionality
Unit		var		S
Unit		s.X r.X		[N] n in CG
Unit		bias.X		S
Projection	var		S
Projection	s.X or r.X	[N] n in to or from Layer.units
Layer		var (of lay)	S
Layer		var.X		S (obj is var)
Layer		var (of U's)	[Y X] (flat) XxY of Units
UnitGroup	var (of UG)	S
UnitGroup	var.X		S  (obj is var)
UnitGroup	var (of U's)	[Y X] XxY of UG.units
(note: CGs are only called internally)
ConGroup	var (of Cons)	[N] # Connections in leafs

ex. (in possible order of use case) 
Object		Var Memb of	Col Type; data
Unit		unit		type(var) (float, int, string)
Unit		con		float[c]
UnitGroup	unit		[y x]; float, unit.act
UnitGroup	ug		type(var)
Layer		con		YxX cols; float[c]
Layer		unit		type[Y X];  unit.act	
Layer		ug		type[gy gx]
Net	etc.

c - connection count
y x -- Unit dims of a UnitGroup (or simple Layer)
Y X -- (flat) Unit dims of a Layer
gy gx -- group dims in a compound layer
  b) Unit dims of a UnitGroup
  
Issues:
  * C is variable
  * any attempt at "funky" inner data rep schemes will likely cause
    lots of other complexity, such as graphing, matrix ops, etc.
Possible Solutions
  * split up outers so that C only has one inner

Assignment algorithm:
  geom.size = 0
  call outer object
  each object invokes more inner ones if necessary
  at the end of an object, do the following:
    increment the geometry
*/
//TODO:
// fix up the mon_vals to use appropriate geom
// fix up the Stats calc routine to use mon_vals

const String NetMonItem::DotCat(const String& lhs, const String& rhs) {
  if (lhs.empty()) return rhs;
  if (rhs.empty()) return lhs;
  STRING_BUF(rval, lhs.length() + rhs.length() + 1);
  rval.cat(lhs);
  if (!(lhs.matches('.', -1) || (rhs.matches('.'))))
    rval.cat('.');
  rval.cat(rhs);
  return rval;
}
  

const String NetMonItem::GetObjName(TAPtr obj, TAPtr own) {
  if (!obj) return _nilString;
/* Name schemas:
  Network: mynet
  Layer: mylay
  Unit_Group: 
    name: mylayer.myug
    anon: mylayer.units[N]
  Projection: myprjn
  Unit: 
    name: mylayer.myunit
    anon: mylayer[fx,fy]
  Con:
*/ 
  if (!own) own = obj->GetOwner(); // might still be null
  String nm = obj->GetName();
  
  //todo: maybe we could use the type name, plus a uniquifier...
  if (!own) goto exit; // can't figure anything more out
  
  // Layers and Projections
  if (obj->InheritsFrom(TA_Layer) && !nm.empty())
    goto exit;
  // if a Unit and names, use that instead of generic
  if (obj->InheritsFrom(TA_Unit)) {
    Unit* u = (Unit*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if (lay) {
      if (nm.empty()) {
        int index = ((Unit_Group*)u->owner)->Find(u);
        nm = String("[") + String(index) + "]";
      }
      nm = DotCat(lay->name, nm);;
   }
  } else if (obj->InheritsFrom(TA_Unit_Group)) {
    Unit_Group* ug = (Unit_Group*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if (lay) {
      String pfx = GetObjName(lay);
      if (nm.empty()) { // most likely case
        nm = own->GetPath(ug, lay); //note: 'own' valid if lay exists
      }
      nm = DotCat(pfx, nm);;
    }
  }
  else if (obj->InheritsFrom(TA_Con_Group)) {
    Con_Group* cg = (Con_Group*)obj;
    Unit* un = GET_OWNER(obj, Unit);
    // note: con groups are not named, will be rooted in send or recv
    if (un) {
     nm = own->GetPath(cg, un);  //note: 'own' valid if lay exists
     String pfx = GetObjName(un);
      nm = DotCat(pfx, nm);
    }
  }
  if (!nm.empty()) goto exit;
  
  // default is try to concat owner name with ours, assuming
  // we are a member
  // note: likely that obj itself has no owner member, so
  // we use the passed-in own to root our search
  if (own) {
    String pfx = GetObjName(own);
    // see if it is a member, otherwise just use its typename
    MemberDef* md = own->FindMember((void*)obj);
    if (!md)   md = own->FindMemberPtr((void*)obj);
    if (md) nm = md->name;
    else nm = obj->GetTypeDef()->name; 
    nm = DotCat(pfx, nm);;
  }
exit:  
  // strip leading .
  if (nm.matches('.'))
    nm = nm.after('.');
  // if no name, punt with type name
  else if (nm.empty())
    nm = obj->GetTypeDef()->name;
  return nm;
/*todo: see how far we get with above!
  // we try to provide full names, to avoid ambiguity in columns
  // if object has a name, we'll use that as a base, otherwise 
  // we'll try to give it a container-based name, ex. [2]
  
  if (obj->InheritsFrom(TA_Unit)) {
    Unit* u = (Unit*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if (lay) {
      nm = lay->name;
      int index = ((Unit_Group*)u->owner)->Find(u);
      nm += String("[") + String(index) + "]";
      return nm;
    }
  } else if (obj->InheritsFrom(TA_Unit_Group)) {
    Unit_Group* ug = (Unit_Group*)obj;
    Layer* lay = GET_OWNER(obj, Layer);
    if (lay) {
      nm = lay->name;
      if (nm.length() > 4) nm = nm.before(4);
      nm += ".un";
      if (ug != &(lay->units)) {
	int index = lay->units.gp.Find(ug);
	if (index >= 0)
	  nm += String("[") + String(index) + "]";
      }
      return nm;
    }
  }
  if (!own) own = obj->GetOwner();
  if (own) {
    // check for member object, if so, use member name
    MemberDef* md = own->FindMember((void*)own);
    if (!md) // try as ptr
      md = own->FindMemberPtr((void*)own);
    if (md) return md->name;
    
    // see if generic group member
    if (own->InheritsFrom(TA_taGroup_impl)) {
      nm = own->name;
  //obs 3.x      if (nm.length() > 4) nm = nm.before(4);
      int index = ((taGroup_impl*)own)->Find_(obj);
      nm += String("[") + String(index) + "]";
      return nm;
    } 
    if (own->InheritsFrom(TA_taList_impl)) {
      nm = own->name;
  //obs 3.x      if (nm.length() > 4) nm = nm.before(4);
      int index = ((taList_impl*)own)->Find_(obj);
      nm += String("[") + String(index) + "]";
      return nm;
    } 
  }
  return "no_nm"; */
}

void NetMonItem::Initialize() {
  object_type = NULL;
  member_var = NULL;
  variable = "act";
  real_val_type = VT_DOUBLE;
  cell_num  = 0;
}

void NetMonItem::InitLinks() {
  inherited::InitLinks();
  taBase::Own(val_specs,this);
  taBase::Own(ptrs,this);
  ptrs.SetBaseType(&TA_taBase);
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
  real_val_type = cp.real_val_type;
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

void NetMonItem::UpdateAfterEdit() {
  if(!owner) return;
  if(!object) return;
  object_type = object->GetTypeDef();
  if(member_var)
    variable = member_var->name;
  if(variable.empty()) return;
  
  if (!taMisc::is_loading) {
    // update name
    // we mod default name by appending .obj.varname
    // we manage part after .
    // user change default name but keep . to retain manage mode
    // user can assign non-typename name without . for no manage
    String suff = GetObjName(object);
    if (!suff.empty()) suff += '_';
    suff += variable;
    if (name.contains('.')) {
      name = name.through('.').cat(suff); //note: through first .
    } 
  //?? String altnm = AltTypeName();
    else if (name.contains(GetTypeDef()->name) ) {
      name.cat('.').cat(suff);
    }
    ScanObject();
  }

  inherited::UpdateAfterEdit();
}

bool NetMonItem::AddCellName(const String& cellname) {
  ChannelSpec* cs = val_specs.Peek();
  if (!cs) return false;
  
  if (cs->isMatrix()) {
    MatrixChannelSpec* mcs = (MatrixChannelSpec*)cs;
    // note: usually either allocated matrix, or growable [1]
    if (cell_num >= mcs->cell_names.size)
      mcs->cell_names.AddFrames(1); 
    mcs->cell_names.SetFmStr_Flat(cellname, cell_num++);
  } else { // scalar
    taMisc::Warning("Can't add cellname to scalar col: ",
      cellname, ", ", cs->GetName());
  }
  return true;
}

MatrixChannelSpec* NetMonItem::AddMatrixChan(const String& valname, ValType vt,
  const MatrixGeom* geom) 
{
  cell_num = 0;
  MatrixChannelSpec* cs = (MatrixChannelSpec*)val_specs.New(1, &TA_MatrixChannelSpec);
  cs->SetName(valname);
  cs->val_type = vt;
  cs->uses_cell_names = true;
  if (geom) {
    cs->cell_geom = *geom;
  } else {
    cs->cell_names.SetGeom(1, 0); //dynamic
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

bool NetMonItem::GetMonVal(int i, Variant& rval) {
  void* obj = NULL;
  MemberDef* md = NULL;
  if (i < ptrs.size) {
    obj = (void*)ptrs.FastEl(i);
    md = members.FastEl(i);
  }
  if (!md || !obj) {
    rval = _nilVariant;
    return false;
  }
  rval = md->GetValVar(obj);
  // pre-process.. 
  // note: NONE op leaves Variant in same format, otherwise converted to float
  // todo: is this evil or not?
//   pre_proc_3.EvaluateVar(pre_proc_2.EvaluateVar(pre_proc_1.EvaluateVar(rval)));
  return true;
}

void NetMonItem::ResetMonVals() {
  val_specs.RemoveAll();
  ptrs.RemoveAll();
  members.RemoveAll();
}

void NetMonItem::ScanObject() {
//TODO: what about orphaned columns in the sink?????
  ResetMonVals();
  if (!object) return;
  
  if (object->InheritsFrom(&TA_Unit)) 
    ScanObject_Unit((Unit*)object.ptr(), variable, NULL, true);
  else if (object->InheritsFrom(&TA_Layer)) 
    ScanObject_Layer((Layer*)object.ptr(), variable);
  else if (object->InheritsFrom(&TA_Unit_Group))
    ScanObject_UnitGroup((Unit_Group*)object.ptr(), variable, true);
  else if (object->InheritsFrom(&TA_Projection))
    ScanObject_Projection((Projection*)object.ptr(), variable);
  else if (object->InheritsFrom(&TA_Network))
    ScanObject_Network((Network*)object.ptr(), variable);
  else {
    // could be any type of object.ptr()
    ScanObject_InObject(object.ptr(), variable, true);			
  }
}

/*
  // new rule: you must provide a logically complete path?
  // or maybe, you *should* provide it, if not, there is a default search
  
  // what are special case:
  Layer: do 2d flat iteration of Units
  UG: do 2d iteration of Units
  Projection: ?? special treatment of s/r?
  Unit: ?? special treatment of s/r?
  // any . in name?
  y: 
    find subobject
    call ourself recursively on that obj
  
  n: 
    first, try flat check on obj itself
    else:
    do case on object:
    special, that we handle? ex Layer or UnitGroup
      call that special handler
    Group? Y: call(or do) the Group handler
    List? Y: call(or do) the list handler
    
Generic InObject(obj, var)
  does var contain .
  y: 
    first, do special handling of certain subobjs
    Layer & units. : do a flat 2d iter on Units "IterLayer"
      : strip 'units.' and fall through (does default)
    Unit & s. send., r. recv. : iter the congroup
    Prjn & s. from., r. layer : delegate to the appr Layer
    
    second, try looking up the subobject
    if find subobject
      return InObject(subobj, after. )
    
  n: 
    if find in ourself
      add value
      return true
  //try default subobject or subiteration
  type of obj:
  Layer: flat iter on the Units "IterLayer"
  UG: iter on the Units "IterUGs"
  Group: iter on the Lists, mark new group
  List: iter on the objects
  
  Prjn: iter on s or r cons
  
  
  return false
  
Generic patternDoObj (obj, var):
  does var contain .
  n: 
    if find in ourself
      add value
      exit
    //need to do our default iteration:
    call DoObj(default iter subobj, var)
  y: 
    if find subobject
      call DoObj(sub, var after .
    
*/
bool NetMonItem::ScanObject_InObject(TAPtr obj, String var, 
  bool mk_col, TAPtr own) 
{
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
    return ScanObject_InObject(ths, var, mk_col, obj);
  } else {
    // caller may not have passed owner, try to look it up
    if (!own) own = obj->GetOwner(); // might still be null
    md = obj->FindMember(var);
    if (md) {
//       String valname = GetObjName(obj, own) + String(".") + var;
      String valname = name;	// always use our name!
      if (mk_col) {
	// todo: maybe need a "DEFAULT" setting for val_type??
	ValType vt = ValTypeForType(md->type);
	if(vt == VT_FLOAT || vt == VT_DOUBLE)
	  vt = real_val_type;
        AddScalarChan(valname, vt);
      } else {
        AddCellName(valname);
      }
      ptrs.Link(obj);
      members.Link(md);
      return true;
    }
  }
  return false;
}

void NetMonItem::ScanObject_ConGroup(Con_Group* mcg, String var,
  Projection* p) 
{
  MemberDef* md;
//   String colname = GetObjName(mcg) + String(".") + var;
  String colname = name;	// always use name of mon, not pre-gen name!
  String unitname;
  //note: assume float, since really no other con types make sense, and using Variant
  // could be extremely wasteful since there are so many cons
  MatrixChannelSpec* cs = AddMatrixChan(colname, real_val_type);
  cs->cell_names.SetGeom(1, 0); //dynamic
  //NOTE: we expand the cell names of the spec on the fly
  int i;
  Con_Group* cg;
  String valname;
  FOR_ITR_GP(Con_Group, cg, mcg->, i) {
    if ((p) && (cg->prjn != p)) continue;
    md = cg->el_typ->members.FindNameR(var);
    if (!md) continue;
    Unit* u = GET_OWNER(cg,Unit);
    if (!u) continue;
    unitname = GetObjName(u) + String("[") + String(i) + "]";
    int j;
    for (j=0; j<cg->size; ++j) {
      Connection* c = (Connection *) cg->Cn(j);
      ptrs.Link(c);
      members.Link(md);
      valname = unitname.cat("[").cat(String(j)).cat("].").cat(var);
      AddCellName(valname);
    }
  }
  // if no units, then remove group, else update
  if (cs->cell_names.size == 0) {
    cs->Close();
  } else {
    cs->cell_geom = cs->cell_names.geom;
    cs->UpdateAfterEdit();
  }
}

void NetMonItem::ScanObject_Layer(Layer* lay, String var) {
  if (ScanObject_InObject(lay, var, true)) return;
  //  String valname = GetObjName(lay) + String(".") + var;
  String valname = name;
  // the default is to scan the units for the var
  // we make a chan spec assuming the var is on units, but we can delete it
  // we have to make a flat col if using a sparse geom
  MatrixGeom geom;
  if (lay->isSparse()) {
    geom.SetGeom(1, lay->numUnits());
  } else {
    geom.SetGeom(2, lay->flat_geom.x, lay->flat_geom.y);
  }
  //  MatrixChannelSpec* mcs = 
  AddMatrixChan(valname, real_val_type, &geom);
  int val_sz = val_specs.size; // lets us detect if new ones made
  Unit* u;
  // because we can have sparse unit groups as well as units
  // we have to scan flat when sparse
  if (geom.size == 1) {
    int i;
    for (i = 0; i < lay->units.leaves; ++i) {
      u = lay->units.Leaf(i); 
      ScanObject_Unit(u, var);
    }
  } else {
    TwoDCoord c;
    for (c.y = 0; c.y < lay->flat_geom.y; ++c.y) {
      for (c.x = 0; c.x < lay->flat_geom.x; ++c.x) {
        u = lay->FindUnitFmCoord(c); // NULL if odd size or not built
        if (!u) goto cont;
        ScanObject_Unit(u, var);
      }
    }
  }
cont:
  // if nested objs made chans, delete ours and mark a new group
  if (val_sz < val_specs.size) {
    val_specs.Remove(val_sz - 1);
    val_specs.FastEl(val_sz)->new_group_name = valname;
  }
  // if no vals scanned, delete ours
  else if (cell_num == 0) {
    val_specs.Remove(val_sz - 1);
  }
}

void NetMonItem::ScanObject_Network(Network* net, String var) {
  if (ScanObject_InObject(net, var, true)) return;
  
  taLeafItr itr;
  Layer* lay;
  FOR_ITR_EL(Layer, lay, net->layers., itr)
    ScanObject_Layer(lay, var);
}

void NetMonItem::ScanObject_Projection(Projection* prjn, String var) {
  if (ScanObject_InObject(prjn, var, true)) return;
  
  Layer* lay = NULL;
  if (var.before('.') == "r") lay = prjn->layer;
  else if (var.before('.') == "s") lay = prjn->from;
  if (lay == NULL) {
    taMisc::Error("NetMonItem Projection does not have layer's set or",
		   "selected var does not apply to connections");
    return;
  }
  int val_sz = val_specs.size; // for detecting chans added
  taLeafItr i;
  Unit* u;
  FOR_ITR_EL(Unit, u, lay->units., i)
    ScanObject_Unit(u, var, prjn);
  // if nested objs made chans, mark a new group
  if (val_sz < val_specs.size) {
    String valname = GetObjName(prjn);
    val_specs.FastEl(val_sz)->new_group_name = valname;
  } 
}

void NetMonItem::ScanObject_Unit(Unit* u, String var, 
  Projection* p, bool mk_col) 
{
  //InObject will handle direct membs and subojects, like biases etc.
  if (ScanObject_InObject(u, var, mk_col)) return;
  
  // otherwise, we only grok the special s. and r. indicating conns
  if (!var.contains('.')) return;
  String varname = var.before('.');
  if (varname=="r") varname = "recv";
  else if(varname=="s") varname = "send";
  else return;
  TAPtr ths = u;
  void* temp;
  //note: this should always succeed...
  MemberDef* md = u->FindMembeR(varname,temp);
  if ((md == NULL) || (temp == NULL)) return;
  ths = (TAPtr) temp; // embedded objects (not ptrs to)
  varname = var.after('.');
  int val_sz= val_specs.size; // for marking current spot
  if (ths->InheritsFrom(&TA_Con_Group)) {
    ScanObject_ConGroup((Con_Group *) ths, varname, p);
    // if nested objs made chans, mark a new group
    if (val_sz < val_specs.size) {
      String valname = GetObjName(u) + String(".") + var;
      val_specs.FastEl(val_sz)->new_group_name = valname;
    } 
    return;
  }
}

void NetMonItem::ScanObject_UnitGroup(Unit_Group* ug, String var, bool mk_col) {
  if (ScanObject_InObject(ug, var, mk_col)) return;
  
  String varname = var;
  // String valname = GetObjName(ug) + String(".") + var;
  String valname = name;
  
  // the default is to scan the units for the var
  MatrixChannelSpec* cs = NULL;
  if (mk_col) {
    MatrixGeom geom;
    if (ug->geom.Product() != ug->size) {
      geom.SetGeom(1, ug->size);
    } else {
      geom.SetGeom(2, ug->geom.x, ug->geom.y);
    }
    cs = AddMatrixChan(valname, real_val_type, &geom);
  }
  int val_sz = val_specs.size; // use to detect con vals made
  Unit* u;
  if (ug->geom.Product() != ug->size) {
    for (int i = 0; i < ug->size; ++i) {
      u = ug->FastEl(i); 
      if (!u) goto cont; // not supposed to happen!
      ScanObject_Unit(u, var);
    }
  } else {
    TwoDCoord c;
    for (c.y = 0; c.y < ug->geom.y; ++c.y) {
      for (c.x = 0; c.x < ug->geom.x; ++c.x) {
        u = ug->FindUnitFmCoord(c); 
        if (!u) goto cont; // not supposed to happen!
        ScanObject_Unit(u, var);
      }
    }
  }
cont:
  if (mk_col) {
    // if nested objs made chans, delete ours and mark a new group
    if (val_sz < val_specs.size) {
      val_specs.Remove(val_sz - 1);
      val_specs.FastEl(val_sz)->new_group_name = valname;
    }
    // if no vals scanned, delete ours
    else if (cell_num == 0) {
      val_specs.Remove(val_sz - 1);
    }
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
}

void NetMonItem::SmartRef_DataChanged(taSmartRef* ref, taBase* obj,
    int dcr, void* op1_, void* op2_) 
{
//NOTE: we get these here when script running and any member vars are updated --
// don't update objects -- instead, we may want to use a RefList for the objects,
// and thus detect deletion there.
 // ScanObject();
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
  old_network = NULL;
}

void NetMonitor::InitLinks() {
  inherited::InitLinks();
  taBase::Own(items, this);
  taBase::Own(data, this);
  taBase::Own(network, this);
}

void NetMonitor::CutLinks() {
  old_network = NULL;
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

void NetMonitor::UpdateAfterEdit() {
  if (taMisc::is_loading || taMisc::is_duplicating) return;
  if(network != old_network) {
    if(old_network != NULL) 
      UpdateNetwork(old_network, network);
    old_network = network;
  }
  UpdateMonitors();
  inherited::UpdateAfterEdit();
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
  Network* old_net = network.ptr();
  if(old_net && net && (old_net != net))
    UpdateNetwork(old_net, network);
  network = net;
}

void NetMonitor::UpdateNetwork(Network* old_net, Network* new_net) {
  items.UpdatePointers_NewPar(old_net, new_net);
  items.UpdatePointers_NewObj(old_net, new_net);
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


