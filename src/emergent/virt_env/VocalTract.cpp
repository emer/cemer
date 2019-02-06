// Copyright 2016-2018, Regents of the University of Colorado,
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

/***************************************************************************
 *  Copyright 1991, 1992, 1993, 1994, 1995, 1996, 2001, 2002               *
 *    David R. Hill, Leonard Manzara, Craig Schock                         *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
// 2014-09
// This file was copied from Gnuspeech and modified by Marcelo Y. Matuda.

/******************************************************************************
*
*     Program:       tube
*
*     Description:   Software (non-real-time) implementation of the Tube
*                    Resonance Model for speech production.
*
*     Author:        Leonard Manzara
*
*     Date:          July 5th, 1994
*
******************************************************************************/

#include "VocalTract.h"
#include <DataTable>
#include <float_Matrix>
#include <MemberDef>
#include <taMath_float>

#include <taMisc>

#include <QFile>

TA_BASEFUNS_CTORS_DEFN(VocalTractConfig);
TA_BASEFUNS_CTORS_DEFN(VoiceParams);
TA_BASEFUNS_CTORS_DEFN(VocalTractCtrl);
TA_BASEFUNS_CTORS_DEFN(VocalTract);

/*  COMPILE SO THAT INTERPOLATION NOT DONE FOR SOME CONTROL RATE PARAMETERS  */
//#define MATCH_DSP                 1

#define GS_TRM_TUBE_MIN_RADIUS (0.001)

#define INPUT_VECTOR_RESERVE 128
#define OUTPUT_VECTOR_RESERVE 1024

#define GLOTTAL_SOURCE_PULSE 0
#define GLOTTAL_SOURCE_SINE 1

/*  PITCH VARIABLES  */
#define PITCH_BASE                220.0
#define PITCH_OFFSET              3           /*  MIDDLE C = 0  */
//#define LOG_FACTOR                3.32193

/*  RANGE OF ALL VOLUME CONTROLS  */
#define VOL_MAX                   60

/*  SCALING CONSTANT FOR INPUT TO VOCAL TRACT & THROAT (MATCHES DSP)  */
//#define VT_SCALE                  0.03125     /*  2^(-5)  */
// this is a temporary fix only, to try to match dsp synthesizer
#define VT_SCALE                  0.125     /*  2^(-3)  */

/*  FINAL OUTPUT SCALING, SO THAT .SND FILES APPROX. MATCH DSP OUTPUT  */
#define OUTPUT_SCALE              0.95

/*  BI-DIRECTIONAL TRANSMISSION LINE POINTERS  */
#define TOP                       0
#define BOTTOM                    1

//#define OUTPUT_SRATE_LOW          22050.0
//#define OUTPUT_SRATE_HIGH         44100.0


/////////////////////////////////////////////////////
//              VocalTractConfig

void VocalTractConfig::Initialize() {
  Defaults();
}

void VocalTractConfig::Defaults() {
  temp = 32.0f;
  loss = 0.8f;
  mouth_coef = 5000.0f;
  nose_coef = 5000.0f;
  throat_cutoff = 1500.0f;
  throat_vol = 6.0f;
  vtl_off = 0.0f;
  waveform = PULSE;
  noise_mod = true;
  mix_off = 48.0f;
}

/////////////////////////////////////////////////////
//              VoiceParams

void VoiceParams::Initialize() {
  Female();
}

void VoiceParams::GlobalDefaultParams() {
  glot_pulse_rise = 40.0;
  aperture_radius = 3.05;
  nose_radius_1 = 1.35;
  nose_radius_2 = 1.96;
  nose_radius_3 = 1.91;
  nose_radius_4 = 1.3;
  nose_radius_5 = 0.73;
  radius_1 = 0.8f;
  nose_radius_coef = 1.0;
  radius_coef = 1.0;
}

void VoiceParams::Male() {
  GlobalDefaultParams();
  tract_length = 17.5;
  glot_pulse_fall_min = 24.0;
  glot_pulse_fall_max = 24.0;
  glot_pitch_ref = -12.0;
  breathiness = 0.5;
}  

void VoiceParams::Female() {
  GlobalDefaultParams();
  tract_length = 15.0;
  glot_pulse_fall_min = 32.0;
  glot_pulse_fall_max = 32.0;
  glot_pitch_ref = 0.0;
  breathiness = 1.5;
}

void VoiceParams::ChildLg() {
  GlobalDefaultParams();
  tract_length = 12.5;
  glot_pulse_fall_min = 24.0;
  glot_pulse_fall_max = 24.0;
  glot_pitch_ref = 2.5;
  breathiness = 1.5;
}

void VoiceParams::ChildSm() {
  GlobalDefaultParams();
  tract_length = 10.0;
  glot_pulse_fall_min = 24.0;
  glot_pulse_fall_max = 24.0;
  glot_pitch_ref = 5.0;
  breathiness = 1.5;
}

