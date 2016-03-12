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

#include <taSound>
#include <DataTable>
#include <taProject>
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(AudInputSpec);
TA_BASEFUNS_CTORS_DEFN(AudDftSpec);
TA_BASEFUNS_CTORS_DEFN(MelFBankSpec);
TA_BASEFUNS_CTORS_DEFN(AudRenormSpec);
TA_BASEFUNS_CTORS_DEFN(MelCepstrumSpec);
TA_BASEFUNS_CTORS_DEFN(AudGaborSpec);
TA_BASEFUNS_CTORS_DEFN(AuditoryProc);

void AudInputSpec::Initialize() {
  win_msec = 25.0f;
  step_msec = 5.0f;
  trial_msec = 100.0f;
  border_steps = 12;
  sample_rate = 16000;
  channels = 1;
  channel = 0;
  ComputeSamples();
}

void AudInputSpec::ComputeSamples() {
  win_samples = MSecToSamples(win_msec);
  step_samples = MSecToSamples(step_msec);
  trial_samples = MSecToSamples(trial_msec);
  trial_steps = taMath_float::round(trial_msec / step_msec);
  total_steps = 2*border_steps + trial_steps;
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


//////////////////////////////////////////
//              AudDftSpec

void AudDftSpec::Initialize() {
  prv_smooth = 0.0f;
  log_pow = true;
  log_off = 0.0f;
  log_min = -100.0f;

  cur_smooth = 1.0f - prv_smooth;
}

void AudDftSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  cur_smooth = 1.0f - prv_smooth;
}
  
//////////////////////////////////////////
//              MelFBankSpec

void MelFBankSpec::Initialize() {
  on = true;
  lo_hz = 120.0f;
  hi_hz = 10000.0f;
  n_filters = 32;
  log_off = 0.0f;
  log_min = -10.0f;
  lo_mel = FreqToMel(lo_hz);
  hi_mel = FreqToMel(hi_hz);
}

void MelFBankSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  lo_mel = FreqToMel(lo_hz);
  hi_mel = FreqToMel(hi_hz);
}


//////////////////////////////////////////
//              AudRenormSpec

void AudRenormSpec::Initialize() {
  on = true;
  ren_min = -10.0f;
  ren_max = 7.0f;
  ren_scale = 1.0f / (ren_max - ren_min);
}

void AudRenormSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  ren_scale = 1.0f / (ren_max - ren_min);
}


//////////////////////////////////////////
//              AudGaborSpec

void AudGaborSpec::Initialize() {
  on = true;
  gain = 2.0f;
  n_horiz = 4;
  sz_time = 6;
  sz_freq = 6;
  wvlen = 1.5f;
  sig_len = 0.6f;
  sig_wd = 0.3f;
  sig_hor_len = 0.3f;
  sig_hor_wd = 0.1f;
  phase_off = 0.0f;
  circle_edge = true;

  n_filters = 3 + n_horiz;
}

void AudGaborSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  n_filters = 3 + n_horiz;
}

