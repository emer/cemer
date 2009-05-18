#include "gammatone.h"
 
#include "ta_project.h"
#include "ta_math.h"

#include <math.h>
#include <float.h>

//////////////////////////////////
//  GammatoneChan		//
//////////////////////////////////

const KeyString GammatoneChan::key_on("on");
const KeyString GammatoneChan::key_cf("cf");
const KeyString GammatoneChan::key_erb("erb");
const KeyString GammatoneChan::key_gain("gain");

void GammatoneChan::Initialize() {
  on = true; // assumed
  cf = 0;
  tpt = 0;
  gain = 0;
  erb = 0;
}

String GammatoneChan::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_on) return String(GetEnabled());
  else if (key == key_cf) return String(cf);
  else if (key == key_erb) return  String(erb);
  else if (key == key_gain) return  String(gain);
  else return inherited::GetColText(key, itm_idx);
}

bool GammatoneChan::ChanFreqOk(float cf, float ear_q, float min_bw,
   float fs)
{
  double erb = min_bw + (cf / ear_q);
  // if 2 bandwidths (2*.5*erb) above cf exceeds nyquist, then shut us off
  bool rval = (cf + erb) < (fs / 2);
  return rval;
}

void GammatoneChan::InitChan(float cf_, float ear_q, float min_bw,
   float fs)
{
  cf = cf_;
  erb = min_bw + (cf / ear_q);
  // if 2 bandwidths (2*.5*erb) above cf exceeds nyquist, then shut us off
  on = ChanFreqOk(cf_, ear_q, min_bw, fs);
  if (!on) return; // nothing else will be used...
  
  tpt = (2 * M_PI) / fs;
  double tptbw = tpt * erb * 1.019; //Moore and Glasberg (1983)
  

  // init filter coefficiants 
  double a = exp(-tptbw);
  a1 = 4.0*a; 
  a2 = -6.0*a*a; 
  a3 = 4.0*a*a*a; 
  a4 = -a*a*a*a; 
  a5 = 4.0*a*a;
  
//  gain = (tptbw*tptbw*tptbw*tptbw)/3; ning
  //gain = pow(tptbw,4) / 4.66; // jim
  gain=(2.0*(1-a1-a2-a3-a4)/(1+a1+a5)); // jim 2.0
  
  coscf = cos(tpt*cf);
  sincf = sin(tpt*cf);
  cs = 1; sn = 0;
  
  
  // init buffer
  p0r=p1r=p2r=p3r=p4r=0;
  p0i=p1i=p2i=p3i=p4i=0;
  
  DataChanged(DCR_ITEM_UPDATED);
}
  
void GammatoneChan::DoFilter(int n, int in_stride, const float* x,
  int out_stride, float* bm, float* env)
{
  double u0r, u0i;
  double oldcs, oldsn;

  //====================================================================================
  // complex z=x+j*y, exp(z) = exp(x)*(cos(y)+j*sin(y)) = exp(x)*cos(x)+j*exp(x)*sin(y).
  // z = -j * tpt*i*cf, exp(z) = cos(tpt*i*cf) - j * sin(tpt*i*cf)
  //====================================================================================
  
  for (int i=0; i<n; i++, x+=in_stride) {
    const float in_x = *x; 
    p0r = cs*(in_x) + a1*p1r + a2*p2r + a3*p3r + a4*p4r;
    p0i = sn*(in_x) + a1*p1i + a2*p2i + a3*p3i + a4*p4i;

    u0r = p0r + a1*p1r + a5*p2r;
    u0i = p0i + a1*p1i + a5*p2i;
 
    p4r = p3r; p3r = p2r; p2r = p1r; p1r = p0r;
    p4i = p3i; p3i = p2i; p2i = p1i; p1i = p0i;
  
   //==========================================
   // Basilar membrane displacement
   //==========================================
    
    double tbm = (u0r*cs+u0i*sn) * gain;
    if (bm) {
      *bm = (float)tbm;
      bm += out_stride;
    }
    
    //==========================================
    // Instantaneous envelope 
    //==========================================
    
    if (env) {
        
      double instp = u0r*u0r+u0i*u0i;
      *env = sqrt(instp) * gain;
      env += out_stride;
    } // env

   //=========================================
   // sin(A+B) = sin(A)*cos(B) + cos(A)*sin(B)
   // cos(A+B) = cos(A)*cos(B) + sin(A)*sin(B)
   // cs = cos(tpt*i*cf); sn = -sin(tpt*i*cf);
   //=========================================
    
    cs = (oldcs=cs)*coscf + (oldsn=sn)*sincf;
    sn = oldsn*coscf - oldcs*sincf;
  }
}
  


//////////////////////////////////
//  GammatoneChan_List		//
//////////////////////////////////

String GammatoneChan_List::GetColHeading(const KeyString& key) const {
  if (key == GammatoneChan::key_on) return "On";
  else if (key == GammatoneChan::key_cf) return "CF";
  else if (key == GammatoneChan::key_erb) return "ERB";
  else if (key == GammatoneChan::key_gain) return "Gain";
  else return inherited::GetColHeading(key);
}

const KeyString GammatoneChan_List::GetListColKey(int col) const {
  switch (col) {
  case 0: return GammatoneChan::key_on;
  case 1: return GammatoneChan::key_cf;
  case 2: return GammatoneChan::key_erb;
  case 3: return GammatoneChan::key_gain;
  default: return _nilKeyString;
  }
}

int GammatoneChan_List::NumListCols() const {
  return 4;
}

//////////////////////////////////
//  GammatoneBlock		//
//////////////////////////////////

void GammatoneBlock::Initialize() {
  chan_spacing = CS_LogLinear;
  ear_q = 9.26449f;
  min_bw = 24.7f;
  cf_lo = 110.0f;
  cf_hi = 9800.0f;
  // legacy compat
  if (taMisc::is_loading)
    chans_per_oct = -1.0f; // sentinel, calc'ed in UAE
  else chans_per_oct = 8.0f;
  n_chans = 32;
  out_vals = OV_SIG;
  num_out_vals = 1;
  delta_env_dt_inv = 1.0f; // safe dummy value
}

void GammatoneBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  // must always have ENV if DELTA_ENV
  if (out_vals & OV_DELTA_ENV) 
    out_vals = (OutVals)(out_vals | OV_ENV);
  // calc chans for legacy projects
  if ((chan_spacing == CS_LogLinear) && (chans_per_oct < 0)) {
    chans_per_oct = (n_chans - 1)/(log(cf_hi/cf_lo)/log(2.0f));
  }
  
  // note: because of the codes used, there is always at least one val
  num_out_vals = 0;
  if (out_vals & OV_SIG) num_out_vals++;
  if (out_vals & OV_ENV) num_out_vals++;
  if (out_vals & OV_FREQ) num_out_vals++;
  if (out_vals & OV_DELTA_ENV) num_out_vals++;
  
}

void GammatoneBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  
  if (CheckError((src_buff->chans > 1), quiet, ok,
    "GammatoneBlock only supports single channel input"))
    return;
    
  if (CheckError((src_buff->vals > 1), quiet, ok,
    "GammatoneBlock only supports single val input"))
    return;
    
  // warn about nyquist violations -- some upper chans will be disabled
  bool on_hi = GammatoneChan::ChanFreqOk(cf_hi, ear_q, min_bw, src_buff->fs.fs_act);
  if ((!on_hi) && check && !quiet) {
    taMisc::Warning("GammatoneBlock: some hi freq chans will be disabled to prevent aliasing");
  }
  
  if (check) return;
  
  // just init all chans, whether enabled or not
  int buff_bit = 1;
  for (int obi = 0; obi < outBuffCount(); ++obi, buff_bit<<=1) {
    DataBuffer* ob = outBuff(obi);
    ob->enabled = (out_vals & buff_bit);
    if (!ob->enabled) continue; // will have mat set to zero 
    ob->fs = src_buff->fs;
    ob->fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
    ob->fields = src_buff->fields;
    ob->chans = n_chans;
    ob->vals = 1;
  }
  // if using delta_env, need min of 2 env stages
  if (out_vals & OV_DELTA_ENV) {
    out_buff_env.min_stages = 2;
    // 1/dt value, in 1/100us based on env guy
    delta_env_dt_inv = out_buff_env.fs / 100.0f;
    if (CheckError((delta_env_dt_inv <= 0), quiet, ok,
      "delta_env_dt value was <= 0 -- sampling rate must be bad!"))
      return;
  }
  
  switch (chan_spacing) {
  case CS_MooreGlassberg: {
    // calc equivalent cpo in case user switches mode
    chans_per_oct = (n_chans - 1)/(log(cf_hi/cf_lo)/log(2.0f));
  } break;
  case CS_LogLinear: {
    // calc equivalent cf_hi in case user switches mode
    cf_hi = exp((log(2.0f)*(n_chans-1) / chans_per_oct) + log(cf_lo));
  } break; 
  default: break;// compiler food -- handle all cases above
  }
}

