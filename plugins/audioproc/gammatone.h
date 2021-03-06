#ifndef GAMMATONE_H
#define GAMMATONE_H
 
#include "audioproc.h"

#include "minmax.h"

class FilterChan;
class FilterChan_List;
class GammatoneChan;
class GammatoneChan_List;
class GammatoneBlock;
class ChansPerOct;
class SharpenBlock;
class ANVal;
class ANBlock;


class AUDIOPROC_API FilterChan: public SignalProcItem
{ // #NO_SHOW ##CAT_Audioproc one channel of a filter
INHERITED(SignalProcItem)
public:
  static bool		ChanFreqOk(float cf, float ear_q, float min_bw,
   float fs); // common routine for determining when a band is "safely" within nyquist limits
  bool	 		on; // #READ_ONLY #SHOW if channel is ok with current sample rate, otherwise ignored
  float 		cf; // #READ_ONLY #SHOW center frequency of this channel
  float			gain; // #READ_ONLY #SHOW the gain of this channel
  double		erb; // #READ_ONLY #SHOW the equivalent rectangular bandwidth of this channel
  
  void			InitChan(float cf, float gain, double erb); // #IGNORE
  
  static const KeyString key_on; // "on"
  static const KeyString key_cf; // "cf"
  static const KeyString key_erb; // "erb"
  static const KeyString key_gain; // "gain"
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  
  override void SetDefaultName() {name = _nilString;}
  override int	GetEnabled() const {return on ? 1 : 0;} // for items that support an enabled/disabled state; -1=n/a, 0=disabled, 1=enabled (note: (bool)-1 = true)
  TA_BASEFUNS(FilterChan)

private:
  SIMPLE_COPY(FilterChan)
  void	Initialize();
  void	Destroy() {}
};

class AUDIOPROC_API FilterChan_List: public taList<FilterChan>
{ // ##CAT_Audioproc list of channels, that will operate in parallel
INHERITED(taList<FilterChan>) 
public:

  override int		NumListCols() const;
  // name, val_type (float, etc.), disp_opts
  override String	GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  
  TA_BASEFUNS_NOCOPY(FilterChan_List)

private:
  void	Initialize() {SetBaseType(&TA_FilterChan);}
  void	Destroy() {}
};


class AUDIOPROC_API GammatoneChan: public FilterChan
{ // #NO_SHOW ##CAT_Audioproc one channel of a gammatone filter
INHERITED(FilterChan)
friend class GammatoneBlock; 
public:
  void			InitChan(float cf, float ear_q, float min_bw,
     float fs); // #IGNORE
  
  TA_BASEFUNS_NOCOPY(GammatoneChan)

protected:
  double tpt;
  double a1, a2, a3, a4, a5;
  double p0r, p1r, p2r, p3r, p4r, p0i, p1i, p2i, p3i, p4i;
  double coscf, sincf, cs, sn;
  
  void 			DoFilter(int n, int in_stride, const float* x,
    int out_stride, float* bm, float* env = NULL);

private:
  void	Initialize();
  void	Destroy() {}
};

class AUDIOPROC_API GammatoneChan_List: public FilterChan_List
{ // ##CAT_Audioproc list of channels, that will operate in parallel
INHERITED(FilterChan_List) 
public:
  GammatoneChan*	FastEl(int i) const
    {return (GammatoneChan*)el[i];} 
  
  TA_BASEFUNS_NOCOPY(GammatoneChan_List)

private:
  void	Initialize() {SetBaseType(&TA_GammatoneChan);}
  void	Destroy() {}
};


class AUDIOPROC_API GammatoneBlock: public StdBlock
{ // ##CAT_Audioproc gammatone filter bank
INHERITED(StdBlock) 
public:
  enum OutVals { // #BITS Output Values 
    OV_SIG	= 0x01, // #LABEL_Signal signal channel
    OV_ENV	= 0x02, // #LABEL_Envelope envelope
    OV_FREQ	= 0x04, // #IGNORE instantaneous frequency, no longer supported
    OV_DELTA_ENV = 0x08, // #LABEL_DeltaEnvelope 1st derivitive (per 100us) of envelope (requires Envelope)
    OV_SIG_ENV	= 0x03, // #NO_BIT signal + envelope
    OV_SIG_ENV_FREQ = 0x07 // #NO_BIT signal + envelope + instantaneous frequency
  };
  
