#ifndef AUDIOPROC_H
#define AUDIOPROC_H

#include "ta_base.h"
#include "ta_matrix.h"
#include "ta_datatable.h"
#include "minmax.h"
#include "ta_geometry.h"

#include "audioproc_def.h"

class SignalProcItem;
class SignalProcBlock;
class SignalProcSet;
class InputBlockBase;
class InputBlock;
class InputBlockSet;
class ListenerBlock;
class OutputBlock;
class StdBlock;
class StimChan;
class ToneChan;
class StimGen;
class DoG1dFilterSpec; 
class LogLinearBlock; //

/*
  Input data to processing blocks is always a whole matrix:
   * a whole table col
   * a whole out_buff matrix from another block
   * a whole external matrix object
   
  The processing works on 1 frame at a time, in one of two modes:
   * continuous/accumulating: each step involves a new frame
   * single frame: only 1 frame at a time will ever be made/generated
  In practice, both modes are similar, since in single mode, the
   frame number will always be 0
*/

class AUDIOPROC_API SampleFreq: public taBase {
// ##NO_TOKENS #EDIT_INLINE ##CAT_Audioproc encapsulates a sampling rate
INHERITED(taBase) 
public:
  enum SampleFreqVal { // standard sampling freqs, plus "auto" to use prev channel
    SF_AUTO	= 0, // LABEL_Auto sets the freq according to the context
    SF_8000	= 8000,  // #LABEL_8,000 this is really only suitable for telephone-like voice
    SF_16000	= 16000, // #LABEL_16,000
    SF_22050	= 22050, // #LABEL_22,050 the default, often used for speech libraries
    SF_44100	= 44100, // #LABEL_44,100 CD and similar
    SF_48000	= 48000, // #LABEL_48,000 DAT and other professional standards
    SF_CUSTOM	= -1 // #LABEL_Custom set your own freq
  };
  
  SampleFreqVal	fs_val; // choose the rate to use, or pick Custom to set manually
  float		fs_act; // #MIN_0 #CONDEDIT_ON_fs_val:SF_CUSTOM the custom or actual rate
  
  void		Set(SampleFreqVal val, float = -1.0f); // set, act not needed for standard
  void		SetCustom(float act) // set a custom value
    {Set(SF_CUSTOM, act);}
  
  operator float() const {return fs_act;} // #IGNORE simplifies equations
  
  TA_BASEFUNS_LITE(SampleFreq)
protected:
  void	UpdateAfterEdit_impl();
  
private:
  void	Initialize() {fs_val = SF_AUTO; fs_act = 0;}
  void	Destroy() {}
  SIMPLE_COPY(SampleFreq)
};


class AUDIOPROC_API Level: public taBase {
// ##NO_TOKENS #EDIT_INLINE ##CAT_Audioproc encapsulates a gain/attenuation value
INHERITED(taBase) 
public:
  enum Units { // units of the level
    UN_PERCENT, // #LABEL_% scale by the given value, in percent
    UN_SCALE,  // #LABEL_* scale by the given value
    UN_DBI,   // #LABEL_dB(I) scale by the given dB (Intensity: 10*log10)
    UN_DBP   // #LABEL_dB(P) scale by the given dB (Power: 20*log10)
  };
  
  float		level; // the value to use, in the given units
  Units		units; // the units to use for the level value
  float		act_level; // #READ_ONLY #NO_SAVE
  
  void		Set(float val, Units units);

  operator float() const {return (float) act_level;} // #IGNORE simplifies equation
  
  TA_BASEFUNS_LITE(Level)
protected:
  void	UpdateAfterEdit_impl();
  
private:
  void	Initialize() {level = 1.0f; units = UN_SCALE; act_level = level;}
  void	Destroy() {}
  SIMPLE_COPY(Level)
};


class AUDIOPROC_API Phase: public taBase {
// ##NO_TOKENS #EDIT_INLINE ##CAT_Audioproc encapsulates an angles
INHERITED(taBase) 
public:
  enum Units { // units of the phase/angles
    UN_DEGREES, // #LABEL_deg degrees
    UN_RADIANS  // #LABEL_rad radians
  };
  
  float		phase; // the value to use, in the given units
  Units		units; // the units to use for the phase value
  float		act_phase; // #READ_ONLY #NO_SAVE actual phase, in radians

  operator float() const {return (float) act_phase;} // #IGNORE simplifies equation
  
  TA_BASEFUNS_LITE(Phase) //
protected:
  void	UpdateAfterEdit_impl(); //
  
private:
  void	Initialize() {phase = 0.0f; units = UN_DEGREES; act_phase = phase;}
  void	Destroy() {} //
  SIMPLE_COPY(Phase)
};


class AUDIOPROC_API Duration: public taBase {
// ##NO_TOKENS #EDIT_INLINE ##CAT_Audioproc provides a way to specify durations in samples or time
INHERITED(taBase) 
public:
  enum Units { // units of the level
    UN_CONT, // #LABEL_cont continuous
    UN_TIME_S, // #LABEL_time(s) time, in seconds
    UN_TIME_MS,  // #LABEL_time(ms) time, in milliseconds
    UN_SAMPLES  // #LABEL_samples in number of samples, regardless of time
  };
  
  static double	StatGetDurationTime(double duration, Units units, float fs); 
   // get the duration as an amount of time, < 0 if continuous
  static double	StatGetDurationSamples(double duration, Units units, float fs); 
   // get the duration as a number of samples, < 0 if continuous
  static int	StatCompare(const Duration& a, const Duration& b, float fs);
   // returns -ve if a<b, 0 if a==b; +ve if a > b
   
  double	duration; // #MIN_0 #CONDEDIT_OFF_units:UN_CONT the value to use, in the given units
  Units		units; // #APPLY_IMMED the units to use for the duration value
   
  double	GetDurationTime(float fs) const 
    {return StatGetDurationTime(duration, units, fs);}
   // get the duration as an amount of time, = 0 if continuous
  double	GetDurationSamples(float fs) const 
    {return StatGetDurationSamples(duration, units, fs);}
   // get the duration as a number of samples, = 0 if continuous (note: could be fractional)
  void		Set(double dur, Units uns) {duration = dur; units = uns;}
  void		AddTime(double dur, float fs);
   // add the duration in s
  void		AddSamples(double samp, float fs);
   // add the duration in samples
  
