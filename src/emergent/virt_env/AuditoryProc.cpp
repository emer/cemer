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

TA_BASEFUNS_CTORS_DEFN(AudInputSpec);
TA_BASEFUNS_CTORS_DEFN(MelFreqSpec);
TA_BASEFUNS_CTORS_DEFN(AuditoryProc);

void AudInputSpec::Initialize() {
  win_msec = 25.0f;
  step_msec = 12.5f;
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
  mel_filter_out.SetGeom(1, mel.n_filters);
  log_mel_filter_out.SetGeom(1, mel.n_filters);
  mfcc_dct_out.SetGeom(1, n_mfcc);
  return true;
}

bool AuditoryProc::InitDataTable() {
  return true;
}

bool AuditoryProc::MelOutputToTable(DataTable* dtab, bool fmt_only) {
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
    mel_filter_out.FastEl_Flat(mi) = sum;
  }
}


bool AuditoryProc::OutputToTable(int chan, int step) {
  return true;
}
