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
#include <V1KwtaSpec>
#include <DataTableRef>
#include <float_Matrix>
#include <int_Matrix>
#include <complex_float_Matrix>
#include <taMath_float>
#include <XYNGeom>

// declare all other types mentioned but not required to include:
class taSound; //


taTypeDef_Of(AudInputSpec);

class E_API AudInputSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound definition of sound input parameters for auditory processing
INHERITED(taOBase)
public:
  float         win_msec;       // #DEF_25 input window -- number of milliseconds worth of sound to filter at a time
  float         step_msec;      // #DEF_5;10;12.5 input step -- number of milliseconds worth of sound that the input is stepped along to obtain the next window sample 
  float         trial_msec;     // #DEF_100 length of a full trial's worth of input -- total number of milliseconds to accumulate into a complete trial of activations to present to a network -- must be a multiple of step_msec -- input will be trial_msec / step_msec = trial_steps wide in the X axis, and number of filters in the Y axis
  int           border_steps;   // number of steps before and after the trial window to preserve -- this is important when applying temporal filters that have greater temporal extent
  int           sample_rate;    // rate of sampling in our sound input (e.g., 16000 = 16Khz) -- can initialize this from a taSound object using InitFromSound method
  int           channels;       // total number of channels to process
  int           channel;        // #CONDSHOW_ON_channels:1 specific channel to process, if input has multiple channels, and we only process one of them (-1 = process all)
  int           win_samples;    // #READ_ONLY #SHOW total number of samples to process (win_msec * .001 * sample_rate)
  int           step_samples;   // #READ_ONLY #SHOW total number of samples to step input by (step_msec * .001 * sample_rate)
  int           trial_samples;  // #READ_ONLY #SHOW total number of samples in a trial  (trail_msec * .001 * sample_rate)
  int           trial_steps;    // #READ_ONLY #SHOW total number of steps in a trial  (trail_msec / step_msec)
  int           total_steps;    // #READ_ONLY #SHOW 2*border_steps + trial_steps -- total in full window

  inline int    MSecToSamples(float msec)
  { return (int)taMath_float::round(msec * 0.001f * (float)sample_rate); }
  // convert milliseconds to samples, in terms of sample_rate
  inline float  SamplesToMSec(int samples)
  { return 1000.0f * ((float)samples / (float)sample_rate); }
  // convert samples to milliseconds, in terms of sample_rate
  
  void          ComputeSamples(); // compute samples from msec * sample_rate -- includes some mods to ensure that generally good sizes are used

  void          InitFromSound(taSound* sound, int n_chans = 1, int chan = 0);
  // get sample_rate and channels info from given sound object -- n_chans arg is how many channels we want to process -- overrides number of channels present in sound obj if >= 1 (i.e., use -1 to just use number of channels present in sound object) -- if sound obj has multiple channels and n_chans = 1, then chan specifies which channel to read from

  TA_SIMPLE_BASEFUNS(AudInputSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};


taTypeDef_Of(AudDftSpec);

class E_API AudDftSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound DFT (discrete fourier transform) specifications
INHERITED(taOBase)
public:
  float         prv_smooth;    // #DEF_0.1 how much of the previous step's power value to include in this one -- smooths out the power spectrum which can be artificially bumpy due to discrete window samples
  bool          log_pow;       // #DEF_true compute the log of the power and save that to a separate table -- generaly more useful for visualization of power than raw power values
  float         log_off;       // #CONDSHOW_ON_log_pow #DEF_0 add this amount when taking the log of the dft power -- e.g., 1.0 makes everything positive -- affects the relative contrast of the outputs
  float         log_min;       // #CONDSHOW_ON_log_pow #DEF_-100 minimum value a log can produce -- puts a lower limit on log output

  float         cur_smooth;     // #READ_ONLY #EXPERT 1 - prv_smooth -- how much of current power to include
  
  TA_SIMPLE_BASEFUNS(AudDftSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};


taTypeDef_Of(MelFBankSpec);