  enum ChanSpacing { // how to space the channels
    CS_0		= 0, // #IGNORE
    CS_MooreGlassberg	= 0, // #LABEL_MooreGlassberg equal ERB crossings
    CS_LogLinear	= 1, // #LABEL_LogLinear simple log linear
    CS_MelCepstrum	= 2, // #LABEL_MelCepstrum uses spacing equivalent to std Mel Cepstrum filters NOTE: filter response itself is NOT MC, only chan ctr freqs
  };
  
  DataBuffer		out_buff_env; //  #SHOW_TREE envelope output (if enabled)
  DataBuffer		out_buff_delta_env; //  #SHOW_TREE delta envelope output (if enabled)
  OutVals		out_vals; // the desired output values
  ChanSpacing		chan_spacing; // how to space the channels
  float			ear_q; // #EXPERT Moore and Glasberg def=9.26449 -- ERB is min_bw + (cf / ear_q)
  float			min_bw; // #EXPERT minimum bandwidth
  float			cf_lo; // #CONDEDIT_OFF_chan_spacing:CS_MelCepstrum lower center frequency (Hz)
  float			cf_hi; // #CONDEDIT_ON_chan_spacing:CS_MooreGlassberg upper center frequency (Hz)
  float			chans_per_oct; // #MIN_0 #CONDSHOW_ON_chan_spacing:CS_LogLinear number of channels per octave
  int			n_chans; // #MIN_1 number of filter bands
  
  GammatoneChan_List	chans; // #NO_SAVE the individual channels
  
  override taList_impl*  children_() {return &chans;} //note: required
  override int		outBuffCount() const {return 3;}
  override DataBuffer* 	outBuff(int idx) {switch (idx) {
    case 0: return &out_buff; 
    case 1: return &out_buff_env;
    case 2: return &out_buff_delta_env;
    default: return NULL;}}
  
  virtual void		GraphFilter(DataTable* disp_data,
    bool log_freq = true, OutVals response = OV_SIG);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter response into a data table and generate a graph; if log_freq then log10 of freq is output; CHOOSE ONLY 1 RESPONSE
  void			MakeStdFilters(float cf_lo=65.4064f, float cf_hi=7902.13f,
   int n_chans=84); // #BUTTON #NO_UPDATE_AFTER make a "standard" filter bank, using linear spacing, with 12 chans/octave, centered on musical scale
  ProcStatus 		AcceptData_GT(float_Matrix* in_mat, int stage = 0);
    // #IGNORE mostly for proc

  SIMPLE_LINKS(GammatoneBlock)
  TA_BASEFUNS(GammatoneBlock) //
  
public: // diagnostic values
  float			delta_env_dt_inv; // #HIDDEN #READ_ONLY #NO_SAVE 1/dt value, calc'ed once for efficiency

protected:
  int			num_out_vals;
  override void		UpdateAfterEdit_impl();
  
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void 	InitChildConfig_impl(bool check, bool quiet, bool& ok); 
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(GammatoneBlock)
}; //


class AUDIOPROC_API MelCepstrumBlock: public StdBlock
{ // ##CAT_Audioproc gammatone filter bank
INHERITED(StdBlock) 
public:
  enum MelFlags { // #BITS
    MF_0		= 0, // #IGNORE
    MF_COMPRESS		= 0x001, //  #LABEL_Compress compress mel outputs logarithmically (using natural log)
    MF_DCT		= 0x002, //  #LABEL_DCT apply Discrete Cosine Transform and create cepstrum coefficients
    MF_USE_MFCC0	= 0x004, //  #LABEL_Use_MFCC0 output the channel 0 DCT (typically not used)
    MF_AUTO_GAIN	= 0x008, //  #LABEL_Use_AutoGain automatically control gain to yield ~ flat in/out gain
  };
  
  enum MelWarp {
    MW_LIN_LOG,		// use a bank of linear freq then a bank of log freq bins -- if lin freq bins are mults of fs there will be less warp distortion
    MW_FORMULA,		// use the formula 1127*ln(1+f/700) for mel warping
  };
  