  TA_BASEFUNS_LITE(Duration)
    
private:
  void	Initialize();
  void	Destroy() {}
  SIMPLE_COPY(Duration)
};


class AUDIOPROC_API DataBuffer: public taOBase {
// ##NO_TOKENS #NO_UPDATE_AFTER #EDIT_INLINE ##CAT_Audioproc provide a matrix for data management
INHERITED(taOBase) 
public://
/*#ifndef __MAKETA__
  static const short	field_mono = 0;
  static const short	field_left = 0;
  static const short	field_right = 1;
#endif*/

  SampleFreq		fs; // the sample frequency of output of this buffer
  Duration		fr_dur; // the length of each frame, typically in terms of the fs;( 0 to disable the output)
  int			min_stages; // #READ_ONLY #NO_SAVE #SHOW some blocks require a min number of stages -- you may set more, but not fewer
  int			stages; // #CONDEDIT_OFF_stages_ro #DEF_1 #MIN_1 (advanced) for when you want more than one frame of history
  int			stage; // #READ_ONLY #NO_SAVE the next stage to be written
  int			items; // #READ_ONLY #NO_SAVE #SHOW the number of items per frame (determined by fs and fr_dur)
  int			item; // #READ_ONLY #NO_SAVE the next item to be written
  short			fields; // #CONDEDIT_OFF_fields_ro #DEF_1 #MIN_1 1 for mono, 2 for stereo
  short			field; // #READ_ONLY #NO_SAVE the field being written -- optional, not used in NextIndex
  short			chans; // #READ_ONLY #NO_SAVE #SHOW the number of channels
  short			vals; // #READ_ONLY #NO_SAVE #SHOW the number of values per channel, usually 1
  int64_t		item_cnt; // #READ_ONLY #NO_SAVE #SHOW number of items that have been written since last Init (can be used for measuring duration)	
  float_Matrix		mat; // #NO_SAVE #EXPERT #EXPERT_TREE the data of the buffer
  bool			enabled; // #READ_ONLY #NO_SAVE #SHOW whether this buffer is being used 
  bool			fields_ro; //#NO_SHOW #NO_SAVE some guys require fixed number, so make it ro

  int			prevStage() const; // returns -offs stage index, which is (usually) one less than the current stage, since we bump the stage before calling
  virtual void		InitConfig(bool check, bool quiet, bool& ok);
    // #IGNORE configure data; we assume fs, geom, fr_dur, etc. are all valid/set


  int			GetChans() const; // gets the # channels, which is usually the 2nd last mat dimension;
  int			GetChanDim() const; // gets the dimension used for channels
  bool			GetBaseGeomOfStageRel(int rel_stage, MatrixGeom& geom);
    // get the root coordinates of the requested stage, relative to CURRENT (not prevStage, which is what you may usually want!), 0=current, -ve is earlier, +ve is later; rel_stage is NOT wrapped if it overflows! (an error is issued)
  bool			GetBaseGeomOfStageAbs(int abs_stage, MatrixGeom& geom);
    // get the root coordinates of the requested absolute stage
  int			GetRelStage(int stage, int offs) const; // the stage relative to the offs (really, just does + modulo the stages)
  bool			NextIndex(); // advances item/stage index, returning true if a stage was completed (thus need to call NotifyClientsBuffStageFull)
  SignalProcBlock*	GetOwnerBlock() {return GET_MY_OWNER(SignalProcBlock);}
  
  SIMPLE_LINKS(DataBuffer);
  TA_BASEFUNS_LITE(DataBuffer)
protected:
  override void		UpdateAfterEdit_impl();
  override void 	CheckThisConfig_impl(bool quiet, bool& ok);
  
  void 			NextStage(); // advance the stage index -- shouldn't really call this from client blocks, because it is called by NextIndex, and we shouldn't skip that!
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(DataBuffer)
}; //

class AUDIOPROC_API DataBuffer_List: public taList<DataBuffer>
{ //  ##CAT_Audioproc list of buffers, for multi in/out, usually managed entirely by the block
INHERITED(taList<DataBuffer>) 
public:
  TA_BASEFUNS_NOCOPY(DataBuffer_List)

private:
  void	Initialize() {SetBaseType(&TA_DataBuffer);}
  void	Destroy() {}
};



// forward
TA_SMART_PTRS(SignalProcBlock);

class AUDIOPROC_API SignalProcItem: public taNBase
{ // #VIRT_BASE ##CAT_Audioproc common stuff for signal processing elements
INHERITED(taNBase) 
public: //
  enum LR_OFFS	{ // L/R field offsets -- mostly for code readability
  OFFS_L	= 0,
  OFFS_MONO	= 0,
  OFFS_R	= 1
  };
  
  int		index; // #READ_ONLY #NO_SAVE index of this guy in an owning list, if applicable, otherwise -1

  virtual bool		off() const {return false;}
  
  void		InitConfig_Int(bool check, bool quiet, bool& ok);
    // #IGNORE what parent or api functions call, NOT virtual! (override _impl)
  
  override int	GetIndex() const {return index;}
  override void	SetIndex(int value) {index = value;};
  TA_BASEFUNS_NOCOPY(SignalProcItem) //

public: // do not call
  
protected:
  override bool		CheckConfig_impl(bool quiet)
   {bool ok = true; if (!off()) InitConfig_impl(true, quiet, ok); return ok;}
  virtual void		InitConfig_impl(bool check, bool quiet, bool& ok);  // #IGNORE
  virtual void		InitThisConfig_impl(bool check, bool quiet, bool& ok) {}
  virtual void		InitThisConfigDataIn_impl(bool check, bool quiet, bool& ok) {}
  virtual void		InitChildConfig_impl(bool check, bool quiet, bool& ok);
    // we handle the default children_ collection, if defined (does not require DEF_CHILD directive)
  virtual void 		InitChildItemConfig_impl(SignalProcItem* itm, 
    bool check, bool quiet, bool& ok); // default just calls itm->InitConfig_int 
  virtual void		InitThisConfigDataOut_impl(bool check, bool quiet, bool& ok) {}
  
private:
  void	Initialize() {index = -1;}
  void	Destroy() {}
};


