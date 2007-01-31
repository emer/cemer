/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// so.cc

#include "so.h"

#include <QtPlugin>

const taVersion SoPlugin::version(1,0,0,0);

SoPlugin::SoPlugin(QObject*){}

int SoPlugin::InitializeTypes() {
  ta_Init_so();  // call the maketa-generated type initialization routine
  return 0;
}

int SoPlugin::InitializePlugin() {
  return 0;
}

const char* SoPlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

Q_EXPORT_PLUGIN2(so, SoPlugin)



//////////////////////////
//	Con,Spec	//
//////////////////////////

void SoConSpec::Initialize() {
  min_obj_type = &TA_SoRecvCons;
  lrate = 0.1f;
  wt_limits.min = 0.0f;
  wt_limits.max = 1.0f;
  wt_limits.type = WeightLimits::MIN_MAX;
  avg_act_source = LAYER_AVG_ACT;
  rnd.mean = .5;
}

void SoConSpec::InitLinks() {
  ConSpec::InitLinks();
}

void SoRecvCons::Initialize() {
  spec.SetBaseType(&TA_SoConSpec);
  avg_in_act = 0.0f;
}

void HebbConSpec::Initialize() {
  wt_limits.min = -1.0f;
  wt_limits.max = 1.0f;
}

//////////////////////////
//	Unit,Spec	//
//////////////////////////

void SoUnitSpec::Initialize() {
  min_obj_type = &TA_SoUnit;
}

void SoUnitSpec::Init_Acts(Unit* u) {
  inherited::Init_Acts(u);
  ((SoUnit*)u)->act_i = 0.0f;
}

void SoUnitSpec::Compute_Act(Unit* u) {
  // simple linear function
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = u->net;
}

void SoUnitSpec::Compute_AvgInAct(Unit* u) {
  SoRecvCons* recv_gp;
  taListItr g;
  FOR_ITR_EL(SoRecvCons, recv_gp, u->recv., g) {
    if(!recv_gp->prjn->from->lesion)
      recv_gp->Compute_AvgInAct(u);
  }
}

void ThreshLinSoUnitSpec::Initialize() {
  threshold = 0.0f;
}

void ThreshLinSoUnitSpec::Compute_Act(Unit* u) {
  if(u->ext_flag & Unit::EXT)
    u->act = u->ext;
  else
    u->act = (u->net > threshold) ? (u->net - threshold) : 0.0f;
}

void SoUnit::Initialize() {
  spec.SetBaseType(&TA_SoUnitSpec);
  act_i = 0.0f;
}

//////////////////////////
//	Layer,Spec	//
//////////////////////////

void SoLayerSpec::Initialize() {
  min_obj_type = &TA_SoLayer;
  netin_type = MAX_NETIN_WINS;	// competitive learning style
}

SoUnit* SoLayerSpec::FindMaxNetIn(SoLayer* lay) {
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;
  float max_val = -1.0e20;
  SoUnit* max_val_u = NULL;
  SoUnit* u;
  taLeafItr i;
  FOR_ITR_EL(SoUnit, u, lay->units., i) {
    u->act = uspec->act_range.min;
    u->act_i = uspec->act_range.min;
    if(u->net > max_val) {
      max_val_u = u;
      max_val = u->net;
    }
  }
  return max_val_u;
}

SoUnit* SoLayerSpec::FindMinNetIn(SoLayer* lay) {
  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;
  float min_val = 1.0e20;
  SoUnit* min_val_u = NULL;
  SoUnit* u;
  taLeafItr i;
  FOR_ITR_EL(SoUnit, u, lay->units., i) {
    u->act = uspec->act_range.min;
    u->act_i = uspec->act_range.min;
    if(u->net < min_val) {
      min_val_u = u;
      min_val = u->net;
    }
  }
  return min_val_u;
}

SoUnit* SoLayerSpec::FindWinner(SoLayer* lay) {
  if(netin_type == MAX_NETIN_WINS)
    return FindMaxNetIn(lay);
  return FindMinNetIn(lay);
}

// default layerspec just iterates over units
void SoLayerSpec::Compute_Netin(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_Netin();
}

void SoLayerSpec::Compute_Act(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_Act();
  
  Compute_AvgAct(lay);		// always compute average layer act..
}

void SoLayerSpec::Compute_dWt(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_dWt();
}

void SoLayerSpec::Compute_Weights(SoLayer* lay) {
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    u->Compute_Weights();
}

