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


#include "leabra_v3_compat.h"


// todo: replace with comparable progs

// //////////////////////////
// // 	Max Da Stat	//
// //////////////////////////

// void LeabraMaxDa::Initialize() {
//   settle_proc = NULL;
//   min_layer = &TA_LeabraLayer;
//   min_unit = &TA_LeabraUnit;
//   net_agg.op = Aggregate::MAX;
//   has_stop_crit = true;
//   loop_init = INIT_START_ONLY;

//   da_type = INET_DA;
//   inet_scale = 1.0f;
//   lay_avg_thr = .01f;

//   da.stopcrit.flag = true;	// defaults
//   da.stopcrit.val = .005f;
//   da.stopcrit.cnt = 1;
//   da.stopcrit.rel = CritParam::LESSTHANOREQUAL;

//   trg_max_act.stopcrit.flag = false;	// defaults
//   trg_max_act.stopcrit.val = .85f;
//   trg_max_act.stopcrit.cnt = 1;
//   trg_max_act.stopcrit.rel = CritParam::GREATERTHANOREQUAL;
// }

// void LeabraMaxDa::UpdateAfterEdit() {
//   Stat::UpdateAfterEdit();
//   if(own_proc == NULL) return;
//   settle_proc = (LeabraSettle*)own_proc->FindProcOfType(&TA_LeabraSettle);
// }

// void LeabraMaxDa::CutLinks() {
//   settle_proc = NULL;
//   Stat::CutLinks();
// }

// void LeabraMaxDa::InitStat() {
//   da.InitStat(InitStatVal());
//   trg_max_act.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void LeabraMaxDa::Init() {
//   da.Init();
//   trg_max_act.Init();
//   Init_impl();
// }

// bool LeabraMaxDa::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   if(da.stopcrit.flag && trg_max_act.stopcrit.flag)
//     return da.Crit() && trg_max_act.Crit();
//   else if(da.stopcrit.flag)
//     return da.Crit();
//   else if(trg_max_act.stopcrit.flag)
//     return trg_max_act.Crit();
//   return false;
// }

// void LeabraMaxDa::Network_Init() {
//   InitStat();
// }

// void LeabraMaxDa::Unit_Stat(Unit* unit) {
//   LeabraUnit* lu = (LeabraUnit*)unit;
//   float fda;
//   if(da_type == DA_ONLY)
//     fda = fabsf(lu->da);
//   else if(da_type == INET_ONLY)
//     fda = fabsf(inet_scale * lu->I_net);
//   else {
//     LeabraLayer* blay = (LeabraLayer*)((Unit_Group*)unit->owner)->own_lay;
//     if(blay->acts.avg <= lay_avg_thr)
//       fda = fabsf(inet_scale * lu->I_net);
//     else
//       fda = fabsf(lu->da);
//   }
//   net_agg.ComputeAgg(&da, fda);
// }

// void LeabraMaxDa::Layer_Stat(Layer* lay) {
//   if(!(lay->ext_flag & Unit::TARG)) return;
//   LeabraLayer* llay = (LeabraLayer*)lay;
//   net_agg.ComputeAgg(&trg_max_act, llay->acts.max);
// }

// void LeabraMaxDa::Network_Stat() {
//   if(settle_proc == NULL) return;
//   int mincyc = settle_proc->min_cycles;
//   if(settle_proc->leabra_trial != NULL) {
//     if(settle_proc->leabra_trial->phase_no > 1)
//       mincyc = settle_proc->min_cycles_phase2;
//   }
//   if(settle_proc->cycle.val < mincyc) {
//     da.val = MAX(da.val, da.stopcrit.val + .01); // keep above thresh!
//   }
// }


// //////////////////////////
// //  	Ae Stat		//
// //////////////////////////

// void LeabraSE_Stat::Initialize() {
//   targ_or_comp = Unit::COMP_TARG;
//   trial_proc = NULL;
//   tolerance = .5;
//   no_off_err = false;
// }