  MelFlags		mel_flags; // flags that control what type of processing and output
  float			out_rate; // output rate, in ms (frames will be 2x this duration, i.e. half-overlapping)
  Level			auto_gain; // #CONDEDIT_OFF_mel_flags:MF_AUTO_GAIN  gain adjustment -- when on AUTO is based on the output type selected; crudely makes 1Khz sine wave have ~1 output (linear) in peak channel of mel fft
  MelWarp		mel_warp; // how to determine the Mel frequency warping -- LIN_LOG is often used by the speech community
  float			cf_lo; // lower center frequency (Hz)
  float			cf_lin_bw; // #CONDSHOW_ON_mel_warp:MW_LIN_LOG linear range bandwidth
  float			cf_log_factor; // #CONDSHOW_ON_mel_warp:MW_LIN_LOG how much to multiple to get next log channel 
  int			n_lin_chans; // #CONDSHOW_ON_mel_warp:MW_LIN_LOG #MIN_1 number of linear bands
  int			n_log_chans; // #CONDSHOW_ON_mel_warp:MW_LIN_LOG #MIN_1 number of log bands
  float			cf_hi; // #CONDSHOW_ON_mel_warp:MW_FORMULA high center frequency (Hz)
  float			n_chans; // #CONDSHOW_ON_mel_warp:MW_FORMULA #MIN_2 number of channels
  Level			comp_thresh; // #CONDEDIT_ON_mel_flags:MF_COMPRESS lower threshold (clamped at this)
  int			n_cepstrum; // #CONDEDIT_ON_mel_flags:MF_DCT #MIN_4 number of cepstrum output coefficients (including 0, even if not used)
  
  FilterChan_List	chans; // #NO_SAVE the individual channels
  
  override taList_impl*  children_() {return &chans;} //note: required
  
  virtual void		GraphFilter(DataTable* disp_data,
    bool log_freq = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter response into a data table and generate a graph; if log_freq then log10 of freq is output; CHOOSE ONLY 1 RESPONSE
  ProcStatus 		AcceptData_MC(float_Matrix* in_mat, int stage = 0);
    // #IGNORE mostly for proc

  SIMPLE_LINKS(MelCepstrumBlock)
  TA_BASEFUNS(MelCepstrumBlock) //

public: // DO NOT USE
  float_Matrix		in_buff; // #EXPERT_TREE #NO_SAVE buffer input values
  float_Matrix		window_filt; // #EXPERT_TREE #NO_SAVE hamming window filter
  float_Matrix		mel_filt; // #EXPERT_TREE #NO_SAVE mel warping filter filter d0:in, d1: out (psuedo-sparse, mel_idx is min,max bin idx)
  int_Matrix		mel_idx; // #EXPERT_TREE #NO_SAVE min..max fft bin indexes; d0:min..max, d1:out
  float_Matrix		fft_in; // #EXPERT_TREE #NO_SAVE buffer used for fft
  float_Matrix		fft_out; // #EXPERT_TREE #NO_SAVE buffer used for fft
  float_Matrix		mel_out; // #EXPERT_TREE #NO_SAVE buffer used for mel spectrum
  float_Matrix		dct_filt; // #EXPERT_TREE #NO_SAVE dct filter coefficients; d0:in, d1:out
  float			cf_lo_m1; // #EXPERT #NO_SAVE #READ_ONLY cf_lo minus 1, for mel triangular filter
  float			cf_hi_p1; // #EXPERT #NO_SAVE #READ_ONLY cf_hi plus 1, for mel triangular filter
protected:
  int			num_out_chans; // will depend on whether dct enabled or not
  int			n_chans_on; // we only "see" the chans that are on
  int			out_size; // output size, in samples (= fs*out_rate)
  int			frame_size; // frame size, in samples = 2* out_size
  int			in_idx; // input values received, 0..frame_size-1
  float			fft_band; // width of each fft band, in Hz
  override void		UpdateAfterEdit_impl();
  
  override void 	InitConfig_impl(bool check, bool quiet, bool& ok);
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void 	InitChildConfig_impl(bool check, bool quiet, bool& ok); 
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(MelCepstrumBlock)
}; //


class AUDIOPROC_API ChansPerOct: public taBase { // #INLINE
INHERITED(taBase)
public:
  bool		auto_lookup; // lookup the value from the appropriate previous block
  float		chans_per_oct; // #CONDEDIT_OFF_auto_lookup the number of channels per octave
  bool		Lookup(SignalProcBlock* parent); // #IGNORE looks up and sets value, true if found
  operator float() const {return chans_per_oct;}
  TA_BASEFUNS(ChansPerOct) //
private:
  void	Initialize() {auto_lookup = true; chans_per_oct = 0.0f;}
  void	Destroy() {}
  SIMPLE_COPY(ChansPerOct)
};

class AUDIOPROC_API SharpenBlock: public StdBlock
{ // ##CAT_Audioproc #AKA_SharpenBlock sharpen the frequency response, similar to the active mechanism of basilar membrane -- usually used after Gammatone.Env filter output
INHERITED(StdBlock) 
public: //
  enum OutputFunction { // the type of function to apply to filtered value
    OF_STRAIGHT = 0, // output of DoG filter (output vals will go negative)
    OF_POWER, // use a power function of the DoG output (output will stay positive, but be compressed)
  };
  