void AudGaborSpec::RenderFilters(float_Matrix& fltrs) {
  fltrs.SetGeom(3, sz_time, sz_freq, n_filters);

  float ctr_t = (float)(sz_time-1) / 2.0f;
  float ctr_f = (float)(sz_freq-1) / 2.0f;
  float ang_inc = taMath_float::pi / (float)4.0f;

  float radius_t = (float)(sz_time) / 2.0f;
  float radius_f = (float)(sz_freq) / 2.0f;

  float len_norm = 1.0f / (2.0f * sig_len * sig_len);
  float wd_norm = 1.0f / (2.0f * sig_wd * sig_wd);
  float hor_len_norm = 1.0f / (2.0f * sig_hor_len * sig_hor_len);
  float hor_wd_norm = 1.0f / (2.0f * sig_hor_wd * sig_hor_wd);

  float twopinorm = (2.0f * taMath_float::pi) / wvlen;

  float hctr_inc = (float)(sz_freq-1) / (float)(n_horiz+1);
  
  int fli = 0;
  for(int hi = 0; hi < n_horiz; hi++, fli++) {

    float hctr_f = hctr_inc * (float)(hi + 1);
    
    float angf = -2.0f * ang_inc;
    for(int y = 0; y < sz_freq; y++) {
      for(int x = 0; x < sz_time; x++) {
        float xf = (float)x - ctr_t;
        float yf = (float)y - hctr_f;
        float xfn = xf / radius_t;
        float yfn = yf / radius_f;

        float dist = taMath_float::hypot(xfn, yfn);
        float val = 0.0f;
        if(!(circle_edge && (dist > 1.0f))) {
          float nx = xfn * cosf(angf) - yfn * sinf(angf);
          float ny = yfn * cosf(angf) + xfn * sinf(angf);
          float gauss = expf(-(hor_wd_norm * (nx * nx) + hor_len_norm * (ny * ny)));
          float sin_val = sinf(twopinorm * ny + phase_off);
          val = gauss * sin_val;
        }
        fltrs.FastEl3d(x, y, fli) = val;
      }
    }
  }
  for(int ang = 1; ang < 4; ang++, fli++) {
    float angf = -(float)ang * ang_inc;

    for(int y = 0; y < sz_freq; y++) {
      for(int x = 0; x < sz_time; x++) {
        float xf = (float)x - ctr_t;
        float yf = (float)y - ctr_f;
        float xfn = xf / radius_t;
        float yfn = yf / radius_f;

        float dist = taMath_float::hypot(xfn, yfn);
        float val = 0.0f;
        if(!(circle_edge && (dist > 1.0f))) {
          float nx = xfn * cosf(angf) - yfn * sinf(angf);
          float ny = yfn * cosf(angf) + xfn * sinf(angf);
          float gauss = expf(-(len_norm * (nx * nx) + wd_norm * (ny * ny)));
          float sin_val = sinf(twopinorm * ny + phase_off);
          val = gauss * sin_val;
        }
        fltrs.FastEl3d(x, y, fli) = val;
      }
    }
  }
  
  // renorm each half
  for(fli = 0; fli < n_filters; fli++) {
    float pos_sum = 0.0f;
    float neg_sum = 0.0f;
    for(int y = 0; y < sz_freq; y++) {
      for(int x = 0; x < sz_time; x++) {
        float& val = fltrs.FastEl3d(x, y, fli);
        if(val > 0.0f)          { pos_sum += val; }
        else if(val < 0.0f)     { neg_sum += val; }
      }
    }
    float pos_norm = 1.0f / pos_sum;
    float neg_norm = -1.0f / neg_sum;
    for(int y = 0; y < sz_freq; y++) {
      for(int x = 0; x < sz_time; x++) {
        float& val = fltrs.FastEl3d(x, y, fli);
        if(val > 0.0f)          { val *= pos_norm; }
        else if(val < 0.0f)     { val *= neg_norm; }
      }
    }
  }
}

void AudGaborSpec::GridFilters(float_Matrix& fltrs, DataTable* graph_data, bool reset) {
  RenderFilters(fltrs);         // just to make sure

  String name;
  if(owner) name = owner->GetName();

  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_V1Gabor_GridFilters", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
//   nmda->SetUserData("WIDTH", 10);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, sz_time,
                                               sz_freq);

  float maxv = taMath_float::vec_abs_max(&fltrs, idx);

  graph_data->SetUserData("N_ROWS", n_filters);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  for(int i=0; i<n_filters; i++) {
    graph_data->AddBlankRow();
    float_MatrixPtr frm; frm = (float_Matrix*)fltrs.GetFrameSlice(i);
    matda->SetValAsMatrix(frm, -1);
    nmda->SetValAsString("Filter: " + String(i), -1);
  }

  graph_data->StructUpdate(false);
  graph_data->FindMakeGridView();
}

//////////////////////////////////////////
//              MelCeptrumSpec

void MelCepstrumSpec::Initialize() {
  on = true;
  n_coeff = 13;
}

void MelCepstrumSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}



//////////////////////////////////////////

void AuditoryProc::Initialize() {
  save_mode = FIRST_ROW;

  first_step = true;
  input_pos = 0;
  trial_start_pos = 0;
  trial_end_pos = trial_start_pos + input.trial_samples;
  dft_size = 0;
  dft_use = 0;
  mel_n_filters_eff = 0;
  mel_filt_max_bins = 0;
  
  gabor_kwta.on = true;
  gabor_kwta.gi = 2.0f;
  gabor_kwta.lay_gi = 1.5f;

  gabor1.on = true;
  gabor1.sz_time = 6;
  gabor1.sz_freq = 6;
  gabor1.spc_time = 2;
  gabor1.spc_freq = 2;

  gabor2.on = true;
  gabor2.sz_time = 12;
  gabor2.sz_freq = 12;
  gabor2.spc_time = 4;
  gabor2.spc_freq = 4;
  
  gabor3.on = false;
  
  
}