void VoiceParams::Baby() {
  GlobalDefaultParams();
  tract_length = 7.5;
  glot_pulse_fall_min = 24.0;
  glot_pulse_fall_max = 24.0;
  glot_pitch_ref = 7.5;
  breathiness = 1.5;
}

void VoiceParams::SetDefault(VoiceDefs voice_defs) {
  switch(voice_defs) {
  case MALE:
    Male();
    break;
  case FEMALE:
    Female();
    break;
  case CHILD_LG:
    ChildLg();
    break;
  case CHILD_SM:
    ChildSm();
    break;
  case BABY:
    Baby();
    break;
  }
}


/////////////////////////////////////////////////////
//              VocalTractCtrl

void VocalTractCtrl::Initialize() {
  glot_pitch = 0.0f;
  glot_vol = 0.0f;
  asp_vol = 0.0f;
  fric_vol = 0.0f;
  fric_pos = 4.0f;
  fric_cf = 2500.0f;
  fric_bw = 2000.0f;
  radius_2 = 1.0f;
  radius_3 = 1.0f;
  radius_4 = 1.0f;
  radius_5 = 1.0f;
  radius_6 = 1.0f;
  radius_7 = 1.0f;
  radius_8 = 1.0f;
  velum = .1f;
}

void VocalTractCtrl::ComputeDeltas(const VocalTractCtrl& cur, const VocalTractCtrl& prv,
                                   const VocalTractCtrl& del_max, float ctrl_freq) {
  for(int i=0; i< N_PARAMS; i++) {
    float cval = cur.ParamVal(i);
    float pval = prv.ParamVal(i);
    float dmax = del_max.ParamVal(i);
    float& nval = ParamVal(i);
    nval = (cval - pval) * ctrl_freq;
    // if(nval > dmax) nval = dmax;
    // else if (nval < -dmax) nval = -dmax;
  }
}

void VocalTractCtrl::UpdateFromDeltas(const VocalTractCtrl& del) {
  for(int i=0; i< N_PARAMS; i++) {
    float dval = del.ParamVal(i);
    float& nval = ParamVal(i);
    nval += dval;
  }
}

void VocalTractCtrl::DefaultMaxDeltas() {
  float ctrl_freq = 1.0f / 501.0f; // default
  // default to entire range ok here for now.. fix when glitches encountered..
  glot_pitch = 10.0f * ctrl_freq;
  glot_vol = 60.0f * ctrl_freq;
  asp_vol = 10.0f * ctrl_freq;
  fric_vol = 24.0f * ctrl_freq;
  fric_pos = 7.0f * ctrl_freq;
  fric_cf = 3000.0f * ctrl_freq;
  fric_bw = 4000.0f * ctrl_freq;
  radius_2 = 3.0f * ctrl_freq;
  radius_3 = 3.0f * ctrl_freq;
  radius_4 = 3.0f * ctrl_freq;
  radius_5 = 3.0f * ctrl_freq;
  radius_6 = 3.0f * ctrl_freq;
  radius_7 = 3.0f * ctrl_freq;
  radius_8 = 3.0f * ctrl_freq;
  velum = 1.5f * ctrl_freq;
}

void VocalTractCtrl::SetFromParams(const VocalTractCtrl& oth) {
  for(int i=0; i< N_PARAMS; i++) {
    ParamVal(i) = oth.ParamVal(i);
  }
}

void VocalTractCtrl::SetFromFloat(float val, ParamIndex param, bool normalized) {
  TypeDef* td = GetTypeDef();
  int stidx = td->members.FindNameIdx("glot_pitch");
  MemberDef* md = td->members[stidx + param];
  float* par = (float*)md->GetOff(this);
  if(normalized) {
    float min = md->OptionAfter("MIN_").toFloat();
    float max = md->OptionAfter("MAX_").toFloat();
    *par = min + val * (max - min);
  }
  else {
    *par = val;
  }
}

float VocalTractCtrl::Normalize(float val, ParamIndex param) {
  TypeDef* td = GetTypeDef();
  int stidx = td->members.FindNameIdx("glot_pitch");
  MemberDef* md = td->members[stidx + param];
  float* par = (float*)md->GetOff(this);
  float min = md->OptionAfter("MIN_").toFloat();
  float max = md->OptionAfter("MAX_").toFloat();
  return (val - min) / (max - min);
}

float VocalTractCtrl::UnNormalize(float val, ParamIndex param) {
  TypeDef* td = GetTypeDef();
  int stidx = td->members.FindNameIdx("glot_pitch");
  MemberDef* md = td->members[stidx + param];
  float* par = (float*)md->GetOff(this);
  float min = md->OptionAfter("MIN_").toFloat();
  float max = md->OptionAfter("MAX_").toFloat();
  return min + val * (max - min);
}

void VocalTractCtrl::SetFromFloats(const float* vals, bool normalized) {
  for(int i=0; i < N_PARAMS; i++) {
    SetFromFloat(vals[i], (ParamIndex)i, normalized);
  }
}

