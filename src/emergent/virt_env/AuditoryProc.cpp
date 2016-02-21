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
TA_BASEFUNS_CTORS_DEFN(AudTimeGaborSpec);
TA_BASEFUNS_CTORS_DEFN(MelCepstrumSpec);
TA_BASEFUNS_CTORS_DEFN(AudGaborSpec);
TA_BASEFUNS_CTORS_DEFN(AuditoryProc);

void AudInputSpec::Initialize() {
  win_msec = 25.0f;
  step_msec = 12.5f;
  trial_msec = 100.0f;
  border_steps = 0;
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
  log_pow = false;
  log_off = 0.0f;
  log_min = -10.0f;
}

//////////////////////////////////////////
//              MelFBankSpec

void MelFBankSpec::Initialize() {
  on = true;
  lo_hz = 300.0f;
  hi_hz = 8000.0f;
  n_filters = 26;
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
//              AudTimeGaborSpec

void AudTimeGaborSpec::Initialize() {
  on = true;
  st_half_size = 1;
  ed_half_size = 3;
  inc_half_size = 1;
  xx1_norm = false;
  xx1_gain = 10.0f;

  max_size = 2 * ed_half_size;
  n_filters = 1 + (ed_half_size - st_half_size) / inc_half_size;
}

void AudTimeGaborSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  max_size = 2 * ed_half_size;
  n_filters = 1 + (ed_half_size - st_half_size) / inc_half_size;
}

void AudTimeGaborSpec::RenderFilters(float_Matrix& fltrs, int_Matrix& flt_sizes) {
  fltrs.SetGeom(2, max_size, n_filters);
  flt_sizes.SetGeom(1, n_filters);
  int idx = 0;
  for(int sz = st_half_size; sz <= ed_half_size; sz += inc_half_size, idx++) {
    flt_sizes[idx] = sz;
    int fsz = 2 * sz;
    float fwdth = sz;
    float fsum = 0.0f;
    for(int fi = 0; fi < fsz; fi++) {
      float pt = (fi - sz) + 0.5f;
      float val = sinf((pt/fwdth) * taMath_float::pi);
      fsum += fabsf(val);
      fltrs.FastEl2d(fi, idx) = val;
    }
    fsum *= 0.5f;
    for(int fi = 0; fi < fsz; fi++) {
      fltrs.FastEl2d(fi, idx) /= fsum;
    }
  }
}

void AudTimeGaborSpec::GridFilters(float_Matrix& fltrs, int_Matrix& flt_sizes,
                               DataTable* graph_data, bool reset) {
  RenderFilters(fltrs, flt_sizes);         // just to make sure

  String name;
  if(owner) name = owner->GetName();

  taProject* proj = GetMyProj();
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_AudTimeGabor_GridFilters", true);
  }
  graph_data->StructUpdate(true);
  if(reset)
    graph_data->ResetData();
  int idx;
  DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
//   nmda->SetUserData("WIDTH", 10);
  DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 1, max_size);

  float maxv = taMath_float::vec_abs_max(&fltrs, idx);

  graph_data->SetUserData("N_ROWS", 4);
  graph_data->SetUserData("SCALE_MIN", -maxv);
  graph_data->SetUserData("SCALE_MAX", maxv);
  graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

  for(int idx=0; idx < n_filters; idx++) {
    graph_data->AddBlankRow();
    float_MatrixPtr frm; frm = (float_Matrix*)fltrs.GetFrameSlice(idx);
    matda->SetValAsMatrix(frm, -1);
    nmda->SetValAsString("Sz: " + String(flt_sizes.FastEl_Flat(idx)), -1);
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
//              AudGaborSpec

void AudGaborSpec::Initialize() {
  on = true;
  gain = 2.0f;
  n_horiz = 4;
  sz_time_msec = 12;
  sz_freq = 12;
  spacing_pct = 0.5f;
  wvlen = 6.0f;
  gauss_sig_len = 0.25f;
  gauss_sig_wd = 0.15f;
  phase_off = 0.0f;
  circle_edge = true;

  n_filters = 3 + n_horiz;
  sz_time_steps = 12;
  spc_time_steps = 6;
  spc_freq = (int)(spacing_pct * sz_freq);
}

void AudGaborSpec::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  n_filters = 3 + n_horiz;
  spc_freq = (int)(spacing_pct * sz_freq);
}

