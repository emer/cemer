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
// and then modified for emergent by Randall C. O'Reilly, Apr, 2016

#ifndef VocalTract_h
#define VocalTract_h

// parent includes:
#include <taSound>
#include "network_def.h"

// member includes:
#include <DataTable>

#ifndef __MAKETA__
#include "BandpassFilter.h"
#include "NoiseFilter.h"
#include "NoiseSource.h"
#include "RadiationFilter.h"
#include "ReflectionFilter.h"
#include "SampleRateConverter.h"
#include "Throat.h"
#include "WavetableGlottalSource.h"
#endif

// declare all other types mentioned but not required to include:
class float_Matrix; //


eTypeDef_Of(VocalTractConfig);

class E_API VocalTractConfig : public taOBase {
  // #STEM_BASE ##CAT_Audio #INLINE #INLINE_DUMP global configuration information for Vocal Tract
  INHERITED(taOBase)
public:
  enum WaveForm {
    PULSE,
    SINE
  };
  
  float temp;
  // #DEF_32 tube temperature (25 - 40 C)
  float loss;
  // #DEF_0.8 junction loss factor in (0 - 5 %) 
  float mouth_coef;
  // #DEF_5000 mouth aperture coefficient
  float nose_coef;
  // #DEF_5000 nose aperture coefficient
  float throat_cutoff;
  // #DEF_1500 throat lp cutoff (50 - nyquist Hz)
  float throat_vol;
  // #DEF_6 throat volume (0 - 48 dB)
  float vtl_off;
  // #DEF_0 tube length offset -- not sure what this is?
  WaveForm waveform;
  // #DEF_PULSE not sure..?
  bool   noise_mod;
  // #DEF_true pulse modulation the noise
  float mix_off;
  // #DEF_48 noise crossmix offset (30 - 60 dB)

  virtual void Defaults();
  // #BUTTON #CAT_VocalTract set default params
  
  TA_SIMPLE_BASEFUNS(VocalTractConfig);
private:
  void Initialize();
  void Destroy() { };
};

eTypeDef_Of(VoiceParams);

class E_API VoiceParams : public taOBase {
  // #STEM_BASE ##CAT_Audio #INLINE #INLINE_DUMP global configuration information for Vocal Tract
  INHERITED(taOBase)
public:
  enum  VoiceDefs {             // default voice configurations
    MALE,                       // adult male
    FEMALE,                     // adult female
    CHILD_LG,                   // large child
    CHILD_SM,                   // small child
    BABY,                       // baby
  };

  float tract_length;
  // #DEF_7.5:17.5 length of the vocal tract in cm -- determines the overall pitch of the sound -- default male voice is 17.5, baby is 7.5
  float glot_pulse_fall_min;
  // #DEF_24;32 percent glottal pulse fall time minimum -- 24 for male, 32 for female
  float glot_pulse_fall_max;
  // #DEF_24;32 percent glottal pulse fall time maximum -- 24 for male, 32 for female
  float glot_pitch_ref;
  // #DEF_-12:7.5 glottal pitch reference, in semitones, ranges from -12 (male) to 7.5 (baby)
  float breathiness;
  // #DEF_0.5:1.5 glottal source breathiness (white noise percentage?) -- 0.5 for male, 1.5 for child or female
  float glot_pulse_rise;
  // #DEF_40 #EXPERT percent glottal pulse rise time -- always 40
  float aperture_radius;
  // #DEF_3.05 #EXPERT aperture scl. radius (3.05 - 12 cm)  */
  float nose_radius_1;
  // #DEF_1.35 #EXPERT fixed nose radius (0 - 3 cm)
  float nose_radius_2;
  // #DEF_1.96 #EXPERT fixed nose radius (0 - 3 cm)
  float nose_radius_3;
  // #DEF_1.91 #EXPERT fixed nose radius (0 - 3 cm)
  float nose_radius_4;
  // #DEF_1.3 #EXPERT fixed nose radius (0 - 3 cm)
  float nose_radius_5;
  // #DEF_0.73 #EXPERT fixed nose radius (0 - 3 cm)
  float radius_1;
  // #DEF_0.8 #EXPERT fixed control parameter effectively -- always .8
  float nose_radius_coef;
  // #DEF_1 #EXPERT global nose radius coefficient
  float radius_coef;
  // #DEF_1 #EXPERT global vocal tract radius coefficient -- also all the specific radius coefficients are automatically 1 as they don't seem to vary in practice..