void VocalTractCtrl::SetFromMatrix(const float_Matrix& matrix, bool normalized) {
  if(TestError(matrix.size < N_PARAMS, "SetFromMatrix", "need at least", String(N_PARAMS),
               "elements in the matrix!")) {
    return;
  }
  SetFromFloats(matrix.el, normalized);
}

void VocalTractCtrl::SetFromDataTable(const DataTable& table, const Variant& col, int row,
                                      bool normalized) {
  float_MatrixPtr mtx;
  mtx = (float_Matrix*)table.GetValAsMatrix(col, row);
  if(TestError(!(bool)mtx, "SetFromDataTable", "matrix column not found")) {
    return;
  }
  SetFromMatrix(*(mtx.ptr()), normalized);
}

/////////////////////////////////////////////////////
//              VocalTract


void VocalTract::CtrlFromFloats(const float* vals, bool normalized) {
  cur_ctrl.SetFromFloats(vals, normalized);
}

void VocalTract::CtrlFromMatrix(const float_Matrix& matrix, bool normalized) {
  cur_ctrl.SetFromMatrix(matrix, normalized);
}

void VocalTract::CtrlFromDataTable(const DataTable& table, const Variant& col, int row,
                                   bool normalized) {
  cur_ctrl.SetFromDataTable(table, col, row, normalized);
}

void VocalTract::SynthFromDataTable(const DataTable& table, const Variant& col, int row,
                                    bool normalized, bool reset_first) {
  float_MatrixPtr mtx;
  mtx = (float_Matrix*)table.GetValAsMatrix(col, row);
  if(TestError(!(bool)mtx, "SynthFromDataTable", "matrix column not found")) {
    return;
  }
  if(mtx->dims() == 2 && mtx->dim(0) == VocalTractCtrl::N_PARAMS) {
    // multi-dim case..
    int n_outer = mtx->dim(1);
    for(int i=0; i< n_outer; i++) {
      float_MatrixPtr frm;
      frm = (float_Matrix*)mtx->GetFrameSlice(i);
      CtrlFromMatrix(*frm, normalized);
      Synthesize(reset_first && (i == 0));
    }
  }
  else {
    // one-shot
    cur_ctrl.SetFromDataTable(table, col, row, normalized);
    Synthesize(reset_first);
  }
}

bool VocalTract::LoadEnglishPhones() {
  QFile qrc_file(":/VocalTractEnglishPhones.dtbl");
  qrc_file.open(QIODevice::ReadOnly);
  QByteArray dat = qrc_file.readAll();
  String str(dat);
  phone_table.Load_String(str);
  phone_table.ClearDataFlag(DataTable::SAVE_ROWS);
  return true;
}

bool VocalTract::LoadEnglishDict() {
  QFile qrc_file(":/VocalTractEnglishDict.dtbl");
  qrc_file.open(QIODevice::ReadOnly);
  QByteArray dat = qrc_file.readAll();
  String str(dat);
  dict_table.Load_String(str);
  dict_table.ClearDataFlag(DataTable::SAVE_ROWS);
  return true;
}

bool VocalTract::SynthPhone(const String& phon, bool stress, bool double_stress,
                            bool syllable, bool reset) {
  if(phone_table.rows == 0)
    LoadEnglishPhones();
  String act = phon;
  if(stress)
    act = act + "'";
  int idx = phone_table.FindVal(act, "phone", 0, true);
  if(idx < 0) return false;
  float duration = phone_table.GetVal("duration", idx).toFloat();
  float transition = phone_table.GetVal("transition", idx).toFloat();
  float tot_time = (duration + transition) * 1.5f;
  int n_reps = taMath_float::ceil(tot_time / synth_dur_msec);
  n_reps = MAX(n_reps, 1);
  CtrlFromDataTable(phone_table, "phone_data", idx, false);
  // todo: syllable, double_stress, qsss other params??
  // taMisc::Info("saying:", phon, "dur:", String(tot_time), "n_reps:", String(n_reps),
  //              "start pos:", String(outputData_.size()));
  if(reset)
    SynthReset();               // MUST reset *AFTER* setting the new ctrls!
  for(int i=0; i<n_reps; i++) {
    Synthesize(false);
  }
  return true;
}

bool VocalTract::SynthPhones(const String& phones, bool reset_first, bool play) {
  int len = phones.length();
  String phon;
  bool stress = false;
  bool double_stress = false;
  bool syllab = false;
  bool first = true;
  for(int pos = 0; pos < len; pos++) {
    int c = phones[pos];
    if(c == '\'') { // stress
      stress = true;
      continue;
    }
    if(c == '\"') { // double stress
      double_stress = true;
      continue;
    }
    if(c == '%') { // double stress
      SynthPhone(phon, stress, double_stress, syllab, reset_first && first);
      phon = "";
      first = false;
      break;                    // done
    }
    if(c == '.') { // syllable
      syllab = true;
      SynthPhone(phon, stress, double_stress, syllab, reset_first && first);
      stress = false; double_stress = false; syllab = false;
      phon = "";
      first = false;
      continue;
    }
    if(c == '_') { // reg separator
      SynthPhone(phon, stress, double_stress, syllab, reset_first && first);
      stress = false; double_stress = false; syllab = false;
      phon = "";
      first = false;
      continue;
    }
    phon += (char)c;
  }
  if(phon.nonempty()) {
    SynthPhone(phon, stress, double_stress, syllab, reset_first && first);
  }
  
  if(play)
    PlaySound();
  return true;
}