  ChansPerOct	chans_per_oct; // channels per octave -- normally looked up from an upstream GammatoneBlock
  OutputFunction	out_fun; // function (if any) to use on filtered  values prior to output to next stage
  float			pow_gain; // #CONDSHOW_ON_out_fun:OF_POWER multiply by value before power
  float			pow_base; // #CONDSHOW_ON_out_fun:OF_POWER power base for filter calc
  
  DoG1dFilterSpec	dog; // #EXPERT_TREE filter specs  
  
  virtual void		GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
  
  
  SIMPLE_LINKS(SharpenBlock);
  TA_BASEFUNS(SharpenBlock) //
  
protected:
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(SharpenBlock)
};


class AUDIOPROC_API TemporalWindowBlock: public StdBlock
{ // ##CAT_Audioproc temporal windowing block
INHERITED(StdBlock) 
public: //
  enum FilterType {
    FT_DEF = 0, // #IGNORE
    FT_MG = 0, // #LABEL_Moore&Glasberg can do forward as well as back filtering (see Moore & Glasberg, 1987)
    FT_Exp = 1, // #HIDDEN TODO #LABEL_Exponential simple exponential (typical of many neural net and machine learning approaches)
    FT_DoG = 2, // #LABEL_DiffOfGaussian difference-of-gaussian -- enables on/off outputs
    FT_Uniform = 3, // #LABEL_Uniform -- simply integrates the data uniformly over the integration window -- often used with half-overlapping output rates
  };
  
  enum OutputType {
    OT_SINGLE, // #LABEL_Single output a single value, whether +ve -ve
    OT_ON_OFF, // #LABEL_On/Off output separate buffers for +ve (on, buff0) and -ve(off, buff1) values, typically only used with DoG type
  };
  
  enum NonLinearity {
    NL_NONE,		// #LABEL_None no non-linearity
    NL_HALF_WAVE,	// #LABEL_HalfWave half-wave rectification
    NL_FULL_WAVE,	// #LABEL_FullWave full-wave rectification
    NL_SQUARE		// #LABEL_Square signal is squared
  };
  
  DataBuffer		out_buff_off; //#CONDSHOW_ON_ot:OT_ON_OFF #SHOW_TREE off values

  override int		outBuffCount() const {return 2;}
  override DataBuffer* 	outBuff(int idx) 
    {if (idx == 1) return &out_buff_off; else return inherited::outBuff(idx);}
  