class AUDIOPROC_API SourceBlockSpec: public taOBase {
// ##NO_TOKENS #NO_UPDATE_AFTER #EDIT_INLINE ##CAT_Audioproc specify an input for a block
INHERITED(taOBase) 
public:
  SignalProcBlockRef	src_block; // the block that feeds this one
  int			buff_index; // #DEF_0 the buffer index in the source block
  
  DataBuffer*		GetBuffer() const; // gets the buffer of the src
  
  SIMPLE_INITLINKS(SourceBlockSpec);
  void	CutLinks();
  TA_BASEFUNS_LITE(SourceBlockSpec)
protected:
//  override void		UpdateAfterEdit_impl();
  override void		SmartRef_DataRefChanging(taSmartRef* ref, 
    taBase* obj, bool setting);
  
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(SourceBlockSpec)
}; //


class AUDIOPROC_API SourceBlockSpec_List: public taList<SourceBlockSpec>
{ //  ##CAT_Audioproc list of source blocks
INHERITED(taList<SourceBlockSpec>) 
public:
  TA_BASEFUNS_NOCOPY(SourceBlockSpec_List)

private:
  void	Initialize() {SetBaseType(&TA_SourceBlockSpec);}
  void	Destroy() {}
};


class AUDIOPROC_API SignalProcBlock: public SignalProcItem, public virtual IRefListClient
{ // #VIRT_BASE ##CAT_Audioproc common routines to signal processing blocks: in and/or out; we include the out data, since almost all blocks make output data, but it can be hidden
INHERITED(SignalProcItem) 
friend class SourceBlockSpec;
public:
  enum BlockFlags { // #CAT_Audioproc #BITS
    BF_0	= 0, // #IGNORE
    BF_OFF	= 0x0002, // #LABEL_Off don't call this item
    BF_DISP_OFF	= 0x0008, // #LABEL_DispOff for display blocks, do not display
    BF_ACCUM	= 0x0020 // #LABEL_Accum output accumulates, otherwise, a single frame is reused each time
  };
  
  enum ProcStatus { // #CAT_Audioproc result of a processing operation
    PS_ERROR	= 0,	// an error occurred during processing
    PS_OK, // processing completed ok, can continue 
    PS_STOP	// processing completed ok, someone asked to stop (ex no more data)
  };
  
  enum OutputMode {  // #CAT_Audioproc stereo/mono for blocks that support this
    OM_Mono,	// #LABEL_Mono outputs a single field only
    OM_Stereo    // #LABEL_Stereo outputs a L and R field
  };
   
  enum Dims { // in buffers, this is the order of storage of indicated element  
    VAL_DIM = 0,
    CHAN_DIM = 1,
    FIELD_DIM = 2,
    ITEM_DIM = 3,
    STAGE_DIM = 4,
    DIM_COUNT // the number of dims
  };
  
  String		desc; // (optional) description of the block
  BlockFlags		flags; // misc control flags
  int			num_clients; // #READ_ONLY #NO_SAVE #SHOW number of client blocks to this one
  
  override bool		off() const {return (flags & BF_OFF);} // off property
  
  //note: in_buffs are not used much, only when the dude needs a buffer
  virtual int		inBuffCount() const {return 0;}
  virtual DataBuffer* 	inBuff(int idx) {return NULL;}
  
  virtual int		outBuffCount() const {return 0;}
  virtual DataBuffer* 	outBuff(int idx) {return NULL;}
  
  virtual int		srcBlockCount() const {return 0;}
  virtual SourceBlockSpec* srcBlock(int) {return NULL;}
    
    
  virtual bool		InitConfig(); //  Initialize the block, return true if ok
  void			InitConfig_Gui(); // #LABEL_InitConfig #MENU #MENU_ON_SigProc #MENU_CONTEXT initialize the block, and all sub-items
  
//  virtual SignalProcSet* GetParentSet() const; // gets the parent set, if any
  double		GetElapsedTime() // get elapsed time in s since input began
    {return GetElapsedTime_impl();}
  
  virtual int 		inputBlockCount() const {return 0;} 
  virtual InputBlockBase* GetInputBlock(int idx = 0); // gets the input block, if found
    
  virtual SignalProcBlock* GetUpstreamBlock(TypeDef* typ); // #TYPE_0_SignalProcBlock #NO_NULL_0 find the first upstream block of the indicated type, returns NULL if not found
  
  int	GetEnabled() const {return (flags & BF_OFF) ? 0 : 1;}
  
  override String GetDesc() const { return desc; }
  SIMPLE_LINKS(SignalProcBlock);
  TA_BASEFUNS(SignalProcBlock) //

public: // IRefListClient
  void*		This() {return this;}
  void		DataDestroying_Ref(taBase_RefList* src, taBase* ta);
    // note: item will already have been removed from list
  void		DataChanged_Ref(taBase_RefList* src, taBase* ta,
    int dcr, void* op1, void* op2);

protected:
  taBase_RefList	clients; // clients, list is managed by the smart ref in client
  ContextFlag		in_init_config; // protect from possible loops
  
  virtual double	GetElapsedTime_impl(); // tries to find the input block
  
  override void		InitConfig_impl(bool check, bool quiet, bool& ok);
  override void		InitThisConfigDataIn_impl(bool check, bool quiet, bool& ok);
  override void		InitThisConfigDataOut_impl(bool check, bool quiet, bool& ok);
  virtual void		InitClientsConfig_impl(bool check, bool quiet, bool& ok);
  
  void			AddRemoveClientBlock(SignalProcBlock* blk, 
    bool adding); 
    // #IGNORE add (if not added) or remove (if not removed) the indicated client block
  void			DataReady(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps); 
    // called by an upstream provider when a stage is full
  virtual void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps){} 
    // only called when we are absolutely sure we want the data -- this is the one to impl; if buff=NULL then stage=-1
  void			NotifyClientsBuffStageFull(DataBuffer* buf,
    int index, ProcStatus& ps);
    // notify clients of this buffer that it has been filled -- calls their DataReady
    
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(SignalProcBlock)
};

// you can use these macros (or copy it exactly) in AcceptData_impl
// to set up the standard loop nesting