void AuditoryProc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateConfig();
  if(!taMisc::is_loading) {
    if(NeedsInit()) Init();
  }
}

void AuditoryProc::UpdateConfig() {
  input.UpdateAfterEdit_NoGui();
  mel_fbank.UpdateAfterEdit_NoGui();
  gabor1.UpdateAfterEdit_NoGui();
  gabor2.UpdateAfterEdit_NoGui();
  gabor3.UpdateAfterEdit_NoGui();
  gabor_kwta.UpdateAfterEdit_NoGui();

  gabor1_geom.x = ((input.trial_steps - 1) / gabor1.spc_time) + 1;
  gabor1_geom.y = ((mel_fbank.n_filters - gabor1.sz_freq - 1) /
                    gabor1.spc_freq) + 1;
  gabor1_geom.UpdateAfterEdit_NoGui();

  gabor2_geom.x = ((input.trial_steps - 1) / gabor2.spc_time) + 1;
  gabor2_geom.y = ((mel_fbank.n_filters - gabor2.sz_freq - 1) /
                    gabor2.spc_freq) + 1;
  gabor2_geom.UpdateAfterEdit_NoGui();

  gabor3_geom.x = ((input.trial_steps - 1) / gabor3.spc_time) + 1;
  gabor3_geom.y = ((mel_fbank.n_filters - gabor3.sz_freq - 1) /
                    gabor3.spc_freq) + 1;
  gabor3_geom.UpdateAfterEdit_NoGui();
}

bool AuditoryProc::NeedsInit() {
  if(dft_size != input.win_samples || mel_n_filters_eff != mel_fbank.n_filters + 2)
    return true;
  return false;
}

bool AuditoryProc::Init() {
  UpdateConfig();
  InitFilters();
  InitOutMatrix();
  InitDataTable();
  InitSound();
  return true;
}

bool AuditoryProc::InitSound() {
  input_pos = 0;
  sound_full.Reset();
  return true;
}

void AuditoryProc::InitFromSound(taSound* sound, int n_chans, int chan) {
  input.InitFromSound(sound, n_chans, chan);
}

bool AuditoryProc::InitFilters() {
  dft_size = input.win_samples;
  dft_use = dft_size / 2 + 1;
  InitFilters_Mel();
  if(gabor1.on) gabor1.RenderFilters(gabor1_filters);
  if(gabor2.on) gabor2.RenderFilters(gabor2_filters);
  if(gabor3.on) gabor3.RenderFilters(gabor3_filters);
  return true;
}

bool AuditoryProc::InitFilters_Mel() {
  mel_n_filters_eff = mel_fbank.n_filters + 2;
  mel_pts_mel.SetGeom(1, mel_n_filters_eff);
  mel_pts_hz.SetGeom(1, mel_n_filters_eff);
  mel_pts_bin.SetGeom(1, mel_n_filters_eff);

  float mel_inc = (mel_fbank.hi_mel - mel_fbank.lo_mel) / (float)(mel_fbank.n_filters+1);

  for(int idx = 0; idx < mel_n_filters_eff; idx++) {
    float ml = mel_fbank.lo_mel + (float)idx*mel_inc;
    float hz = mel_fbank.MelToFreq(ml);
    int bin = mel_fbank.FreqToBin(hz, dft_use, input.sample_rate);
    mel_pts_mel[idx] = ml;
    mel_pts_hz[idx] = hz;
    mel_pts_bin[idx] = bin;
  }

  mel_filt_max_bins = mel_pts_bin[mel_n_filters_eff-1] - mel_pts_bin[mel_n_filters_eff-3] + 1;
  mel_filters.SetGeom(2, mel_filt_max_bins, mel_fbank.n_filters);

  for(int flt = 0; flt < mel_fbank.n_filters; flt++) {
    int mnbin = mel_pts_bin[flt];
    int pkbin = mel_pts_bin[flt+1];
    int mxbin = mel_pts_bin[flt+2];
    float pkmin = (float)(pkbin - mnbin);
    float pkmax = (float)(mxbin - pkbin);
    int fi = 0;
    int bin;
    for(bin = mnbin; bin <= pkbin; bin++, fi++) {
      float fval = (float)(bin - mnbin) / pkmin;
      mel_filters.FastEl2d(fi, flt) = fval;
    }
    for(; bin <= mxbin; bin++, fi++) {
      float fval = (float)(mxbin - bin) / pkmax;
      mel_filters.FastEl2d(fi, flt) = fval;
    }
  }
  return true;
}

