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
  void 	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy() { };
};


taTypeDef_Of(AudDftSpec);

class E_API AudDftSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound DFT (discrete fourier transform) specifications
INHERITED(taOBase)
public:
  bool          log_pow;       // compute the log of the power and save that to a separate table
  float         log_off;       // #CONDSHOW_ON_log_pow add this amount when taking the log of the dft power -- e.g., 1.0 makes everything positive -- affects the relative contrast of the outputs
  float         log_min;       // #CONDSHOW_ON_log_pow minimum value a log can produce -- puts a lower limit on log output

  TA_SIMPLE_BASEFUNS(AudDftSpec);
// protected:
//   void 	UpdateAfterEdit_impl();
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
  float         lo_hz;         // #DEF_300 #CONDSHOW_ON_on low frequency end of mel frequency spectrum
  float         hi_hz;         // #DEF_8000 #CONDSHOW_ON_on high frequency end of mel frequency spectrum -- must be <= sample_rate / 2 (i.e., less than the Nyquist frequency)
  int           n_filters;     // #DEF_26 #CONDSHOW_ON_on number of Mel frequency filters to compute
  float         log_off;       // #CONDSHOW_ON_on add this amount when taking the log of the Mel filter sums to produce the filter-bank output -- e.g., 1.0 makes everything positive -- affects the relative contrast of the outputs
  float         log_min;       // #CONDSHOW_ON_on minimum value a log can produce -- puts a lower limit on log output
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
  void 	UpdateAfterEdit_impl();
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
  void 	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy() { };
};


taTypeDef_Of(AudTimeGaborSpec);