  float NoseRadiusVal(int idx)
  { return (&nose_radius_1)[idx]; }
  // get nose radius value, using *zero-based* index value (= radius_1, etc)

  virtual void GlobalDefaultParams();
  // #CAT_Voice set all params that have a global default to that default value
  virtual void Male();
  // #CAT_Voice set default male params
  virtual void Female();
  // #CAT_Voice set default female params
  virtual void ChildLg();
  // #CAT_Voice set default large child params
  virtual void ChildSm();
  // #CAT_Voice set default small child params
  virtual void Baby();
  // #CAT_Voice set default baby params
  virtual void SetDefault(VoiceDefs voice_defs);
  // #BUTTON #CAT_Voice set default voice params per given selection
  
  TA_SIMPLE_BASEFUNS(VoiceParams);
private:
  void Initialize();
  void Destroy()  { };
};

eTypeDef_Of(VocalTractCtrl);

class E_API VocalTractCtrl : public taOBase {
  // #STEM_BASE ##CAT_Audio #INLINE #INLINE_DUMP control parameters for driving a given sound from the vocal tract -- there are different such parameters for each "phoneme" etc.
  INHERITED(taOBase)
public:
  enum ParamIndex {         // parameter indexes for reading from matrix data
    GLOT_PITCH = 0,
    GLOT_VOL   = 1,
    ASP_VOL    = 2,
    FRIC_VOL   = 3,
    FRIC_POS   = 4,
    FRIC_CF    = 5,
    FRIC_BW    = 6,
    R2         = 7, 
    R3         = 8, 
    R4         = 9, 
    R5         = 10,
    R6         = 11,
    R7         = 12,
    R8         = 13,
    VELUM      = 14,
    N_PARAMS   = 15,
  };

  float glot_pitch;
  // #MIN_-10 #MAX_0 glottal pitch value -- ranges from -10 for phoneme k to 0 for most, with some being -2 or -1 -- called microInt in gnuspeech data files
  float glot_vol;
  // #MIN_0 #MAX_60 glottal volume (DB?) typically 60 when present and 0 when not, and sometimes 54, 43.5, 42, 
  float asp_vol;
  // #MIN_0 #MAX_10 aspiration volume -- typically 0 when not present and 10 when present
  float fric_vol;
  // #MIN_0 #MAX_24 fricative volume -- typically 0 or .25 .4, .5, .8 but 24 for ph
  float fric_pos;
  // #MIN_1 #MAX_7 ficative position -- varies continuously between 1-7
  float fric_cf;
  // #MIN_864 #MAX_5500 fricative center? frequency ranges between 864 to 5500 with values around 1770, 2000, 2500, 4500 being common
  float fric_bw;
  // #MIN_500 #MAX_4500 fricative bw?  seems like a frequency -- common intermediate values are 600, 900, 2000, 2600
  float radius_2;
  // #MIN_0.1 #MAX_3 radius of pharynx vocal tract segment as determined by tongue etc -- typically around 1, ranging .5 - 1.7
  float radius_3;
  // #MIN_0.1 #MAX_3 radius of pharynx vocal tract segment as determined by tongue etc -- typically around 1, ranging .6 - 1.91
  float radius_4;
  // #MIN_0.1 #MAX_3 radius of velum vocal tract segment as determined by tongue and jaw -- typically around 1, ranging .6 - 1.99
  float radius_5;
  // #MIN_0.1 #MAX_3 radius of behind alveolar ridge vocal tract segment as determined by tongue and jaw -- typically around 1, ranging .1 - 2.2
  float radius_6;
  // #MIN_0.001 #MAX_3 radius of alveolar ridge vocal tract segment as determined by tongue and jaw -- typically around 1, ranging 0 - 1.93
  float radius_7;
  // #MIN_0.1 #MAX_3 radius of teeth to alveolar ridge vocal tract segment as determined by tongue and jaw -- typically around 1, ranging .26 - 1.97
  float radius_8;
  // #MIN_0.01 #MAX_3 radius of lips to teeth vocal tract segment as determined by tongue and lips -- typically around 1, ranging .01 - 2.06
  float velum;
  // #MIN_0.1 #MAX_1.5 velum opening -- 1.5 when fully open, .1 when closed, and .25, .5 intermediates used