bool AuditoryProc::InitOutMatrix() {
  window_in.SetGeom(1, input.win_samples);
  dft_out.SetGeom(2, 2, dft_size);
  dft_power_out.SetGeom(1, dft_use);
  dft_power_trial_out.SetGeom(3, dft_use, input.total_steps, input.channels);
  if(dft.log_pow) {
    dft_log_power_out.SetGeom(1, dft_use);
    dft_log_power_trial_out.SetGeom(3, dft_use, input.total_steps, input.channels);
  }
  if(mel_fbank.on) {
    mel_fbank_out.SetGeom(1, mel_fbank.n_filters);
    mel_fbank_trial_out.SetGeom(3, mel_fbank.n_filters, input.total_steps, input.channels);
    if(gabor1.on) {
      gabor1_trial_raw.SetGeom(5, gabor1.n_filters, 2, gabor1_geom.y,
                               gabor1_geom.x, input.channels);
      gabor1_trial_out.SetGeom(5, gabor1.n_filters, 2, gabor1_geom.y,
                               gabor1_geom.x, input.channels);
    }
    if(gabor2.on) {
      gabor2_trial_raw.SetGeom(5, gabor2.n_filters, 2, gabor2_geom.y,
                               gabor2_geom.x, input.channels);
      gabor2_trial_out.SetGeom(5, gabor2.n_filters, 2, gabor2_geom.y,
                               gabor2_geom.x, input.channels);
    }
    if(gabor3.on) {
      gabor3_trial_raw.SetGeom(5, gabor3.n_filters, 2, gabor3_geom.y,
                               gabor3_geom.x, input.channels);
      gabor3_trial_out.SetGeom(5, gabor3.n_filters, 2, gabor3_geom.y,
                               gabor3_geom.x, input.channels);
    }
    if(mfcc.on) {
      mfcc_dct_out.SetGeom(1, mel_fbank.n_filters);
      mfcc_dct_trial_out.SetGeom(3, mel_fbank.n_filters, input.total_steps,
                                 input.channels);
    }
  }
  return true;
}

bool AuditoryProc::LoadSound(taSound* sound) {
  if(NeedsInit()) Init();
  
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
  StartNewSound();
  return true;
}

bool AuditoryProc::StartNewSound() {
  first_step = true;
  input_pos = 0;
  trial_start_pos = 0;
  trial_end_pos = trial_start_pos + input.trial_samples;
  dft_power_trial_out.InitVals(0.0f);
  if(dft.log_pow) {
    dft_log_power_trial_out.InitVals(0.0f);
  }
  if(mel_fbank.on) {
    mel_fbank_trial_out.InitVals(0.0f);
    if(gabor1.on) {
      gabor1_trial_raw.InitVals(0.0f);
      gabor1_trial_out.InitVals(0.0f);
    }
    if(gabor2.on) {
      gabor2_trial_raw.InitVals(0.0f);
      gabor2_trial_out.InitVals(0.0f);
    }
    if(gabor3.on) {
      gabor3_trial_raw.InitVals(0.0f);
      gabor3_trial_out.InitVals(0.0f);
    }
    if(mfcc.on) {
      mfcc_dct_trial_out.InitVals(0.0f);
    }
  }
  return true;
}

int AuditoryProc::InputStepsLeft() {
  int samples_left = sound_full.Frames() - input_pos;
  return samples_left / input.step_samples;
}

bool AuditoryProc::ProcessTrial() {
  if(NeedsInit()) Init();
  NewTableRow();
  if(TestError(InputStepsLeft() < 1, "ProcessTrial",
               "no steps worth of input sound available -- load a new sound")) {
    return false;
  }
  int st_in_pos = input_pos;
  int bord_eff = 2 * input.border_steps; // full amount to wrap
  int tot_steps = input.total_steps;
  trial_start_pos = input_pos - input.step_samples * input.border_steps;
  trial_end_pos = trial_start_pos + input.trial_samples;
  for(int chan=0; chan < input.channels; chan++) {
    input_pos = st_in_pos;      // always start at same place per channel
    WrapBorder(chan);
    for(int step = bord_eff; step < tot_steps; step++) {
      ProcessStep(chan, step);
    }
    FilterTrial(chan);
    OutputToTable(chan);
  }
  return true;
}