// use this one to iterate vals in the inner loop
#define ACCEPT_DATA_LOOP_VAL(ps,in_mat,i,f,chan,val) \
  for (int i = 0; ((ps == PS_OK) && (i < in_mat->dim(SignalProcBlock::ITEM_DIM))); ++i) \
  for (int f = 0; ((ps == PS_OK) && (f < in_mat->dim(SignalProcBlock::FIELD_DIM))); ++f) \
  for (int chan = 0; ((ps == PS_OK) && (chan < in_mat->dim(SignalProcBlock::CHAN_DIM))); ++chan) \
  for (int val = 0; ((ps == PS_OK) && (val < in_mat->dim(SignalProcBlock::VAL_DIM))); ++val)

// use this one to not iterate vals (loop handles them inline, or doesn't use them)
#define ACCEPT_DATA_LOOP_NOVAL(ps,in_mat,i,f,chan) \
  for (int i = 0; ((ps == PS_OK) && (i < in_mat->dim(SignalProcBlock::ITEM_DIM))); ++i) \
  for (int f = 0; ((ps == PS_OK) && (f < in_mat->dim(SignalProcBlock::FIELD_DIM))); ++f) \
  for (int chan = 0; ((ps == PS_OK) && (chan < in_mat->dim(SignalProcBlock::CHAN_DIM))); ++chan)


class AUDIOPROC_API SignalProcBlock_List: public taList<SignalProcBlock>
{ //  ##CAT_Audioproc list of blocks, typically that will operate in sequence
INHERITED(taList<SignalProcBlock>) 
public:

  TA_BASEFUNS_NOCOPY(SignalProcBlock_List)

private:
  void	Initialize() {SetBaseType(&TA_SignalProcBlock);}
  void	Destroy() {}
};


class AUDIOPROC_API InputBlockBase: public SignalProcBlock
{ // #VIRT_BASE ##CAT_Audioproc a block that serves as a source of input -- can be either a single item, or a collection of items
friend class InputBlockSet;
INHERITED(SignalProcBlock) 
public:
  bool			use_fs; //  #NO_SHOW #NO_SAVE intrinsic: whether to use fs; ex. wav files already determine this
  Level			gain; // #DEF_1 the relative gain of the output, often 1
  SampleFreq		fs; // #CONDEDIT_ON_use_fs the sample frequency of output of this block
  Duration		max_dur; // duration or maximum duration of the input
  double		elapsed; // #READ_ONLY #NO_SAVE #SHOW the amount of time elapsed in s
  //NOTE: do NOT reference this value in processing loops: it is only updated each frame; use GetElapsedTime()
  double		samples; // #READ_ONLY #NO_SAVE #SHOW the amount of time elapsed in samples
  
  override int 		inputBlockCount() const {return 1;} 
  override InputBlockBase* GetInputBlock(int idx = 0) {return this;}
  virtual const FloatTDCoord&	GetPos() const {return no_pos;}
  
  virtual ProcStatus	ProcNext_Samples(int n); 
    // for input blocks, do the next n samples
  virtual ProcStatus	ProcNext_Duration(float s);
   // for input blocks, do the next s seconds (converted to n samples at current fs)
  
  SIMPLE_LINKS(InputBlockBase);
  TA_BASEFUNS(InputBlockBase)

protected:
  static FloatTDCoord	no_pos; // default (0)
  
  double		max_dur_samp; // so we don't compute it every time
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override double	GetElapsedTime_impl();
  virtual void		ProcNext_Samples_impl(int n, ProcStatus& ps)
    {ps = PS_ERROR;}
    // #IGNORE for input blocks, do n samples of the processing -- use i to iterate n

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(InputBlockBase) //
};//


class AUDIOPROC_API InputBlock: public InputBlockBase
{ // #VIRT_BASE ##CAT_Audioproc a block that serves as a source of input
INHERITED(InputBlockBase) 
public:
  DataBuffer		out_buff; //  #SHOW_TREE default output buffer 
  FloatTDCoord		pos; // #AUDIO3D for localization processing, abs position in 3D space (relative to an arbitrary reference); default unit is m
  
  override int		outBuffCount() const {return 1;}
  override DataBuffer* 	outBuff(int idx) 
    {if (idx == 0) return &out_buff; else return NULL;}
  
  override const FloatTDCoord&	GetPos() const {return pos;}
  
  SIMPLE_LINKS(InputBlock);
  TA_BASEFUNS(InputBlock)

protected:

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(InputBlock) //
};//

// use this macro (or copy it exactly) in ProcNext_Samples_impl
// so samples is updated each time through the loop
#define PROC_NEXT_LOOP(ps,i) \
  for (int i = 0; ((ps == PS_OK) && (i < n)); ++i, samples += 1)


class AUDIOPROC_API InputBlockSet: public InputBlockBase
{ // #DEF_CHILD_blocks encapsulates a set of blocks that are processed in parallel, typically as a set of sound sources that will be fed into a mixer/listener
INHERITED(InputBlockBase) 
public:
  SignalProcBlock_List	blocks; // the blocks that make up the set -- they should be input blocks
  int			stages; // #MIN_1 (advanced) for when you want more than one frame of history (sets into all the blocks)
  
  override taList_impl*  children_() {return &blocks;} 
  override int		outBuffCount() const {return blocks.size;} // 1 per block only
  override DataBuffer* 	outBuff(int idx);
  override int 		inputBlockCount() const {return blocks.size;} 
  override InputBlockBase* GetInputBlock(int idx = 0) 
    {return dynamic_cast<InputBlockBase*>( blocks.SafeEl(idx));}
  
  SIMPLE_LINKS(InputBlockSet)
  TA_BASEFUNS(InputBlockSet)
  
public:

protected:
  override void		UpdateAfterEdit_impl();
  override void 	InitChildItemConfig_impl(SignalProcItem* itm, 
    bool check, bool quiet, bool& ok); 
  override void		ProcNext_Samples_impl(int n, ProcStatus& ps);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(InputBlockSet)
};


class AUDIOPROC_API OutputBlock: public SignalProcBlock
{ // #VIRT_BASE #NO_TOKENS ##CAT_Audioproc a block that just provides output, but no data
INHERITED(SignalProcBlock) 
public:
  SourceBlockSpec	in_block;  // 1st (or only) input block to this block

