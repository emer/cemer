#ifndef GAMMATONE_H
#define GAMMATONE_H
 
#include "audioproc.h"

#include "minmax.h"

 
class GammatoneChan;
class GammatoneBlock;//
//class TemporalDeltaBlock;
class ANVal;
class ANBlock;

class AUDIOPROC_API GammatoneChan: public SignalProcItem
{ // #NO_SHOW ##CAT_Audioproc one channel of a gammatone filter
INHERITED(SignalProcItem)
friend class GammatoneBlock; 
public:
  static bool		ChanFreqOk(float cf, float ear_q, float min_bw,
   float fs); // common routine for determining when a band is "safely" within nyquist limits
  bool	 		on; // #READ_ONLY #SHOW if channel is ok with current sample rate, otherwise ignored
  float 		cf; // #READ_ONLY #SHOW center frequency of this channel
  float			gain; // #READ_ONLY #SHOW the gain of this channel
  double		erb; // #READ_ONLY #SHOW the equivalent rectangular bandwidth of this channel
  
  static const KeyString key_on; // "on"
  static const KeyString key_cf; // "cf"
  static const KeyString key_erb; // "erb"
  static const KeyString key_gain; // "gain"
  override String 	GetColText(const KeyString& key, int itm_idx = -1) const;
  
  override void SetDefaultName() {name = _nilString;}
  override int	GetEnabled() const {return on ? 1 : 0;} // for items that support an enabled/disabled state; -1=n/a, 0=disabled, 1=enabled (note: (bool)-1 = true)
  TA_BASEFUNS(GammatoneChan)

protected:
  double tpt;
  double a1, a2, a3, a4, a5;
  double p0r, p1r, p2r, p3r, p4r, p0i, p1i, p2i, p3i, p4i;
  double coscf, sincf, cs, sn;
  
  void			InitChan(float cf, float ear_q, float min_bw,
     float fs);
  void 			DoFilter(int n, int in_stride, const float* x,
    int out_stride, float* bm, float* env = NULL, float* instf = NULL);

private:
  SIMPLE_COPY(GammatoneChan)
  void	Initialize();
  void	Destroy() {}
};

class AUDIOPROC_API GammatoneChan_List: public taList<GammatoneChan>
{ // ##CAT_Audioproc list of channels, that will operate in parallel
INHERITED(taList<GammatoneChan>) 
public:

  override int		NumListCols() const;
  // name, val_type (float, etc.), disp_opts
  override String	GetColHeading(const KeyString& key) const;
  // header text for the indicated column
  override const KeyString GetListColKey(int col) const;
  
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
    OV_FREQ	= 0x04, // #LABEL_InstFreq instantaneous frequency
    OV_SIG_ENV	= 0x03, // #NO_BIT signal + envelope
    OV_SIG_ENV_FREQ = 0x07 // #NO_BIT signal + envelope + instantaneous frequency
  };
  
  enum NonLinearity {
    NL_NONE,		// #LABEL_None no non-linearity
    NL_HALF_WAVE,	// #LABEL_HalfWave half-wave rectification
    NL_FULL_WAVE,	// #LABEL_FullWave full-wave rectification
    NL_SQUARE		// #LABEL_Square signal is squared
  };
  
  enum ChanSpacing { // how to space the channels
    CS_0		= 0, // #IGNORE
    CS_MooreGlassberg	= 0, // #LABEL_MooreGlassberg equal ERB crossings
    CS_LogLinear	= 1, // #LABEL_LogLinear simple log linear
  };
  
  DataBuffer		out_buff_env; //  #SHOW_TREE envelope output (if enabled)
  DataBuffer		out_buff_freq; //  #SHOW_TREE frequency output (if enabled)
  
  ChanSpacing		chan_spacing; // how to space the channels
  float			ear_q; //Moore and Glasberg ERB values
  float			min_bw;
  float			cf_lo; // lower center frequency (Hz)
  float			cf_hi; // upper center frequency (Hz)
  int			n_chans; // #MIN_1 number of filter bands
  OutVals		out_vals; // the desired output values
  NonLinearity		non_lin; // type of non-linearity to apply to signal output
  Level			auto_gain; // #READ_ONLY #SHOW #NO_SAVE an automatically applied gain adjustment based on the non-lin selected
  
  GammatoneChan_List	chans; // #NO_SAVE the individual channels
  
  override taList_impl*  children_() {return &chans;} //note: required
  override int		outBuffCount() const {return 3;}
  override DataBuffer* 	outBuff(int idx) {switch (idx) {
    case 0: return &out_buff; 
    case 1: return &out_buff_env;
    case 2: return &out_buff_freq;
    default: return NULL;}}
  
  virtual void		GraphFilter(DataTable* disp_data,
    bool log_freq = true, OutVals response = OV_SIG);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter response into a data table and generate a graph; if log_freq then log10 of freq is output; CHOOSE ONLY 1 RESPONSE
  void			MakeStdFilters(float cf_lo=65.4064f, float cf_hi=7902.13f,
   int n_chans=84); // #BUTTON #NO_UPDATE_AFTER make a "standard" filter bank, using linear spacing, with 12 chans/octave, centered on musical scale
  ProcStatus 		AcceptData_GT(float_Matrix* in_mat, int stage = 0);
    // #IGNORE mostly for proc

  SIMPLE_LINKS(GammatoneBlock)
  TA_BASEFUNS(GammatoneBlock)
  
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