void SoLayerSpec::Compute_AvgAct(SoLayer* lay) {
  lay->sum_act = 0.0f;
  if(lay->units.leaves == 0)	return;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i)
    lay->sum_act += u->act;
  lay->avg_act = lay->sum_act / (float)lay->units.leaves;
}

void SoLayer::Copy_(const SoLayer& cp) {
  spec = cp.spec;
  avg_act = cp.avg_act;
  sum_act = cp.sum_act;
}

void SoLayer::Initialize() {
  spec.SetBaseType(&TA_SoLayerSpec);
  units.SetBaseType(&TA_SoUnit);
  unit_spec.SetBaseType(&TA_SoUnitSpec);
  avg_act = 0.0f;
  sum_act = 0.0f;
  winner = NULL;
}

void SoLayer::InitLinks() {
  Layer::InitLinks();
  spec.SetDefaultSpec(this);
}

void SoLayer::CutLinks() {
  spec.CutLinks();
  Layer::CutLinks();
  winner = NULL;
}

bool SoLayer::SetLayerSpec(LayerSpec* sp) {
  if(sp == NULL)	return false;
  if(sp->CheckObjectType(this))
    spec.SetSpec((SoLayerSpec*)sp);
  else
    return false;
  return true;
} 

//////////////////////////////////
//	Simple SoftMax		//
//////////////////////////////////

void SoftMaxLayerSpec::Initialize() {
  softmax_gain = 1.0f;
}

void SoftMaxLayerSpec::Compute_Act(SoLayer* lay) {
  if(lay->ext_flag & Unit::EXT) { // input layer
    SoLayerSpec::Compute_Act(lay);
    return;
  }

  SoUnitSpec* uspec = (SoUnitSpec*)lay->unit_spec.spec;

  float sum = 0.0f;
  Unit* u;
  taLeafItr i;
  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->Compute_Act();
    u->act = expf(softmax_gain * u->net); // e to the net
    sum += u->act;
  }

  FOR_ITR_EL(Unit, u, lay->units., i) {
    u->act = uspec->act_range.Project(u->act / sum);
    // normalize by sum, rescale to act range range
  }

  Compute_AvgAct(lay);
}


//////////////////////////
//  SoNetwork		//
//////////////////////////

void SoNetwork::Initialize() {
  layers.SetBaseType(&TA_SoLayer);
}


void SoNetwork::Compute_Act() {
  // compute activations in feed-forward fashion
  Layer* lay;
  taLeafItr l;
  FOR_ITR_EL(Layer, lay, layers., l) {
//TODO: NetIn
    lay->Compute_Netin();
#ifdef DMEM_COMPILE    
    lay->DMem_SyncNet();
#endif
    lay->Compute_Act();
  }
  taiMiscCore::RunPending();
}

void SoNetwork::Trial_Run() {
  DataUpdate(true);
  

  Compute_Act();

  // compute the delta - weight (only if not testing...)
//TODO: need to put the right criteria for 4.0, or maybe we don't even do this???
//  if((epoch_proc != NULL) && (epoch_proc->wt_update != EpochProcess::TEST))
    Compute_dWt();

  // weight update taken care of by the process
  DataUpdate(false);
}

bool SoNetwork::CheckBuild(bool quiet) {
  if (dmem_sync_level != DMEM_SYNC_LAYER) {
    dmem_sync_level = DMEM_SYNC_LAYER;
  }
  return inherited::CheckBuild(quiet);
}

//////////////////////////
//   SoProject	        //
//////////////////////////

void SoProject::Initialize() {
  networks.SetBaseType(&TA_SoNetwork);
}


/*TODO void SoUnitSpec::GraphActFun(GraphLog* graph_log, float min, float max) {
  if(graph_log == NULL) {
    graph_log = (GraphLog*) pdpMisc::GetNewLog(GET_MY_OWNER(Project), &TA_GraphLog);
    if(graph_log == NULL) return;
  }
  graph_log->name = name + ": Act Fun";
  DataTable* dt = &(graph_log->data);
  dt->Reset();
  dt->NewColFloat("netin");
  dt->NewColFloat("act");

  SoUnit un;

  float x;
  for(x = min; x <= max; x += .01f) {
    un.net = x;
    Compute_Act(&un);
    dt->AddBlankRow();
    dt->SetLastFloatVal(x, 0);
    dt->SetLastFloatVal(un.act, 1);
  }
  dt->UpdateAllRanges();
  graph_log->ViewAllData();
} */