class E_API MelFBankSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound mel frequency feature bank sampling parameters
INHERITED(taOBase)
public:
  bool          on;            // perform mel-frequency filtering of the fft input
  float         lo_hz;         // #DEF_120;300 #CONDSHOW_ON_on low frequency end of mel frequency spectrum
  float         hi_hz;         // #DEF_10000;8000 #CONDSHOW_ON_on high frequency end of mel frequency spectrum -- must be <= sample_rate / 2 (i.e., less than the Nyquist frequency)
  int           n_filters;     // #DEF_32;26 #CONDSHOW_ON_on number of Mel frequency filters to compute
  float         log_off;       // #CONDSHOW_ON_#DEF_0 on add this amount when taking the log of the Mel filter sums to produce the filter-bank output -- e.g., 1.0 makes everything positive -- affects the relative contrast of the outputs
  float         log_min;       // #CONDSHOW_ON_on #DEF_-10 minimum value a log can produce -- puts a lower limit on log output
  float         lo_mel;        // #READ_ONLY #SHOW #CONDSHOW_ON_on low end of mel scale in mel units
  float         hi_mel;        // #READ_ONLY #SHOW #CONDSHOW_ON_on high end of mel scale in mel units

  inline float         FreqToMel(const float freq)
  { return 1127.0f * logf(1.0f + freq/700.0f); }
  // convert frequency to mel scale
  
  inline float         MelToFreq(const float mel)
  { return 700.0f * (expf(mel / 1127.0f) - 1.0f); }
  // convert mel scale to frequency

  inline int           FreqToBin(const float freq, const float n_fft,
                                 const float sample_rate)
  { return (int)floor(((n_fft+1) * freq) / sample_rate); }
  // convert frequency into FFT bin number, using parameters of number of FFT bins and sample rate

  TA_SIMPLE_BASEFUNS(MelFBankSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};


taTypeDef_Of(AudRenormSpec);

class E_API AudRenormSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound auditory renormalization parameters
INHERITED(taOBase)
public:
  bool          on;            // perform renormalization of this level of the auditory signal
  float         ren_min;       // #CONDSHOW_ON_on minimum value to use for renormalization -- you must experiment with range of inputs to determine appropriate values
  float         ren_max;       // #CONDSHOW_ON_on maximum value to use for renormalization -- you must experiment with range of inputs to determine appropriate values
  float         ren_scale;     // #READ_ONLY 1.0 / (ren_max - ren_min)

  TA_SIMPLE_BASEFUNS(AudRenormSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};



taTypeDef_Of(MelCepstrumSpec);

class E_API MelCepstrumSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound mel frequency sampling parameters
INHERITED(taOBase)
public:
  bool          on;            // perform cepstrum discrete cosine transform (dct) of the mel-frequency filter bank features
  int           n_coeff;       // #CONDSHOW_ON_on #DEF_13 number of mfcc coefficients to output -- typically 1/2 of the number of filterbank features

  TA_SIMPLE_BASEFUNS(MelCepstrumSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};


taTypeDef_Of(AudGaborSpec);

