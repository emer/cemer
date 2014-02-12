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

#include "Unit_Group.h"
#include <Network>
#include <NetMonitor>
#include <taFiler>
#include <MemberDef>
#include <float_Array>
#include <int_Array>
#include <taProject>
#include <DataTable>

#include <tabMisc>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(Unit_Group);


using namespace std;

void Unit_Group::Initialize() {
  own_lay = NULL;
  idx = -1;
}

void Unit_Group::InitLinks() {
  taGroup<Unit>::InitLinks();
  taBase::Own(pos, this);
  own_lay = GET_MY_OWNER(Layer);
}

void Unit_Group::CutLinks() {
  own_lay = NULL;
  idx = -1;
  taGroup<Unit>::CutLinks();
}

void Unit_Group::Copy_(const Unit_Group& cp) {
  pos = cp.pos;
  output_name = cp.output_name;
}

void Unit_Group::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if((own_lay == NULL) || (own_lay->own_net == NULL)) return;
}

Unit* Unit_Group::UnitAtCoord(int x, int y) {
  if(!own_lay) return NULL;
  if(x >= own_lay->un_geom.x) return NULL; // y will be caught by safe..
  int idx = y * own_lay->un_geom.x + x;
  return SafeEl(idx);
}

taVector2i Unit_Group::GpLogPos() {
  if(!own_lay) return pos;
  taVector2i rval;
  rval.y = idx / own_lay->gp_geom.x;
  rval.x = idx % own_lay->gp_geom.x;
  return rval;
}

void Unit_Group::AddRelPos(taVector3i& rel_pos) {
  // note: vastly most likely case is a flat root group of units...
  Layer* lay = dynamic_cast<Layer*>(owner);
  if (lay) {
    rel_pos += lay->pos;
    lay->AddRelPos(rel_pos);
  }
  else { // better be in a group then!
    Unit_Group* ugp = GET_MY_OWNER(Unit_Group);
    if (ugp) {
      rel_pos += ugp->pos;
      ugp->AddRelPos(rel_pos);
    }
  }
}

void Unit_Group::AddRelPos2d(taVector2i& rel_pos) {
  // note: vastly most likely case is a flat root group of units...
  Layer* lay = dynamic_cast<Layer*>(owner);
  if (lay) {
    rel_pos += lay->pos2d;
    lay->AddRelPos2d(rel_pos);
  }
  else { // better be in a group then!
    Unit_Group* ugp = GET_MY_OWNER(Unit_Group);
    if (ugp) {
      rel_pos += ugp->pos;
      ugp->AddRelPos2d(rel_pos);
    }
  }
}

void Unit_Group::MonitorVar(NetMonitor* net_mon, const String& variable) {
  if(!net_mon) return;
  net_mon->AddObject(this, variable);
}

void Unit_Group::Copy_Weights(const Unit_Group* src) {
  Unit* u, *su;
  taLeafItr i,si;
  for(u = (Unit*)FirstEl(i), su = (Unit*)src->FirstEl(si);
      (u) && (su);
      u = (Unit*)NextEl(i), su = (Unit*)src->NextEl(si))
  {
    u->Copy_Weights(su);
  }
}

void Unit_Group::SaveWeights_strm(ostream& strm, RecvCons::WtSaveFormat fmt) {
  strm << "<Ug>\n";
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    int lfi = u->GetMyLeafIndex();
    strm << "<UgUn " << lfi << " " << u->name << ">\n";
    u->SaveWeights_strm(strm, NULL, fmt);
    strm << "</UgUn>\n";
  }
  strm << "</Ug>\n";
}

int Unit_Group::LoadWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  String tag, val;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Ug", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "UgUn") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    int lfi = (int)val.before(' ');
    if(leaves > lfi) {
      Unit* u = Leaf(lfi);
      stat = u->LoadWeights_strm(strm, NULL, fmt, quiet);
    }
    else {
      stat = Unit::SkipWeights_strm(strm, fmt, quiet);
    }
    if(stat != taMisc::TAG_END) break;
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "UgUn", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  RecvCons::LoadWeights_EndTag(strm, "Ug", tag, stat, quiet);
  return stat;
}

int Unit_Group::SkipWeights_strm(istream& strm, RecvCons::WtSaveFormat fmt, bool quiet) {
  String val, tag;
  int stat = RecvCons::LoadWeights_StartTag(strm, "Ug", val, quiet);
  if(stat != taMisc::TAG_GOT) return stat;

  while(true) {
    stat = taMisc::read_tag(strm, tag, val);
    if(stat != taMisc::TAG_GOT) break;          // *should* break at TAG_END
    if(tag != "UgUn") { stat = taMisc::TAG_NONE;  break; } // bumping up against some other tag
    stat = Unit::SkipWeights_strm(strm, fmt, quiet);
    if(stat != taMisc::TAG_END) break;
    stat = taMisc::TAG_NONE;           // reset so EndTag will definitely read new tag
    RecvCons::LoadWeights_EndTag(strm, "UgUn", tag, stat, quiet);
    if(stat != taMisc::TAG_END) break;
  }
  RecvCons::LoadWeights_EndTag(strm, "Ug", tag, stat, quiet);
  return stat;
}