// void LeabraSE_Stat::UpdateAfterEdit() {
//   SE_Stat::UpdateAfterEdit();
//   if(own_proc == NULL) return;
//   trial_proc = (LeabraNetwork*)own_proc->FindProcOfType(&TA_LeabraNetwork);
//   if(time_agg.real_stat == NULL)
//     return;
//   LeabraSE_Stat* rstat = (LeabraSE_Stat*)time_agg.real_stat;
//   // don't rename unless one or the other
//   if((rstat->targ_or_comp != Unit::TARG) && (rstat->targ_or_comp != Unit::COMP))
//     return;
//   MemberDef* md = GetTypeDef()->members.FindName("targ_or_comp");
//   name += String("_") + md->type->GetValStr(md->GetOff((void*)rstat), (void*)rstat, md);
// }

// void LeabraSE_Stat::NameStatVals() {
//   Stat::NameStatVals();
//   se.AddDispOption("MIN=0");
//   se.AddDispOption("TEXT");
//   if(time_agg.real_stat == NULL)
//     return;
//   LeabraSE_Stat* rstat = (LeabraSE_Stat*)time_agg.real_stat;
//   // don't rename unless one or the other
//   if((rstat->targ_or_comp != Unit::TARG) && (rstat->targ_or_comp != Unit::COMP))
//     return;
//   MemberDef* md = GetTypeDef()->members.FindName("targ_or_comp");
//   String vlnm = md->type->GetValStr(md->GetOff((void*)rstat), (void*)rstat, md);
//   vlnm.downcase();
//   se.name += String("_") + vlnm.at(0,4);
// }

// void LeabraSE_Stat::CutLinks() {
//   trial_proc = NULL;
//   SE_Stat::CutLinks();
// }

// void LeabraSE_Stat::Init() {
//   // don't init if it's not the right time!
//   if((time_agg.from == NULL) && (trial_proc != NULL)) {
//     // run targ-only comparisons in the first minus phase only
//     if((targ_or_comp == Unit::TARG) && (trial_proc->phase_no != 1))
//       return;
//     // run comp-only comparisons in the last minus phase only
//     if((targ_or_comp == Unit::COMP) && (trial_proc->phase_no != 3))
//       return;
//   }
//   SE_Stat::Init();
// }

// void LeabraSE_Stat::Network_Run() {
//   if(trial_proc != NULL) {
//     // run targ-only comparisons in the first minus phase only
//     if((targ_or_comp == Unit::TARG) && (trial_proc->phase_no != 1))
//       return;
//     // run comp-only comparisons in the last minus phase only
//     if((targ_or_comp == Unit::COMP) && (trial_proc->phase_no != 3))
//       return;
//   }
//   SE_Stat::Network_Run();
// }

// void LeabraSE_Stat::Unit_Stat(Unit* unit) {
//   if(!(unit->ext_flag & targ_or_comp))
//     return;
//   float act = ((LeabraUnit*)unit)->act_eq;
//   float tmp = 0.0f;
//   if(no_off_err) {
//     if(act > tolerance) {	// was active
//       if(unit->targ < tolerance) // shouldn't have been
// 	tmp = 1.0f;
//     }
//   }
//   else {
//     tmp = fabsf(act - unit->targ);
//     if(tmp >= tolerance)
//       tmp *= tmp;
//     else
//       tmp = 0.0f;
//   }
//   net_agg.ComputeAgg(&se, tmp);
// }

// ////////////////////////////////
// // 	LeabraGoodStat        //
// ////////////////////////////////

// void LeabraGoodStat::Initialize() {
//   min_layer = &TA_LeabraLayer;
//   min_unit = &TA_LeabraUnit;
//   net_agg.op = Aggregate::AVG;
//   subtr_inhib = false;
// }

// void LeabraGoodStat::Destroy() {
// }

// void LeabraGoodStat::Copy_(const LeabraGoodStat& cp) {
//   subtr_inhib = cp.subtr_inhib;
//   hrmny = cp.hrmny;
//   strss = cp.strss;
//   gdnss = cp.gdnss;
// }

// void LeabraGoodStat::InitStat() {
//   hrmny.InitStat(InitStatVal());
//   strss.InitStat(InitStatVal());
//   gdnss.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void LeabraGoodStat::Init() {
//   hrmny.Init();
//   strss.Init();
//   gdnss.Init();
//   Init_impl();
// }

