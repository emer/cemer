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

#include "LeabraXCALSpikeConSpec.h"
#include <taProject>
#include <DataGroupSpec>
#include <DataGroupEl>
#include <Aggregate>
#include <taDataProc>
#include <taDataAnal>

void XCALSpikeSpec::Initialize() {
  ss_sr = false;
  ca_norm = 5.0f;
  k_ca = 0.3f / ca_norm;
  ca_vgcc = 1.3f / ca_norm;
  ca_v_nmda = 0.0223f / ca_norm;
  ca_nmda = 0.5 / ca_norm;
  ca_dt = 20.0f;
  ca_rate = 1.0f / ca_dt;
  ca_off = 0.1f;
  nmda_dt = 40.0f;
  nmda_rate = 1.0f / nmda_dt;
}

void XCALSpikeSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  k_ca = 0.3f / ca_norm;
  ca_vgcc = 1.3f / ca_norm;
  ca_v_nmda = 0.0223f / ca_norm;
  ca_nmda = 0.5 / ca_norm;
  ca_rate = 1.0f / ca_dt;
  nmda_rate = 1.0f / nmda_dt;
}

void LeabraXCALSpikeConSpec::Initialize() {
  min_obj_type = &TA_LeabraSpikeCon;
}

void LeabraXCALSpikeConSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  xcal_spike.UpdateAfterEdit_NoGui();
}

void LeabraXCALSpikeConSpec::GraphXCALSpikeSim(DataTable* graph_data,
                                               LeabraUnitSpec* unit_spec,
                                               float rate_min, float rate_max, float rate_inc,
                                               float max_time, int reps_per_point,
                                               float lin_norm) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_XCALSpikeSim", true);
  }

  bool local_us = false;
  if(!unit_spec) {
    unit_spec = new LeabraUnitSpec;
    local_us = true;
  }

  String sim_data_name = name + "_XCALSpikeSim_Tmp";
  DataTable* sim_data = proj->GetNewAnalysisDataTable(sim_data_name, true);

  sim_data->StructUpdate(true);
//   graph_data->ResetData();
  int idx;