bool AuditoryProc::StepToSample(int samp_pos) {
  if(NeedsInit()) Init();
  NewTableRow();
  if(TestError(InputStepsLeft() < 1, "StepToSample",
               "no steps worth of input sound available -- load a new sound")) {
    return false;
  }
  if(input_pos == 0)            // first process a trial if nothing loaded yet
    ProcessTrial();

  if(TestError(samp_pos < trial_start_pos, "StepToSample",
               "target sample position is earlier than current trial start position!")) {
    return false;
  }
  int steps_fwd = (samp_pos - trial_start_pos) / input.step_samples;

  int end_step = input.total_steps-1;

  for(int stp = 0; stp < steps_fwd; stp++) {
    int st_in_pos = input_pos;
    trial_start_pos += input.step_samples;
    trial_end_pos += input.step_samples;
    for(int chan=0; chan < input.channels; chan++) {
      input_pos = st_in_pos;      // always start at same place per channel
      StepForward(chan);
      ProcessStep(chan, end_step);
      FilterTrial(chan);
    }
  }

  for(int chan=0; chan < input.channels; chan++) {
    OutputToTable(chan);
  }
  return true;
}

bool AuditoryProc::WrapBorder(int chan) {
  if(input.border_steps == 0) return true;
  int bord_eff = 2 * input.border_steps; // full amount to wrap
  int src_st_step = input.total_steps - bord_eff;
  for(int step=0; step < bord_eff; step++) {
    CopyStepFromStep(step, src_st_step + step, chan);
  }
  return true;
}

bool AuditoryProc::StepForward(int chan) {
  int tot_m1 = input.total_steps - 1;
  for(int step=0; step < tot_m1; step++) {
    CopyStepFromStep(step, step+1, chan);
  }
  return true;
}

bool AuditoryProc::CopyStepFromStep(int to_step, int fm_step, int chan) {
  for(int i=0; i<dft_use; i++) {
    dft_power_trial_out.FastEl3d(i, to_step, chan) =
      dft_power_trial_out.FastEl3d(i, fm_step, chan);
    if(dft.log_pow) {
      dft_log_power_trial_out.FastEl3d(i, to_step, chan) =
        dft_log_power_trial_out.FastEl3d(i, fm_step, chan);
    }
  }
  if(mel_fbank.on) {
    for(int i=0; i < mel_fbank.n_filters; i++) {
      mel_fbank_trial_out.FastEl3d(i, to_step, chan) =
        mel_fbank_trial_out.FastEl3d(i, fm_step, chan);
    }
    if(mfcc.on) {
      for(int i=0; i < mel_fbank.n_filters; i++) {
        mfcc_dct_trial_out.FastEl3d(i, to_step, chan) =
          mfcc_dct_trial_out.FastEl3d(i, fm_step, chan);
      }
    }
  }
  return true;
}

bool AuditoryProc::ProcessStep(int chan, int step) {
  SoundToWindow(input_pos, chan);
  FilterWindow(chan, step);
  input_pos += input.step_samples;
  first_step = false;
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
      // todo: this is not right:
      memcpy(window_in.el, (void*)&(sound_full.FastEl2d(chan, in_pos)), sz);
    }
  }

  samp_cpy = MAX(samp_cpy, 0);  // prevent negatives here -- otherwise overflows
  // pad remainder with zero
  int zero_n = input.win_samples - samp_cpy;
  if(zero_n > 0) {
    int sz = zero_n * sizeof(float);
    memset(window_in.el + samp_cpy, 0, sz);
  }
  return true;
}

bool AuditoryProc::FilterWindow(int chan, int step) {
  DftInput(chan, step);
  if(mel_fbank.on) {
    PowerOfDft(chan, step);
    MelFilterDft(chan, step);
    if(mfcc.on) {
      CepstrumDctMel(chan, step);
    }
  }
  return true;
}

bool AuditoryProc::FilterTrial(int chan) {
  if(gabor1.on) {
    GaborFilter_impl(chan, gabor1, gabor1_filters,
                     gabor1_trial_raw, gabor1_trial_out);
  }
  if(gabor2.on) {
    GaborFilter_impl(chan, gabor2, gabor2_filters,
                     gabor2_trial_raw, gabor2_trial_out);
  }
  if(gabor3.on) {
    GaborFilter_impl(chan, gabor3, gabor3_filters,
                     gabor3_trial_raw, gabor3_trial_out);
  }
  return true;
}

void AuditoryProc::DftInput(int chan, int step) {
  taMath_float::fft_real(&dft_out, &window_in);
}