class E_API AudGaborSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound params for auditory gabor filters: 2d Gaussian envelope times a sinusoidal plane wave -- by default produces 2 phase asymmetric edge detector filters -- horizontal tuning is different from V1 version -- has elongated frequency-band specific tuning, not a parallel horizontal tuning -- and has multiple of these
INHERITED(taOBase)
public:
  bool          on;             // use this gabor filtering of the time-frequency space filtered input (time in terms of steps of the DFT transform, and discrete frequency factors based on the FFT window and input sample rate)
  int		sz_time;	// #CONDSHOW_ON_on #DEF_6;8;12;16;24 size of the filter in the time (horizontal) domain, in terms of steps of the underlying DFT filtering steps
  int		sz_freq;	// #CONDSHOW_ON_on #DEF_6;8;12;16;24 size of the filter in the frequency domain, in terms of discrete frequency factors based on the FFT window and input sample rate
  int		spc_time;	// #CONDSHOW_ON_on spacing in the time (horizontal) domain, in terms of steps
  int		spc_freq;	// #CONDSHOW_ON_on spacing in the frequency (vertical) domain
  float		wvlen;		// #CONDSHOW_ON_on #DEF_1.5;2 wavelength of the sine waves in normalized units
  float		sig_len;	// #CONDSHOW_ON_on #DEF_0.6 gaussian sigma for the length dimension (elongated axis perpendicular to the sine waves) -- normalized as a function of filter size in relevant dimension
  float		sig_wd;	// #CONDSHOW_ON_on #DEF_0.3 gaussian sigma for the width dimension (in the direction of the sine waves) -- normalized as a function of filter size in relevant dimension
  float		sig_hor_len;    // #CONDSHOW_ON_on #DEF_0.4 gaussian sigma for the length of special horizontal narrow-band filters -- normalized as a function of filter size in relevant dimension
  float		sig_hor_wd;     // #CONDSHOW_ON_on #DEF_0.3 gaussian sigma for the horizontal dimension for special horizontal narrow-band filters -- normalized as a function of filter size in relevant dimension
  float		gain;		// #CONDSHOW_ON_on #DEF_2 overall gain multiplier applied after gabor filtering -- only relevant if not using renormalization (otherwize it just gets renormed away)
  int		n_horiz;	// #CONDSHOW_ON_on #DEF_4 number of horizontally-elongated,  pure time-domain, frequency-band specific filters to include, evenly spaced over the available frequency space for this filter set -- in addition to these, there are two diagonals (45, 135) and a vertically-elongated (wide frequency band) filter
  float		phase_off;	// #CONDSHOW_ON_on #DEF_0;1.5708 offset for the sine phase -- default is an asymmetric sine wave -- can make it into a symmetric cosine gabor by using PI/2 = 1.5708
  bool		circle_edge;	// #CONDSHOW_ON_on #DEF_true cut off the filter (to zero) outside a circle of diameter filter_size -- makes the filter more radially symmetric

  int           n_filters;      // #CONDSHOW_ON_on #READ_ONLY #SHOW total number of filters = 3 + n_horiz

  
  virtual void	RenderFilters(float_Matrix& fltrs);
  // generate filters into the given matrix, which is formatted as: [sz_time_steps][sz_freq][n_filters]

  virtual void	GridFilters(float_Matrix& fltrs, DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filters into data table and generate a grid view (reset any existing data first)

  TA_SIMPLE_BASEFUNS(AudGaborSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};


eTypeDef_Of(AuditoryProc);

class E_API AuditoryProc : public ImgProcThreadBase {
  // Auditory processing system -- parameters for performing various auditory processing steps on sound waveforms, resulting in activation patterns suitable as input to a network
INHERITED(ImgProcThreadBase)
public:
  enum SaveMode {		// how to add new data to the data table
    NONE_SAVE,			// don't save anything at all -- overrides any more specific save guys and prevents any addition or modification to the data table
    FIRST_ROW,			// always overwrite the first row -- does EnforceRows(1) if rows = 0
    ADD_ROW,			// always add a new row and write to that, preserving a history of inputs over time -- should be reset at some interval!
  };


  DataTableRef	data_table;	// data table for saving filter results for viewing and applying to networks etc
  SaveMode	save_mode;	// how to add new data to the data table
  AudInputSpec  input;          // specifications of the raw auditory input
  AudDftSpec    dft;            // specifications for how to compute the discrete fourier transform (DFT, using FFT)
  MelFBankSpec  mel_fbank;      // specifications of the mel feature bank frequency sampling of the DFT (FFT) of the input sound
  AudRenormSpec fbank_renorm;   // #CONDSHOW_ON_mel_fbank.on renormalization parmeters for the mel_fbank values -- performed prior to further processing
  AudGaborSpec  gabor1;    // #CONDSHOW_ON_mel_fbank.on full set of frequency / time gabor filters -- first size
  AudGaborSpec  gabor2;    // #CONDSHOW_ON_mel_fbank.on full set of frequency / time gabor filters -- second size
  AudGaborSpec  gabor3;    // #CONDSHOW_ON_mel_fbank.on full set of frequency / time gabor filters -- third size
  MelCepstrumSpec mfcc;         // #CONDSHOW_ON_mel_fbank.on specifications of the mel cepstrum discrete cosine transform of the mel fbank filter features
  V1KwtaSpec	gabor_kwta;	// #CONDSHOW_ON_gabor1.on k-winner-take-all inhibitory dynamics for the time-gabor output


  ///////////////////////////////////////////////////
  //    Filters

  int                   dft_size;    // #READ_ONLY #NO_SAVE full size of fft output -- should be input.win_samples
  int                   dft_use;     // #READ_ONLY #NO_SAVE number of dft outputs to actually use -- should be dft_size / 2 + 1
  int                   mel_n_filters_eff; // #READ_ONLY #NO_SAVE effective number of mel filters: mel.n_filters + 2
  float_Matrix          mel_pts_mel; // #READ_ONLY #NO_SAVE [mel_n_filters_eff] scale points in mel units (mels)
  float_Matrix          mel_pts_hz;  // #READ_ONLY #NO_SAVE [mel_n_filters_eff] mel scale points in hz units 
  int_Matrix            mel_pts_bin; // #READ_ONLY #NO_SAVE [mel_n_filters_eff] mel scale points in fft bins
  int                   mel_filt_max_bins; // #READ_ONLY #NO_SAVE maximum number of bins for mel filter -- number of bins in highest filter
  float_Matrix          mel_filters; // #READ_ONLY #NO_SAVE [mel_filt_max_bins][mel.n_filters] the actual filters for actual number of mel filters

  float_Matrix          gabor1_filters; // #READ_ONLY #NO_SAVE full gabor filters
  float_Matrix          gabor2_filters; // #READ_ONLY #NO_SAVE full gabor filters
  float_Matrix          gabor3_filters; // #READ_ONLY #NO_SAVE full gabor filters
  
  
  //////////////////////////////////////////////////////////////
  //	Outputs

  bool                  first_step; // #READ_ONLY #NO_SAVE #SHOW is this the first step of processing -- turns of prv smoothing of dft power
  int                   input_pos;   // #READ_ONLY #NO_SAVE #SHOW current position in the sound_full input -- in terms of sample number
  int                   trial_start_pos;   // #READ_ONLY #NO_SAVE #SHOW starting position of the current trial -- in terms of sample number
  int                   trial_end_pos;   // #READ_ONLY #NO_SAVE #SHOW ending position of the current trial -- in terms of sample number
  XYNGeom               gabor1_geom;     // #CONDSHOW_ON_gabor1.on #READ_ONLY #SHOW overall geometry of gabor1 output (group-level geometry -- feature / unit level geometry is n_features, 2)
  XYNGeom               gabor2_geom;     // #CONDSHOW_ON_gabor2.on #READ_ONLY #SHOW overall geometry of gabor1 output (group-level geometry -- feature / unit level geometry is n_features, 2)
  XYNGeom               gabor3_geom;     // #CONDSHOW_ON_gabor3.on #READ_ONLY #SHOW overall geometry of gabor1 output (group-level geometry -- feature / unit level geometry is n_features, 2)


  float_Matrix          sound_full;  // #READ_ONLY #NO_SAVE the full sound input obtained from the sound input
  float_Matrix          window_in;  // #READ_ONLY #NO_SAVE [input.win_samples] the raw sound input, one channel at a time
  complex_float_Matrix	dft_out;   // #READ_ONLY #NO_SAVE [2, dft_size] discrete fourier transform (fft) output complex representation
  float_Matrix          dft_power_out; // #READ_ONLY #NO_SAVE [dft_use] power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)
  float_Matrix          dft_log_power_out; // #READ_ONLY #NO_SAVE [dft_use] log power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)
  float_Matrix          dft_power_trial_out; // #READ_ONLY #NO_SAVE [dft_use][input.total_steps][input.channels] full trial's worth of power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)
  float_Matrix          dft_log_power_trial_out; // #READ_ONLY #NO_SAVE [dft_use][input.total_steps][input.channels] full trial's worth of log power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)

  float_Matrix          mel_fbank_out; // #READ_ONLY #NO_SAVE [mel.n_filters] mel scale transformation of dft_power, using triangular filters, resulting in the mel filterbank output -- the natural log of this is typically applied
  float_Matrix          mel_fbank_trial_out; // #READ_ONLY #NO_SAVE [mel.n_filters][input.total_steps][input.channels] full trial's worth of mel feature-bank output -- only if using gabors

  float_Matrix	        gabor_gci;	 // #READ_ONLY #NO_SAVE inhibitory conductances, for computing kwta
  float_Matrix          gabor1_trial_raw; // #READ_ONLY #NO_SAVE [gabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] raw output of gabor1 -- full trial's worth of gabor steps
  float_Matrix          gabor1_trial_out; // #READ_ONLY #NO_SAVE [gabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] post-kwta output of full trial's worth of gabor steps
  
  float_Matrix          gabor2_trial_raw; // #READ_ONLY #NO_SAVE [gabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] raw output of gabor1 -- full trial's worth of gabor steps
  float_Matrix          gabor2_trial_out; // #READ_ONLY #NO_SAVE [gabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] post-kwta output of full trial's worth of gabor steps
  
  float_Matrix          gabor3_trial_raw; // #READ_ONLY #NO_SAVE [gabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] raw output of gabor1 -- full trial's worth of gabor steps
  float_Matrix          gabor3_trial_out; // #READ_ONLY #NO_SAVE [gabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] post-kwta output of full trial's worth of gabor steps
  
  float_Matrix          mfcc_dct_out; // #READ_ONLY #NO_SAVE discrete cosine transform of the log_mel_filter_out values, producing the final mel-frequency cepstral coefficients 
  float_Matrix          mfcc_dct_trial_out; // #READ_ONLY #NO_SAVE full trial's worth of discrete cosine transform of the log_mel_filter_out values, producing the final mel-frequency cepstral coefficients 

  virtual bool  NeedsInit();
  // #CAT_Auditory does system need init?
  
  virtual bool 	Init();
  // #BUTTON initialize everything to be ready to start filtering -- calls InitFilters, InitOutMatrix, InitDataTable

  virtual void  InitFromSound(taSound* sound, int n_chans = 1, int chan = 0);
  // #CAT_Auditory get sample_rate and channels info from given sound object -- n_chans arg is how many channels we want to process -- overrides number of channels present in sound obj if >= 1 (i.e., use -1 to just use number of channels present in sound object) -- if sound obj has multiple channels and n_chans = 1, then chan specifies which channel to read from

  virtual bool	InitSound();
  // #CAT_Auditory reset the current sound information -- unloads any existing sound
  
  virtual bool	LoadSound(taSound* sound);
  // #CAT_Auditory load a new sound into the system for subsequent processing -- grabs the sound data into sound_full

  virtual bool	StartNewSound();
  // #CAT_Auditory start a new sound -- zeros out stuff from previous sound..
  
  virtual int   InputStepsLeft();
  // #CAT_Auditory number of steps left to process in the current input sound

  virtual bool	ProcessTrial();
  // #CAT_Auditory process a full trial worth of sound -- iterates over steps to fill a trial's worth of sound data

  virtual bool	StepToSample(int samp_pos);
  // #CAT_Auditory incrementally step forward through sound until given sample position is within the nearest step to the start of the trial

  virtual bool	WrapBorder(int chan);
  // #CAT_Auditory at start of trial, wrap-around previous end-of-trial border values to now be the current start-of-trial border values

  virtual bool	StepForward(int chan);
  // #CAT_Auditory Step the current trial's worth of data forward by one step -- just does a copy of the _trial output data one step to the left, making room for a new step at the end

  virtual bool	CopyStepFromStep(int to_step, int fm_step, int chan);
  // #CAT_Auditory copy a step of trial data to given step index from given step index

  virtual bool	ProcessStep(int chan, int step);
  // #CAT_Auditory process a step worth of sound input from current input_pos, and increment input_pos by input.step_samples

  virtual bool	SoundToWindow(int in_pos, int chan);
  // #CAT_Auditory get sound from sound_full at given position and channel, into window_in -- pads with zeros for any amount not available in the sound_full input

  virtual bool  FilterWindow(int chan, int step);
  // #CAT_Auditory filter the current window_in input data according to current settings -- called by ProcessStep, but can be called separately 
  
  virtual bool	FilterTrial(int chan);
  // #CAT_Auditory process filters that operate over an entire trial at a time 

  virtual bool  NewTableRow();
  // #CAT_Auditory prepare data_table for a new output, according to save_mode setting
  
  virtual bool  OutputToTable(int chan);
  // #CAT_Auditory record the current trial's worth of filter output into data table for given channel
  
  virtual void	PlotMelFilters(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the Mel scale triangular filters
  virtual void	GridGaborFilters(DataTable* disp_data, int gabor_n = 1);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the full gabor filters as a grid display -- choose which set of gabors to plot (1-3)
  
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
  virtual bool  InitDataTable_chan(int chan);
  // initialize data table to fit data saving as configured

  virtual bool  MelOutputToTable(DataTable* dtab, int chan, bool fmt_only = false);
  // mel filter bank to output table

  virtual void  DftInput(int chan, int step);
  // apply dft (fft) to input
  virtual void  PowerOfDft(int chan, int step);
  // compute power of dft output
  virtual void  MelFilterDft(int chan, int step);
  // apply mel filters to power of dft
  virtual void  CepstrumDctMel(int chan, int step);
  // apply discrete cosine transform (DCT) to get the cepstrum coefficients on the mel filterbank values

  virtual void  GaborFilter_impl
    (int chan, const AudGaborSpec& spec, const float_Matrix& filters,
     float_Matrix& out_raw, float_Matrix& out);
  // compute time gabors over entire trial's worth of data
  
public:  
  TA_SIMPLE_BASEFUNS(AuditoryProc);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // AuditoryProc_h
