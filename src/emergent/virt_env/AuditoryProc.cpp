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

#include "AuditoryProc.h"

#include <taMath_float>
#include <taSound>
#include <DataTable>
#include <taProject>

TA_BASEFUNS_CTORS_DEFN(AudInputSpec);
TA_BASEFUNS_CTORS_DEFN(MelFreqSpec);
TA_BASEFUNS_CTORS_DEFN(AuditoryProc);

void AudInputSpec::Initialize() {
  win_msec = 25.0f;
  step_msec = 12.5f;
  trial_msec = 100.0f;
  sample_rate = 16000;
  channels = 1;
  channel = 0;
  ComputeSamples();
}

void AudInputSpec::ComputeSamples() {
  win_samples = (int)taMath_float::round(win_msec * 0.001f * (float)sample_rate);
  step_samples = (int)taMath_float::round(step_msec * 0.001f * (float)sample_rate);
  trial_samples = (int)taMath_float::round(trial_msec * 0.001f * (float)sample_rate);
  trial_steps = (int)taMath_float::round(trial_msec / (float)step_msec);
}

void AudInputSpec::InitFromSound(taSound* sound, int n_chans, int chan) {
  if(TestError(!sound || !sound->IsValid(), "InitFromSound",
               "sound object NULL or not valid")) {
    return;
  }
  sample_rate = sound->SampleRate();
  ComputeSamples();
  if(n_chans < 1) {
    channels = sound->ChannelCount();
  }
  else {
    channels = MIN(n_chans, sound->ChannelCount());
  }
  if(channels > 1)
    channel = chan;
  else
    channel = 0;
}

void AudInputSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ComputeSamples();
}



void MelFreqSpec::Initialize() {
  on = true;
  lo_hz = 300.0f;
  hi_hz = 8000.0f;
  n_filters = 26;
  lo_mel = FreqToMel(lo_hz);
  hi_mel = FreqToMel(hi_hz);
}

void MelFreqSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lo_mel = FreqToMel(lo_hz);
  hi_mel = FreqToMel(hi_hz);
}


//////////////////////////////////////////

void AuditoryProc::Initialize() {
  save_mode = FIRST_ROW;
  input_save = NO_SAVE;
  mfcc_on = true;
  input_pos = 0;
}

void AuditoryProc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateConfig();
}

void AuditoryProc::UpdateConfig() {
  input.UpdateAfterEdit_NoGui();
  mel.UpdateAfterEdit_NoGui();
}

bool AuditoryProc::Init() {
  UpdateConfig();
  InitFilters();
  InitOutMatrix();
  InitDataTable();
  return true;
}

void AuditoryProc::InitFromSound(taSound* sound, int n_chans, int chan) {
  input.InitFromSound(sound, n_chans, chan);
}

bool AuditoryProc::InitFilters() {
  dft_size = input.win_samples;
  dft_use = dft_size / 2 + 1;
  InitFilters_Mel();
  return true;
}

bool AuditoryProc::InitFilters_Mel() {
  mel_n_filters_eff = mel.n_filters + 2;
  mel_pts_mel.SetGeom(1, mel_n_filters_eff);
  mel_pts_hz.SetGeom(1, mel_n_filters_eff);
  mel_pts_bin.SetGeom(1, mel_n_filters_eff);

  float mel_inc = (mel.hi_mel - mel.lo_mel) / (float)(mel.n_filters-1);

  for(int idx = 0; idx < mel_n_filters_eff; idx++) {
    float ml = mel.lo_mel + (float)idx*mel_inc;
    float hz = mel.MelToFreq(ml);
    int bin = mel.FreqToBin(hz, dft_use, input.sample_rate);
    mel_pts_mel[idx] = ml;
    mel_pts_hz[idx] = hz;
    mel_pts_bin[idx] = bin;
  }

  mel_filt_max_bins = mel_pts_bin[mel_n_filters_eff-1] - mel_pts_bin[mel_n_filters_eff-3] + 1;
  mel_filters.SetGeom(2, mel_filt_max_bins, mel.n_filters);

  for(int idx = 1; idx <= mel.n_filters; idx++) {
    int mxbin = mel_pts_bin[idx + 1];
    int mnbin = mel_pts_bin[idx - 1];
    int pkbin = mel_pts_bin[idx];
    float pkmin = (float)(pkbin - mnbin);
    float pkmax = (float)(mxbin - pkbin);
    int fi = 0;
    int bin;
    for(bin = mnbin; bin <= pkbin; bin++, fi++) {
      float fval = (float)(bin - mnbin) / pkmin;
      mel_filters.FastEl2d(fi, idx) = fval;
    }
    for(; bin <= mxbin; bin++, fi++) {
      float fval = (float)(mxbin - bin) / pkmax;
      mel_filters.FastEl2d(fi, idx) = fval;
    }
  }
  return true;
}

bool AuditoryProc::InitOutMatrix() {
  window_in.SetGeom(1, input.win_samples);
  dft_out.SetGeom(2, 2, dft_size);
  dft_power_out.SetGeom(1, dft_use);
  mel_fbank_out.SetGeom(1, mel.n_filters);
  mfcc_dct_out.SetGeom(1, n_mfcc);
  return true;
}

bool AuditoryProc::LoadSound(taSound* sound) {
  if(TestError(!sound || !sound->IsValid(), "LoadSound",
               "sound object NULL or not valid")) {
    return false;
  }
  if(input.channels > 1) {
    sound->SoundToMatrix(sound_full, -1);
  }
  else {
    sound->SoundToMatrix(sound_full, input.channel);
  }
  input_pos = 0;
  return true;
}

int AuditoryProc::InputStepsLeft() {
  int samples_left = sound_full.Frames() - input_pos;
  return samples_left / input.step_samples;
}