void AuditoryProc::PowerOfDft(int chan, int step) {
  // Mag() is absolute value   SqMag is square of it - r*r + i*i
  for(int i=0; i<dft_use; i++) {
    float r = dft_out.FastEl_Flat(2*i);
    float j = dft_out.FastEl_Flat(2*i+1);
    float powr = r*r + j*j;
    if(!first_step) {
      powr = dft.prv_smooth * dft_power_out.FastEl_Flat(i) +
        dft.cur_smooth * powr;
    }
    dft_power_out.FastEl_Flat(i) = powr;
    dft_power_trial_out.FastEl3d(i, step, chan) = powr;
    if(dft.log_pow) {
      powr += dft.log_off;
      float logp;
      if(powr == 0.0f) {
        logp = dft.log_min;
      }
      else
        logp = logf(powr);
      dft_log_power_out.FastEl_Flat(i) = logp;
      dft_log_power_trial_out.FastEl3d(i, step, chan) = logp;
    }
  }
}

void AuditoryProc::MelFilterDft(int chan, int step) {
  int mi = 0;
  for(int flt = 0; flt < mel_fbank.n_filters; flt++, mi++) {
    int mnbin = mel_pts_bin[flt];
    int mxbin = mel_pts_bin[flt + 2];

    float sum = 0.0f;
    int fi=0;
    for(int bin = mnbin; bin <= mxbin; bin++, fi++) {
      const float fval = mel_filters.FastEl2d(fi, mi);
      const float pval = dft_power_out.FastEl_Flat(bin);
      sum += fval * pval;
    }
    sum += mel_fbank.log_off;
    float val;
    if(sum == 0.0f) {
      val = mel_fbank.log_min;
    }
    else {
      val = logf(sum);
    }
    if(fbank_renorm.on) {
      val -= fbank_renorm.ren_min;
      if(val < 0.0f) val = 0.0f;
      val *= fbank_renorm.ren_scale;
      if(val > 1.0f) val = 1.0f;
    }
    mel_fbank_out.FastEl_Flat(mi) = val;
    mel_fbank_trial_out.FastEl3d(mi, step, chan) = val;
  }
}

void AuditoryProc::CepstrumDctMel(int chan, int step) {
  memcpy(mfcc_dct_out.el, mel_fbank_out.el, mel_fbank.n_filters * sizeof(float));
  taMath_float::dct(&mfcc_dct_out);
  float& el0 = mfcc_dct_out.FastEl_Flat(0);
  el0 = logf(1.0f + el0*el0);              // replace with log energy instead..
  for(int i=0; i<mel_fbank.n_filters; i++) {
    mfcc_dct_trial_out.FastEl3d(i, step, chan) = mfcc_dct_out.FastEl_Flat(i);
  }
}