void AudGaborSpec::RenderFilters(float_Matrix& fltrs) {
  // fltrs.SetGeom(3, filter_size, filter_size, n_angles);

  // float ctr = (float)(filter_size-1) / 2.0f;
  // float ang_inc = taMath_float::pi / (float)n_angles;

  // float circ_radius = (float)(filter_size) / 2.0f;

  // float gs_len_eff = gauss_sig_len * (float)filter_size;
  // float gs_wd_eff = gauss_sig_wd * (float)filter_size;

  // float len_norm = 1.0f / (2.0f * gs_len_eff * gs_len_eff);
  // float wd_norm = 1.0f / (2.0f * gs_wd_eff * gs_wd_eff);

  // float twopinorm = (2.0f * taMath_float::pi) / wvlen;

  // for(int ang = 0; ang < n_angles; ang++) {
  //   float angf = -(float)ang * ang_inc;

  //   float pos_sum = 0.0f;
  //   float neg_sum = 0.0f;
  //   for(int x = 0; x < filter_size; x++) {
  //     for(int y = 0; y < filter_size; y++) {
  //       float xf = (float)x - ctr;
  //       float yf = (float)y - ctr;

  //       float dist = taMath_float::hypot(xf, yf);
  //       float val = 0.0f;
  //       if(!(circle_edge && (dist > circ_radius))) {
  //         float nx = xf * cosf(angf) - yf * sinf(angf);
  //         float ny = yf * cosf(angf) + xf * sinf(angf);
  //         float gauss = expf(-(len_norm * (nx * nx) + wd_norm * (ny * ny)));
  //         float sin_val = sinf(twopinorm * ny + phase_off);
  //         val = gauss * sin_val;
  //         if(val > 0.0f)        { pos_sum += val; }
  //         else if(val < 0.0f)   { neg_sum += val; }
  //       }
  //       fltrs.FastEl3d(x, y, ang) = val;
  //     }
  //   }
  //   // renorm each half
  //   float pos_norm = 1.0f / pos_sum;
  //   float neg_norm = -1.0f / neg_sum;
  //   for(int x = 0; x < filter_size; x++) {
  //     for(int y = 0; y < filter_size; y++) {
  //       float& val = fltrs.FastEl3d(x, y, ang);
  //       if(val > 0.0f)          { val *= pos_norm; }
  //       else if(val < 0.0f)     { val *= neg_norm; }
  //     }
  //   }
  // }
}

void AudGaborSpec::GridFilters(float_Matrix& fltrs, DataTable* graph_data, bool reset) {
//   RenderFilters(fltrs);         // just to make sure

//   String name;
//   if(owner) name = owner->GetName();

//   taProject* proj = GetMyProj();
//   if(!graph_data) {
//     graph_data = proj->GetNewAnalysisDataTable(name + "_V1Gabor_GridFilters", true);
//   }
//   graph_data->StructUpdate(true);
//   if(reset)
//     graph_data->ResetData();
//   int idx;
//   DataCol* nmda = graph_data->FindMakeColName("Name", idx, VT_STRING);
// //   nmda->SetUserData("WIDTH", 10);
//   DataCol* matda = graph_data->FindMakeColName("Filter", idx, VT_FLOAT, 2, filter_size, filter_size);

//   float maxv = taMath_float::vec_abs_max(&fltrs, idx);

//   graph_data->SetUserData("N_ROWS", 4);
//   graph_data->SetUserData("SCALE_MIN", -maxv);
//   graph_data->SetUserData("SCALE_MAX", maxv);
//   graph_data->SetUserData("BLOCK_HEIGHT", 0.0f);

//   int ang_inc = 180 / n_angles;

//   for(int ang=0; ang<n_angles; ang++) {
//     graph_data->AddBlankRow();
//     float_MatrixPtr frm; frm = (float_Matrix*)fltrs.GetFrameSlice(ang);
//     matda->SetValAsMatrix(frm, -1);
//     nmda->SetValAsString("Angle: " + String(ang * ang_inc), -1);
//   }

//   graph_data->StructUpdate(false);
//   graph_data->FindMakeGridView();
}


//////////////////////////////////////////

void AuditoryProc::Initialize() {
  save_mode = FIRST_ROW;
  input_save = NO_SAVE;
  input_pos = 0;
  trial_start_pos = 0;
  trial_end_pos = trial_start_pos + input.trial_samples;
  dft_size = 0;
  dft_use = 0;
  mel_n_filters_eff = 0;
  mel_filt_max_bins = 0;
  
  tgabor_kwta.mode = V1KwtaSpec::FFFB;
  tgabor_kwta.gi = 2.0f;
  tgabor_kwta.lay_gi = 1.5f;
  tgabor_kwta.gp_k = 1;
  tgabor_kwta.gp_g = 0.02f;
}