bool AuditoryProc::ProcessTrial() {
  if(TestError(InputStepsLeft() < 1, "ProcessTrial",
               "no steps worth of input sound available -- load a new sound")) {
    return false;
  }
  for(int step = 0; step < input.trial_steps; step++) {
    ProcessStep(step);
  }
  return true;
}

bool AuditoryProc::ProcessStep(int step) {
  for(int chan=0; chan < input.channels; chan++) {
    SoundToWindow(input_pos, chan);
    FilterWindow();
    OutputToTable(chan, step);
  }
  input_pos += input.step_samples;
  return true;
}

bool AuditoryProc::SoundToWindow(int in_pos, int chan) {
  int samp_avail = sound_full.Frames() - in_pos;
  int samp_cpy = MIN(samp_avail, input.win_samples);

  if(samp_cpy > 0) {
    int sz = samp_cpy * sizeof(float);
    if(sound_full.dims() == 1) {
      memcpy(window_in.el, sound_full.el + in_pos, sz);
    }
    else {
      memcpy(window_in.el, (void*)&(sound_full.FastEl2d(chan, in_pos)), sz);
    }
  }

  // pad remainder with zero
  int zero_n = input.win_samples - samp_cpy;
  if(zero_n > 0) {
    int sz = zero_n * sizeof(float);
    memset(window_in.el + samp_cpy, 0, sz);
  }
  return true;
}

bool AuditoryProc::FilterWindow() {
  DftInput();
  if(mel.on) {
    PowerOfDft();
    MelFilterDft();
    if(mfcc_on) {
      // todo
    }
  }
  return true;
}

void AuditoryProc::DftInput() {
  taMath_float::fft_real(&dft_out, &window_in);
}

void AuditoryProc::PowerOfDft() {
  // Mag() is absolute value   SqMag is square of it - r*r + i*i
  for(int i=0; i<dft_use; i++) {
    float r = dft_out.FastEl_Flat(2*i);
    float j = dft_out.FastEl_Flat(2*i+1);
    dft_power_out.FastEl_Flat(i) = r*r + j*j;
  }
}

void AuditoryProc::MelFilterDft() {
  int mi = 0;
  for(int idx = 1; idx <= mel.n_filters; idx++, mi++) {
    int mxbin = mel_pts_bin[idx + 1];
    int mnbin = mel_pts_bin[idx - 1];

    float sum = 0.0f;
    int fi=0;
    for(int bin = mnbin; bin <= mxbin; bin++, fi++) {
      const float fval = mel_filters.FastEl2d(fi, mi);
      const float pval = dft_power_out.FastEl_Flat(bin);
      sum += fval * pval;
    }
    mel_fbank_out.FastEl_Flat(mi) = logf(1.0f + sum); // todo: 1.0 + or not!?
  }
}


bool AuditoryProc::InitDataTable() {
  if(!data_table) {
    return false;
  }
  if(input.channels > 1) {
    for(int chan = 0; chan < input.channels; chan++) {
      InitDataTable_chan(chan);
    }
  }
  else {
    InitDataTable_chan(input.channel);
  }
  return true;
}

bool AuditoryProc::InitDataTable_chan(int chan) {
  if(mel.on) {
    MelOutputToTable(data_table, chan, 0, true);
  }
  return true;
}

bool AuditoryProc::OutputToTable(int chan, int step) {
  if(!data_table) {
    return false;
  }
  if(mel.on) {
    MelOutputToTable(data_table, chan, step, false); // not fmt_only
  }
  return true;
}

bool AuditoryProc::MelOutputToTable(DataTable* dtab, int chan, int step, bool fmt_only) {
  DataCol* col;
  int idx;
  String col_sufx;
  if(input.channels > 1) {
    col_sufx = "_ch" + String(chan);
  }
  col = data_table->FindMakeColName(name + "_dft_pow" + col_sufx, idx,
                                    DataTable::VT_FLOAT, 2,
                                    input.trial_steps, dft_use);
  if(!fmt_only) {
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    for(int i=0; i< dft_use; i++) {
      dout->FastEl2d(step, i) = dft_power_out.FastEl_Flat(i);
    }
  }

  col = data_table->FindMakeColName(name + "_mel_fbank" + col_sufx, idx,
                                    DataTable::VT_FLOAT, 2,
                                    input.trial_steps, mel.n_filters);
  if(!fmt_only) {
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    for(int i=0; i< mel.n_filters; i++) {
      dout->FastEl2d(step, i) = mel_fbank_out.FastEl_Flat(i);
    }
  }

  // todo: mfcc_dct_out
  
  return true;
}

void AuditoryProc::PlotMelFilters(DataTable* graph_data) {
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_MelFilters", true);
  }
  graph_data->StructUpdate(true);
  int idx;
  DataCol* bincol = graph_data->FindMakeColName("Bin", idx, VT_INT);
  DataCol* fltcol = graph_data->FindMakeColName("Filters", idx, VT_FLOAT, 1,
                                             mel.n_filters);

  graph_data->ResetData();
  for(int bin = 0; bin < dft_use; bin++) {
    graph_data->AddBlankRow();
    bincol->SetVal(bin, -1);
    float_MatrixPtr mat; mat = (float_Matrix*)fltcol->GetValAsMatrix(-1);
    for(int flt = 0; flt < mel.n_filters; flt++) {
      int mxbin = mel_pts_bin[idx + 3];
      int mnbin = mel_pts_bin[idx + 1];
      float val = 0.0f;
      if(bin >= mnbin && bin <= mxbin) {
        int brel = bin - mnbin;
        val = mel_filters.FastEl2d(brel, flt);
      }
      mat->FastEl_Flat(flt) = val;
    }
  }
  graph_data->StructUpdate(false);
  graph_data->FindMakeGraphView();

}