void GammatoneBlock::InitChildConfig_impl(bool check, bool quiet, bool& ok) {
//TODO: when sharpening, correct the gain of the edge channels to compensate
// for the loss caused by not integrating the full DoG filter width

  chans.SetSize(n_chans);
  for (int i = 0; i < chans.size; ++i) {
    // don't move on to next child, if prev step didn't succeed
    if (!check && !ok) return;
    GammatoneChan* gc = chans.FastEl(i);
    if (!check) {
      // center frequency
      float cf;
      switch (chan_spacing) {
      case CS_MooreGlassberg: {
        if (i < (chans.size - 1))
          cf = -(ear_q * min_bw) + exp((n_chans-i - 1) *
          (-log(cf_hi + ear_q*min_bw) + log(cf_lo + ear_q*min_bw))/(n_chans-1)) *
          (cf_hi + ear_q*min_bw);
        else 
          cf = cf_hi;
      } break;
      case CS_LogLinear: {
        if (i == 0)
          cf = cf_lo;
        else 
          cf = exp((log(2.0f)*(i) / chans_per_oct) + log(cf_lo));
      } break; 
      default: break;// compiler food -- handle all cases above
      }
      gc->InitChan(cf, ear_q, min_bw, out_buff.fs);
    }
  }
  // do the default, which calls all the items, prob does nothing
  inherited::InitChildConfig_impl(check, quiet, ok);
}

void GammatoneBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_GT(in_mat, stage);
}

SignalProcBlock::ProcStatus GammatoneBlock::AcceptData_GT(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int in_chans = in_mat->dim(CHAN_DIM); // only 1 in allowed!!!
  const int in_vals = in_mat->dim(VAL_DIM); // only 1 in allowed!!!
  const int in_chan = 0; // only 1 in allowed
  const int in_val = 0; // only 1 in allowed
  
  // in_stride: the num items between each x in a channel
  const int in_stride = in_vals * in_chans * in_fields;
  // out_stride: the num items between each y in an output channel
  const int out_stride =  n_chans * in_fields;
  
  // note: we only support 1 chan, 1 val input, so don't iterate those
  for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f) {
    for (int g_ch = 0; g_ch < n_chans; ++g_ch) {
      GammatoneChan* sc = chans.FastEl(g_ch);
      if (!sc->on) continue; // will just leave 0;s everywhere
      float& dat = in_mat->FastEl(in_val, in_chan, f, 0, stage);
      // we only pass buffer addresses for values actually used
      // instf
      float* bm = NULL;
      float* env = NULL;
      if (out_vals & OV_SIG) {
        bm = &(out_buff.mat.FastEl(0, g_ch, f, 0, out_buff.stage));
      }
      if (out_vals & OV_ENV) {
        env =  &(out_buff_env.mat.FastEl(0, g_ch, f, 0, out_buff_env.stage));
      }
      
      sc->DoFilter(in_items, in_stride, &dat, out_stride, bm, env);
          
      if (out_vals & OV_DELTA_ENV) {
        float env_prev = out_buff_env.mat.FastEl(0, g_ch, f, 0, out_buff_env.prevStage());
        out_buff_delta_env.mat.FastEl(0, g_ch, f, 0, out_buff_delta_env.stage) =
          (*env - env_prev) * delta_env_dt_inv;
      }
    }
  } // field
  
  // advance index pointer, and notify clients
  for (int obi = 0; ((ps != PS_ERROR) && (obi < outBuffCount())); ++obi) {
    DataBuffer* ob = outBuff(obi);
    if (!ob->enabled) continue; // will have mat set to zero 
    if (ob->NextIndex())
      NotifyClientsBuffStageFull(ob, obi, ps);
  }
  
  return ps;
}

void GammatoneBlock::GraphFilter(DataTable* graph_data,
  bool log_freq, GammatoneBlock::OutVals response) 
{
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_GammatoneFilter");
    graph_data->ClearDataFlag(DataTable::SAVE_ROWS); // don't save by default!
    newguy = true;
  }
  
  // we will set params to make nice output
  
  const int n = 1024; 
  // need vars here so we can goto
  bool ok = true;
  int idx = -1;
  ProcStatus ps = PS_OK;
  int anali = 0;
  
  // to plot the filter, we make a copy of ourself and plot the impulse response
  GammatoneBlock* gb = new GammatoneBlock;
  float_Matrix* in_mat = new float_Matrix;
  
  gb->InitLinks();
  gb->Copy(*this);
  if ((response != OV_SIG) && (response != OV_ENV) && (response != OV_FREQ))
    response = OV_SIG;
  gb->out_vals = response;

  if (!ok) goto exit;
  // need to configure, and check it is valid, ex. fs set, etc.
  
  if (!gb->InitConfig()) goto exit;
  
{ int buff_bit = 1;
  // note: we only analyze one channel -- whatever is highest user selected
  for (int obi = 0; ((ps != PS_ERROR) && (obi <= 2)); ++obi, buff_bit<<=1) {
    DataBuffer* ob = gb->outBuff(obi);
    if (!ob->enabled) continue; // will have mat set to zero 
    anali = obi;
    ob->fr_dur.Set(n, Duration::UN_SAMPLES);
    ob->InitConfig(false, false, ok);
    if (!ok) goto exit;
  }
}  
  in_mat->SetGeom(5, 1,1,1,n,1); //val,ch,f,it,st
  in_mat->Set(1.0f, 0); // the impulse!
  
  // ok, run one round
  ps = gb->AcceptData_GT(in_mat);
  if (CheckError((ps != PS_OK), false, ok,
    "ProcNextFrame did not complete ok")) goto exit;
  // NOTE: we only analyze one of the outputs
{     
  DataBuffer* ob = gb->outBuff(anali);
  // do an fft on it -- have to invert the data
  float_Matrix fft_in(2, n, gb->n_chans);
  for (int chan = 0; chan < gb->n_chans; ++chan)
  for (int i = 0; i < n; ++i) {
    fft_in.FastEl(i, chan) = ob->mat.SafeEl(0, chan, 0, i);
  }
  float_Matrix fft;
  // get the real (power) only
  ok = taMath_float::fft_real_transform(&fft, &fft_in,
    true, false);
  if (CheckError((!ok), false, ok,
    "fft did not complete ok")) goto exit;
{
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  xda->SetUserData("X_AXIS", true);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);
  valda->SetUserData("PLOT_1", true);
  DataCol* zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
  zda->SetUserData("Z_AXIS", true);
  graph_data->StructUpdate(false);
  
  float_Matrix* mat = &fft;
  //NOTE: find the peak value of all the filters, for 0db point
  float peak = mat->SafeEl(0);
  for (int i = 1; i < mat->size; ++i) {
    float val = mat->FastEl_Flat(i);
    if (val > peak) peak = val;
  }
  taMisc::Info("Gammatone filter peak (for normalization) was:", String(peak));
  float fs = ob->fs;
  graph_data->DataUpdate(true);
  graph_data->AllocRows(mat->dim(1) * ((mat->dim(0) / 2)));
  for (int ch = 0; ch < mat->dim(1); ++ch) {
    // note: we omit DC, and the aliased freq's above 1/2 fs
    for (int i = 1; i < (mat->dim(0) / 2); ++i) {
      float freq = i * (fs / mat->dim(0));//gb->chans.SafeEl(i);
      if (log_freq) freq = log10(freq);
      float val = mat->FastEl(i, ch);
      float val_db;
      if (peak <= 0) val_db = -120; // shouldn't really happen
      else val_db = 10 * log10(val/peak);
      if (val_db < -120) val_db = -120; // floor for the graph
      graph_data->AddBlankRow();
      zda->SetValAsFloat(ch, -1);
      xda->SetValAsFloat(freq, -1);
      valda->SetValAsFloat(val_db, -1);
    } 
  }
  graph_data->SetUserData("VIEW_ROWS", graph_data->rows);
  graph_data->DataUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}}