//   DataTable sim_data;
//   taBase::Ref(sim_data);
  DataCol* sim_r_rate = sim_data->FindMakeColName("r_rate", idx, VT_FLOAT);
  DataCol* sim_s_rate = sim_data->FindMakeColName("s_rate", idx, VT_FLOAT);
  DataCol* sim_ca_avg = sim_data->FindMakeColName("ca_avg", idx, VT_FLOAT);
  DataCol* sim_sravg_ss = sim_data->FindMakeColName("sravg_ss", idx, VT_FLOAT);
  DataCol* sim_sravg_s = sim_data->FindMakeColName("sravg_s", idx, VT_FLOAT);
  DataCol* sim_sravg_m = sim_data->FindMakeColName("sravg_m", idx, VT_FLOAT);
  DataCol* sim_srprod_s = sim_data->FindMakeColName("srprod_s", idx, VT_FLOAT);
  DataCol* sim_srprod_m = sim_data->FindMakeColName("srprod_m", idx, VT_FLOAT);
  DataCol* sim_sravg_lin = sim_data->FindMakeColName("sravg_lin", idx, VT_FLOAT);

  float s_rate, r_rate;
  for(r_rate = rate_min; r_rate <= rate_max; r_rate += rate_inc) {
    for(s_rate = rate_min; s_rate <= rate_max; s_rate += rate_inc) {
      for(int rep=0; rep < reps_per_point; rep++) {
        float nmda = 0.0f;
        float ca = 0.0f;
        float ca_avg = 0.0f;
        float ca_sum = 0.0f;
        float vmd = 0.0f;
        float r_p = r_rate / 1000.0f;
        float s_p = s_rate / 1000.0f;
        float time = 0.0f;
        bool s_act = false;
        bool r_act = false;
        float s_avg = 0.0f;
        float r_avg = 0.0f;
        float s_lin = 0.0f;
        float r_lin = 0.0f;
        float s_avg_s = 0.15f;
        float s_avg_m = 0.15f;
        float r_avg_s = 0.15f;
        float r_avg_m = 0.15f;
        float sravg_ss = 0.15f;
        float sravg_s = 0.15f;
        float sravg_m = 0.15f;
        for(time = 0.0f; time < max_time; time += 1.0f) {
          s_act = (bool)Random::Poisson(s_p);
          r_act = (bool)Random::Poisson(r_p);
          if(r_act) {
            vmd += unit_spec->spike_misc.vm_dend;
            r_avg += 1.0f;
          }
          vmd -= vmd / unit_spec->spike_misc.vm_dend_dt;
          float dnmda = -nmda * xcal_spike.nmda_rate;
          float dca = (nmda * (xcal_spike.ca_v_nmda * vmd + xcal_spike.ca_nmda))
            - (ca * xcal_spike.ca_rate);
          if(s_act) { s_avg += 1.0f; dnmda += xcal_spike.k_ca / (xcal_spike.k_ca + ca); }
          if(r_act) { dca += xcal_spike.ca_vgcc; }
          nmda += dnmda;
          ca += dca;
          ca_sum += ca;

          float sr = (ca - xcal_spike.ca_off); if(sr < 0.0f) sr = 0.0f;
          sravg_ss += unit_spec->act_avg.ss_dt * (sr - sravg_ss);
          sravg_s += unit_spec->act_avg.s_dt * (sravg_ss - sravg_s);
          sravg_m += unit_spec->act_avg.m_dt * (sravg_s - sravg_m);

          r_avg_s += unit_spec->act_avg.s_dt * ((float)r_act - r_avg_s);
          r_avg_m += unit_spec->act_avg.m_dt * (r_avg_s - r_avg_m);
          s_avg_s += unit_spec->act_avg.s_dt * ((float)s_act - s_avg_s);
          s_avg_m += unit_spec->act_avg.m_dt * (s_avg_s - s_avg_m);
        }
        ca_avg = ca_sum / max_time;
        s_lin *= lin_norm;
        r_lin *= lin_norm;
        float sravg_lin = s_avg * r_avg;

        float srprod_s = r_avg_s * s_avg_s;
        float srprod_m = r_avg_m * s_avg_m;

        sim_data->AddBlankRow();
        sim_r_rate->SetValAsFloat(r_rate, -1);
        sim_s_rate->SetValAsFloat(s_rate, -1);
        sim_ca_avg->SetValAsFloat(ca_avg, -1);
        sim_sravg_ss->SetValAsFloat(sravg_ss, -1);
        sim_sravg_s->SetValAsFloat(sravg_s, -1);
        sim_sravg_m->SetValAsFloat(sravg_m, -1);
        sim_srprod_s->SetValAsFloat(srprod_s, -1);
        sim_srprod_m->SetValAsFloat(srprod_m, -1);
        sim_sravg_lin->SetValAsFloat(sravg_lin, -1);
      }
    }
  }

  if(local_us) {
    delete unit_spec;
  }

  sim_data->StructUpdate(false);

  DataGroupSpec dgs;
  taBase::Ref(dgs);
  dgs.append_agg_name = false;
  //  dgs.SetDataTable(sim_data);
  dgs.AddAllColumns(sim_data);
  dgs.ClearColumns();

  ((DataGroupEl*)dgs.ops[0])->agg.op = Aggregate::GROUP; // r_rate
  ((DataGroupEl*)dgs.ops[1])->agg.op = Aggregate::GROUP; // s_rate
  for(int i=2; i< dgs.ops.size; i++)
    ((DataGroupEl*)dgs.ops[i])->agg.op = Aggregate::MEAN;

  taDataProc::Group(graph_data, sim_data, &dgs);
  dgs.ClearColumns();
  taDataAnal::Matrix3DGraph(graph_data, "s_rate", "r_rate");

  DataCol* gp_r_rate = graph_data->FindMakeColName("r_rate", idx, VT_FLOAT);
  gp_r_rate->SetUserData("X_AXIS", true);
  DataCol* gp_s_rate = graph_data->FindMakeColName("s_rate", idx, VT_FLOAT);
  gp_s_rate->SetUserData("Z_AXIS", true);
  DataCol* gp_sravg_m = graph_data->FindMakeColName("sravg_m", idx, VT_FLOAT);
  gp_sravg_m->SetUserData("PLOT_1", true);
  DataCol* gp_sravg_s = graph_data->FindMakeColName("sravg_s", idx, VT_FLOAT);
  gp_sravg_s->SetUserData("PLOT_2", true);
  DataCol* gp_srprod_m = graph_data->FindMakeColName("srprod_m", idx, VT_FLOAT);
  gp_srprod_m->SetUserData("PLOT_3", true);
  DataCol* gp_srprod_s = graph_data->FindMakeColName("srprod_s", idx, VT_FLOAT);
  gp_srprod_s->SetUserData("PLOT_4", true);

  graph_data->FindMakeGraphView();

  proj->data.RemoveLeafName(sim_data_name); // nuke it
}