  float RadiusVal(int idx)
  { if(idx <= 0) return 0.8f; return (&radius_2)[idx-1]; }
  // get radius value using *zero-based* index value (0 = radius_1 which is constant..)

  void  ComputeDeltas(const VocalTractCtrl& cur, const VocalTractCtrl& prv,
                      float ctrl_freq);
  // compute values in this set of params as deltas from (cur - prv) * ctrl_freq
  void  UpdateFromDeltas(const VocalTractCtrl& del);
  // update values in this set of params from deltas

  void  SetFromFloat(float val, ParamIndex param, bool normalized);
  // set given parameter to value
  
  void  SetFromFloats(const float* vals, bool normalized = true);
  // set values from array of 15 float values -- see ParamIndex for indexes of each value -- for normalized values then 0..1 floats are expanded according to the min/max values given for each parameter, otherwise the value is used directly
  void  SetFromMatrix(const float_Matrix& matrix, bool normalized = true);
  // set values from a matrix of 15 float values -- see ParamIndex for indexes of each value -- for normalized values then 0..1 floats are expanded according to the min/max values
  void  SetFromDataTable(const DataTable& table, const Variant& col, int row,
                         bool normalized = true);
  // set values from a matrix cell -- see ParamIndex for indexes of each value -- for normalized values then 0..1 floats are expanded according to the min/max values
  
  TA_SIMPLE_BASEFUNS(VocalTractCtrl);
private:
  void Initialize();
  void Destroy()  { };
};

eTypeDef_Of(VocalTract);

class E_API VocalTract : public taSound {
  // vocal tract synthesis system based on GnuSpeech (originally developed by David R. Hill, Leonard Manzara, Craic Schock, ported to C++ by Marcelo Y. Matuda) -- this is a taSound object and contains the rendered sound
  INHERITED(taSound)
public:
  enum OroPharynxRegions { // different regions of the vocal tract
    R1 = 0, /*  S1  */
    R2 = 1, /*  S2  */
    R3 = 2, /*  S3  */
    R4 = 3, /*  S4 & S5  */
    R5 = 4, /*  S6 & S7  */
    R6 = 5, /*  S8  */
    R7 = 6, /*  S9  */
    R8 = 7, /*  S10  */
    TOTAL_REGIONS = 8
  };
  
  enum NasalTractSections { // different sections of the nasal tract
    N1 = 0,
    N2 = 1,
    N3 = 2,
    N4 = 3,
    N5 = 4,
    N6 = 5,
    TOTAL_NASAL_SECTIONS = 6,
    VELUM = N1, // #IGNORE
  };

  enum OroPharynxCoeff { //  oropharynx scattering junction coefficients (between each region)
    C1 = R1, /*  R1-R2 (S1-S2)  */
    C2 = R2, /*  R2-R3 (S2-S3)  */
    C3 = R3, /*  R3-R4 (S3-S4)  */
    C4 = R4, /*  R4-R5 (S5-S6)  */
    C5 = R5, /*  R5-R6 (S7-S8)  */
    C6 = R6, /*  R6-R7 (S8-S9)  */
    C7 = R7, /*  R7-R8 (S9-S10)  */
    C8 = R8, /*  R8-AIR (S10-AIR)  */
    TOTAL_COEFFICIENTS = TOTAL_REGIONS
  };

  enum OroPharynxSections { // sections of the oropharynx
    S1  = 0, /*  R1  */
    S2  = 1, /*  R2  */
    S3  = 2, /*  R3  */
    S4  = 3, /*  R4  */
    S5  = 4, /*  R4  */
    S6  = 5, /*  R5  */
    S7  = 6, /*  R5  */
    S8  = 7, /*  R6  */
    S9  = 8, /*  R7  */
    S10 = 9, /*  R8  */
    TOTAL_SECTIONS = 10
  };