void AuditoryProc::GaborFilter_impl(int chan, const AudGaborSpec& spec,
                                    const float_Matrix& filters, float_Matrix& out_raw,
                                    float_Matrix& out) {
  const int t_half_sz = spec.sz_time / 2;
  const int h_half_sz = spec.sz_freq / 2;

  const int t_off = t_half_sz - input.border_steps;
  int t_min = t_off;
  if(t_min < 0) t_min = 0;
  int t_max = input.trial_steps - t_min;

  const int f_min = 0;
  const int f_max = mel_fbank.n_filters - spec.sz_freq;
  
  int tidx = 0;
  for(int stp = t_min; stp < t_max; stp += spec.spc_time, tidx++) {
    int in_st = stp - t_off;
    if(TestError(tidx > out_raw.dim(3), "GaborFilter_impl",
                 "time index", String(tidx),"out of range:", String(out_raw.dim(3)))) {
      break;
    }
      
    int fidx = 0;
    for(int flt = f_min; flt < f_max; flt += spec.spc_freq, fidx++) {
      if(TestError(fidx > out_raw.dim(2), "GaborFilter_impl",
                   "freq index", String(fidx),"out of range:", String(out_raw.dim(2)))) {
        break;
      }
      const int nf = spec.n_filters;
      for(int fi = 0; fi < nf; fi++) {
        float fsum = 0.0f;
        for(int ff = 0; ff < spec.sz_freq; ff++) {
          for(int ft = 0; ft < spec.sz_time; ft++) {
            float fval = filters.FastEl3d(ft, ff, fi);
            float ival = mel_fbank_trial_out.FastEl3d(flt + ff, in_st + ft, chan);
            fsum += fval * ival;
          }
        }
        bool pos = (fsum >= 0.0f);
        float act = spec.gain * fabsf(fsum);
        if(pos) {
          out_raw.FastEl(fi, 0, fidx, tidx, chan) = act;
          out_raw.FastEl(fi, 1, fidx, tidx, chan) = 0.0f;
        }
        else {
          out_raw.FastEl(fi, 0, fidx, tidx, chan) = 0.0f;
          out_raw.FastEl(fi, 1, fidx, tidx, chan) = act;
        }
      }
    }
  }

  float_MatrixPtr raw_frm; raw_frm = (float_Matrix*)out_raw.GetFrameSlice(chan);
  float_MatrixPtr out_frm; out_frm = (float_Matrix*)out.GetFrameSlice(chan);
  
  if(gabor_kwta.On()) {
    gabor_kwta.Compute_Inhib(*raw_frm, *out_frm, gabor_gci);
  }
  else {
    memcpy(out_frm->el, raw_frm->el, raw_frm->size * sizeof(float));
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
  if(mel_fbank.on) {
    MelOutputToTable(data_table, chan, true);
  }
  return true;
}

bool AuditoryProc::NewTableRow() {
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return false;
  if(save_mode == FIRST_ROW) {
    data_table->EnforceRows(1);
    data_table->WriteItem(0);
    data_table->ReadItem(0);
  }
  else {
    data_table->AddBlankRow();
  }
  return true;
}

bool AuditoryProc::OutputToTable(int chan) {
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return false;
  data_table->StructUpdate(true);
  if(mel_fbank.on) {
    MelOutputToTable(data_table, chan, false); // not fmt_only
  }
  data_table->StructUpdate(false);
  return true;
}

bool AuditoryProc::MelOutputToTable(DataTable* dtab, int chan, bool fmt_only) {
  DataCol* col;
  int idx;
  String col_sufx;
  if(input.channels > 1) {
    col_sufx = "_ch" + String(chan);
  }
  col = data_table->FindMakeColName(name + "_dft_pow" + col_sufx, idx,
                                    DataTable::VT_FLOAT, 2,
                                    input.total_steps, dft_use);
  if(!fmt_only) {
    float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
    for(int stp = 0; stp < input.total_steps; stp++) {
      for(int i=0; i< dft_use; i++) {
        if(dft.log_pow) {
          dout->FastEl2d(stp, i) = dft_log_power_trial_out.FastEl3d(i, stp, chan);
        }
        else {
          dout->FastEl2d(stp, i) = dft_power_trial_out.FastEl3d(i, stp, chan);
        }
      }
    }
  }

  if(mel_fbank.on) {
    col = data_table->FindMakeColName(name + "_mel_fbank" + col_sufx, idx,
                                      DataTable::VT_FLOAT, 2,
                                      input.total_steps, mel_fbank.n_filters);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(int stp = 0; stp < input.total_steps; stp++) {
        for(int i=0; i< mel_fbank.n_filters; i++) {
          dout->FastEl2d(stp, i) = mel_fbank_trial_out.FastEl3d(i, stp, chan);
        }
      }
    }

    if(gabor1.on) {
      col = data_table->FindMakeColName(name + "_mel_gabor1_raw" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        gabor1.n_filters, 2,
                                        gabor1_geom.x, gabor1_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        const int nf = gabor1.n_filters;
        for(int stp = 0; stp < gabor1_geom.x; stp++) {
          for(int i=0; i< gabor1_geom.y; i++) {
            for(int ti=0; ti < nf; ti++) {
              dout->FastEl4d(ti, 0, stp, i) = gabor1_trial_raw.FastEl(ti, 0, i, stp, chan);
              dout->FastEl4d(ti, 1, stp, i) = gabor1_trial_raw.FastEl(ti, 1, i, stp, chan);
            }
          }
        }
      }

      col = data_table->FindMakeColName(name + "_mel_gabor1" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        gabor1.n_filters, 2,
                                        gabor1_geom.x, gabor1_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        const int nf = gabor1.n_filters;
        for(int stp = 0; stp < gabor1_geom.x; stp++) {
          for(int i=0; i< gabor1_geom.y; i++) {
            for(int ti=0; ti < nf; ti++) {
              dout->FastEl4d(ti, 0, stp, i) = gabor1_trial_out.FastEl(ti, 0, i, stp, chan);
              dout->FastEl4d(ti, 1, stp, i) = gabor1_trial_out.FastEl(ti, 1, i, stp, chan);
            }
          }
        }
      }
    }

    if(gabor2.on) {
      col = data_table->FindMakeColName(name + "_mel_gabor2_raw" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        gabor2.n_filters, 2,
                                        gabor2_geom.x, gabor2_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        const int nf = gabor2.n_filters;
        for(int stp = 0; stp < gabor2_geom.x; stp++) {
          for(int i=0; i< gabor2_geom.y; i++) {
            for(int ti=0; ti < nf; ti++) {
              dout->FastEl4d(ti, 0, stp, i) = gabor2_trial_raw.FastEl(ti, 0, i, stp, chan);
              dout->FastEl4d(ti, 1, stp, i) = gabor2_trial_raw.FastEl(ti, 1, i, stp, chan);
            }
          }
        }
      }

      col = data_table->FindMakeColName(name + "_mel_gabor2" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        gabor2.n_filters, 2,
                                        gabor2_geom.x, gabor2_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        const int nf = gabor2.n_filters;
        for(int stp = 0; stp < gabor2_geom.x; stp++) {
          for(int i=0; i< gabor2_geom.y; i++) {
            for(int ti=0; ti < nf; ti++) {
              dout->FastEl4d(ti, 0, stp, i) = gabor2_trial_out.FastEl(ti, 0, i, stp, chan);
              dout->FastEl4d(ti, 1, stp, i) = gabor2_trial_out.FastEl(ti, 1, i, stp, chan);
            }
          }
        }
      }
    }

    if(gabor3.on) {
      col = data_table->FindMakeColName(name + "_mel_gabor3_raw" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        gabor3.n_filters, 2,
                                        gabor3_geom.x, gabor3_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        const int nf = gabor3.n_filters;
        for(int stp = 0; stp < gabor3_geom.x; stp++) {
          for(int i=0; i< gabor3_geom.y; i++) {
            for(int ti=0; ti < nf; ti++) {
              dout->FastEl4d(ti, 0, stp, i) = gabor3_trial_raw.FastEl(ti, 0, i, stp, chan);
              dout->FastEl4d(ti, 1, stp, i) = gabor3_trial_raw.FastEl(ti, 1, i, stp, chan);
            }
          }
        }
      }

      col = data_table->FindMakeColName(name + "_mel_gabor3" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        gabor3.n_filters, 2,
                                        gabor3_geom.x, gabor3_geom.y);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        const int nf = gabor3.n_filters;
        for(int stp = 0; stp < gabor3_geom.x; stp++) {
          for(int i=0; i< gabor3_geom.y; i++) {
            for(int ti=0; ti < nf; ti++) {
              dout->FastEl4d(ti, 0, stp, i) = gabor3_trial_out.FastEl(ti, 0, i, stp, chan);
              dout->FastEl4d(ti, 1, stp, i) = gabor3_trial_out.FastEl(ti, 1, i, stp, chan);
            }
          }
        }
      }
    }
  }

  if(mfcc.on) {
    col = data_table->FindMakeColName(name + "_mel_mfcc" + col_sufx, idx,
                                      DataTable::VT_FLOAT, 2,
                                      input.total_steps, mfcc.n_coeff);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(int stp = 0; stp < input.total_steps; stp++) {
        for(int i=0; i< mfcc.n_coeff; i++) {
          dout->FastEl2d(stp, i) = mfcc_dct_trial_out.FastEl3d(i, stp, chan);
        }
      }
    }
  }
  
  return true;
}