bool VocalTract::SynthWord(const String& word, bool reset_first, bool play) {
  if(dict_table.rows == 0)
    LoadEnglishDict();
  int idx = dict_table.FindVal(word, "word", 0, true);
  if(idx < 0) return false;
  String phones = dict_table.GetVal("phones", idx).toString();
  return SynthPhones(phones, reset_first, play);
}

bool VocalTract::SynthWords(const String& words, bool reset_first, bool play) {
  String_Array sary;
  sary.Split(words, " ");
  bool rval = true;
  for(int i=0; i < sary.size; i++) {
    rval = SynthWord(sary[i], (reset_first && (i == 0)), false);
    if(!rval) break;
    if(i < sary.size-1) {
      SynthPhone("#");
    }
  }
  if(play)
    PlaySound();
  return rval;
}

void VocalTract::Initialize() {
  volume = 60.0f;
  balance = 0.0f;
  synth_dur_msec = 25.0f;
  controlRate_ = 0.0;
  del_max.DefaultMaxDeltas();
  reset();
  outputData_.reserve(OUTPUT_VECTOR_RESERVE);
}

void
VocalTract::reset()
{
  controlPeriod_    = 0;
  actualTubeLength_ = 0.0;
  memset(&oropharynx_[0][0][0], 0, sizeof(float) * TOTAL_SECTIONS * 2 * 2);
  memset(oropharynxCoeff_,      0, sizeof(float) * TOTAL_COEFFICIENTS);
  memset(&nasal_[0][0][0],      0, sizeof(float) * TOTAL_NASAL_SECTIONS * 2 * 2);
  memset(nasalCoeff_,           0, sizeof(float) * TOTAL_NASAL_COEFFICIENTS);
  memset(alpha_,                0, sizeof(float) * TOTAL_ALPHA_COEFFICIENTS);
  currentPtr_ = 1;
  prevPtr_    = 0;
  memset(fricationTap_, 0, sizeof(float) * TOTAL_FRIC_COEFFICIENTS);
  dampingFactor_     = 0.0;
  crossmixFactor_    = 0.0;
  breathinessFactor_ = 0.0;
  prevGlotAmplitude_ = -1.0;
  outputData_.resize(0);

  if (srConv_.get() != NULL) srConv_->reset();
  if (mouthRadiationFilter_.get() != NULL) mouthRadiationFilter_->reset();
  if (mouthReflectionFilter_.get() != NULL) mouthReflectionFilter_->reset();
  if (nasalRadiationFilter_.get() != NULL) nasalRadiationFilter_->reset();
  if (nasalReflectionFilter_.get() != NULL) nasalReflectionFilter_->reset();
  if (throat_.get() != NULL) throat_->reset();
  if (glottalSource_.get() != NULL) glottalSource_->reset();
  if (bandpassFilter_.get() != NULL) bandpassFilter_->reset();
  if (noiseFilter_.get() != NULL) noiseFilter_->reset();
  if (noiseSource_.get() != NULL) noiseSource_->reset();
}

/******************************************************************************
 *
 *  function:  speedOfSound
 *
 *  purpose:   Returns the speed of sound according to the value of
 *             the temperature (in Celsius degrees).
 *
 ******************************************************************************/
float
VocalTract::speedOfSound(float temperature)
{
  return 331.4 + (0.6 * temperature);
}

/******************************************************************************
 *
 *  function:  initializeSynthesizer
 *
 *  purpose:   Initializes all variables so that the synthesis can
 *             be run.
 *
 ******************************************************************************/