// bool LeabraGoodStat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   if(hrmny.Crit())	return true;
//   if(strss.Crit())	return true;
//   if(gdnss.Crit())	return true;
//   return false;
// }

// void LeabraGoodStat::Network_Init() {
//   InitStat();
// }

// void LeabraGoodStat::Unit_Stat(Unit* un) {
//   LeabraUnit* lun = (LeabraUnit*) un;

//   float harm = lun->net;
//   if(subtr_inhib)
//     harm -= lun->gc.i;

//   net_agg.ComputeAggNoUpdt(hrmny.val, lun->act * harm);

//   float act_c = 1.0f - lun->act_eq;
//   float stress = 0.0f;
//   if(lun->act_eq > 0.0f) stress += lun->act_eq * logf(lun->act_eq);
//   if(act_c > 0.0f) stress += act_c * logf(act_c);
//   net_agg.ComputeAgg(strss.val, -stress);
// }

// void LeabraGoodStat::Network_Stat() {
//   // this is not aggregated, because it is the difference of two sum terms..
//   gdnss.val = hrmny.val - strss.val;
// }

// ////////////////////////////////
// // 	LeabraSharpStat        //
// ////////////////////////////////

// void LeabraSharpStat::Initialize() {
//   min_layer = &TA_LeabraLayer;
//   net_agg.op = Aggregate::AVG;
// }

// void LeabraSharpStat::Destroy() {
// }

// void LeabraSharpStat::Copy_(const LeabraSharpStat& cp) {
//   sharp = cp.sharp;
// }

// void LeabraSharpStat::InitStat() {
//   sharp.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void LeabraSharpStat::Init() {
//   sharp.Init();
//   Init_impl();
// }

// bool LeabraSharpStat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   if(sharp.Crit())	return true;
//   return false;
// }

// void LeabraSharpStat::Network_Init() {
//   InitStat();
// }

// void LeabraSharpStat::Layer_Stat(Layer* lay) {
//   LeabraLayer* llay = (LeabraLayer*)lay;

//   float val = 0.0f;
//   if(llay->acts.avg > 0.0f)
//     val = llay->acts.max / llay->acts.avg;

//   net_agg.ComputeAgg(sharp.val, val);
// }

// //////////////////////////
// // 	WrongOnStat	//
// //////////////////////////

// void WrongOnStat::Initialize() {
//   threshold = .5f;
//   trg_lay = NULL;
// }

// void WrongOnStat::InitLinks() {
//   Stat::InitLinks();
// }

// void WrongOnStat::CutLinks() {
//   Stat::CutLinks();
//   taBase::DelPointer((TAPtr*)&trg_lay);
// }

// void WrongOnStat::NameStatVals() {
//   Stat::NameStatVals();
//   wrng.AddDispOption("MIN=0");
//   wrng.AddDispOption("TEXT");
// }

// void WrongOnStat::InitStat() {
//   float init_val = InitStatVal();
//   wrng.InitStat(init_val);
//   InitStat_impl();
// }

// void WrongOnStat::Init() {
//   wrng.Init();
//   Init_impl();
// }

// bool WrongOnStat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   return wrng.Crit();
// }

// bool WrongOnStat::CheckLayerInNet() {
//   bool ok = Stat::CheckLayerInNet();
//   if(!ok)
//     return false;
//   if(trg_lay == NULL) return true;
//   if(trg_lay->own_net == network)
//     return true;

//   taMisc::Error("*** WrongOnStat:", name, "layer:", trg_lay->name,
// 		 "is not in current network, finding one of same name...");
//   Layer* nw_lay = (Layer*)network->layers.FindName(trg_lay->name);
//   if(nw_lay == NULL) {
//     taMisc::Error("==> could not find layer with same name in current network, aborting");
//     return false;
//   }
//   taBase::SetPointer((TAPtr*)&trg_lay, nw_lay);
//   return true;
// }
 
// void WrongOnStat::Network_Init() {
//   InitStat();
// }