  NonLinearity		non_lin; // type of non-linearity to apply to input values -- choose NONE for known +ve inputs (ex. Gamma Env)
  Level			auto_gain; // #READ_ONLY #SHOW #NO_SAVE an automatically applied gain adjustment based on the non-lin and ot selected
  float			l_dur; // duration of lower (forward) window in ms, typ 24 ms
  float			u_dur; // duration of upper (backward) window in ms, type 8 ms
  float			out_rate; // output rate, in ms; typ 4 ms (we set our fs by this)		
  FilterType		ft; // filter type
  OutputType		ot; // output type,   
// MooreGlasberg parameters:
  Level			w; // #CONDSHOW_ON_ft:FT_MG w parameter, typically around -30dB
  float			tpl; // #CONDSHOW_ON_ft:FT_MG ms, peak of lower, typ ~5.5 
  float			tsl; // #CONDSHOW_ON_ft:FT_MG ms, skirt of lower, typ ~25 
  float			tpu; // #CONDSHOW_ON_ft:FT_MG ms, peak of upper, typ ~2.5 
  float			tsu; // #CONDSHOW_ON_ft:FT_MG ms, skirt of upper, typ ~12 
  
// Exponential parameters:
  float			sigma;	// #CONDSHOW_ON_ft:FT_Exp the sigma, normalized to duration of 1
  
// DoG parameters:
  float			on_sigma; // #CONDSHOW_ON_ft:FT_DoG width of the narrower central 'on' gaussian, duration normalized to 3 s.d., typ 0.5
  float			off_sigma; // #CONDSHOW_ON_ft:FT_DoG width of the wider surround 'off' gaussian, duration normalized to 3 s.d., typ 2*on_sigma
  
// hidden guys, for reference
  float_Matrix		filter; // #READ_ONLY #EXPERT_TREE #NO_SAVE filter 1d, contains l+u samples
  int 			out_wd; // #READ_ONLY #HIDDEN #NO_SAVE width of output, in fs.in samples
  int			flt_wd;  // #READ_ONLY #HIDDEN #NO_SAVE total virt+real filt width, in fs.in samples
  int 			v_flt_wd; // #READ_ONLY #HIDDEN #NO_SAVE virtual pre-l width (-ve indices), in fs.in samples
  int 			l_flt_wd; // #READ_ONLY #HIDDEN #NO_SAVE l width of filter, in fs.in samples 
  int 			u_flt_wd; // #READ_ONLY #HIDDEN #NO_SAVE u width of filter, in fs.in samples
  
  virtual void		GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
//  virtual void		GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view
  
  
  SIMPLE_LINKS(TemporalWindowBlock);
  TA_BASEFUNS(TemporalWindowBlock) //
  
public: // DO NOT USE
  int			conv_stages; // #READ_ONLY #EXPERT #NO_SAVE number of conv_mat stages needed to hold the in-flight conv data 
  int_Array		conv_idx; // #READ_ONLY #EXPERT_TREE #NO_SAVE 1 item per out frame, tracks the index; pre-decrement
  float_Matrix		conv_mat; // #READ_ONLY #EXPERT_TREE #EXPERT #NO_SAVE
protected:

  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  override void		InitThisConfigDataOut_impl(bool check, bool quiet, bool& ok);
  
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
    
  virtual void		CheckMakeFilter(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok);
    
  virtual void		CheckMakeFilter_MooreGlasberg(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok);
  virtual void		CheckMakeFilter_Exponential(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok);
  virtual void		CheckMakeFilter_DoG(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok);
  virtual void		CheckMakeFilter_Uniform(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(TemporalWindowBlock)
};


class AUDIOPROC_API DeltaBlock: public OutputBlock
{ // ##CAT_Audioproc Delta Block -- provides a bipolar split, or delta+ (0) and - (1) channel, usually used after a TemporalWindowBlock 
INHERITED(OutputBlock) 
public: //
  enum DerivDegree {
    ZERO,		// just splits a bipolar signal
    FIRST,		// first derivative
  };
  
  DerivDegree		degree; // type of delta to compute
  DataBuffer		out_buff_pl; // #SHOW_TREE plus delta
  DataBuffer		out_buff_mi; // #SHOW_TREE minus delta
  Level			auto_gain; // #READ_ONLY #SHOW #NO_SAVE an automatically applied gain adjustment based on the degree
  
  override int		outBuffCount() const {return 2;}
  override DataBuffer* 	outBuff(int idx) {switch (idx) {
    case 0: return &out_buff_pl;  case 1: return &out_buff_mi;} return NULL;}

  
  SIMPLE_LINKS(DeltaBlock);
  TA_BASEFUNS(DeltaBlock) //

public: //
  float_Array		data; // #IGNORE working buffer, used by CalcDelta
protected:
  
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
  virtual float		CalcDelta();
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(DeltaBlock)
};


class AUDIOPROC_API IIDBlock: public OutputBlock
{ // ##CAT_Audioproc Interaural Intensity Difference Block -- provides an IID+ (0) and - (1) channel for each field, usually used for chans > ~1Khz 
INHERITED(OutputBlock) 
public: //
  DataBuffer		out_buff_pl; // #EXPERT_TREE #EXPERT plus difference (contra > ipso)
  DataBuffer		out_buff_mi; // #EXPERT_TREE #EXPERT minus difference (contra < ipso)
  MinMaxInt		chan; // input channel range, max=-1 is 'last' -- usually set min to ~ 1Khz channel (ITD is usually used with f<~1.6Khz)
  MinMaxInt		chan_eff; // #READ_ONLY #NO_SAVE actual channels that will be used