void
VocalTract::initializeSynthesizer()
{
  float nyquist = 1.0f;

  /*  CALCULATE THE SAMPLE RATE, BASED ON NOMINAL TUBE LENGTH AND SPEED OF SOUND  */
  if (voice.tract_length > 0.0) {
    float c = speedOfSound(config.temp);
    controlPeriod_ = static_cast<int>(rint((c * TOTAL_SECTIONS * 100.0) / (voice.tract_length * controlRate_)));
    sampleRate_ = static_cast<int>(controlRate_ * controlPeriod_);
    actualTubeLength_ = (c * TOTAL_SECTIONS * 100.0) / sampleRate_;
    nyquist = sampleRate_ / 2.0;
  } else {
    taMisc::Error("Illegal tube length");
  }

  /*  CALCULATE THE BREATHINESS FACTOR  */
  breathinessFactor_ = voice.breathiness / 100.0;

  /*  CALCULATE CROSSMIX FACTOR  */
  crossmixFactor_ = 1.0 / amplitude(config.mix_off);

  /*  CALCULATE THE DAMPING FACTOR  */
  dampingFactor_ = (1.0 - (config.loss / 100.0));

  /*  INITIALIZE THE WAVE TABLE  */
  glottalSource_.reset(new WavetableGlottalSource
                       (config.waveform == GLOTTAL_SOURCE_PULSE ?
                        WavetableGlottalSource::TYPE_PULSE :
                        WavetableGlottalSource::TYPE_SINE,
                        sampleRate_,
                        voice.glot_pulse_rise, voice.glot_pulse_fall_min,
                        voice.glot_pulse_fall_max));

  /*  INITIALIZE REFLECTION AND RADIATION FILTER COEFFICIENTS FOR MOUTH  */
  float mouthApertureCoeff = (nyquist - config.mouth_coef) / nyquist;
  mouthRadiationFilter_.reset(new RadiationFilter(mouthApertureCoeff));
  mouthReflectionFilter_.reset(new ReflectionFilter(mouthApertureCoeff));

  /*  INITIALIZE REFLECTION AND RADIATION FILTER COEFFICIENTS FOR NOSE  */
  float nasalApertureCoeff = (nyquist - config.nose_coef) / nyquist;
  nasalRadiationFilter_.reset(new RadiationFilter(nasalApertureCoeff));
  nasalReflectionFilter_.reset(new ReflectionFilter(nasalApertureCoeff));

  /*  INITIALIZE NASAL CAVITY FIXED SCATTERING COEFFICIENTS  */
  initializeNasalCavity();

  /*  INITIALIZE THE THROAT LOWPASS FILTER  */
  throat_.reset(new Throat(sampleRate_, config.throat_cutoff,
                           amplitude(config.throat_vol)));

  /*  INITIALIZE THE SAMPLE RATE CONVERSION ROUTINES  */
  srConv_.reset(new SampleRateConverter(sampleRate_, SampleRate(), outputData_));

  /*  INITIALIZE THE OUTPUT VECTOR  */
  outputData_.clear();

  bandpassFilter_.reset(new BandpassFilter());
  noiseFilter_.reset(new NoiseFilter());
  noiseSource_.reset(new NoiseSource());
}

void
VocalTract::InitSynth() {
  SynthInitBuffer();
  reset();
  float ctrl_rate = 1.0f / (synth_dur_msec / 1000.0f);
  controlRate_ = ctrl_rate;
  initializeSynthesizer();
  prv_ctrl.SetFromParams(cur_ctrl); // no deltas if reset
  currentData_.SetFromParams(cur_ctrl);
  SigEmitUpdated();
}

void
VocalTract::SetVoice() {
  voice.CallFun("SetDefault");
  InitSynth();
  SigEmitUpdated();
}

void
VocalTract::SynthInitBuffer() {
  InitBuffer((synth_dur_msec / 1000.0f) * 44100.0, 44100.0);
}

void
VocalTract::SynthReset(bool init_buffer) {
  InitSynth();
  if(init_buffer)
    SynthInitBuffer();
}

void
VocalTract::Synthesize(bool reset_first)
{
  float ctrl_rate = 1.0f / (synth_dur_msec / 1000.0f);
  if(ctrl_rate != controlRate_ || !IsValid()) {
    InitSynth();
  }
  else if(reset_first) {
    SynthReset(true);           // init buffer
  }

  float controlFreq = 1.0f / controlPeriod_;

  // taMisc::Info("control period:", String(controlPeriod_), "freq:", String(controlFreq));
  
  del_ctrl.ComputeDeltas(cur_ctrl, prv_ctrl, del_max, controlFreq);

  for (int j = 0; j < controlPeriod_; j++) {
    Synthesize_impl();
    currentData_.UpdateFromDeltas(del_ctrl);
  }

  prv_ctrl.SetFromParams(currentData_); // prev is where we actually got, not where we wanted to get..

  int samp_size = SampleSize();
  SoundSampleType stype = SampleType();
  
  int n_frm = outputData_.size();
  if(FrameCount() < n_frm) {
    InitBuffer(n_frm, SampleRate(), ChannelCount(), samp_size, stype);
  }
  float scale = calculateMonoScale();
#if (QT_VERSION >= 0x050000)
  void* buf = q_buf.data();
  for(int i=0; i < n_frm; i++) {
    WriteFloatAtIdx(outputData_[i] * scale, buf, i, stype, samp_size);
  }
#endif
  SigEmitUpdated();
}