  override int		srcBlockCount() const {return 1;}
  override SourceBlockSpec* srcBlock(int) {return &in_block;}
  
  SIMPLE_LINKS(OutputBlock);
  TA_BASEFUNS(OutputBlock) //

protected:
  
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(OutputBlock)
};


class AUDIOPROC_API StdBlock: public SignalProcBlock
{ // #VIRT_BASE #NO_TOKENS ##CAT_Audioproc common routines for std processing blocks
INHERITED(SignalProcBlock) 
public:
  SourceBlockSpec	in_block;  // 1st (or only) input block to this block
  DataBuffer		out_buff; // #SHOW_TREE 

  override int		outBuffCount() const {return 1;}
  override DataBuffer* 	outBuff(int idx) 
    {if (idx == 0) return &out_buff; else return NULL;}
  override int		srcBlockCount() const {return 1;}
  override SourceBlockSpec* srcBlock(int) {return &in_block;}
  
  SIMPLE_LINKS(StdBlock);
  TA_BASEFUNS(StdBlock) //

protected:
  
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(StdBlock)
}; //


class AUDIOPROC_API ListenerBlock: public StdBlock
{ // a block that mixes many inputs located in space, and outputs a composite 3D sound signal -- all sources must use the same fs -- an InputBlockSet is usually the input
INHERITED(StdBlock) 
public:
  static int		CalcMinInputStages(float fs, float head_radius = 0.09f, float speed_sound = 344.0f);
    // convenience function, calculates min needed input stages
  float			head_radius; // #DEF_0.09 #AUDIO3D radius of head (1/2 separation between ears)
  FloatTDCoord		pos; // #AUDIO3D abs position of center of head in 3D space (relative to an arbitrary reference); default units is m
  FloatTDCoord		pos_ear_L; // #AUDIO3D pos of L ear -- NOT USUALLY SET MANUALLY
  FloatTDCoord		pos_ear_R; // #AUDIO3D pos of R ear -- NOT USUALLY SET MANUALLY
  FloatTDCoord*		pos_ear() {return &pos_ear_L;} // #IGNORE to access as [2]
  bool			norm_by_n; // normalize by the number of sources, else just add them
  OutputMode		output_mode; // #DEF_OM_Stereo whether to use stereo (usual) or mono (L and R are averaged)
  float			speed_sound; // #AUDIO3D #DEF_344 #EXPERT speed of sound, in units of m/s
  float			dist_min; // #AUDIO3D #DEF_0.03 #EXPERT minimum distance of sound sources (m), to avoid /0 in dist_atten calculation
  float			dist_atten; // #AUDIO3D #DEF_16 #EXPERT attenuation factor in dist calc: level_eff = level_src/(dist_atten * dist^2) -- for default, unity gain = 25cm distance; maximally close (dist_min=3cm) sounds have gain~70 or 36db

  void			SetPos(const FloatTDCoord& head_pos, const Phase& azim); // #CAT_Audioproc #MENU #MENU_ON_Listener set the head/ear positions according to the parameters; azimuth +ve = counterclockwise
  virtual void		UpdateAfterEdit_Audio3D(); 
  // must call if any AUDIO3D params of a Source or this Listener are changed while running

  SIMPLE_LINKS(ListenerBlock);
  TA_BASEFUNS(ListenerBlock) //
  
public:
  enum ParamIndex { // #IGNORE
    PI_DIST_L, // distance (m) to L
    PI_DIST_R, // distance(m) to R
    PI_ATTEN_L, // atten (fractional gain) for L, based on dist and head shadow
    PI_ATTEN_R, // atten (fractional gain) for R, based on dist and head shadow
    PI_DELAY_L, // delay of L field, in samples (0 <= d < stages); based on phase
    PI_DELAY_R, // delay of R field, in samples (0 <= d < stages); based on phase
    PI_AZIM,   // azimuth to source (radians); used for head shadow portion of atten
    
    PI_COUNT // #IGNORE number of items
  };
  float_Matrix		params; // #NO_SAVE #HIDDEN [param#, N] we cache items from ParamIndex (in order)
  float			dyn_gain; // #NO_SAVE #HIDDEN dynamic overall gain (ex. norm_by_n, etc.)
protected:
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void 	AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
  inline void		AcceptData_OneInput(InputBlockBase* blk, 
    DataBuffer* buff, int idx, double* res_buff);
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(ListenerBlock)
};


/* oh boy...
class AUDIOPROC_API MonDimSpec: public taBase {
// ##NO_TOKENS #EDIT_INLINE spec for the value in single output dim
INHERITED(taBase) 
public:
  enum SrcDim {
    DIM_0		= 0, #IGNORE
    DIM_VAL,		// #LABEL_Val the dim value of a val 
    DIM_CHAN,		// #LABEL_Chan the dim value of a chan
    DIM_FIELD,		// #LABEL_Field the dim value of a field (l/r) -- implies row iteration by field
    DIM_ITEM,		// #LABEL_Item the dim value of an item
    DIM_STAGE,		// #LABEL_Stage the dim value of a stage
  };
  
  Dim			dim; // the source data copied in this dimension
  
  TA_BASEFUNS_LITE(MonDimSpec)
protected:
  void	UpdateAfterEdit_impl();
  
private:
  void	Initialize();
  void	Destroy() {}
  SIMPLE_COPY(MonBlockColSpec)
};

class AUDIOPROC_API MonBlockItemSpec: public taBase {
// ##NO_TOKENS #EDIT_INLINE spec for a certain item type
INHERITED(taBase) 
public:
  enum MonType {
    MT_NONE,		// #LABEL_None don't monitor this value
    MT_COL_VAL,		// #LABEL_ColVal the value goes into the col, ex chan#
    MT_COL_DIM,		// #LABEL_ColDim the dim value goes into the col, ex chan#
    MT_COL_XTAB	// #LABEL_Col_XTab makes cross-tab cols for each dim value
  };
  
  MonType		mon_type; // the type of value monitored in this column
  Duration::Units	dur_units; 
  String		col_name; // the name, or base name, for the col
  DataCol::ValType 	val_type; // type of the value (usually float)
  
  TA_BASEFUNS_LITE(MonBlockItemSpec)
protected:
  void	UpdateAfterEdit_impl();
  
private:
  void	Initialize();
  void	Destroy() {}
  SIMPLE_COPY(MonBlockItemSpec)
};
*/