exit:
  if (in_mat) delete in_mat;
  if (gb) delete gb;
}


void GammatoneBlock::MakeStdFilters(float cf_lo, float cf_hi, int n_chans) {
  this->cf_lo = cf_lo;
  this->cf_hi = cf_hi;
  this->n_chans = n_chans;
  chan_spacing = CS_LogLinear;
  ear_q = 9.26449f;
  min_bw = 24.7f;
  UpdateAfterEdit();
}


//////////////////////////////////
//  ChansPerOct		//
//////////////////////////////////

bool ChansPerOct::Lookup(SignalProcBlock* parent) {
  GammatoneBlock* gtb = dynamic_cast<GammatoneBlock*>(
    parent->GetUpstreamBlock(&TA_GammatoneBlock));
  if (gtb) {
    chans_per_oct = gtb->chans_per_oct;
    return true;
  }
  return false;
}


//////////////////////////////////
//  SharpenBlock		//
//////////////////////////////////

/*
  The Sharpen Block models several adjacency and level effects in
  the BM.
  
  1. Suppression (Moore, pp. 45-46)
  
  (Assume that the "critical band" to -20dB for a CF is ~.25*CF.)
  
  This is a somewhat classic lateral inhibition effect. A sound on
  or near the CF is attentuated by "up to 20%" by sounds outside.
  The tails of the effect are quite large, being longer on the low
  end (quite extended, ex 1/2 or less of CF) than the high end
  (fairly narrow, ex. similar to critical band itself.)
  
  2. Active Mechanism
  
  In the ear, this is achieved by selective amplification, of up
  to 50 dB (Plack, 2005) at low levels, up to 0 at very high levels.
  In the BM, it is also non-symetrical, with more emphasis added at
  the high end, thus shifting the CF higher under active mechanism.
  
  The active mechanism (outer hair cells) causes the
  predominant peak on the BM in a local neighborhood to be amplified
  more than the response to adjacent frequencies.
  
  "At very low sound levels, below 20-30dB SPL, the gain is roughly
  constant and at its maximal value. As the sound level increases,
  the gain progressively reduces." [it maxes out around 90 dB SPL)
*/

void SharpenBlock::Initialize() {
  out_fun = OF_STRAIGHT;
  pow_gain = 1.0f;
  pow_base = 20.0f;
  //TODO defaults for dog
}

void SharpenBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}


void SharpenBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of vals, chans, fields, or items
  
  if (check) return;
  
  // get cpo if enabled
  if (chans_per_oct.auto_lookup) {
    chans_per_oct.Lookup(this);
  }
  if (CheckError((chans_per_oct <= 0), quiet, ok,
    "chans_per_oct must be > 0"))
    return;
  
  out_buff.fs = src_buff->fs;
  out_buff.fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
  out_buff.fields = src_buff->fields;
  out_buff.chans = src_buff->chans;
  out_buff.vals = src_buff->vals;
  
  // filter
  dog.UpdateAfterEdit();
}

void SharpenBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int in_stage, ProcStatus& ps)
{
  if (!src_buff) return;
  float_Matrix* in_mat = &src_buff->mat;
  float_Matrix* out_mat = &out_buff.mat;
  
  const int in_vals = in_mat->dim(VAL_DIM); 
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int n_chans = in_mat->dim(CHAN_DIM); 
  const int out_stage = out_buff.stage;
  float pow_gain_eff = pow_gain / dog.filter_size; 
  
  // note that there are small gain errors at the edges, but the lowest
  // and highest freq channels are typically low information anyway
  
  for (int v = 0; ((ps == PS_OK) && (v < in_vals)); ++v)
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i)
  for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f)
  for (int out_ch = 0; out_ch < n_chans; ++out_ch)
  {
    float out_val = 0.0f;
    for (int offs = -dog.half_width; offs <= dog.half_width; ++offs) {
      int in_ch = out_ch + offs;
      // check for under/overflow (edges)
      if ((in_ch < 0) || (in_ch >= n_chans)) continue;
      float val = in_mat->FastEl(v, in_ch, f, i, in_stage);
      out_val += dog.FilterPoint(offs, val);
    }
    switch (out_fun) {
    case OF_STRAIGHT: break; // out_val is our guy
    case OF_POWER: {
      // use out_val as a power to which to apply to this in val
      float in_val = in_mat->FastEl(v, out_ch, f, i, in_stage);
      out_val = in_val * pow(pow_base, (out_val * pow_gain_eff));
      } break;
    // no default, let compiler complain if unhandled
    }
    out_mat->FastEl(v, out_ch, f, i, out_stage) = out_val;
  }
  
  // advance stage pointer, and notify clients
  if (out_buff.NextIndex())
    NotifyClientsBuffStageFull(&out_buff, 0, ps);
}




void SharpenBlock::GraphFilter(DataTable* graph_data) {
  dog.GraphFilter(graph_data);
}


//////////////////////////////////
//  TemporalWindowBlock		//
//////////////////////////////////

/*
  The gain for the TWB will be the sum of the weights; it therefore
  normalizes the values such that their sum is 1.
  
  * the lag between input and output, will always
    be dur (l_dur+u_dur)
  * you can output faster than the dur -- this doesn't change the lag, it
    just gives you updates at a faster rates, basically it gives you overlap
    between the long sampling windows
  * 
  
  Timing variables relationship:
    note: anything counted (ex samples) is an integer
  
  dur = l_dur + u_dur -- total duration, in ms, >0
  l/u_flt_wd = filter width of l/u_dur, in in.fs samples, >=1
  flt_wd = l_flt_wd + u_flt_wd;
  out_rate = duration between output samples, usually >> in.rate and < dur
    typical: in = 1/16K, out = 100ms
  out_wd = output width, in in.fs samples, >=1
  vl_flt_wd = imaginary additional wd of l_flt_wd, for following calc:
  v_flt_wd = virtual width, vl_flt_wd + flt_wd,
    such that v_flt_wd/out_wd is integer >= 1
  stages = v_flt_wd/out_wd, >=1 (axiomatically, because of def of v_flt_wd)
  
  When vl_flt_wd >0 we do not have perfect overlap bewteen in and out
  
  For each stage, we maintain a circular filter index value, that varies
    over the range from -vl_flt_wd:flt_wd-1; when -ve, we ignore the input
    sample, otherwise we convolve input with filter(index) to output
  if out_rate < dur {typical case}
    stages = 
  if out_rate > dur {subsampling}
  
  if out_rate = dur 
    stages = 1
*/

void TemporalWindowBlock::Initialize() {
  non_lin = NL_HALF_WAVE;
  ft = FT_DEF; //
  ot = OT_SINGLE;
  out_rate = 4.0f;
  l_dur = 24.0f;
  u_dur = 8.0f;
  out_wd = 0;
  flt_wd = 0;
  v_flt_wd = 0;
  l_flt_wd = 0;
  u_flt_wd = 0;
  
// MooreGlasberg filter parameters:
  w.Set(-30.0f, Level::UN_DBI);
  tpl = 5.5f;
  tsl = 26.0f;
  tpu = 2.5f;
  tsu = 12.0f;
// Exponential:
  sigma = 1.0f;
// DoG
  on_sigma = 0.5f;
  off_sigma = 1.0f;
  conv_stages = 1; // set later
}

void TemporalWindowBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (out_rate <= 0) out_rate = 4.0;
  
  float ot_gn = 1.0f;
  switch (ot) {
  case OT_ON_OFF:
    ot_gn = 2.0f; // empirical
    break;
  default: break;
  };
  // gain for non-linearity 
  switch (non_lin) {
  case NL_HALF_WAVE:
    // chops out half the signal, so, duh...
    auto_gain.Set(2*ot_gn, Level::UN_SCALE);
    break;
  case NL_SQUARE:
    //NOTE: the required gain has not been determined!!!
    auto_gain.Set(1*ot_gn, Level::UN_SCALE);
    break;
  // all these guys are gain-neutral
  case NL_NONE:
  case NL_FULL_WAVE:
  default: 
    auto_gain.Set(1*ot_gn, Level::UN_SCALE);
    break;
  }
}

void TemporalWindowBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{ //NOTE: even in check mode, we still calc the derived fields, for next check steps
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff || !ok) return;
//  float_Matrix* in_mat = &src_buff->mat;
  float fs_in = src_buff->fs;
  // note: we can support any number of vals, chans, or items
  
  // warn about the gain for square nonlin
  if ((non_lin == NL_SQUARE) && check && !quiet) {
    taMisc::Warning("GammatoneBlock: use of Squaring non-linearity does not apply any gain correction");
  }
    
  // always calc these derived guys, to simplify checking
  out_wd = Duration::StatGetDurationSamples(
    out_rate, Duration::UN_TIME_MS, fs_in);
  // our frame rate must be > 0 (or fs calc will fault!)
  if (CheckError((out_wd <= 0), quiet, ok,
    "out_rate must be >0 and at least 1 input sample"))
    return;
    
  l_flt_wd = Duration::StatGetDurationSamples(
    l_dur, Duration::UN_TIME_MS, fs_in);
  u_flt_wd = Duration::StatGetDurationSamples(
    u_dur, Duration::UN_TIME_MS, fs_in);

  if (CheckError((l_flt_wd < 0) || (u_flt_wd < 0) ||
    ((l_flt_wd + u_flt_wd) <= 0), quiet, ok,
    "l_dur/u_dur must each be >=0, must add to >0 and at least 1 input sample"))
    return;
    
  const int lu_flt_wd = l_flt_wd + u_flt_wd;
  
  // now, add additional v_wd to make out fit exactly in total flt_wd
  if (out_wd <= lu_flt_wd) { // typical case
    int rem = lu_flt_wd % out_wd; // fraction of out_wd 
    if (rem == 0)
      v_flt_wd = 0; // no padding needed
    else v_flt_wd = out_wd - rem;
  } else // subsampling case -- v makes up empty space
    v_flt_wd = out_wd - lu_flt_wd;
  flt_wd = v_flt_wd + l_flt_wd + u_flt_wd;
  // at this point, each flt_wd is >= 1 integral # of out_wd slices
  conv_stages = flt_wd / out_wd;
  if (conv_stages < 1) conv_stages = 1;

  if (!ok) return;
  // we always init the conv guys
  conv_idx.SetSize(conv_stages);
  
  if (!check) {
    out_buff_off.enabled = (ot != OT_SINGLE);
    for (int i = 0; (ot==OT_SINGLE)? (i<1) : (i < 2); ++i) {
      DataBuffer* buff = outBuff(i);
      // output duration is set in terms of the input sample rate
      // since we are convolving the temporal filter as we get inputs
      buff->fs.SetCustom(1 / (out_rate / 1000));
      buff->fr_dur.Set(1, Duration::UN_SAMPLES);
      buff->fields = src_buff->fields;
      buff->chans = src_buff->chans;
      buff->vals = src_buff->vals;
      buff->UpdateAfterEdit();
    }
    conv_mat.SetGeom(5, src_buff->vals, src_buff->chans,
      src_buff->fields, 1, conv_stages);
  }
  // filter
  CheckMakeFilter(src_buff->fs, check, quiet, ok);
}

void TemporalWindowBlock::InitThisConfigDataOut_impl(bool check,
  bool quiet, bool& ok)
{
  // clear out old data
  if (!check) {
    out_buff.mat.Clear();
    conv_mat.Clear();
  }
  inherited::InitThisConfigDataOut_impl(check, quiet, ok);
  if (!ok) return;
  if (check) return;
  
  // init the conv indexes, as if we've already been processing
  for (int i = 0; i < conv_idx.size; ++i) {
    // spread the conv idx's evenly over the virtual filter range
    // because we apply in asc order, we need to preload in desc order
    // i.e. next guy out will be one most through the seq
    int idx = ((int) ((conv_idx.size - i - 1) * ((float)flt_wd) / (float)conv_stages)) - v_flt_wd;
    conv_idx[i] = idx;
  }
}

void TemporalWindowBlock::CheckMakeFilter(const SampleFreq& fs_in,
  bool check, bool quiet, bool& ok)
{
    
  // do some common setting -- but check guys can still stop us
  if (!check) {
    filter.SetGeom(1, l_flt_wd + u_flt_wd);
  }
  
  switch (ft) {
  case FT_MG:
    CheckMakeFilter_MooreGlasberg(fs_in, check, quiet, ok); 
    break;
  case FT_Exp:
    CheckMakeFilter_Exponential(fs_in, check, quiet, ok);
    break;
  case FT_DoG:
    CheckMakeFilter_DoG(fs_in, check, quiet, ok);
    break;
  case FT_Uniform:
    CheckMakeFilter_Uniform(fs_in, check, quiet, ok);
    break;
  }
  
  if (check || !ok) return;
  
  // DoG normalizes itself, otherwise we normalize +ve filter types
  if (ft != FT_DoG) {
    // normalize filter -- norm +ve and -ve separately
    double filt_gain = 0; // gain of the filter -- we normalize by this
    for (int i = 0; i < filter.size; ++i) {
      filt_gain += (double)filter.FastEl(i);
    } 
    filt_gain = 1 / filt_gain; // faster to multiply by this
    for (int i = 0; i < filter.size; ++i) {
      filter.FastEl(i) *= filt_gain;
    } 
    //TEMP
    //taMisc::Info("Normalized TemporalWindowBlock by ", String(filt_gain));
  }
}

void TemporalWindowBlock::CheckMakeFilter_MooreGlasberg(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok)
{
  if (CheckError(((w > 1) || (w < 0)), quiet, ok,
    "w must be a weighting factor from 0 to 1"))
    return;
  if (CheckError( ((l_dur > 0) && ((tpl <= 0) || (tsl <= 0))) || 
    ((u_dur > 0) && ((tpu <= 0) || (tsu <= 0))),
    quiet, ok, "tpl/tsl/tpu/tsu values must be > 0"))
    return;
    
  if (check) return;
  
  // make filter -- note: we define lower[0] as t = 0
  // lower -- time goes -ve
  // note: the formula uses -t, but easier for us to calc
  for (int i = 0; i < l_flt_wd; ++i) {
    float nt = -( (((float)(l_flt_wd - i)) / l_flt_wd) * l_dur);
    float val = (1-w) * exp(nt/tpl) + (w * exp(nt/tsl));
    filter.Set_Flat(val, i);
  } 
  
  // upper -- time is +ve
  for (int i = 0; i < u_flt_wd; ++i) {
    float nt = -( (((float) (i + 1)) / u_flt_wd) * u_dur);
    float val = (1-w) * exp(nt/tpu) + (w * exp(nt/tsu));
    filter.Set_Flat(val, l_flt_wd + i);
  } 
}

void TemporalWindowBlock::CheckMakeFilter_Exponential(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok)
{
}

void TemporalWindowBlock::CheckMakeFilter_DoG(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok)
{
  if (CheckError( ((l_dur > 0) && (on_sigma <= 0)) || 
    ((u_dur > 0) && (off_sigma <= 0)),
    quiet, ok, "on/off_sigma values must be > 0"))
    return;
    
  if (check) return;
  
  // we use temp mats to normalize the on and off before summing
  float_Matrix on_flt; 
  float_Matrix off_flt; 
  on_flt.SetGeom(1, filter.size);
  off_flt.SetGeom(1, filter.size);

  // make filter -- note: we define lower[0] as t = 0
  // lower -- time goes -ve
  // note: the formula uses -t, but easier for us to calc
  for (int i = 0; i < l_flt_wd; ++i) {
    float dist = ((float)(l_flt_wd - i - 1)) / l_flt_wd;
    float ong = taMath_float::gauss_den_sig(dist*3, on_sigma);
    float offg = taMath_float::gauss_den_sig(dist*3, off_sigma);
    on_flt.Set(ong, i);
    off_flt.Set(offg, i);
  } 
  for (int i = 0; i < u_flt_wd; ++i) {
    float dist = ((float)(i+1)) / u_flt_wd;
    float ong = taMath_float::gauss_den_sig(dist*3, on_sigma);
    float offg = taMath_float::gauss_den_sig(dist*3, off_sigma);
    on_flt.Set(ong, l_flt_wd+i);
    off_flt.Set(offg, l_flt_wd+i);
  } 
  // normalize filter -- for OT_ON_OFF we split into 2 layers, so *2
  float sum = (ot == OT_SINGLE) ? 1.0f : 2.0f;
  taMath_float::vec_norm_sum(&on_flt, sum); 
  taMath_float::vec_norm_sum(&off_flt, sum);
  for (int i = 0; i < filter.size; ++i) {
    filter.FastEl(i) = on_flt.FastEl(i) - off_flt.FastEl(i);
  } 
}