void
VocalTract::Synthesize_impl()
{
  /*  CONVERT PARAMETERS HERE  */
  float f0 = frequency(currentData_.glot_pitch);
  float ax = amplitude(currentData_.glot_vol);
  float ah1 = amplitude(currentData_.asp_vol);
  calculateTubeCoefficients();
  setFricationTaps();
  bandpassFilter_->update(sampleRate_, currentData_.fric_bw, currentData_.fric_cf);

  /*  DO SYNTHESIS HERE  */
  /*  CREATE LOW-PASS FILTERED NOISE  */
  float lpNoise = noiseFilter_->filter(noiseSource_->getSample());

  /*  UPDATE THE SHAPE OF THE GLOTTAL PULSE, IF NECESSARY  */
  if (config.waveform == GLOTTAL_SOURCE_PULSE) {
    if (ax != prevGlotAmplitude_) {
      glottalSource_->updateWavetable(ax);
    }
  }

  /*  CREATE GLOTTAL PULSE (OR SINE TONE)  */
  float pulse = glottalSource_->getSample(f0);

  /*  CREATE PULSED NOISE  */
  float pulsedNoise = lpNoise * pulse;

  /*  CREATE NOISY GLOTTAL PULSE  */
  pulse = ax * ((pulse * (1.0 - breathinessFactor_)) +
                (pulsedNoise * breathinessFactor_));

  float signal;
  /*  CROSS-MIX PURE NOISE WITH PULSED NOISE  */
  if (config.noise_mod) {
    float crossmix = ax * crossmixFactor_;
    crossmix = (crossmix < 1.0) ? crossmix : 1.0;
    signal = (pulsedNoise * crossmix) +
      (lpNoise * (1.0 - crossmix));
  } else {
    signal = lpNoise;
  }

  /*  PUT SIGNAL THROUGH VOCAL TRACT  */
  signal = vocalTract(((pulse + (ah1 * signal)) * VT_SCALE),
                      bandpassFilter_->filter(signal));

  /*  PUT PULSE THROUGH THROAT  */
  signal += throat_->process(pulse * VT_SCALE);

  /*  OUTPUT SAMPLE HERE  */
  srConv_->dataFill(signal);

  prevGlotAmplitude_ = ax;
}


/******************************************************************************
 *
 *  function:  initializeNasalCavity
 *
 *  purpose:   Calculates the scattering coefficients for the fixed
 *             sections of the nasal cavity.
 *
 ******************************************************************************/
void
VocalTract::initializeNasalCavity()
{
  float radA2, radB2;

  /*  CALCULATE COEFFICIENTS FOR INTERNAL FIXED SECTIONS OF NASAL CAVITY  */
  for (int i = N2, j = NC2; i < N6; i++, j++) {
    radA2 = voice.NoseRadiusVal(i); radA2 *= radA2;
    radB2 = voice.NoseRadiusVal(i + 1); radB2 *= radB2;
    nasalCoeff_[j] = (radA2 - radB2) / (radA2 + radB2);
  }

  /*  CALCULATE THE FIXED COEFFICIENT FOR THE NOSE APERTURE  */
  radA2 = voice.NoseRadiusVal(N6); radA2 *= radA2;
  radB2 = voice.aperture_radius * voice.aperture_radius;
  nasalCoeff_[NC6] = (radA2 - radB2) / (radA2 + radB2);
}


/******************************************************************************
 *
 *  function:  calculateTubeCoefficients
 *
 *  purpose:   Calculates the scattering coefficients for the vocal
 *             ract according to the current radii.  Also calculates
 *             the coefficients for the reflection/radiation filter
 *             pair for the mouth and nose.
 *
 ******************************************************************************/
void
VocalTract::calculateTubeCoefficients()
{
  float radA2, radB2, r0_2, r1_2, r2_2, sum;

  /*  CALCULATE COEFFICIENTS FOR THE OROPHARYNX  */
  for (int i = 0; i < (TOTAL_REGIONS - 1); i++) {
    radA2 = currentData_.RadiusVal(i); radA2 *= radA2;
    radB2 = currentData_.RadiusVal(i + 1);  radB2 *= radB2;
    oropharynxCoeff_[i] = (radA2 - radB2) / (radA2 + radB2);
  }

  /*  CALCULATE THE COEFFICIENT FOR THE MOUTH APERTURE  */
  radA2 = currentData_.RadiusVal(R8); radA2 *= radA2;
  radB2 = voice.aperture_radius * voice.aperture_radius;
  oropharynxCoeff_[C8] = (radA2 - radB2) / (radA2 + radB2);

  /*  CALCULATE ALPHA COEFFICIENTS FOR 3-WAY JUNCTION  */
  /*  NOTE:  SINCE JUNCTION IS IN MIDDLE OF REGION 4, r0_2 = r1_2  */
  r0_2 = currentData_.RadiusVal(R4);  r0_2 *= r0_2;
  r1_2 = r0_2;
  r2_2 = currentData_.velum * currentData_.velum;
  sum = 2.0 / (r0_2 + r1_2 + r2_2);
  alpha_[LEFT]  = sum * r0_2;
  alpha_[RIGHT] = sum * r1_2;
  alpha_[UPPER] = sum * r2_2;

  /*  AND 1ST NASAL PASSAGE COEFFICIENT  */
  radA2 = currentData_.velum * currentData_.velum;
  radB2 = voice.NoseRadiusVal(N2); radB2 *= radB2;
  nasalCoeff_[NC1] = (radA2 - radB2) / (radA2 + radB2);
}

/******************************************************************************
 *
 *  function:  setFricationTaps
 *
 *  purpose:   Sets the frication taps according to the current
 *             position and amplitude of frication.
 *
 ******************************************************************************/
