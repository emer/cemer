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
#include <taMisc>

TA_BASEFUNS_CTORS_DEFN(AudInputSpec);
TA_BASEFUNS_CTORS_DEFN(MelFBankSpec);
TA_BASEFUNS_CTORS_DEFN(AudRenormSpec);
TA_BASEFUNS_CTORS_DEFN(AudDeltaSpec);
TA_BASEFUNS_CTORS_DEFN(MelCepstrumSpec);
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


void AudDeltaSpec::Initialize() {
  on = true;
  n_steps = 2;
  avg = false;
  xx1_norm = true;
  xx1_gain = 10.0f;
}


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
  input_save = NO_SAVE;
  input_pos = 0;
  dft_size = 0;
  dft_use = 0;
  mel_n_filters_eff = 0;
  mel_filt_max_bins = 0;
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
  if(mel_fbank.on) {
    mel_fbank_out.SetGeom(1, mel_fbank.n_filters);
    if(fbank_delta.on) {
      mel_fbank_tavgs.SetGeom(2, fbank_delta.n_steps, mel_fbank.n_filters);
      mel_fbank_deltas.SetGeom(2, fbank_delta.n_steps*2 + 1, mel_fbank.n_filters);
    }
    if(mfcc.on) {
      mfcc_dct_out.SetGeom(1, mel_fbank.n_filters);
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
  input_pos = 0;
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
  for(int step = 0; step < input.trial_steps; step++) {
    ProcessStep(step);
  }
  return true;
}

bool AuditoryProc::ProcessStep(int step) {
  for(int chan=0; chan < input.channels; chan++) {
    SoundToWindow(input_pos, chan);
    FilterWindow(chan, step);
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

bool AuditoryProc::FilterWindow(int chan, int step) {
  DftInput();
  if(mel_fbank.on) {
    PowerOfDft();
    MelFilterDft();
    if(mfcc.on) {
      CepstrumDctMel();
    }
    MelFilterDeltas(chan, step);
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
  }
}

void AuditoryProc::MelFilterDeltas(int chan, int step) {
  int idx;
  String col_sufx;
  if(input.channels > 1) {
    col_sufx = "_ch" + String(chan);
  }
  DataCol* col = data_table->FindColName(name + "_mel_fbank" + col_sufx);
  if(TestError(!col, "MelFilterDeltas", "cannot find mel_fbank column in data table!")) {
    return;
  }
  float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
  for(int flt = 0; flt < mel_fbank.n_filters; flt++) {
    for(int stp = 1; stp <= fbank_delta.n_steps; stp++) {
      float cmp_val = 0.0f;
      if(fbank_delta.avg) {
        for(int bkstp = 1; bkstp <= stp; bkstp++) {
          if(step >= bkstp) {
            cmp_val += dout->FastEl2d(step-bkstp, flt);
          }
          else {
            cmp_val += dout->FastEl2d(input.trial_steps + step-bkstp, flt); // wrap..
          }
        }
        cmp_val /= (float)stp;
      }
      else {
        if(step >= stp) {
          cmp_val = dout->FastEl2d(step-stp, flt);
        }
        else {
          cmp_val = dout->FastEl2d(input.trial_steps + step-stp, flt); // wrap..
        }
      }
      mel_fbank_tavgs.FastEl2d((stp-1), flt) = cmp_val;

      float cur = mel_fbank_out.FastEl_Flat(flt);
      float del = cur - cmp_val;
      float del_mag = fabsf(del);
      if(fbank_delta.xx1_norm) {
        del_mag = fbank_delta.XX1(del_mag);
      }
      int stidx = (stp-1)*2;
      if(del >= 0.0f) {
        mel_fbank_deltas.FastEl2d(stidx, flt) = del_mag;
        mel_fbank_deltas.FastEl2d(stidx+1, flt) = 0.0f;
      }
      else {
        mel_fbank_deltas.FastEl2d(stidx, flt) = 0.0f;
        mel_fbank_deltas.FastEl2d(stidx+1, flt) = del_mag;
      }
    }
  }
}

void AuditoryProc::CepstrumDctMel() {
  memcpy(mfcc_dct_out.el, mel_fbank_out.el, mel_fbank.n_filters * sizeof(float));
  taMath_float::dct(&mfcc_dct_out);
  float& el0 = mfcc_dct_out.FastEl_Flat(0);
  el0 = logf(1.0f + el0*el0);              // replace with log energy instead..
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
    MelOutputToTable(data_table, chan, 0, true);
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

bool AuditoryProc::OutputToTable(int chan, int step) {
  if(!data_table || save_mode == NONE_SAVE) // bail now
    return false;
  data_table->StructUpdate(true);
  if(mel_fbank.on) {
    MelOutputToTable(data_table, chan, step, false); // not fmt_only
  }
  data_table->StructUpdate(false);
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

  if(mel_fbank.on) {
    col = data_table->FindMakeColName(name + "_mel_fbank" + col_sufx, idx,
                                      DataTable::VT_FLOAT, 2,
                                      input.trial_steps, mel_fbank.n_filters);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(int i=0; i< mel_fbank.n_filters; i++) {
        dout->FastEl2d(step, i) = mel_fbank_out.FastEl_Flat(i);
      }
    }

    if(fbank_delta.on) {
      col = data_table->FindMakeColName(name + "_mel_fbank_tavg" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        1, fbank_delta.n_steps,
                                        input.trial_steps, mel_fbank.n_filters);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        for(int i=0; i< mel_fbank.n_filters; i++) {
          for(int stp=0; stp< fbank_delta.n_steps; stp++) {
            dout->FastEl4d(0, stp, step, i) = mel_fbank_tavgs.FastEl2d(stp, i);
          }
        }
      }

      col = data_table->FindMakeColName(name + "_mel_fbank_delta" + col_sufx, idx,
                                        DataTable::VT_FLOAT, 4,
                                        fbank_delta.n_steps*2 + 1, 1,
                                        input.trial_steps, mel_fbank.n_filters);
      if(!fmt_only) {
        float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
        for(int i=0; i< mel_fbank.n_filters; i++) {
          dout->FastEl4d(0, 0, step, i) = mel_fbank_out.FastEl_Flat(i);
          for(int stp=0; stp< 2*fbank_delta.n_steps; stp++) {
            dout->FastEl4d(1 + stp, 0, step, i) = mel_fbank_deltas.FastEl2d(stp, i);
          }
        }
      }
    }
  }

  if(mfcc.on) {
    col = data_table->FindMakeColName(name + "_mel_mfcc" + col_sufx, idx,
                                      DataTable::VT_FLOAT, 2,
                                      input.trial_steps, mfcc.n_coeff);
    if(!fmt_only) {
      float_MatrixPtr dout; dout = (float_Matrix*)col->GetValAsMatrix(-1);
      for(int i=0; i< mfcc.n_coeff; i++) {
        dout->FastEl2d(step, i) = mfcc_dct_out.FastEl_Flat(i);
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