  enum NasalTractCoeff { // 
    NC1 = N1, /*  N1-N2  */
    NC2 = N2, /*  N2-N3  */
    NC3 = N3, /*  N3-N4  */
    NC4 = N4, /*  N4-N5  */
    NC5 = N5, /*  N5-N6  */
    NC6 = N6, /*  N6-AIR  */
    TOTAL_NASAL_COEFFICIENTS = TOTAL_NASAL_SECTIONS
  };

  enum ThreeWayJunct { // three-way junction alpha coefficients 
    LEFT  = 0,
    RIGHT = 1,
    UPPER = 2,
    TOTAL_ALPHA_COEFFICIENTS = 3
  };

  enum FricationInjCoeff { // frication injection coefficients
    FC1 = 0, /*  S3  */
    FC2 = 1, /*  S4  */
    FC3 = 2, /*  S5  */
    FC4 = 3, /*  S6  */
    FC5 = 4, /*  S7  */
    FC6 = 5, /*  S8  */
    FC7 = 6, /*  S9  */
    FC8 = 7, /*  S10  */
    TOTAL_FRIC_COEFFICIENTS = 8
  };

  float                 volume;
  // #DEF_60 master volume (0 - 60 dB)
  float                 balance;
  // #DEF_0 stereo balance (-1 to +1) if multi-channel sound being used
  float                 synth_dur_msec;
  // #DEF_25 number of milliseconds per individual control input signal -- the main Synthesize command will generate this amount of sound output based on cur_ctrl input parameters -- new input control parameters will increment with del_ctrl delta values over time
  VocalTractConfig      config;
  // configuration params for the vocal tract overall -- generally don't change from defaults
  VoiceParams           voice;
  // voice parameters -- varying these gives different voice sounds from male to female to children or babies
  VocalTractCtrl        cur_ctrl;
  // current control parameters -- set these to drive the sound as desired, for one synthesis period 
  VocalTractCtrl        prv_ctrl;
  // #READ_ONLY #SHOW previous control parameters -- automatically updated from previous cur_ctrl every time Synthesize is called
  VocalTractCtrl        del_ctrl;
  // #READ_ONLY #SHOW delta between current and previous control parameters -- automatically updated every time Synthesize is called
  DataTable             phone_table;
  // #SHOW_TREE #NO_SAVE #HIDDEN table of standard phonemes containing posture configurations for each phoneme
  DataTable             dict_table;
  // #SHOW_TREE #NO_SAVE #HIDDEN dictionary with pronunciation rules for words in terms of phones described in phone_table -- not loaded by default as it is large

  virtual void InitSynth();
  // #BUTTON #CAT_VocalTract initialize the synthesizer -- call this after changing any of the basic sound / voice config parameters -- also call InitBuffers first if you want a different sample rate than the default 16000
  virtual void  CtrlFromFloats(const float* vals, bool normalized = true);
  // #CAT_VocalTract set current control values from array of 15 float values -- see ParamIndex for indexes of each value -- for normalized values then 0..1 floats are expanded according to the min/max values given for each parameter, otherwise the value is used directly
  virtual void  CtrlFromMatrix(const float_Matrix& matrix, bool normalized = true);
  // #CAT_VocalTract set current control values from a matrix of 15 float values -- see ParamIndex for indexes of each value -- for normalized values then 0..1 floats are expanded according to the min/max values
  virtual void  CtrlFromDataTable(const DataTable& table, const Variant& col, int row,
                         bool normalized = true);
  // #CAT_VocalTract set current control values from a matrix cell -- see ParamIndex for indexes of each value -- for normalized values then 0..1 floats are expanded according to the min/max values
  virtual void  SynthInitBuffer();
  // #CAT_VocalTract init the sound buffer to default params -- requires 44100 sample rate to work properly, so this is set, along with mono sound by default
  virtual void  SynthReset(bool init_buffer = true);
  // #CAT_VocalTract reset any existing speech that has been synthesized, so next synthesis will start at beginning -- init buffer will init the sound buffer to get rid of any  existing sound there, and start it all over
  virtual void  Synthesize(bool reset_first = false);
  // #BUTTON #CAT_VocalTract synthesize sound using the current parameters for given duration of time, optionally starting over as a new sound if reset_first is true, or otherwise adding at the end of any existing sound present in the sound buffer -- the control parameters continue to evolve over time from previous values if you don't do reset_first
  virtual void  SetVoice();
  // #BUTTON #CAT_VocalTract set the voice to one of the default voices