/* taxonomy of monitoring:

  MT_VAL -- this makes one record per value, esp. for graphing etc.
  
  MT_ITEM -- this makes one record per item, putting the item in a cell
  
  
  Cols:
    timestamp -- in any of the duration units
    chan -- dim value of the chan
    val -- dim value of the val
    "x" -- some arbitrary x, usually a calc on a dim value, item number, etc.
    dat -- value data, either scalar, or mat (item etc.);
      note: either as a single col, or xtabbed, typ by val
  
  Additional Data
    Timestamp -- the duration since last start, in designated unit;
      this is available for any type of monitoring

*/
//TODO: update SMB for fields (mono/stereo)
class AUDIOPROC_API SignalMonBlock: public OutputBlock
{ // ##CAT_Audioproc a block for monitoring to a datatable, often for graphing
INHERITED(OutputBlock) 
public:
  enum MonFlags { // #BITS
    MF_0 = 0, // #IGNORE
    MF_ACCUM		= 0x001, // #LABEL_Accum accumulate, otherwise reset after each frame
    MF_RESET_ON_INIT	= 0x002, // #LABEL_ResetOnConfig reset data on each init config, otherwise not (program must manage data)
    MF_FOR_PLOT		= 0x004, // #LABEL_ForPlot set user data for plotting
    MF_USE_TRIAL	= 0x008, // #LABEL_UseTrial use a trial column for multiple runs (prog must set it manually)
    MF_SPLIT_FIELDS	= 0x010 // #LABEL_SplitFields use a separate col for L and R when monitoring stereo -- appends _L and _R to col name
  };
  
  enum MonType { // the type of the monitor
    MT_VAL	= 0,  // #LABEL_ScalarVal individual vals are monitored, one val per row
    MT_ITEM	= 1,  // #LABEL_MatrixCell entire stage or stages are monitored, in a matrix cell 
  };
  
  enum SrcDim { // what is the source for a given target dim
    DIM_OFF		= 0x0F, // #LABEL_Off do not use this destination dim
    DIM_VAL		= 0x00, // #LABEL_Val the dim value of a val 
    DIM_CHAN		= 0x01, // #LABEL_Chan the dim value of a chan
    DIM_FIELD		= 0x02, // #LABEL_Field the dim value of a field (l/r) --NOTE: often you will want to use the SplitField option instead
    DIM_ITEM		= 0x03, // #LABEL_Item the dim value of an item
    DIM_STAGE_DESC	= 0x14, // #LABEL_Stage_DESC the dim value of a stage IN DESCENDING ORDER FROM MOST RECENT
    DIM_DUMMY		= 0x0E, // #LABEL_Dummy placeholder for unused dim (dim_size=1, dim_value=0)
    DIM_DIR_MASK	= 0x10 // #IGNORE
  };

  MonFlags		mon_flags;
  MonType		mon_type;

  DataTableRef		mon_data; // the table used to monitor -- cols are made automatically
  Duration::Units	x_units; // units for x (don't choose CONT)
  int			trial; // #NO_SAVE optional trial number for when MF_USE_TRIAL is set
  String		item_col; // #CONDEDIT_ON_mon_type:MT_ITEM name of the item col, def='item'; if stereo and SplitField then cols will get _L/_R appended
  int			item_dims; // #SHOW #READ_ONLY number of dimensions in dest item cell, usually 2 or 4 -- set by choosing items below
  SrcDim		item_dim0; // #CONDEDIT_ON_mon_type:MT_ITEM source for dest dim0
  SrcDim		item_dim1; // #CONDEDIT_ON_mon_type:MT_ITEM source for dest dim1
  SrcDim		item_dim2; // #CONDEDIT_ON_mon_type:MT_ITEM source for dest dim2
  SrcDim		item_dim3; // #CONDEDIT_ON_mon_type:MT_ITEM source for dest dim3
  SrcDim		item_dim4; // #CONDEDIT_ON_mon_type:MT_ITEM source for dest dim3
  SrcDim		item_dim5; // #CONDEDIT_ON_mon_type:MT_ITEM source for dest dim3
  
  SrcDim		itemDim_(int idx) const; // enables raw prog access to the guys above
  int			itemDim(int idx) const {return itemDim_(idx) & ~DIM_DIR_MASK;}
    // dimension number, ASC/DESC stripped off
  bool			itemIsDesc(int idx) const {return (itemDim_(idx) & DIM_DIR_MASK);}
    // whether dimension is Descending, else Ascending
  void			GetItemDimDesc(int idx, int& dim, bool& desc)
    {SrcDim sd = itemDim_(idx); dim = sd & ~DIM_DIR_MASK; desc = (sd & DIM_DIR_MASK);} 
  
  inline bool		isForPlot() const {return (mon_flags & MF_FOR_PLOT);}
  
  SIMPLE_LINKS(SignalMonBlock);
  TA_BASEFUNS(SignalMonBlock)
  
protected:
  int64_t		last_x; //#IGNORE last x value -- we use that for each mon, then update it from buff->item_cnt, which will thus be the next first x index value
  
  override void		UpdateAfterEdit_impl();
  double    		GetX(const SampleFreq& fs); 
  override void		InitThisConfigDataOut_impl(bool check, 
    bool quiet, bool& ok);
    
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);
    
  void			Init_Common(bool check, bool quiet, bool& ok);
  
  void			Init_MT_VAL(bool check, bool quiet, bool& ok);
  void 			AcceptData_MT_VAL(DataBuffer* src_buff,
    float_Matrix* mat, int stage, ProcStatus& ps);
  
  void			Init_MT_ITEM(bool check, bool quiet, bool& ok);
  void 			AcceptData_MT_ITEM(DataBuffer* src_buff,
    float_Matrix* mat, int stage, ProcStatus& ps);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(SignalMonBlock)
};


class AUDIOPROC_API SignalProcSet: public SignalProcBlock
{ // #VIRT_BASE a set of processing blocks, usually configured to work sequentially, or else to encapsulate a parallel set
INHERITED(SignalProcBlock) 
public:
  SignalProcBlock_List	blocks; // the blocks that make up the set
  
  override taList_impl*  children_() {return &blocks;} 
    