/*class AUDIOPROC_API TemporalDeltaBlock: public InputBlock
{ // ##CAT_Audioproc provides an onset and offset output -- input is generally a Gammatone block (using inst) or a 
INHERITED(InputBlock) 
public: //
  DataBuffer_List	out_buffs; // #EXPERT_TREE #EXPERT 0=onset 1=offset

  override int		outBuffCount() const {return out_buffs.size;} // S/B 2!!!
  override DataBuffer* 	outBuff(int idx) {return out_buffs.SafeEl(idx);}

// windowing parameters:
  float			l_dur; // duration of lower (forward) window in ms, typ 24 ms
  float			u_dur; // duration of upper (backward) window in ms, typ 8 ms
  float			out_rate; // output rate, in ms; typ 4 ms (we set our fs by this)		
  
// hidden guys, for reference
  float_Matrix		filter; // #READ_ONLY #EXPERT_TREE #NO_SAVE filter 1d
  int 			l_flt_wd; // #READ_ONLY #HIDDEN #NO_SAVE l side of filter size
  int 			u_flt_wd; // #READ_ONLY #HIDDEN #NO_SAVE u side of filter size
  
  virtual void		GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
//  virtual void		GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view
  
  
  void	InitLinks();
  SIMPLE_CUTLINKS(TemporalDeltaBlock);
  TA_BASEFUNS(TemporalDeltaBlock) //
  
public: // DO NOT USE
  int_Matrix		conv_idx; // #READ_ONLY #EXPERT_TREE #NO_SAVE 1 item per out frame, tracks the index; pre-decrement

protected:

  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok); 
  override void		InitThisConfigDataOut_impl(bool check, bool quiet, bool& ok);
  
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
    
  virtual void		CheckMakeFilter(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(TemporalDeltaBlock)
};*/


class AUDIOPROC_API TemporalWindowBlock: public StdBlock
{ // ##CAT_Audioproc temporal windowing block
INHERITED(StdBlock) 
public: //
  enum FilterType {
    FT_DEF = 0, // #IGNORE
    FT_MG = 0, // #LABEL_Moore&Glasberg can do forward as well as back filtering (see Moore & Glasberg, 1987)
    FT_Exp = 1, // #HIDDEN TODO #LABEL_Exponential simple exponential (typical of many neural net and machine learning approaches)
    FT_DoG = 2, // #LABEL_DiffOfGaussian difference-of-gaussian -- enables on/off outputs
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
  Level			auto_gain; // #READ_ONLY #SHOW #NO_SAVE an automatically applied gain adjustment based on the non-lin selected
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
  int_Matrix		conv_idx; // #READ_ONLY #EXPERT_TREE #NO_SAVE 1 item per out frame, tracks the index; pre-decrement

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

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(TemporalWindowBlock)
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
  enum ANValType {
    AN_EXP,	// #LABEL_Exponential
    AN_SIG,	// #LABEL_Sigmoid (biologically realistic)
    AN_GAUSS	// #LABEL_Gaussian (recommended for Leabra)
  };
  
  Level		in_gain; // a gain factor applied to the input values -- eff gain should result in values from 0-1
  ANVal_List	val_list; // the individual channels
  
  override taList_impl*  children_() {return &val_list;} //note: required
  
  void			MakeVals(ANValType val_type = AN_SIG, int n_vals = 3,
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


class AUDIOPROC_API ANVal: public SignalProcItem
{ // ##CAT_Audioproc one value of an auditory nerve output
INHERITED(SignalProcItem)
friend class ANBlock; 
public:
  ANBlock::ANValType	val_type;
  float 		cl; // center level (in dB) of this channel
  float			width; // the ~90% (.05-.95) width in dB; GAUSS: ~1.6sds. ~90% 
  float			norm; // #EXPERT the normalization factor
  float			f; // #EXPERT the factor in the exponential term
  
  virtual float			CalcValue(float in); 
    // #IGNORE return result based on parameters
  void			SetParams(ANBlock::ANValType val_type,
    float cl, float width);
    
  override void SetDefaultName() {name = _nilString;}
  TA_BASEFUNS(ANVal)

protected:
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  void			UpdateParams();

private:
  SIMPLE_COPY(ANVal)
  void	Initialize();
  void	Destroy() {}
};


#endif