void TemporalWindowBlock::CheckMakeFilter_Uniform(const SampleFreq& fs_in,
    bool check, bool quiet, bool& ok)
{
    
  if (check) return;
  
  if ((l_flt_wd + u_flt_wd) <= 0) return; // huh?
  float val = 1.0f / (l_flt_wd + u_flt_wd); 
  for (int i = 0; i < l_flt_wd; ++i) {
    filter.Set_Flat(val, i);
  } 
  
  // upper -- time is +ve
  for (int i = 0; i < u_flt_wd; ++i) {
    filter.Set_Flat(val, l_flt_wd + i);
  } 
}

void TemporalWindowBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  const int fields = in_mat->dim(FIELD_DIM);
  const int chans = in_mat->dim(CHAN_DIM); 
  const int vals = in_mat->dim(VAL_DIM);
  float_Matrix* out_mat = &out_buff.mat;
  float_Matrix* out_mat_off = &out_buff_off.mat; // only if ot=ON_OFF
  // we do one input item at a time, convolving with all our out guys
  for (int i = 0; ((ps == PS_OK) && (i < in_mat->dim(SignalProcBlock::ITEM_DIM))); ++i) {
  
    // iterate all our outputs, convolving in, and outputting when appropriate
    // note that we always convolve to conv_buff -- we only do on/off split 
    // at data output time
    for (int ci = 0; ci < conv_idx.size; ++ci) {
      int& cidx = conv_idx.FastEl(ci); // we test/inc/set
      if (cidx >= 0) { // ignore if in -ve virtual part of the filter (=0)
        float filt_val = filter.SafeEl(cidx);
        // std accept loop:
        for (int f = 0; ((ps == PS_OK) && (f < fields)); ++f)
        for (int chan = 0; ((ps == PS_OK) && (chan < chans)); ++chan) 
        for (int val = 0; ((ps == PS_OK) && (val < vals)); ++val)
        {
          float dat = in_mat->SafeElAsFloat(val, chan, f, i, stage);
          //non-linearity
          switch (non_lin) {
          //case NL_NONE:
          case NL_HALF_WAVE:
            if (dat < 0) dat = 0;
            break;
          case NL_FULL_WAVE:
            if (dat < 0) dat = -dat;
            break;
          case NL_SQUARE:
            dat = dat * dat;
            break;
          default: break; // compiler food
          }
          float& item = conv_mat.FastEl(val, chan, f, 0, ci);
          item += (dat * filt_val);
        }
      }
      // now, inc the conv_indx; if overflows, means need to output and reset 
      if (++cidx >= filter.size) {
        cidx = -v_flt_wd;
        // if doing on/off, need to set the two items according to value of out
        if (ot == OT_SINGLE) {
          for (int f = 0; ((ps == PS_OK) && (f < fields)); ++f)
          for (int chan = 0; ((ps == PS_OK) && (chan < chans)); ++chan) 
          for (int val = 0; ((ps == PS_OK) && (val < vals)); ++val)
          {
            float& conv_val = conv_mat.FastEl(val, chan, f, 0, ci);
            float& out_val = out_mat->FastEl(val, chan, f, 0, out_buff.stage);
            out_val = conv_val * auto_gain;
            conv_val = 0.0f; // resets
          }
        } else { // ON_OFF
          for (int f = 0; ((ps == PS_OK) && (f < fields)); ++f)
          for (int chan = 0; ((ps == PS_OK) && (chan < chans)); ++chan) 
          for (int val = 0; ((ps == PS_OK) && (val < vals)); ++val)
          {
            float& conv_val = conv_mat.FastEl(val, chan, f, 0, ci);
            float& out_val = out_mat->FastEl(val, chan, f, 0, out_buff.stage);
            float& out_val_off = out_mat_off->FastEl(val, chan, f, 0, 
              out_buff_off.stage);
            // apply gain
            conv_val *= auto_gain;
            if (conv_val >= 0) {
              out_val = conv_val;
              out_val_off = 0.0f;
            } else {
              out_val = 0.0f;
              out_val_off = -conv_val;
            }
            conv_val = 0.0f; // resets
          }
        }
        // send output
        out_buff.NextIndex(); // note: always rolls over, so we don't test
        NotifyClientsBuffStageFull(&out_buff, 0, ps);
        if (ot == OT_ON_OFF) {
          out_buff_off.NextIndex(); // note: always rolls over, so we don't test
          NotifyClientsBuffStageFull(&out_buff_off, 1, ps);
        }
      }
    }
  }
}

void TemporalWindowBlock::GraphFilter(DataTable* graph_data) {
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;

  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_TemporalFilter", true);
    newguy = true;
  }
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  int idx;
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  xda->SetUserData("X_AXIS", true);
  DataCol* valda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);
  valda->SetUserData("PLOT_1", true);
  graph_data->StructUpdate(false);
  
  graph_data->DataUpdate(true);
  float_Matrix* mat = &filter;
  float t;
  float val = 0.0f;
  // lower virtual (all zero)
  float vl_dur = l_dur + (Duration::StatGetDurationTime(v_flt_wd, Duration::UN_SAMPLES,
    src_buff->fs) * 1000.0f);
  for (int i = 0; i < v_flt_wd; ++i) {
    t = ( (((float)( -(l_flt_wd + v_flt_wd - 1) + i)) / (v_flt_wd + l_flt_wd)) * vl_dur);
    graph_data->AddBlankRow();
    xda->SetValAsFloat(t, -1);
    valda->SetValAsFloat(val, -1);
  } 
  // lower real, idx0=time0
  for (int i = 0; i < l_flt_wd; ++i) {
    t = -( (((float)(l_flt_wd - i)) / l_flt_wd) * l_dur);
    val = mat->SafeEl(i);
    graph_data->AddBlankRow();
    xda->SetValAsFloat(t, -1);
    valda->SetValAsFloat(val, -1);
  } 
  
  // upper -- time is +ve
  for (int i = 0; i < u_flt_wd; ++i) {
    t = ( (((float) (i + 1)) / u_flt_wd) * u_dur);
    val = mat->SafeEl(l_flt_wd + i);
    graph_data->AddBlankRow();
    xda->SetValAsFloat(t, -1);
    valda->SetValAsFloat(val, -1);
  }
  graph_data->DataUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}


//////////////////////////////////
//  DeltaBlock			//
//////////////////////////////////

void DeltaBlock::Initialize() {
  degree = FIRST;
}

void DeltaBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  //note: these gains are just empirical
  float gn = 1.0f; 
  gn *= (degree+2); // based on 1/n derivitive factor
  auto_gain.Set(gn, Level::UN_SCALE);
}

void DeltaBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of vals, chans, or items
  const int data_dims = degree + 2;
  // need enough prev stages to do the delta
  if (CheckError((src_buff->stages < data_dims), quiet, ok,
    "DeltaBlock: input requires degree+1 input stages"))
    return;
    
  if (!ok) return;
  
  if (check) return;
  data.SetSize(data_dims);
  
  for (int obi = 0; obi <= 1; ++obi) {
    DataBuffer* ob = outBuff(obi);
    ob->fs = src_buff->fs;
    ob->fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
    ob->fields = src_buff->fields;
    ob->chans = src_buff->chans;
    ob->vals = src_buff->vals;
  }
}

void DeltaBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  const int data_dims = degree + 2;
  ps = PS_OK;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM); 
  const int in_chans = in_mat->dim(CHAN_DIM); 
  const int in_vals = in_mat->dim(VAL_DIM); 
  
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i) { 
    for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f) 
    for (int v = 0; ((ps == PS_OK) && (v < in_vals)); ++v)
    {
      for (int ch = 0;
        ((ps == PS_OK) && (ch < in_chans)); ++ch)
      {
        // retrieve the data for doing the delta calcs
        data[0] = in_mat->SafeElAsFloat(v, ch, f, i, stage);
        for (int d = 1; d < data_dims; ++d) {
          int dstage = src_buff->GetRelStage(stage, -d);
          data[d] = in_mat->SafeElAsFloat(v, ch, f, i, dstage);
        }
        float delt = CalcDelta();
        float pl, mi;
        if (delt >= 0.0f) {
          pl = delt; mi = 0.0f;
        } else {
          pl = 0.0f; mi = -delt;
        }
        // output
        out_buff_pl.mat.FastEl(v, ch, f, out_buff_pl.item,
          out_buff_pl.stage) = pl*auto_gain;
        out_buff_mi.mat.FastEl(v, ch, f, out_buff_mi.item,
          out_buff_mi.stage) = mi*auto_gain;
      }
    }
    if (out_buff_pl.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff_pl, 0, ps);
    }
    if (out_buff_mi.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff_mi, 1, ps);
    }
  }
}

float DeltaBlock::CalcDelta() {
//TODO: accel
  return data[0] - data[1];
}

//////////////////////////////////
//  IIDBlock			//
//////////////////////////////////

void IIDBlock::Initialize() {
  chan.max = -1;
}

/*void IIDBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}*/

void IIDBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of vals, chans, or items
  
  if (CheckError((src_buff->fields > 2), quiet, ok,
    "IIDBlock: requires mono (dummy mode) or stereo"))
    return;
  
  // won't produce output unless stereo
  if ((src_buff->fields == 1) && check && !quiet) {
    taMisc::Warning("IIDBlock: requires stereo (fields==2) to produce output");
  }
  
  if (!ok) return;
  
  if (check) return;
  
  // determine eff chan
  chan_eff = chan;
  if (chan_eff.min < 0) 
    chan_eff.min = 0;
  if (chan_eff.min >= src_buff->chans) 
    chan_eff.min = src_buff->chans - 1;
  if (chan_eff.max < 0) 
    chan_eff.max = src_buff->chans - 1;
  if (chan_eff.max < chan_eff.min)
    chan_eff.max = chan_eff.min;
  
  int n_chans = chan_eff.Count();
  
  if (CheckError((n_chans == 0), quiet, ok,
    "IIDBlock -- no effective channels to process!"))
    return;
  
  for (int obi = 0; obi <= 1; ++obi) {
    DataBuffer* ob = outBuff(obi);
    ob->fs = src_buff->fs;
    ob->fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
    ob->fields = src_buff->fields; // sb 2
    ob->chans = n_chans;
    ob->vals = src_buff->vals;
  }
}

void IIDBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_IID(in_mat, stage);
}

SignalProcBlock::ProcStatus IIDBlock::AcceptData_IID(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM); // needs to be 2 for meaningful output!
  int in_chan_max = in_mat->dim(CHAN_DIM) - 1; // typically many, but we only use range
  in_chan_max = MIN(in_chan_max, chan_eff.max);
  const int in_vals = in_mat->dim(VAL_DIM); 
  
  // we use this hack to do dummy processing when input is mono (r = l)
  // note that this hack is merely a convenience so stuff still works, but is
  // not usually used in practice, so don't worry that we repeat saves etc. below
  const int field_r = (in_fields == 2) ? 1 : 0;
  
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i) 
//  for (int f = 0; ((ps == PS_OK) && (f < in_mat->dim(SignalProcBlock::FIELD_DIM))); ++f) 
  for (int v = 0; ((ps == PS_OK) && (v < in_vals)); ++v)
  {
    int out_ch = 0;
    for (int in_ch = chan_eff.min;
      ((ps == PS_OK) && (in_ch <= in_chan_max)); ++in_ch, ++out_ch)
    {
      float dat_l = in_mat->SafeElAsFloat(v, in_ch, 0, i, stage);
      float dat_r =  in_mat->SafeElAsFloat(v, in_ch, field_r, i, stage);
      float lmr = dat_l - dat_r;
      float pl_l, mi_l, pl_r, mi_r;
      if (lmr >= 0.0f) {
        pl_l = lmr; mi_l = 0.0f;  pl_r = 0.0f; mi_r = lmr;
      } else {
        pl_l = 0.0f; mi_l = -lmr;  pl_r = -lmr; mi_r = 0.0f;
      }
      // left output
      out_buff_pl.mat.FastEl(v, out_ch, 0, i, out_buff_pl.stage) = pl_l;
      out_buff_mi.mat.FastEl(v, out_ch, 0, i, out_buff_mi.stage) = mi_l;
      // right output
      // note: all vals are 0 anyway if mono, so just rewrite for that case
      out_buff_pl.mat.FastEl(v, out_ch, field_r, i, out_buff_pl.stage) = pl_r;
      out_buff_mi.mat.FastEl(v, out_ch, field_r, i, out_buff_mi.stage) = mi_r;
    }
  }
  return ps;  
}


//////////////////////////////////
//  ITDBlock			//
//////////////////////////////////

void ITDBlock::Initialize() {
  chan.max = -1;
}

/*void IIDBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}*/


//////////////////////////////////
//  ANVal			//
//////////////////////////////////

void ANVal::Initialize() {
  val_type = AN_EXP; // legacy default
  units = Level::UN_DBI;
  prev_units = units;
  // note: following would be for only one channel, spanning 60 dB
  cl = -30;
  width = 60;
  f = .3f; // determined empirically - gives ~ .5-.95 points for +-10dB
}

void ANVal::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (val_type == AN_LIN)
    units = Level::UN_SCALE;
  else units = Level::UN_DBI;
  if (!taMisc::is_loading && (units != prev_units)) {
    cl = Level::Convert(cl, prev_units, units);
    width = Level::Convert(width, prev_units, units);
  }
  UpdateParams();
  prev_units = units;
}

void ANVal::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  if (val_type == AN_LIN) {
    if (CheckError(((cl < 0) || (cl >= 1)), quiet, ok,
      "cl should be in range: 0:1, typically 0.5")) return;
    if (CheckError(((width <= 0) || (width > 20)), quiet, ok,
      "width should be in range: >0:20, typically 1")) return;
  } else {
    if (CheckError(((cl < -120) || (cl >= 0)), quiet, ok,
      "cl should be in range: -120:0")) return;
    if (CheckError(((width <= 0) || (width > 120)), quiet, ok,
      "width should be in range: >0:120")) return;
  } 
    
  if (check) return;
}

float ANVal::CalcValue(float in) {
#ifdef DEBUG
  if (TestError((isnan(in) || isinf(in)), "CalcValue",
    "nan or inf received -- converted to 0"))
    return 0.0f;

#endif
  if (val_type == AN_LIN) {
    return (in - (cl - 0.5f)) / width;
  }
  
  // note: avoid bad log10 results for -ve (nan) or very small or denorm inputs (-inf)
  // note: the ref max is 1, but entirely ok if exceeded
  double in_db = 0;
  if (in < 1e-12) in_db = -120; // pin at our design minimum
  else            in_db = 10 * log10(in); 
  
  // translate so that cf is at 0, and normalize
  double rval = (in_db - cl) * norm; 
  switch (val_type) {
  case AN_EXP: {  
    // do the exponential
    rval = 1 / (1 + exp(-(rval * f)));
    } break;
  case AN_SIG: {
    //TODO:
    } break;
  case AN_GAUSS: {  
    // do the guassian
    rval = exp(-((rval * rval)/2)) * f;
    } break;
  //no actual default 
  default: break; // compiler food, we handled all 
  }
  return rval;
}

void ANVal::SetParams(ANValType val_type_,
    float cl_, float width_)
{
  val_type = val_type_;
  cl = cl_;
  width = width_;
  UpdateAfterEdit();
}

void ANVal::UpdateParams()
{
  switch (val_type) {
  case AN_EXP: {  
    norm = (20 / width);
    f = .3f; // determined empirically - gives ~ .5-.95 points for +-10dB
    } break;
  case AN_SIG: {
    //TODO:
    } break;
  case AN_GAUSS: {  
    // a 90% confidence interval is 1.64485 sd's, so we normalize accordingly
    // divide by 2 because 2-tailed
    norm = 1.64485 / (width / 2);
    f = 1.0f;//1 / sqrt(2 * M_PI);
    } break;
  case AN_LIN:
    break;
  default: break;
  }
}