void Unit_Group::SaveWeights(const String& fname, RecvCons::WtSaveFormat fmt) {
  taFiler* flr = GetSaveFiler(fname, ".wts", true);
  if(flr->ostrm)
    SaveWeights_strm(*flr->ostrm, fmt);
  flr->Close();
  taRefN::unRefDone(flr);
}

int Unit_Group::LoadWeights(const String& fname, RecvCons::WtSaveFormat fmt, bool quiet) {
  taFiler* flr = GetLoadFiler(fname, ".wts", true);
  int rval = false;
  if(flr->istrm)
    rval = LoadWeights_strm(*flr->istrm, fmt, quiet);
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

void Unit_Group::TransformWeights(const SimpleMathSpec& trans) {
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    u->TransformWeights(trans);
  }
}

void Unit_Group::AddNoiseToWeights(const Random& noise_spec) {
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    u->AddNoiseToWeights(noise_spec);
  }
}

int Unit_Group::PruneCons(const SimpleMathSpec& pre_proc,
                        Relation::Relations rel, float cmp_val)
{
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    rval += u->PruneCons(pre_proc, rel, cmp_val);
  }
  return rval;
}

int Unit_Group::LesionCons(float p_lesion, bool permute) {
  int rval = 0;
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    rval += u->LesionCons(p_lesion, permute);
  }
  return rval;
}

int Unit_Group::LesionUnits(float p_lesion, bool permute) {
  int rval = 0;
  StructUpdate(true);
  UnLesionUnits();              // always start unlesioned
  if(permute) {
    rval = (int) (p_lesion * (float)leaves);
    if(rval == 0) return 0;
    int_Array ary;
    int j;
    for(j=0; j<leaves; j++)
      ary.Add(j);
    ary.Permute();
    ary.size = rval;
    ary.Sort();
    for(j=ary.size-1; j>=0; j--) {
      Unit* un = Leaf(ary.FastEl(j));
      un->Lesion();
//       un->DisConnectAll();
//       RemoveLeafEl(un);
    }
  }
  else {
    int j;
    for(j=leaves-1; j>=0; j--) {
      if(Random::ZeroOne() <= p_lesion) {
        Unit* un = (Unit*)Leaf(j);
        un->Lesion();
//         un->DisConnectAll();
//         RemoveLeafIdx(j);
        rval++;
      }
    }
  }
//   own_lay->units_lesioned = true;       // record that units were lesioned
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
  return rval;
}

void Unit_Group::UnLesionUnits() {
  StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    u->UnLesion();
  }
  StructUpdate(false);
  UpdtAfterNetModIfNecc();
}

void Unit_Group::UpdtAfterNetModIfNecc() {
  if(!own_lay || own_lay->InStructUpdate()) return;
  if(!own_lay->own_net || own_lay->own_net->InStructUpdate()) return;
  own_lay->own_net->UpdtAfterNetMod();
}

bool Unit_Group::UnitValuesToArray(float_Array& ary, const String& variable) {
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesToArray",
                 "Variable:", variable, "not found or not a float on units of type:",
                 el_typ->name)) {
    return false;
  }
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    float* val = (float*)md->GetOff((void*)u);
    ary.Add(*val);
  }
  return true;
}

bool Unit_Group::UnitValuesToMatrix(float_Matrix& mat, const String& variable) {
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesToMatrix",
                 "Variable:", variable, "not found or not a float on units of type:",
                 el_typ->name)) {
    return false;
  }
  if(mat.size < leaves) return false;
  int cnt=0;
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    float* val = (float*)md->GetOff((void*)u);
    mat.FastEl_Flat(cnt++) = *val;
  }
  return true;
}

bool Unit_Group::UnitValuesFromArray(float_Array& ary, const String& variable) {
  if(ary.size == 0) return false;
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesFromArray",
                 "Variable:", variable, "not found or not a float on units of type:",
                 el_typ->name)) {
    return false;
  }
  int cnt=0;
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    float* val = (float*)md->GetOff((void*)u);
    *val = ary[cnt++];
    if(cnt >= ary.size)
      break;
  }
  return true;
}