void
VocalTract::setFricationTaps()
{
  int integerPart;
  float complement, remainder;
  float fricationAmplitude = amplitude(currentData_.fric_vol);

  /*  CALCULATE POSITION REMAINDER AND COMPLEMENT  */
  integerPart = (int) currentData_.fric_pos;
  complement = currentData_.fric_pos - (float) integerPart;
  remainder = 1.0 - complement;

  /*  SET THE FRICATION TAPS  */
  for (int i = FC1; i < TOTAL_FRIC_COEFFICIENTS; i++) {
    if (i == integerPart) {
      fricationTap_[i] = remainder * fricationAmplitude;
      if ((i + 1) < TOTAL_FRIC_COEFFICIENTS) {
        fricationTap_[++i] = complement * fricationAmplitude;
      }
    } else {
      fricationTap_[i] = 0.0;
    }
  }

#if 0
  /*  PRINT OUT  */
  printf("fricationTaps:  ");
  for (i = FC1; i < TOTAL_FRIC_COEFFICIENTS; i++)
    printf("%.6f  ", fricationTap[i]);
  printf("\n");
#endif
}

/******************************************************************************
 *
 *  function:  vocalTract
 *
 *  purpose:   Updates the pressure wave throughout the vocal tract,
 *             and returns the summed output of the oral and nasal
 *             cavities.  Also injects frication appropriately.
 *
 ******************************************************************************/
float
VocalTract::vocalTract(float input, float frication)
{
  int i, j, k;
  float delta, output, junctionPressure;

  /*  INCREMENT CURRENT AND PREVIOUS POINTERS  */
  if (++currentPtr_ > 1) {
    currentPtr_ = 0;
  }
  if (++prevPtr_ > 1) {
    prevPtr_ = 0;
  }

  /*  UPDATE OROPHARYNX  */
  /*  INPUT TO TOP OF TUBE  */
  oropharynx_[S1][TOP][currentPtr_] =
    (oropharynx_[S1][BOTTOM][prevPtr_] * dampingFactor_) + input;

  /*  CALCULATE THE SCATTERING JUNCTIONS FOR S1-S2  */
  delta = oropharynxCoeff_[C1] *
    (oropharynx_[S1][TOP][prevPtr_] - oropharynx_[S2][BOTTOM][prevPtr_]);
  oropharynx_[S2][TOP][currentPtr_] =
    (oropharynx_[S1][TOP][prevPtr_] + delta) * dampingFactor_;
  oropharynx_[S1][BOTTOM][currentPtr_] =
    (oropharynx_[S2][BOTTOM][prevPtr_] + delta) * dampingFactor_;

  /*  CALCULATE THE SCATTERING JUNCTIONS FOR S2-S3 AND S3-S4  */
  for (i = S2, j = C2, k = FC1; i < S4; i++, j++, k++) {
    delta = oropharynxCoeff_[j] *
      (oropharynx_[i][TOP][prevPtr_] - oropharynx_[i + 1][BOTTOM][prevPtr_]);
    oropharynx_[i + 1][TOP][currentPtr_] =
      ((oropharynx_[i][TOP][prevPtr_] + delta) * dampingFactor_) +
      (fricationTap_[k] * frication);
    oropharynx_[i][BOTTOM][currentPtr_] =
      (oropharynx_[i + 1][BOTTOM][prevPtr_] + delta) * dampingFactor_;
  }

  /*  UPDATE 3-WAY JUNCTION BETWEEN THE MIDDLE OF R4 AND NASAL CAVITY  */
  junctionPressure = (alpha_[LEFT] * oropharynx_[S4][TOP][prevPtr_])+
    (alpha_[RIGHT] * oropharynx_[S5][BOTTOM][prevPtr_]) +
    (alpha_[UPPER] * nasal_[VELUM][BOTTOM][prevPtr_]);
  oropharynx_[S4][BOTTOM][currentPtr_] =
    (junctionPressure - oropharynx_[S4][TOP][prevPtr_]) * dampingFactor_;
  oropharynx_[S5][TOP][currentPtr_] =
    ((junctionPressure - oropharynx_[S5][BOTTOM][prevPtr_]) * dampingFactor_)
    + (fricationTap_[FC3] * frication);
  nasal_[VELUM][TOP][currentPtr_] =
    (junctionPressure - nasal_[VELUM][BOTTOM][prevPtr_]) * dampingFactor_;

  /*  CALCULATE JUNCTION BETWEEN R4 AND R5 (S5-S6)  */
  delta = oropharynxCoeff_[C4] *
    (oropharynx_[S5][TOP][prevPtr_] - oropharynx_[S6][BOTTOM][prevPtr_]);
  oropharynx_[S6][TOP][currentPtr_] =
    ((oropharynx_[S5][TOP][prevPtr_] + delta) * dampingFactor_) +
    (fricationTap_[FC4] * frication);
  oropharynx_[S5][BOTTOM][currentPtr_] =
    (oropharynx_[S6][BOTTOM][prevPtr_] + delta) * dampingFactor_;

  /*  CALCULATE JUNCTION INSIDE R5 (S6-S7) (PURE DELAY WITH DAMPING)  */
  oropharynx_[S7][TOP][currentPtr_] =
    (oropharynx_[S6][TOP][prevPtr_] * dampingFactor_) +
    (fricationTap_[FC5] * frication);
  oropharynx_[S6][BOTTOM][currentPtr_] =
    oropharynx_[S7][BOTTOM][prevPtr_] * dampingFactor_;

  /*  CALCULATE LAST 3 INTERNAL JUNCTIONS (S7-S8, S8-S9, S9-S10)  */
  for (i = S7, j = C5, k = FC6; i < S10; i++, j++, k++) {
    delta = oropharynxCoeff_[j] *
      (oropharynx_[i][TOP][prevPtr_] - oropharynx_[i + 1][BOTTOM][prevPtr_]);
    oropharynx_[i + 1][TOP][currentPtr_] =
      ((oropharynx_[i][TOP][prevPtr_] + delta) * dampingFactor_) +
      (fricationTap_[k] * frication);
    oropharynx_[i][BOTTOM][currentPtr_] =
      (oropharynx_[i + 1][BOTTOM][prevPtr_] + delta) * dampingFactor_;
  }

  /*  REFLECTED SIGNAL AT MOUTH GOES THROUGH A LOWPASS FILTER  */
  oropharynx_[S10][BOTTOM][currentPtr_] =  dampingFactor_ *
    mouthReflectionFilter_->filter(oropharynxCoeff_[C8] *
                                   oropharynx_[S10][TOP][prevPtr_]);

  /*  OUTPUT FROM MOUTH GOES THROUGH A HIGHPASS FILTER  */
  output = mouthRadiationFilter_->filter((1.0 + oropharynxCoeff_[C8]) *
                                         oropharynx_[S10][TOP][prevPtr_]);

  /*  UPDATE NASAL CAVITY  */
  for (i = VELUM, j = NC1; i < N6; i++, j++) {
    delta = nasalCoeff_[j] *
      (nasal_[i][TOP][prevPtr_] - nasal_[i + 1][BOTTOM][prevPtr_]);
    nasal_[i+1][TOP][currentPtr_] =
      (nasal_[i][TOP][prevPtr_] + delta) * dampingFactor_;
    nasal_[i][BOTTOM][currentPtr_] =
      (nasal_[i + 1][BOTTOM][prevPtr_] + delta) * dampingFactor_;
  }

  /*  REFLECTED SIGNAL AT NOSE GOES THROUGH A LOWPASS FILTER  */
  nasal_[N6][BOTTOM][currentPtr_] = dampingFactor_ *
    nasalReflectionFilter_->filter(nasalCoeff_[NC6] * nasal_[N6][TOP][prevPtr_]);

  /*  OUTPUT FROM NOSE GOES THROUGH A HIGHPASS FILTER  */
  output += nasalRadiationFilter_->filter((1.0 + nasalCoeff_[NC6]) *
                                          nasal_[N6][TOP][prevPtr_]);
  /*  RETURN SUMMED OUTPUT FROM MOUTH AND NOSE  */
  return output;
}