// void WrongOnStat::Layer_Run() {
//   if(layer != NULL) {
//     if(!CheckLayerInNet()) return;
//     if(layer->lesion)      return;
//     if(trg_lay == NULL) {
//       taMisc::Error("*** WrongOnStat:", name, "trg_lay must be set!");
//       return;
//     }
//     Layer_Init(layer);
//     Unit_Run(layer);
//     Layer_Stat(layer);
//     return;
//   }
//   taMisc::Error("*** WrongOnStat:", name, "layer must be set!");
//   return;
// }

// void WrongOnStat::Unit_Run(Layer* lay) {
//   Unit* unit, *tu;
//   taLeafItr i, ti;
//   bool no_above_thr = true;
//   for(unit = (Unit*)lay->units.FirstEl(i), tu = (Unit*)trg_lay->units.FirstEl(ti);
//       (unit != NULL) && (tu != NULL);
//       unit = (Unit*)lay->units.NextEl(i), tu = (Unit*)trg_lay->units.NextEl(ti)) {
//     Unit_Init(unit);
//     float err = 0.0f;
//     if(unit->act > threshold) {
//       no_above_thr = false;
//       if(tu->act < threshold)
// 	err = 1.0f;
//     }
//     net_agg.ComputeAgg(&wrng, err);
//   }
//   if(no_above_thr)		// get a point off if nobody above threshold!
//     net_agg.ComputeAgg(&wrng, 1.0f);
// }

// ////////////////////////////////////////
// // 	LeabraPrjnRelNetinStat        //
// ////////////////////////////////////////

// void LeabraPrjnRelNetinStat::Initialize() {
//   min_layer = &TA_LeabraLayer;
//   min_con_group = &TA_LeabraCon_Group;
//   net_agg.op = Aggregate::LAST;
//   recv_act_thr = .1f;
// }

// void LeabraPrjnRelNetinStat::Destroy() {
// }

// void LeabraPrjnRelNetinStat::InitLinks() {
//   Stat::InitLinks();
//   taBase::Own(relnet, this);
// }

// void LeabraPrjnRelNetinStat::Copy_(const LeabraPrjnRelNetinStat& cp) {
//   relnet = cp.relnet;
// }

// void LeabraPrjnRelNetinStat::UpdateAfterEdit() {
//   Stat::UpdateAfterEdit();
// }

// void LeabraPrjnRelNetinStat::InitStat() {
//   relnet.InitStat(InitStatVal());
//   InitStat_impl();
// }

// void LeabraPrjnRelNetinStat::Init() {
//   if(loop_init == NO_INIT) return;
//   relnet.Init();
//   Init_impl();
// }

// bool LeabraPrjnRelNetinStat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   if(relnet.Crit())	return true;
//   return false;
// }

// String LeabraPrjnRelNetinStat::GetPrjnNm(const char* prjn_nm) {
//   String nm = prjn_nm;
//   if(nm.contains("Fm_"))
//     nm = nm.after("Fm_");
//   if(nm.length() > 4)
//     nm = nm.before(4);
//   return nm;
// }

// void LeabraPrjnRelNetinStat::NameStatVals() {
//   if(time_agg.from == NULL) {
//     if(layer == NULL) return;
//     String lnm = layer->name;
//     if(lnm.length() > 3) lnm = lnm.before(3);
//     lnm = String("rlnt_") + lnm + "_";
//     relnet.EnforceSize(layer->projections.size);
//     for(int i=0;i<layer->projections.size;i++) {
//       Projection* prjn = (Projection*)layer->projections[i];
//       String snm = lnm + GetPrjnNm(prjn->name);
//       if(i==0)
// 	((StatVal*)relnet[i])->SetFloatVecNm(snm, layer->projections.size);
//       else
// 	((StatVal*)relnet[i])->SetName(snm);
//     }
//   }
//   else {
//     Stat::NameStatVals();
//   }
// }

// void LeabraPrjnRelNetinStat::Network_Init() {
//   InitStat();
//   if(layer == NULL) {
//     taMisc::Error("LeabraPrjnRelNetinStat: Error -- you must set the layer to record from!");
//   }
// }