class E_API AudTimeGaborSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Sound time-dimension only gabor filtering -- like a delta code but smoother -- really just a cosine filter -- uses both polarities and takes positive-only output
INHERITED(taOBase)
public:
  bool          on;            // use time-dimension gabors
  int           st_half_size;  // #CONDSHOW_ON_on #DEF_1 starting half-size of filters in terms of time steps -- filters are always even sized 
  int           ed_half_size;  // #CONDSHOW_ON_on #DEF_3 ending half-size of filters in terms of time steps -- requires input.border_steps >= this number! -- filters are always even sized 
  int           inc_half_size; // #CONDSHOW_ON_on #DEF_1 increment in half-size steps to take between start and end sizes
  bool          xx1_norm;      // #CONDSHOW_ON_on apply an x / (x+1) normalization to the deltas to keep within 0-1 range -- is relatively linear at the start then saturates 
  float         xx1_gain;      // #CONDSHOW_ON_on&&xx1_norm gain on the xx1 normalization function

  int           max_size;       // #READ_ONLY maximum size = ed_half_size * 2
  int           n_filters;      // #READ_ONLY number of filters = (ed-st) / inc + 1

  inline float  XX1(float delta)
  { float gm = delta * xx1_gain; return gm / (1.0f + gm); }
  // the X-over-X+1 normalization function
  
  virtual void	RenderFilters(float_Matrix& fltrs, int_Matrix& flt_sizes);
  // generate filters into the given matrix, which is formatted as: [max_size][n_filters] and record the actual half-sizes of these filters in flt_sizes

  virtual void	GridFilters(float_Matrix& fltrs, int_Matrix& flt_sizes,
                            DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filters into data table and generate a grid view (reset any existing data first)

  TA_SIMPLE_BASEFUNS(AudTimeGaborSpec);
protected:
  void 	UpdateAfterEdit_impl();
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
  void 	UpdateAfterEdit_impl();
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
  float		gain;		// #CONDSHOW_ON_on #DEF_2 overall gain multiplier applied after gabor filtering -- only relevant if not using renormalization (otherwize it just gets renormed away)
  int		n_horiz;	// #CONDSHOW_ON_on #DEF_4 number of horizontally-elongated,  pure time-domain, frequency-band specific filters to include, evenly spaced over the available frequency space for this filter set -- in addition to these, there are two diagonals (45, 135) and a vertically-elongated (wide frequency band) filter
  int		sz_time_msec;	// #CONDSHOW_ON_on #DEF_6;8;12;16;24 size of the filter in the time (horizontal) domain, in terms of milliseconds -- converted into sz_time_steps units based on input params 
  int		sz_freq;	// #CONDSHOW_ON_on #DEF_6;8;12;16;24 size of the filter in the frequency domain, in terms of discrete frequency factors based on the FFT window and input sample rate
  float		spacing_pct;	// #CONDSHOW_ON_on #DEF_0.5 how far apart to space the centers of the gabor filters, as a proportion of the size of the filter in each dimension -- .5 = 1/2 overlap and is typical
  float		wvlen;		// #CONDSHOW_ON_on #DEF_6;12;18;24  wavelength of the sine waves -- number of pixels over which a full period of the wave takes place (computation adds a 2 PI factor to translate into pixels instead of radians)
  float		gauss_sig_len;	// #CONDSHOW_ON_on #DEF_0.225;0.3 gaussian sigma for the length dimension (elongated axis perpendicular to the sine waves) -- normalized as a function of filter_size
  float		gauss_sig_wd;	// #CONDSHOW_ON_on #DEF_0.15;0.2 gaussian sigma for the width dimension (in the direction of the sine waves) -- normalized as a function of filter_size
  float		phase_off;	// #CONDSHOW_ON_on #DEF_0;1.5708 offset for the sine phase -- can make it into a symmetric gabor by using PI/2 = 1.5708
  bool		circle_edge;	// #CONDSHOW_ON_on #DEF_true cut off the filter (to zero) outside a circle of diameter filter_size -- makes the filter more radially symmetric

  int           n_filters;      // #CONDSHOW_ON_on #READ_ONLY #SHOW total number of filters = 3 + n_horiz
  int		sz_time_steps;	// #CONDSHOW_ON_on #READ_ONLY #SHOW size of the filter in the time (horizontal) domain, in terms of steps
  int		spc_time_steps;	// #CONDSHOW_ON_on #READ_ONLY #SHOW spacing in the time (horizontal) domain, in terms of steps
  int		spc_freq;	// #CONDSHOW_ON_on #READ_ONLY #SHOW spacing in the frequency (vertical) domain

  
  virtual void	RenderFilters(float_Matrix& fltrs);
  // generate filters into the given matrix, which is formatted as: [sz_time_steps][sz_freq][n_filters]

  virtual void	GridFilters(float_Matrix& fltrs, DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filters into data table and generate a grid view (reset any existing data first)

  TA_SIMPLE_BASEFUNS(AudGaborSpec);
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
  AudDftSpec    dft;            // specifications for how to compute the discrete fourier transform (DFT, using FFT)
  MelFBankSpec  mel_fbank;      // specifications of the mel feature bank frequency sampling of the DFT (FFT) of the input sound
  AudRenormSpec fbank_renorm;   // #CONDSHOW_ON_mel_fbank.on renormalization parmeters for the mel_fbank values -- performed prior to further processing
  AudTimeGaborSpec  fbank_tgabor; // #CONDSHOW_ON_mel_fbank.on time-dimension only 1d gabor-like filters for transforming the normalized mel fbank outputs
  MelCepstrumSpec mfcc;         // #CONDSHOW_ON_mel_fbank.on specifications of the mel cepstrum discrete cosine transform of the mel fbank filter features
  V1KwtaSpec	tgabor_kwta;	// #CONDSHOW_ON_fbank_tgabor.on k-winner-take-all inhibitory dynamics for the time-gabor output


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
  float_Matrix          tgabor_filters; // #READ_ONLY #NO_SAVE temporal gabor filters
  int_Matrix            tgabor_flt_sizes; // #READ_ONLY #NO_SAVE temporal gabor half-sizes of filters
  
  
  //////////////////////////////////////////////////////////////
  //	Outputs

  float_Matrix          sound_full;  // #READ_ONLY #NO_SAVE the full sound input obtained from the sound input
  int                   input_pos;   // #READ_ONLY #NO_SAVE #SHOW current position in the sound_full input -- in terms of sample number
  int                   trial_start_pos;   // #READ_ONLY #NO_SAVE #SHOW starting position of the current trial -- in terms of sample number
  int                   trial_end_pos;   // #READ_ONLY #NO_SAVE #SHOW ending position of the current trial -- in terms of sample number
  float_Matrix          window_in;  // #READ_ONLY #NO_SAVE [input.win_samples] the raw sound input, one channel at a time
  complex_float_Matrix	dft_out;   // #READ_ONLY #NO_SAVE [2, dft_size] discrete fourier transform (fft) output complex representation
  float_Matrix          dft_power_out; // #READ_ONLY #NO_SAVE [dft_use] power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)
  float_Matrix          dft_log_power_out; // #READ_ONLY #NO_SAVE [dft_use] log power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)
  float_Matrix          dft_power_trial_out; // #READ_ONLY #NO_SAVE [dft_use][input.total_steps][input.channels] full trial's worth of power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)
  float_Matrix          dft_log_power_trial_out; // #READ_ONLY #NO_SAVE [dft_use][input.total_steps][input.channels] full trial's worth of log power of the dft, up to the nyquist limit frequency (1/2 input.win_samples)

  float_Matrix          mel_fbank_out; // #READ_ONLY #NO_SAVE [mel.n_filters] mel scale transformation of dft_power, using triangular filters, resulting in the mel filterbank output -- the natural log of this is typically applied
  float_Matrix          mel_fbank_trial_out; // #READ_ONLY #NO_SAVE [mel.n_filters][input.total_steps][input.channels] full trial's worth of mel feature-bank output -- only if using time gabors

  float_Matrix          tgabor_trial_raw; // #READ_ONLY #NO_SAVE [tgabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] raw output of time gabor -- full trial's worth of time gabor steps
  float_Matrix	        tgabor_gci;	 // #READ_ONLY #NO_SAVE inhibitory conductances, for computing kwta
  float_Matrix          tgabor_trial_out; // #READ_ONLY #NO_SAVE [tgabor.n_filters*2][mel.n_filters][input.trial_steps][input.channels] post-kwta output of full trial's worth of time gabor steps
  
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
  virtual void	GridTimeGaborFilters(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the time gabor filters as a grid display

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
  virtual void  TimeGaborFilter(int chan);
  // compute time gabors over entire trial's worth of data
  
public:  
  TA_SIMPLE_BASEFUNS(AuditoryProc);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // AuditoryProc_h