float
VocalTract::calculateMonoScale()
{
  float scale = static_cast<float>((OUTPUT_SCALE / srConv_->maximumSampleValue()) * amplitude(volume));
  return scale;
}

void
VocalTract::calculateStereoScale(float& leftScale, float& rightScale)
{
  leftScale = static_cast<float>(-((balance / 2.0) - 0.5));
  rightScale = static_cast<float>(((balance / 2.0) + 0.5));
  float newMax = static_cast<float>(srConv_->maximumSampleValue() * (balance > 0.0 ? rightScale : leftScale));
  float scale = static_cast<float>((OUTPUT_SCALE / newMax) * amplitude(volume));
  leftScale  *= scale;
  rightScale *= scale;
}

/******************************************************************************
 *
 *  function:  amplitude
 *
 *  purpose:   Converts dB value to amplitude value.
 *
 ******************************************************************************/
float
VocalTract::amplitude(float decibelLevel)
{
  /*  CONVERT 0-60 RANGE TO -60-0 RANGE  */
  decibelLevel -= VOL_MAX;

  /*  IF -60 OR LESS, RETURN AMPLITUDE OF 0  */
  if (decibelLevel <= (-VOL_MAX)) {
    return 0.0;
  }

  /*  IF 0 OR GREATER, RETURN AMPLITUDE OF 1  */
  if (decibelLevel >= 0.0) {
    return 1.0;
  }

  /*  ELSE RETURN INVERSE LOG VALUE  */
  return pow(10.0, decibelLevel / 20.0);
}

/******************************************************************************
 *
 *  function:  frequency
 *
 *  purpose:   Converts a given pitch (0 = middle C) to the
 *             corresponding frequency.
 *
 ******************************************************************************/
float
VocalTract::frequency(float pitch)
{
  return PITCH_BASE * pow(2.0, (pitch + PITCH_OFFSET) / 12.0);
}