  SIMPLE_LINKS(SignalProcSet)
  TA_BASEFUNS(SignalProcSet)
  
public:

protected:

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(SignalProcSet)
};


class AUDIOPROC_API SequentialProcSet: public SignalProcSet
{ // encapsulates a set of blocks to make them appear as one block -- first block is the input (if applicable), last block is the output (if applicable)
INHERITED(SignalProcSet) 
public:

  override int		outBuffCount() const;
  override DataBuffer* 	outBuff(int idx);
  
  SIMPLE_LINKS(SequentialProcSet)
  TA_BASEFUNS(SequentialProcSet)
  
public:

protected:

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(SequentialProcSet)
};


class AUDIOPROC_API StimChan: public SignalProcItem
{ // #VIRT_BASE ##CAT_Audioproc ##NO_UPDATE_AFTER base class for a stimulus generator channel
INHERITED(SignalProcItem) 
public:
  enum ChanFlags { // #CAT_Audioproc #BITS
    CF_0	= 0, // #IGNORE
    CF_OFF	= 0x0002, // #LABEL_Off don't call this item
  };
  
  ChanFlags		chan_flags;
  
  virtual float		GetNext() {return 0.0f;} // return current sample, and advance
  
  int	GetEnabled() const {return (chan_flags & CF_OFF) ? 0 : 1;}
  override void SetDefaultName() {name = _nilString;}
  override TAPtr SetOwner(TAPtr own);
  TA_BASEFUNS_NOCOPY(StimChan)

protected:
  StimGen*	stim_gen; // cached for efficiency
  
private:
  void	Initialize() {stim_gen = NULL;}
  void	Destroy() {stim_gen = NULL;}
};


class AUDIOPROC_API StimChan_List: public taList<StimChan>
{ // ##CAT_Audioproc list of channels, that will operate in parallel
INHERITED(taList<StimChan>) 
public:

  TA_BASEFUNS_NOCOPY(StimChan_List)

private:
  void	Initialize() {SetBaseType(&TA_StimChan);}
  void	Destroy() {}
};


class AUDIOPROC_API ToneChan: public StimChan
{ // ##CAT_Audioproc channel for generating tones, such as sine waves, square waves, etc.
INHERITED(StimChan) 
public:
  enum WaveType {
    WT_SINE,	// #LABEL_Sine
    WT_COSINE,	// #LABEL_Cosine
    WT_SQUARE,	// #LABEL_Square has only odd harmonics (1, 3, 5...), weighted as 1/n
    WT_TRIANGLE,	// #LABEL_Triangle has only odd harmonics (1, 3, 5...), weighted as 1/n^2 (i.e. like a square wave, but harmonics fall off much faster)
    WT_SAWTOOTH, // #LABEL_Sawtooth has all harmonics (1, 2, 3...), weighted as 1/n
  };
  
  enum FreqMode {
    FM_ABSOLUTE, // #LABEL_Absolute -- the freq value is the value in Hz
    FM_HARMONIC // #LABEL_Harmonic -- the freq value specifies a harmonic multiplier from the first channel (first channel must be Absolute)
  };
  
  WaveType	wave_type; // the type of waveform to generate
  FreqMode	freq_mode; // #APPLY_IMMED the mode to use
  float		freq; // the frequency or harmonic multiplier to use (depending on mode)
  float		act_freq; // #READ_ONLY #NO_SAVE #SHOW the actual frequency to use
  Level		level; // the level, relative to the overall stimulus level
  Phase		starting_phase; // the phase at which to start, relative to 0
// note: "phase" range depends on type: sine:rads, sq,saw,tri:0-1
  float		phase_per_samp; // #READ_ONLY #NO_SAVE phase units per sample
  float		cur_phase; // #READ_ONLY #NO_SAVE current place in the gen cycle
  
  override float	GetNext();
  
  SIMPLE_LINKS(ToneChan)
  TA_BASEFUNS(ToneChan)

protected:
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(ToneChan)
};


class AUDIOPROC_API NoiseChan: public StimChan
{ // ##CAT_Audioproc channel for generating tones, such as sine waves, square waves, etc.
INHERITED(StimChan) 
public:
  enum NoiseType {
    NT_WHITE,	// #LABEL_White_Noise
  }; //
  
  NoiseType	noise_type; // the type of noise to generate
  Level		level; // the level, relative to the overall stimulus level
  
  override float	GetNext();
  
  SIMPLE_LINKS(NoiseChan)
  TA_BASEFUNS(NoiseChan)
  
protected:
  static const double	rand_fact; // scale factor, we calc once
  
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(NoiseChan)
};


class AUDIOPROC_API ImpulseChan: public StimChan
{ // ##CAT_Audioproc channel for a single impulse, usually for characterizing a filter response
INHERITED(StimChan) 
public:  
  override float	GetNext()
    {if (triggered) return 0; triggered = true; return 1;}
  
  TA_BASEFUNS_NOCOPY(ImpulseChan)
  
protected:
  bool			triggered; // IGNORED
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok)
    {triggered = false;}
  
private:
  void	Initialize() {triggered = false;}
  void	Destroy() {}
};


class AUDIOPROC_API StimGen: public InputBlock
{ // ##CAT_Audioproc  stimulus generator
INHERITED(InputBlock) 
public:
  StimChan_List		chans; // the stimulus channels that will make up the stimulus
  
  override taList_impl*  children_() {return &chans;} //note: required
  
  void			AddChan(TypeDef* chan_type, int num = 1); // #TYPE_0_StimChan #MENU #MENU_CONTEXT #BUTTON add num channels of the requested type
  
  SIMPLE_LINKS(StimGen)
  TA_BASEFUNS(StimGen)
  
public:

protected:
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		ProcNext_Samples_impl(int n, ProcStatus& ps);

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(StimGen)
};


class AUDIOPROC_API SourceBlock: public StdBlock
{ // a block that serves as a sound source for a Listener
INHERITED(StdBlock) 
public:

  SIMPLE_LINKS(SourceBlock);
  TA_BASEFUNS(SourceBlock) //

protected:
//  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
//  override void		ProcNext_Samples_impl(int n, ProcStatus& ps);
  
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(SourceBlock)
};