  virtual bool  LoadEnglishPhones();
  // load default English phones into phone_table
  virtual bool  LoadEnglishDict();
  // load default English pronunciation dictionary into dict_table
  virtual bool  SynthPhone(const String& phon, bool stress = false,
                           bool double_stress = false, bool syllable = false,
                           bool reset_first = false);
  // #BUTTON synthesize a single phoneme with given extra factors -- standard English phones will be loaded if phone_table is empty
  virtual bool  SynthPhones(const String& phones, bool reset_first = true, bool play = false);
  // #BUTTON synthesize a sequence of phonemes, syllables are separated by . and phones are separated by _, stress marked with a preceding ' -- returns false if any phonemes not found -- if play is set then sound will be played to default output -- standard English phones will be loaded if phone_table is empty
  virtual bool  SynthWord(const String& word, bool reset_first = true, bool play = false);
  // #BUTTON synthesize a word by looking it up in the dictionary (after downcasing) and calling SynthPhones on the resulting phones -- returns false if word not found -- if play is set then sound will be played to default output -- standard English dictionary and phones will be loaded if tables are empty
  
#ifndef __MAKETA__
  std::vector<float>& OutputData() { return outputData_; }
  // #IGNORE get the output data as a std vector
#endif
  void ResetOutputData() {
    outputData_.clear();
  }
  // reset the output data
  float MaximumOutputSampleValue() const { return srConv_->maximumSampleValue(); }
  // maximum output sample rate supported

  TA_SIMPLE_BASEFUNS(VocalTract);
protected:

  void reset();
  void Synthesize_impl();
  void initializeSynthesizer();
  void calculateTubeCoefficients();
  void initializeNasalCavity();
  void setFricationTaps();
  float vocalTract(float input, float frication);
  float calculateMonoScale();
  void calculateStereoScale(float& leftScale, float& rightScale);

  static float amplitude(float decibelLevel);
  static float frequency(float pitch);
  static float speedOfSound(float temperature);

  /*  DERIVED VALUES  */
  float  controlRate_;                 /*  1.0-1000.0 input tables/second (Hz)  */
  int    controlPeriod_;
  int    sampleRate_;
  float actualTubeLength_;            /*  actual length in cm  */

  VocalTractCtrl currentData_;  // current control data

#ifndef __MAKETA__
  /*  MEMORY FOR TUBE AND TUBE COEFFICIENTS  */
  float oropharynx_[TOTAL_SECTIONS][2][2];
  float oropharynxCoeff_[TOTAL_COEFFICIENTS];

  float nasal_[TOTAL_NASAL_SECTIONS][2][2];
  float nasalCoeff_[TOTAL_NASAL_COEFFICIENTS];

  float alpha_[TOTAL_ALPHA_COEFFICIENTS];
  int currentPtr_;
  int prevPtr_;

  /*  MEMORY FOR FRICATION TAPS  */
  float fricationTap_[TOTAL_FRIC_COEFFICIENTS];
  
  float dampingFactor_;               /*  calculated damping factor  */
  float crossmixFactor_;              /*  calculated crossmix factor  */
  float breathinessFactor_;

  float prevGlotAmplitude_;

  std::vector<float> outputData_;
  std::unique_ptr<SampleRateConverter> srConv_;
  std::unique_ptr<RadiationFilter> mouthRadiationFilter_;
  std::unique_ptr<ReflectionFilter> mouthReflectionFilter_;
  std::unique_ptr<RadiationFilter> nasalRadiationFilter_;
  std::unique_ptr<ReflectionFilter> nasalReflectionFilter_;
  std::unique_ptr<Throat> throat_;
  std::unique_ptr<WavetableGlottalSource> glottalSource_;
  std::unique_ptr<BandpassFilter> bandpassFilter_;
  std::unique_ptr<NoiseFilter> noiseFilter_;
  std::unique_ptr<NoiseSource> noiseSource_;
#endif

private:
  void Initialize();
  void Destroy() { };
};

#endif /* VocalTract_h */