void AuditoryProc::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateConfig();
  if(!taMisc::is_loading) {
    if(NeedsInit()) Init();
  }
}

void AuditoryProc::UpdateConfig() {
  if(fbank_tgabor.on) {
    input.border_steps = fbank_tgabor.ed_half_size; // must be!
  }
  input.UpdateAfterEdit_NoGui();
  mel_fbank.UpdateAfterEdit_NoGui();
  fbank_tgabor.UpdateAfterEdit_NoGui();
  tgabor_kwta.UpdateAfterEdit_NoGui();
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
  if(fbank_tgabor.on) {
    fbank_tgabor.RenderFilters(tgabor_filters, tgabor_flt_sizes);
  }
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
    if(fbank_tgabor.on) {
      tgabor_trial_raw.SetGeom(5, fbank_tgabor.n_filters, 2, mel_fbank.n_filters,
                               input.trial_steps, input.channels);
      tgabor_trial_out.SetGeom(5, fbank_tgabor.n_filters, 2, mel_fbank.n_filters,
                               input.trial_steps, input.channels);
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
  input_pos = 0;
  trial_start_pos = 0;
  trial_end_pos = trial_start_pos + input.trial_samples;
  dft_power_trial_out.InitVals(0.0f);
  if(dft.log_pow) {
    dft_log_power_trial_out.InitVals(0.0f);
  }
  if(mel_fbank.on) {
    mel_fbank_trial_out.InitVals(0.0f);
    if(fbank_tgabor.on) {
      tgabor_trial_raw.InitVals(0.0f);
      tgabor_trial_out.InitVals(0.0f);
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
  if(fbank_tgabor.on) {
    TimeGaborFilter(chan);
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

void AuditoryProc::TimeGaborFilter(int chan) {
  for(int stp = 0; stp < input.trial_steps; stp++) {
    for(int flt = 0; flt < mel_fbank.n_filters; flt++) {
      const int tgnf = fbank_tgabor.n_filters;
      for(int ti = 0; ti < tgnf; ti++) {
        int sz = tgabor_flt_sizes[ti];
        int fsz = sz * 2;
        int in_st = input.border_steps + stp - sz;
        float fsum = 0.0f;
        for(int fi = 0; fi < fsz; fi++) {
          float fval = tgabor_filters.FastEl2d(fi, ti);
          float ival = mel_fbank_trial_out.FastEl3d(flt, in_st + fi, chan);
          fsum += fval * ival;
        }
        bool pos = (fsum >= 0.0f);
        float act = fabsf(fsum);
        if(fbank_tgabor.xx1_norm) {
          act = fbank_tgabor.XX1(act);
        }
        if(pos) {
          tgabor_trial_raw.FastEl(ti, 0, flt, stp, chan) = act;
          tgabor_trial_raw.FastEl(ti, 1, flt, stp, chan) = 0.0f;
        }
        else {
          tgabor_trial_raw.FastEl(ti, 0, flt, stp, chan) = 0.0f;
          tgabor_trial_raw.FastEl(ti, 1, flt, stp, chan) = act;
        }
      }
    }
  }

  float_MatrixPtr raw_frm; raw_frm = (float_Matrix*)tgabor_trial_raw.GetFrameSlice(chan);
  float_MatrixPtr out_frm; out_frm = (float_Matrix*)tgabor_trial_out.GetFrameSlice(chan);
  
  if(tgabor_kwta.On()) {
    tgabor_kwta.Compute_Inhib(*raw_frm, *out_frm, tgabor_gci);
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

    if(fbank_tgabor.on) {
      col = data_table->FindMakeColName(name + "_mel_tgabor" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        fbank_tgabor.n_filters, 2,
                                        input.trial_steps, mel_fbank.n_filters);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        const int tgnf = fbank_tgabor.n_filters;
        for(int stp = 0; stp < input.trial_steps; stp++) {
          for(int i=0; i< mel_fbank.n_filters; i++) {
            for(int ti=0; ti < tgnf; ti++) {
              dout->FastEl4d(ti, 0, stp, i) = tgabor_trial_out.FastEl(ti, 0, i, stp, chan);
              dout->FastEl4d(ti, 1, stp, i) = tgabor_trial_out.FastEl(ti, 1, i, stp, chan);
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

void AuditoryProc::GridTimeGaborFilters(DataTable* graph_data) {
  if(NeedsInit()) Init();
  fbank_tgabor.GridFilters(tgabor_filters, tgabor_flt_sizes, graph_data, true);
}