  override int		outBuffCount() const {return 2;}
  override DataBuffer* 	outBuff(int idx) {switch (idx) {
    case 0: return &out_buff_pl;  case 1: return &out_buff_mi;} return NULL;}

  
  SIMPLE_LINKS(IIDBlock);
  TA_BASEFUNS(IIDBlock) //

protected:
//  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
  ProcStatus		AcceptData_IID(float_Matrix* in_mat, int stage);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(IIDBlock)
};


class AUDIOPROC_API ITDBlock: public StdBlock
{ // ##CAT_Audioproc Interaural Time Difference Block -- provides n ITD phase delay values per channel, r - del(l) and l - del(r), usually used for chans < ~1.6Khz (for human-like .625 us ear-to-ear max delay)
INHERITED(StdBlock) 
public: //
  MinMaxInt		chan; // input channel range, max=-1 is 'last' -- usually set max to ~1.6Khz channel (ILD is usually used with f>~1Khz)
  MinMaxInt		chan_eff; // #READ_ONLY #NO_SAVE actual channels that will be used

  
  SIMPLE_LINKS(ITDBlock);
  TA_BASEFUNS(ITDBlock) //

protected:
//  override void		UpdateAfterEdit_impl();
//  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  
//  override void		AcceptData_impl(SignalProcBlock* src_blk,
//    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
//  ProcStatus		AcceptData_ITD(float_Matrix* in_mat, int stage);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(ITDBlock)
};


class AUDIOPROC_API ANVal: public SignalProcItem
{ // ##CAT_Audioproc one value of an auditory nerve output
INHERITED(SignalProcItem)
friend class ANBlock; 
public:
  enum ANValType {
    AN_EXP,	// #LABEL_Exponential
    AN_SIG,	// #LABEL_Sigmoid #NO_SHOW TODO:finish (biologically realistic)
    AN_GAUSS,	// #LABEL_Gaussian
    AN_LIN,	// #LABEL_Linear linear is often best for already-compressed material like speech files
  };
  
  ANValType		val_type;
  Level::Units		units; // #NO_SAVE #SHOW #READ_ONLY units for cl and width
  float 		cl; // center level (in dB) of this channel
  float			width; // SIG: the ~90% (.05-.95) width in dB; GAUSS: ~1.6sds. ~90%; LIN:
  float			norm; // #EXPERT the normalization factor
  float			f; // #EXPERT the factor in the exponential term
  
  virtual float			CalcValue(float in); 
    // #IGNORE return result based on parameters
  void			SetParams(ANValType val_type,
    float cl, float width);
    
  override void SetDefaultName() {name = _nilString;}
  TA_BASEFUNS(ANVal)

protected:
  Level::Units		prev_units; 
  
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  void			UpdateParams();

private:
  SIMPLE_COPY(ANVal)
  void	Initialize();
  void	Destroy() {}
};

class AUDIOPROC_API ANVal_List: public taList<ANVal>
{ // ##CAT_Audioproc list of channels, that will operate in parallel
INHERITED(taList<ANVal>) 
public:

  TA_BASEFUNS_NOCOPY(ANVal_List)

private:
  void	Initialize() {SetBaseType(&TA_ANVal);}
  void	Destroy() {}
};

class AUDIOPROC_API ANBlock: public StdBlock
{ // ##CAT_Audioproc auditory nerve block -- intended for +ve signal values only (ex rectified)
INHERITED(StdBlock) 
public:
  Level		in_gain; // a gain factor applied to the input values -- eff gain should result in values from 0-1
  ANVal_List	val_list; // the individual channels
  
  override taList_impl*  children_() {return &val_list;} //note: required
  
  void			MakeVals(ANVal::ANValType val_type, int n_vals = 3,
    float cl_min = -84, float cl_max = -12);
    // #BUTTON make vals to cover level space, with each val of l_wid, adjacent bands overlapping by l_ovl, and the min lower band at l_min, and max upper band at l_max)
    
  virtual void		GraphFilter(DataTable* disp_data, float l_min = -96);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the block response (from l_min:0 dB) into data table and generate a graph
  
  ProcStatus 		AcceptData_AN(float_Matrix* in_mat, int stage = 0);
    // #IGNORE mostly for proc