//////////////////////////////////
//  ANBlock			//
//////////////////////////////////

void ANBlock::Initialize() {
}

void ANBlock::InitLinks() {
  inherited::InitLinks();
  InitLinks_taAuto(&TA_ANBlock);
  if (!taMisc::is_loading) {
    val_list.New(1);
  }
}

void ANBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  
  if (CheckError((val_list.size < 1), quiet, ok,
    "You must have at least one output value"))
    return;
    
  if (CheckError((src_buff->vals > 1), quiet, ok,
    "ANBLock only supports single val input"))
    return;
    
    
  if (check) return;
  out_buff.fs = src_buff->fs;
  out_buff.fields = src_buff->fields;
  out_buff.chans = src_buff->chans;
  out_buff.vals = val_list.size;
}

void ANBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  ps = AcceptData_AN(in_mat, stage);
}

SignalProcBlock::ProcStatus ANBlock::AcceptData_AN(float_Matrix* in_mat, int stage)
{
  ProcStatus ps = PS_OK;
  float_Matrix* out_mat = &out_buff.mat;
  const int out_vals = out_buff.vals;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int in_chans = in_mat->dim(CHAN_DIM);
  const int in_val = 0;
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i) {
    for (int f = 0; ((ps == PS_OK) && (f < in_fields)); ++f) 
    for (int chan = 0; ((ps == PS_OK) && (chan < in_chans)); ++chan) 
    {
      float dat = in_mat->SafeEl(in_val, chan, f, i, stage);
      dat *= in_gain; 
      for (int out_val = 0; out_val < out_vals; ++out_val) {
        ANVal* av = val_list.FastEl(out_val);
        float res = av->CalcValue(dat);
        out_mat->Set(res, out_val, chan, f, out_buff.item, out_buff.stage);
      }
    }
    if (out_buff.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff, 0, ps);
      //TODO: also write to output table if we have one
    }
  }
  return ps;
}

void ANBlock::MakeVals(ANVal::ANValType val_type, int n_vals,
    float cl_min, float cl_max)
{
  bool ok = true;
  if (CheckError(!((n_vals >= 1) &&
    (cl_min < cl_max)), false, ok, 
    "Params must be: (n_vals >= 1) && "
      "(cl_min < cl_max))")) return;
      
  // just do it analytically
  float l_ovl = 6.0f;
  float cl_step = (cl_max - cl_min) / (n_vals - 1);
  float l_wid = cl_step - (l_ovl / 2);
  // width is double for GAUSS
  if (val_type == ANVal::AN_GAUSS)
    l_wid *= 2.0f;
   
  val_list.SetSize(n_vals);
  float cl = cl_min;
  for (int i = 0; i < n_vals; ++i, cl += cl_step) {
    ANVal* val = val_list.FastEl(i);
    val->SetParams(val_type, cl, l_wid);
  }
}

void ANBlock::GraphFilter(DataTable* graph_data, float l_min) {
  if (l_min < -120) l_min = -120;
  else if (l_min > -48) l_min = -48;
  taProject* proj = GET_MY_OWNER(taProject);
  bool newguy = false;
  if(!graph_data) {
    graph_data = proj->GetNewAnalysisDataTable(name + "_ANFilter", true);
    newguy = true;
  }
  
  // we will set params to make nice output
  
  const double l_inc = 0.1f;
  const int n = (int)(-l_min / l_inc) + 1;
  // need vars here so we can goto
  bool ok = true;
  int idx = -1;
  ProcStatus ps = PS_OK;
  Level l;
  
  // to plot the filter, we make a copy of ourself and plot the impulse response
  ANBlock* an = new ANBlock;
  float_Matrix* in_mat = new float_Matrix;
  
  an->InitLinks();
  an->Copy(*this);
  an->out_buff.fr_dur.Set(n, Duration::UN_SAMPLES);
  an->out_buff.stages = 1;
  an->out_buff.chans = 1;
  an->out_buff.vals = val_list.size;
  // need to configure, and check it is valid, ex. fs set, etc.
  if (!an->InitConfig()) goto exit;
  
  in_mat->SetGeom(5, 1,1,1,1,1); 
  
  
  // ok, run one round
  for (int i = 0; i < n; ++i) {
    l.Set(l_min + (i * l_inc), Level::UN_DBI);
    in_mat->Set(1.0f * l, 0); // the unitary input!
    ps = an->AcceptData_AN(in_mat);
    if (CheckError((ps != PS_OK), false, ok,
      "AcceptData_AN did not complete ok")) goto exit;
  }
{
{
  float_Matrix* mat = &an->out_buff.mat;
  const int vals = mat->dim(VAL_DIM);
  graph_data->StructUpdate(true);
  graph_data->ResetData();
  DataCol* xda = graph_data->FindMakeColName("X", idx, VT_FLOAT);
  xda->SetUserData("X_AXIS", true);
  DataCol* yda = graph_data->FindMakeColName("Y", idx, VT_FLOAT);
  yda->SetUserData("PLOT_1", true);
  DataCol* zda = NULL;
  if (vals > 1) {
    zda = graph_data->FindMakeColName("Z", idx, VT_FLOAT);
    zda->SetUserData("Z_AXIS", true);
  }
  graph_data->StructUpdate(false);
  
  graph_data->DataUpdate(true);
  graph_data->AllocRows(mat->size);
  // do each val as a separate sequence
  const int ch = 0;
  const int fld = 0;
  const int stage = 0;
  for (int val = 0; val < vals; ++val) {
    for (int i = 0; i < n; ++i) {
      float l = l_min + (i * l_inc);
      float dat = mat->SafeEl(val, ch, fld, i, stage);
      graph_data->AddBlankRow();
      xda->SetValAsFloat(l, -1);
      yda->SetValAsFloat(dat, -1);
      if (zda) zda->SetValAsFloat(val, -1);
    } 
  }
  graph_data->DataUpdate(false);
  if(newguy)
    graph_data->NewGraphView();
}}
exit:
  if (in_mat) delete in_mat;
  if (an) delete an;
}


//////////////////////////////////
//  NormBlock			//
//////////////////////////////////

void NormBlock::Initialize() {
  scale_type = NONE;
  scale_factor = 1.0f;
  norm_top_n = 1;
  in_thresh.Set(-50, Level::UN_DBI); // good value for speech
  norm_dt_out = 1.0f;
  cur_norm_factor = 1.0;
}

void NormBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void NormBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  switch (scale_type) {
  case NONE: break;
  case POWER:
    if (CheckError((scale_factor <= 0), quiet, ok,
    "NormBlock (scale_type=POWER): scale_factor must be > 0"))
    return;
    break;
  }
  
  in_thresh_lin_scaled = Scale(in_thresh);
  
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of vals, chans, or items
    
  if (!ok) return;
  
  if (check) return;
  cur_norm_factor = 1.0; // TODO: should we offer an init param???
  const int in_fields = src_buff->fields; 
  const int in_chans = src_buff->chans; 
  const int in_vals = src_buff->vals; 
  const int in_size = in_fields * in_chans * in_vals;
  scaled.SetGeom(3, in_vals, in_chans, in_fields);
  data.SetSize(in_size);
  // have to restrict N to be <= in_size
  norm_top_n = MIN(norm_top_n, in_size);
  
  // main normalized output data
  DataBuffer* ob = &out_buff;
  ob->fs = src_buff->fs;
  ob->fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
  ob->fields = src_buff->fields;
  ob->chans = src_buff->chans;
  ob->vals = src_buff->vals;
  
  // norm factor
  ob = &out_buff_norm;
  ob->fs = src_buff->fs;
  ob->fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
  ob->fields = 1;
  ob->chans = 1;
  ob->vals = 1;
}

void NormBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  float_Matrix* in_mat = &src_buff->mat;
  float_Matrix* out_mat = &out_buff.mat;
  ps = PS_OK;
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM); 
  const int in_chans = in_mat->dim(CHAN_DIM); 
  const int in_vals = in_mat->dim(VAL_DIM); 
  
  for (int i = 0; ((ps == PS_OK) && (i < in_items)); ++i) { 
    // first, get the incoming values, scale them, and put in topN buffer
    data.Reset();
    for (int f = 0; f < in_fields; ++f) 
    for (int ch = 0; ch < in_chans; ++ch)
    for (int v = 0; v < in_vals; ++v) {
      float& val = scaled.FastEl(v, ch, f);
      val = Scale(in_mat->FastEl(v, ch, f, i, stage));
      data.Add(val);
    }
    // do topN and calc scale value for this frame
    data.Sort();
    double topn_avg = 0;
    for (int j = norm_top_n; j >= 1 ; --j)
      topn_avg += data[data.size - j];
    topn_avg /= norm_top_n;
    // note: we don't update integrator when we fall below thresh because
    // we assume sound already low and norm high, and will be needed likewise
    // on next onset
    if (!((topn_avg < in_thresh_lin_scaled) ||
      (topn_avg < 1e-12f))) { // note: e-12 is arbitrary 0
      // apply the dt
      cur_norm_factor = ((1.0 - norm_dt_out) * cur_norm_factor) +
        (norm_dt_out / topn_avg); // note / inverts it to be scale
    }
    const double this_norm = cur_norm_factor;
    for (int f = 0; f < in_fields; ++f) 
    for (int ch = 0; ch < in_chans; ++ch)
    for (int v = 0; v < in_vals; ++v) {
      float val = scaled.FastEl(v, ch, f);
      val *= this_norm;
      // output
      out_mat->FastEl(v, ch, f, out_buff.item,
        out_buff.stage) = val;
    }
    
    if (out_buff.NextIndex()) {
      NotifyClientsBuffStageFull(&out_buff, 0, ps);
    }
    
    if (out_buff_norm.enabled) {
      out_buff_norm.mat.FastEl(0, 0, 0, out_buff_norm.item,
          out_buff_norm.stage) = this_norm;
      if (out_buff_norm.NextIndex()) {
        NotifyClientsBuffStageFull(&out_buff_norm, 1, ps);
      }
    }
  }
}

float NormBlock::Scale(float val) {
  switch (scale_type) {
  case NONE: 
    return val;
  case POWER: 
    return powf(val, scale_factor);
  case LOG10_1P:
    return log10f(1.0f + val);
  }
  return val; // compiler food
}


//////////////////////////////////
//  HarmonicSieveBlock		//
//////////////////////////////////

/*
  The HarmonicSieve Block extracts a value indicating how likely that
  a given fundamental has harmonically related components. It is used
  to extract pitches (for musical processing) and to detect harmonic
  sound sources for other uses, such as vowels in speech recognition.
  
  The sieve is based on the idea that if there are no harmonics to an f0,
  then the other channels above it are basically random; however if there
  are harmonics, then the n*f0 values should be higher than the average,
  and therefore the sum of the remaining channels should be lower. By
  subtracting sum(non) from sum(n*f0) we should have the excess  
*/

void HarmonicSieveBlock::Initialize() {
  out_octs = 2;
  cpo_eff = 1; // set in config
  in_octs = 0;
}

void HarmonicSieveBlock::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}


void HarmonicSieveBlock::InitThisConfig_impl(bool check, bool quiet, bool& ok)
{
  inherited::InitThisConfig_impl(check, quiet, ok);
  DataBuffer* src_buff = in_block.GetBuffer();
  if (!src_buff) return;
//  float_Matrix* in_mat = &src_buff->mat;
  // note: we can support any number of chans, fields, or items
  // output: chan: pitch; val: octave
  
  if (CheckError((src_buff->vals > 1), quiet, ok,
    "HarmonicSieveBlock only supports single val input"))
    return;
  
  if (check) return;
  
  // get cpo if enabled
  if (chans_per_oct.auto_lookup) {
    chans_per_oct.Lookup(this);
  }
  if (CheckError((chans_per_oct < 1), quiet, ok,
    "chans_per_oct must be >= 1"))
    return;
  cpo_eff = (int)chans_per_oct.chans_per_oct;
  if (CheckError((((2 * on_half_width) + 1) >= cpo_eff), quiet, ok,
    "(2 * on_half_width) + 1 must be less than chans_per_oct"))
    return;

  in_octs = src_buff->chans / cpo_eff;
  // octs must be within input
  if (CheckError((out_octs >= in_octs), quiet, ok,
    "out_octs must be < num octaves available in input"))
    return;
  
  out_buff.fs = src_buff->fs;
  out_buff.fr_dur.Set(src_buff->items, Duration::UN_SAMPLES);
  out_buff.fields = src_buff->fields;
  out_buff.chans = cpo_eff;
  out_buff.vals = out_octs;
  
  CheckMakeFilter(quiet, ok);
}

void HarmonicSieveBlock::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int in_stage, ProcStatus& ps)
{
  if (!src_buff) return;
  float_Matrix* in_mat = &src_buff->mat;
  float_Matrix* out_mat = &out_buff.mat;
  
  const int out_vals = out_mat->dim(VAL_DIM); // octs
  const int out_chans = out_mat->dim(CHAN_DIM); // chans/oct
  
  const int in_items = in_mat->dim(ITEM_DIM);
  const int in_fields = in_mat->dim(FIELD_DIM);
  const int in_chans = in_mat->dim(CHAN_DIM); 
  const int out_stage = out_buff.stage;
  
  // we process in terms of the output
  // out_v is the output octave, 0..out_octs-1
  // out_chan is the pitch (0..octave-1)
  for (int f = 0; (/*(ps == PS_OK) && */(f < in_fields)); ++f)
  for (int i = 0; (/*(ps == PS_OK) && */(i < in_items)); ++i)
  for (int out_v = 0; (/*(ps == PS_OK) && */(out_v < out_vals)); ++out_v) {
    int max_harm = (in_octs - out_v) - 1;
  for (int out_ch = 0; out_ch < out_chans; ++out_ch)
  {
    // input fundamental (f0)
    int in_fund = (out_v * cpo_eff) + out_ch;
    float fund_val = in_mat->FastEl(0, in_fund, f, i, in_stage);
    double out_val = 0.0f; 
    float gain = 0.0f; // successively 1, 2, 3, etc. so we divide
    // harmonic (0-based) goes from 1 (2*f0) to max num avail
    for (int harm = 1; harm <= max_harm; ++harm) {
      gain += 1.0f;
      float sieve = 0; 
      // offs from harm*f0 
      float val = 0;
      for (int filt_idx = 0; filt_idx < cpo_eff; ++filt_idx) {
        int offs = filt_idx + on_half_width + 1;
        int in_ch = in_fund + (harm * cpo_eff) + offs;
        if (in_ch >= in_chans) { // overflow: last +offs of last harm
          // just add in some of the last val again, for balance
          sieve += val * 0.7f;
          break; 
        }
        val = in_mat->SafeEl(0, in_ch, f, i, in_stage);
        sieve += val * filter.FastEl(filt_idx);
      }
      out_val += sieve ;//* (harm + 1);
    }
    if (gain > 0.0f) // guard against doing nothing!
      out_val /= gain; // normalize
    out_val += fund_val; // note: don't normalize fund
#ifdef DEBUG
    out_mat->Set(out_val, out_v, out_ch, f, i, out_stage);
#else
    out_mat->FastEl(out_v, out_ch, f, i, out_stage) = out_val;
#endif
  }}
  // advance stage pointer, and notify clients
  if (out_buff.NextIndex())
    NotifyClientsBuffStageFull(&out_buff, 0, ps);
}

void HarmonicSieveBlock::CheckMakeFilter(bool quiet, bool& ok) {
  filter.SetGeom(1, cpo_eff);
  filter.Clear(); // for successive calls
  const int off_width = cpo_eff - ((2 * on_half_width) + 1);
  // indexes go from: fund + on_hw+1 .. 2*fund + on_hw
  // "on" portion -- normalized to be 1 sd = +- 1/6 octave
  for (int offs = -on_half_width; offs <= on_half_width; ++offs) {
    float x = ((float)offs / cpo_eff) * 6.0f;
    float val = taMath_float::gauss_den(x);
    filter.Set(val, (off_width + on_half_width + offs));
  }
  // normalize that to sum=1, then we'll set others to 1/off_wdith
  taMath_float::vec_norm_sum(&filter); // make sure sums to 1.0
  const float val = -1.0f / off_width;
  for (int offs = 0; offs < off_width; ++offs) {
    filter.Set(val, offs);
  }
}
