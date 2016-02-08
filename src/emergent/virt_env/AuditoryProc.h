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

#ifndef AuditoryProc_h
#define AuditoryProc_h 1

// parent includes:
#include <ImgProcThreadBase>

// member includes:
#include <DataTableRef>
#include <float_Matrix>
#include <int_Matrix>
#include <complex_float_Matrix>

// declare all other types mentioned but not required to include:
class taSound; //


taTypeDef_Of(AudInputSpec);

class E_API AudInputSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound definition of sound input parameters for auditory processing
INHERITED(taOBase)
public:
  float         win_msec;       // #DEF_25 input window -- number of milliseconds worth of sound to filter at a time
  float         step_msec;      // #DEF_12.5 input step -- number of milliseconds worth of sound that the input is stepped along to obtain the next window sample 
  float         trial_msec;     // #DEF_100 length of a full trial's worth of input -- total number of milliseconds to accumulate into a complete trial of activations to present to a network -- must be a multiple of step_msec -- input will be trial_msec / step_msec = trial_steps wide in the X axis, and number of filters in the Y axis
  int           sample_rate;    // rate of sampling in our sound input (e.g., 16000 = 16Khz) -- can initialize this from a taSound object using InitFromSound method
  int           channels;       // total number of channels to process
  int           channel;        // #CONDSHOW_ON_channels:1 specific channel to process, if input has multiple channels, and we only porcess one of them
  int           win_samples;    // #READ_ONLY #SHOW total number of samples to process (win_msec * .001 * sample_rate)
  int           step_samples;   // #READ_ONLY #SHOW total number of samples to step input by (step_msec * .001 * sample_rate)
  int           trial_samples;  // #READ_ONLY #SHOW total number of samples in a trial  (trail_msec * .001 * sample_rate)
  int           trial_steps;    // #READ_ONLY #SHOW total number of stepss in a trial  (trail_msec / step_msec)

  void          ComputeSamples(); // compute samples from msec * sample_rate -- includes some mods to ensure that generally good sizes are used

  void          InitFromSound(taSound* sound, int n_chans = 1, int chan = 0);
  // get sample_rate and channels info from given sound object -- n_chans arg is how many channels we want to process -- overrides number of channels present in sound obj if >= 1 (i.e., use -1 to just use number of channels present in sound object) -- if sound obj has multiple channels and n_chans = 1, then chan specifies which channel to read from

  TA_SIMPLE_BASEFUNS(AudInputSpec);
protected:
  void 	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy() { };
};


taTypeDef_Of(MelFreqSpec);

class E_API MelFreqSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound mel frequency sampling parameters
INHERITED(taOBase)
public:
  bool          on;            // perform mel-frequency filtering of the fft input
  float         lo_hz;         // #DEF_300 #CONDSHOW_ON_on low frequency end of mel frequency spectrum
  float         hi_hz;         // #DEF_8000 #CONDSHOW_ON_on high frequency end of mel frequency spectrum -- must be <= sample_rate / 2 (i.e., less than the Nyquist frequency)
  int           n_filters;     // #DEF_26 #CONDSHOW_ON_on number of Mel frequency filters to compute
  float         lo_mel;        // #READ_ONLY #SHOW #CONDSHOW_ON_on low end of mel scale in mel units
  float         hi_mel;        // #READ_ONLY #SHOW #CONDSHOW_ON_on high end of mel scale in mel units

  inline float         FreqToMel(const float freq)
  { return 1127.01f * logf(1.0f + freq/700.0f); }
  // convert frequency to mel scale
  
  inline float         MelToFreq(const float mel)
  { return 700.0f * (expf(mel / 1127.01f) - 1.0f); }
  // convert mel scale to frequency

  inline int           FreqToBin(const float freq, const float n_fft,
                                 const float sample_rate)
  { return (int)floor(((n_fft+1) * freq) / sample_rate); }
  // convert frequency into FFT bin number, using parameters of number of FFT bins and sample rate

  TA_SIMPLE_BASEFUNS(MelFreqSpec);
protected:
  void 	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy() { };
};


eTypeDef_Of(AuditoryProc);

class E_API AuditoryProc : public ImgProcThreadBase {
  // Auditory processing system -- parameters for performing various auditory processing steps on sound waveforms, resulting in activation patterns suitable as input to a network
INHERITED(ImgProcThreadBase)
public:
  enum DataSave {		// #BITS how to save data to the data table (computation happens on internal table)
    NO_SAVE 	= 0,	 	// #NO_BIT don't save data for this component
    SAVE_DATA	= 0x0001, 	// save data to the data table, by default with all features in subgroups within one matrix (unless SEP_MATRIX is checked)
    SEP_MATRIX	= 0x0002,	// each feature is saved in a separate 2d map, instead of a common subgroup within a single matrix
    ONLY_GUI	= 0x0004,	// only save when the gui is active -- for data that is for debugging and visualization purposes only, not presented to a network etc
    SAVE_DEBUG	= 0x0008,	// save extra debugging-level internal computation data relevant to this processing step
  };
  enum SaveMode {		// how to add new data to the data table
    NONE_SAVE,			// don't save anything at all -- overrides any more specific save guys and prevents any addition or modification to the data table
    FIRST_ROW,			// always overwrite the first row -- does EnforceRows(1) if rows = 0
    ADD_ROW,			// always add a new row and write to that, preserving a history of inputs over time -- should be reset at some interval!
  };