class AUDIOPROC_API RampGen: public StdBlock
{ // ##CAT_Audioproc ramp generator, for gating a stimulus on or off
INHERITED(StdBlock) 
public:
  enum RampDir {
    RD_ON,	// #LABEL_On ramp goes from 0 to on
    RD_OFF	// #LABEL_Off ramp goes from on to 0
  };
  
  enum RampType {
    RT_TRIANGULAR,	// #LABEL_Triangular a triangular ramp
    RT_RAISED_COSINE,	// #LABEL_Hanning(raised cos) a Hanning (aka raised cosine) ramp (common)
    RT_HAMMING		// #LABEL_Hamming (Hanning, w/ a small pedestal)
#ifndef __MAKETA__
    ,RT_DEF	= RT_RAISED_COSINE
#endif
  };
  
  RampDir		ramp_dir; // direction of the ramp
  RampType		ramp_type; // the type of the ramp
  Duration		ramp_onset; // the delay to the onset of the ramp, in ms
  Duration		ramp_dur; // the duration of the ramp, in ms
    
  SIMPLE_LINKS(RampGen)
  TA_BASEFUNS(RampGen)
  
public: // DO NOT USE
  DataBuffer		zero_buff; // #NO_SHOW #NO_SAVE easiest to just make a zero buff

protected:
  enum RampState {
    RS_PRE, // pretrigger
    RS_RAMP, // ramp period
    RS_POST  // post period
  };
  
  RampState		ramp_state;
  double		cnt_samp; // sample heartbeat counter
  double		ramp_onset_samp; // absolute samples until start of onset
  double		ramp_dur_samp;  // duration of ramp in samples
  
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);

  void			UpdateState(); // we have advanced (or init'ed) the samps, so set state
  float			GetRampGain(); // only called during ramp, to get the gain factor (0-1)

private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(RampGen)
};


class AUDIOPROC_API DoG1dFilterSpec : public taNBase {
  // #INLINE ##CAT_Audioproc defines a difference-of-gaussians (center minus surround or "mexican hat") filter that highlights contrast -- this is the 1d version
  INHERITED(taNBase)
public:
  enum FilterType {
    FT_DOG, 	// standard DoG filter
    FT_SIEVE,	// for the Harmonic Sieve
  };
  
  FilterType	filter_type;
  int		half_width;	// #MIN_1 half-width of the filter (typically 2-4 semi-tones)
  int		filter_size;	// #READ_ONLY size of the filter: 2 * half_width + 1
  float		on_sigma_norm;	// width of the narrower central 'on' gaussian, normalized to half_width of 4 (typically 1)
  float		off_sigma_norm;	// width of the wider surround 'off' gaussian, normalized to half_width of 4 (typically 2 * on_sigma)
  float_Matrix	on_filter;	// #SHOW_TREE #READ_ONLY #NO_SAVE #NO_COPY on-gaussian 
  float_Matrix	off_filter;	// #SHOW_TREE #READ_ONLY #NO_SAVE #NO_COPY off-gaussian (values are positive)
  float_Matrix	net_filter;	// #SHOW_TREE #READ_ONLY #NO_SAVE #NO_COPY net overall filter (for display purposes)

  float		FilterPoint(int x, float val);
  // #CAT_DoG1dFilter apply filter at given x point to given value
  float		GetFilterVal(int x) {return net_filter.FastEl(x+half_width);}
  // #CAT_DoG1dFilter apply filter at given x point to given value

  void		RenderFilter();
  // #CAT_DoG1dFilter render filter into matrix

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view (reset any existing data first)

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoG1dFilterSpec);
protected:
  void	UpdateAfterEdit_impl();
  virtual void	RenderFilter_impl();
  void	RenderFilter_DoG_impl();
  void	RenderFilter_Sieve_impl();
};

class AUDIOPROC_API LogLinearBlock: public StdBlock
{ // ##CAT_Audioproc auditory nerve block -- intended for +ve signal values only (ex rectified)
INHERITED(StdBlock) 
public:
  float 		cl; // center level (in dB) of the block
  float			width; // the ~90% (.05-.95) width in dB 
  float			norm; // #EXPERT the normalization factor
  Level			in_gain; // a gain factor applied to the input values -- eff gain should result in values from 0-1
  
  ProcStatus 		AcceptData_LL(float_Matrix* in_mat, int stage = 0);
    // #IGNORE mostly for proc
  
  SIMPLE_LINKS(LogLinearBlock)
  TA_BASEFUNS(LogLinearBlock)
  
protected:
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);

  virtual float		CalcValue(float in);
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(LogLinearBlock)
};

class AUDIOPROC_API AGCBlock: public StdBlock
{ // ##CAT_Audioproc automatic gain control block -- intended for +ve signal values only (ex rectified)
INHERITED(StdBlock) 
public:
  
  DataBuffer		out_buff_gain; //  #SHOW_TREE provides the gain values used: v0:cl, v1: (note: is always mono, even for stereo feeds)
  Level			dyn_range_out; // the desired output dynamic range
  Duration  		agc_dt; // the time constant of the gain integration -- try 50-100 ms
  
  ProcStatus 		AcceptData_AGC(float_Matrix* in_mat, int stage = 0);
    // #IGNORE mostly for proc
  
  override int		outBuffCount() const {return 2;}
  override DataBuffer* 	outBuff(int idx) {switch (idx) {
    case 0: return &out_buff; 
    case 1: return &out_buff_gain;
    default: return NULL;}}
  
  SIMPLE_LINKS(AGCBlock)
  TA_BASEFUNS(AGCBlock)
  
public: // TEMP
  float			ths_peak; // #NO_SAVE #EXPERT #READ_ONLY highest value in current stream
  double		ths_avg; // #NO_SAVE #EXPERT #READ_ONLY avg value in current stream
  float			cl; // #NO_SAVE #EXPERT #READ_ONLY current cl
  float			width; // #NO_SAVE #EXPERT #READ_ONLY current width
  
  float targ_cl; // #NO_SAVE #EXPERT #READ_ONLY current width
  float targ_width; // #NO_SAVE #EXPERT #READ_ONLY current width
  
protected:
  
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps);

  virtual float		CalcValue(float in);
  void			UpdateAGC();
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
  SIMPLE_COPY(AGCBlock)
};

#endif