  void	InitLinks();
  SIMPLE_CUTLINKS(ANBlock)
  TA_BASEFUNS(ANBlock)
  
protected:
//  override void		UpdateAfterEdit_impl();
  
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(ANBlock)
};


class AUDIOPROC_API NormBlock: public StdBlock
{ // ##CAT_Audioproc Norm Block -- normalizes a bank (chans/vals) of input values to uniformly fill the 0..1 activation range, with optional non-linear scaling, and threshold
INHERITED(StdBlock) 
public: //
  enum AGCFlags { // #BITS flags for the AGC block
    AF_0	= 0, // #IGNORE
    AGC_ON	= 0x002, // use Automatic Gain Control, else use current norm setting
    AGC_UPDATE_INIT = 0x004, // continuously update the initial value with current value
  };
  
  enum ScaleType {
    NONE,	// no scale factor
    POWER,	// scale_factor is the power (> 0) to which to raise input
    LOG10,	// log10(x) (with threshold)	
    LN,		// ln(x)	
  };
  
  DataBuffer		out_buff_norm; // #SHOW_TREE the normalization factor used
  
  AGCFlags		agc_flags; // flags to control features
  ScaleType		scale_type; //
  float			scale_factor; // #CONDEDIT_ON_scale_type:POWER the scale factor, if defined by the scale_type
  float			std_devs; // #MIN_.25 #MAX_3 #DEF_2 number of standard deviations in input to use for the out_range
  
  Level			in_thresh; // this is the threshold of the avg below which all data should be considered 0; also the ln and log10 thresholds
  
  float			out_center; // #DEF_0.5 center of output signal, usually 0.5 for Leabra networks
  float			out_range; // #DEF_1 range of output signal, typically 1 for Leabra networks
  
  float			init_norm_offset; // the initial norm offset to make the avg input be 0
  float			init_norm_factor; // the initial norm factor to make N std_devs of input fit range of 1 
  Duration		norm_tc; // #CONDEDIT_ON_agc_flags:AGC_ON time constant of integration of norm_xxx; 0 means update fully each item (note: we don't update when input falls below thresh)
  
  double		norm_offset; // #READ_ONLY #NO_SAVE #SHOW the norm offset that was most recently applied
  double		norm_factor; // #READ_ONLY #NO_SAVE #SHOW the norm factor that was most recently applied
  override int		outBuffCount() const {return 2;}
  override DataBuffer* 	outBuff(int idx) {if (idx == 1)  
    return &out_buff_norm; return inherited::outBuff(idx);}

  
  SIMPLE_LINKS(NormBlock);
  TA_BASEFUNS(NormBlock) //

public: //
  float			norm_dt; // #READ_ONLY #EXPERT time constant of integration of norm_xxx, per output sample time period;
  float_Matrix		scaled; // #NO_SHOW #NO_SAVE val, chan, field -- scaled
//  float_Array		data; // #NO_SHOW for topN sort
protected:
  float			in_thresh_lin_scaled; // linear and scaled
  int			n_bottom; // we set it min 1
  
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
  float			Scale(float val);
  float			Norm(float val);
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(NormBlock)
};


class AUDIOPROC_API HarmonicSieveBlock: public StdBlock
{ // ##CAT_Audioproc #AKA_HarmonicSieveBlock measure the degree of harmonically related values to a fundamental -- usually used after Temporal or Gammatone.Env filter output
INHERITED(StdBlock) 
public: //
  
  ChansPerOct	chans_per_oct; // channels per octave -- normally looked up from an upstream GammatoneBlock
  int		out_octs; // #MIN_1 number of output octaves (generally <= 1/2 total)
  int		on_half_width; // #MIN_0 how many bands away from octave center to include, typ 1/12 to 1/6 octave
  
  
  
  SIMPLE_LINKS(HarmonicSieveBlock);
  TA_BASEFUNS(HarmonicSieveBlock) //
  
public: // DO NOT USE
//  DoG1dFilterSpec	dog; // #EXPERT_TREE #NO_SAVE filter specs  
  float_Matrix		filter; // #EXPERT_TREE #NO_SAVE in order from fund+on_hw+1 to 2*fund+hw

protected:
  int			cpo_eff;
  int			in_octs; // how many input octaves there are
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
  virtual void		CheckMakeFilter(bool quiet, bool& ok);
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(HarmonicSieveBlock)
};



#endif