  DataTableRef	data_table;	// data table for saving filter results for viewing and applying to networks etc
  SaveMode	save_mode;	// how to add new data to the data table
  DataSave	input_save;	// how to save the input sound for each filtering step
  AudInputSpec  input;          // specifications of the raw auditory input 
  MelFreqSpec   mel;            // specifications of the mel frequency sampling of the DFT (FFT) of the input sound
  bool          mfcc_on;        // compute mel-frequency cepstrum coefficients


  ///////////////////////////////////////////////////
  //    Filters

  int                   dft_size;    // #READ_ONLY #NO_SAVE full size of fft output -- should be input.win_samples
  int                   dft_use;     // #READ_ONLY #NO_SAVE number of dft outputs to actually use -- should be dft_size / 2 + 1
  int                   mel_n_filters_eff; // #READ_ONLY #NO_SAVE effective number of mel filters: mel.n_filters + 2
  int                   n_mfcc;            // #READ_ONLY #NO_SAVE mel.n_filters / 2 -- number of coefficients
  float_Matrix          mel_pts_mel; // #READ_ONLY #NO_SAVE [mel_n_filters_eff] scale points in mel units (mels)
  float_Matrix          mel_pts_hz;  // #READ_ONLY #NO_SAVE [mel_n_filters_eff] mel scale points in hz units 
  int_Matrix            mel_pts_bin; // #READ_ONLY #NO_SAVE [mel_n_filters_eff] mel scale points in fft bins
  int                   mel_filt_max_bins; // #READ_ONLY #NO_SAVE maximum number of bins for mel filter -- number of bins in highest filter
  float_Matrix          mel_filters; // #READ_ONLY #NO_SAVE [mel_filt_max_bins][mel.n_filters] the actual filters for actual number of mel filters
  
  
  //////////////////////////////////////////////////////////////
  //	Outputs

  float_Matrix          sound_full;  // #READ_ONLY #NO_SAVE the full sound input obtained from the sound input
  int                   input_pos;   // #READ_ONLY #NO_SAVE current position in the sound_full input -- in terms of sample number
  float_Matrix          window_in;  // #READ_ONLY #NO_SAVE [input.win_samples] the raw sound input, one channel at a time
  complex_float_Matrix	dft_out;   // #READ_ONLY #NO_SAVE [2, dft_size] discrete fourier transform (fft) output complex representation
  float_Matrix          dft_power_out; // #READ_ONLY #NO_SAVE [dft_use] power of the dft, only for range of frequencies of interest
  float_Matrix          mel_filter_out; // #READ_ONLY #NO_SAVE [mel.n_filters] scale transformation of dft_power, using triangular filteres
  float_Matrix          log_mel_filter_out; // #READ_ONLY #NO_SAVE log of mel_filter_out values
  float_Matrix          mfcc_dct_out; // #READ_ONLY #NO_SAVE discrete cosine transform of the log_mel_filter_out values, producing the final mel-frequency cepstral coefficients 
  

  virtual bool 	Init();
  // #BUTTON initialize everything to be ready to start filtering -- calls InitFilters, InitOutMatrix, InitDataTable

  virtual void  InitFromSound(taSound* sound, int n_chans = 1, int chan = 0);
  // get sample_rate and channels info from given sound object -- n_chans arg is how many channels we want to process -- overrides number of channels present in sound obj if >= 1 (i.e., use -1 to just use number of channels present in sound object) -- if sound obj has multiple channels and n_chans = 1, then chan specifies which channel to read from

  virtual bool	LoadSound(taSound* sound);
  // #CAT_Auditory load a new sound into the system for subsequent processing -- grabs the sound data into sound_full

  virtual int   InputStepsLeft();
  // #CAT_Auditory number of steps left to process in the current input sound

  virtual bool	ProcessTrial();
  // #CAT_Auditory process a full trial worth of sound -- iterates over steps to fill a trial's worth of sound data

  virtual bool	ProcessStep(int step);
  // #CAT_Auditory process a step worth of sound input from current input_pos, and increment input_pos by input.step_samples

  virtual bool	SoundToWindow(int in_pos, int chan);
  // #CAT_Auditory get sound from sound_full at given position and channel, into window_in -- pads with zeros for any amount not available in the sound_full input

  virtual bool  FilterWindow();
  // #CAT_Auditory filter the current window_in input data according to current settings -- called by ProcessStep, but can be called separately 
  
  virtual bool  OutputToTable(int chan, int step);
  // #CAT_Auditory record the current filter output into data table for given channel and step
  
protected:
  void	UpdateAfterEdit_impl() override;

  virtual void  UpdateConfig();
  // update all configuration params, called by UAE
  
  virtual bool  InitFilters();
  // initialize the filters -- overload for derived types and call parent

  virtual bool  InitFilters_Mel();
  // init filters for Mel stack

  virtual bool  InitOutMatrix();
  // initialize data output matrcies to fit output of filters
  virtual bool  InitDataTable();
  // initialize data table to fit data saving as configured

  virtual bool  MelOutputToTable(DataTable* dtab, bool fmt_only = false);
  // mel filter bank to output table

  virtual void  DftInput();
  // apply dft (fft) to input
  virtual void  PowerOfDft();
  // compute power of dft output
  virtual void  MelFilterDft();
  // apply mel filters to power of dft
  
public:  
  TA_SIMPLE_BASEFUNS(AuditoryProc);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // AuditoryProc_h