void AuditoryProc::PlotMelFilters(DataTable* graph_data) {
  if(NeedsInit()) Init();
  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_MelFilters", true);
  }
  graph_data->StructUpdate(true);
  int idx;
  DataCol* bincol = graph_data->FindMakeColName("Bin", idx, VT_INT);
  DataCol* fltcol = graph_data->FindMakeColName("Filters", idx, VT_FLOAT, 1,
                                             mel_fbank.n_filters);

  graph_data->ResetData();
  for(int bin = 0; bin < dft_use; bin++) {
    graph_data->AddBlankRow();
    bincol->SetVal(bin, -1);
    float_MatrixPtr mat; mat = (float_Matrix*)fltcol->GetValAsMatrix(-1);
    for(int flt = 0; flt < mel_fbank.n_filters; flt++) {
      int mxbin = mel_pts_bin[flt + 2];
      int mnbin = mel_pts_bin[flt];
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

void AuditoryProc::GridGaborFilters(DataTable* graph_data, int gabor_n) {
  if(NeedsInit()) Init();
  switch(gabor_n) {
  case 1:
    gabor1.GridFilters(gabor1_filters, graph_data, true);
    break;
  case 2:
    gabor2.GridFilters(gabor2_filters, graph_data, true);
    break;
  case 3:
    gabor3.GridFilters(gabor3_filters, graph_data, true);
    break;
  }
}