bool Unit_Group::UnitValuesFromMatrix(float_Matrix& mat, const String& variable) {
  if(mat.size == 0) return false;
  MemberDef* md = el_typ->members.FindName(variable);
  if(TestWarning(!md || !md->type->InheritsFrom(TA_float), "UnitValuesFromMatrix",
                 "Variable:", variable, "not found or not a float on units of type:",
                 el_typ->name)) {
    return false;
  }
  int cnt=0;
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    float* val = (float*)md->GetOff((void*)u);
    *val = mat.FastEl_Flat(cnt++);
    if(cnt >= mat.size)
      break;
  }
  return true;
}

DataTable* Unit_Group::VarToTable(DataTable* dt, const String& variable) {
  bool new_table = false;
  if (!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable(name + "_Var_" + variable, true);
    new_table = true;
  }

  Network* net = GET_MY_OWNER(Network);
  if(!net) return NULL;

  NetMonitor nm;
  taBase::Own(nm, this);
  nm.AddUnitGroup(this, variable);
  nm.items[0]->max_name_len = 20; // allow long names
  nm.SetDataNetwork(dt, net);
  nm.UpdateDataTable();
  dt->AddBlankRow();
  nm.GetMonVals();
  dt->WriteClose();
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

DataTable* Unit_Group::ConVarsToTable(DataTable* dt, const String& var1, const String& var2,
                                const String& var3, const String& var4, const String& var5,
                                const String& var6, const String& var7, const String& var8,
                                const String& var9, const String& var10, const String& var11,
                                const String& var12, const String& var13, const String& var14,
                                Projection* prjn) {
  bool new_table = false;
  if(!dt) {
    taProject* proj = GET_MY_OWNER(taProject);
    dt = proj->GetNewAnalysisDataTable("ConVars", true);
    new_table = true;
  }
  dt->StructUpdate(true);
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    u->ConVarsToTable(dt, var1, var2, var3, var4, var5, var6, var7, var8,
                      var9, var10, var11, var12, var13, var14,  prjn);
  }
  dt->StructUpdate(false);
  if(new_table)
    tabMisc::DelayedFunCall_gui(dt, "BrowserSelectMe");
  return dt;
}

bool Unit_Group::VarToVarCopy(const String& dest_var, const String& src_var) {
  MemberDef* dest_md = el_typ->members.FindName(dest_var);
  if(TestWarning(!dest_md || !dest_md->type->InheritsFrom(TA_float), "VarToVarCopy",
                 "Variable:", dest_var, "not found or not a float on units of type:",
                 el_typ->name)) {
    return false;
  }
  MemberDef* src_md = el_typ->members.FindName(src_var);
  if(TestWarning(!src_md || !src_md->type->InheritsFrom(TA_float), "VarToVarCopy",
                 "Variable:", src_var, "not found or not a float on units of type:",
                 el_typ->name)) {
    return false;
  }
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    *((float*)dest_md->GetOff((void*)u)) = *((float*)src_md->GetOff((void*)u));
  }
  return true;
}

bool Unit_Group::VarToVal(const String& dest_var, float val) {
  MemberDef* dest_md = el_typ->members.FindName(dest_var);
  if(TestWarning(!dest_md || !dest_md->type->InheritsFrom(TA_float), "VarToVarCopy",
                 "Variable:", dest_var, "not found or not a float on units of type:",
                 el_typ->name)) {
    return false;
  }
  FOREACH_ELEM_IN_GROUP(Unit, u, *this) {
    *((float*)dest_md->GetOff((void*)u)) = val;
  }
  return true;
}

Unit* Unit_Group::MostActiveUnit(int& idx) {
  idx = -1;
  if(leaves == 0) return NULL;
  Unit* max_un = Leaf(0);
  float max_act = max_un->act;
  for(int i=1;i<leaves;i++) {
    Unit* un = Leaf(i);
    if(un->act > max_act) {
      max_un = un;
      idx = i;
      max_act = max_un->act;
    }
  }
  return max_un;
}

bool Unit_Group::Dump_QuerySaveChildren() {
  if (!own_lay || !own_lay->own_net) return false; // huh? should always be valid...
  // always save if forced
  if (own_lay->own_net->HasNetFlag(Network::SAVE_UNITS_FORCE)) {
    return true;
  }
  // else arbitrate: true if layer says SAVE, or net says SAVE and we don't override
  return (own_lay->HasLayerFlag(Layer::SAVE_UNITS) ||
    (own_lay->own_net->HasNetFlag(Network::SAVE_UNITS)
     && !own_lay->HasLayerFlag(Layer::NO_SAVE_UNITS)));
}

taObjDiffRec* Unit_Group::GetObjDiffVal(taObjDiff_List& odl, int nest_lev, MemberDef* memb_def,
                           const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  // do NOT go below unit group in diffing!
  return taBase::GetObjDiffVal(odl, nest_lev, memb_def, par, par_typ, par_od);
}