// void LeabraPrjnRelNetinStat::Layer_Stat(Layer* lay) {
//   LeabraLayer* llay = (LeabraLayer*)lay;

//   float_RArray net_avg;
//   float_RArray net_cnt;
//   net_avg.EnforceSize(layer->projections.size);
//   net_cnt.EnforceSize(layer->projections.size);

//   for(int i=0;i<layer->projections.size;i++) {
//     Projection* prjn = (Projection*)layer->projections[i];
    
//     LeabraUnit* u;
//     taLeafItr ui;
//     FOR_ITR_EL(LeabraUnit, u, llay->units., ui) {
//       if(u->act_eq < recv_act_thr) continue;
//       LeabraCon_Group* cg = (LeabraCon_Group*)u->recv.gp.SafeEl(prjn->recv_idx);
//       if(cg == NULL) continue;
//       float net = cg->Compute_Net(u);
//       cg->net = net;
//       net_avg[i] += net;
//       net_cnt[i] += 1.0f;
//     }
//   }

//   float sum_net = 0.0f;
//   for(int i=0;i<net_avg.size;i++) {
//     if(net_cnt[i] > 0)
//       net_avg[i] /= net_cnt[i];
//     sum_net += net_avg[i];
//   }
//   for(int i=0;i<net_avg.size;i++) {
//     if(sum_net > 0)
//       net_avg[i] /= sum_net;
//     StatVal* sv = (StatVal*)relnet[i];
//     net_agg.ComputeAggNoUpdt(sv->val, net_avg[i]);
//   }
//   net_agg.IncUpdt();
// }

// //////////////////////////////////
// // 	ExtRew_Stat		//
// //////////////////////////////////

// void ExtRew_Stat::Initialize() {
// }

// void ExtRew_Stat::NameStatVals() {
//   Stat::NameStatVals();
//   rew.AddDispOption("MIN=0");
//   rew.AddDispOption("TEXT");
// }

// void ExtRew_Stat::InitStat() {
//   float init_val = InitStatVal();
//   rew.InitStat(init_val);
//   InitStat_impl();
// }

// void ExtRew_Stat::Init() {
//   if(loop_init == NO_INIT) return;
//   if(time_agg.from != NULL) {	// no init for real stats!!
//     rew.Init();
//     Init_impl();
//   }
// }

// bool ExtRew_Stat::Crit() {
//   if(!has_stop_crit)    return false;
//   if(n_copy_vals > 0)   return copy_vals.Crit();
//   return rew.Crit();
// }

// void ExtRew_Stat::Network_Run() {
//   if(network == NULL) return;
//   LeabraLayer* er_lay = NULL;
//   LeabraLayer* lay;
//   taLeafItr i;
//   FOR_ITR_EL(LeabraLayer, lay, network->layers., i) {
//     if(lay->lesion || !lay->spec.spec->InheritsFrom(&TA_ExtRewLayerSpec))
//       continue;
//     er_lay = lay;
//     break;
//   }
//   if(er_lay == NULL) return;
//   LeabraUnit* eru = (LeabraUnit*)er_lay->units.Leaf(0);
//   if(eru->misc_1 == 0.0f) { // indication of no reward available
//     rew.val = -1.1f;
//   }
//   else {
//     rew.val = eru->act_eq;	// just set it!
//   }
// }

// void ExtRew_Stat::ComputeAggregates() {
//   if(time_agg.from == NULL)
//     return;

//   ExtRew_Stat* fms = (ExtRew_Stat*)time_agg.from;
//   if(fms->rew.val == -1.1f) return; // don't agg no reward cases!!
//   time_agg.ComputeAggNoUpdt(&rew, &(fms->rew));
//   time_agg.IncUpdt();
// }

//////////////////////////////////////////
// 	Phase-Order Environment		//
//////////////////////////////////////////

// void PhaseOrderEventSpec::Initialize() {
//   phase_order = MINUS_PLUS;
// }

// void PhaseOrderEventSpec::Copy_(const PhaseOrderEventSpec& cp) {
//   phase_order = cp.phase_order;
// }
