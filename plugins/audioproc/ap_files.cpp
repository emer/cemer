#include "ap_files.h"

#include <QFileInfo>

#include <sndfile.hh> // C++ api

// forwards
class SndfileCodec;


class SndfileCodec: public AudioCodec {
  // codec for sndfile lib
INHERITED(AudioCodec) 
public:
  SndfileCodec();
  ~SndfileCodec();

public:
  override int		fields() const 
    {return (sfh) ? sfh->channels() : 0;}
  override int		samplerate() const 
    {return (sfh) ? sfh->samplerate() : 0;}
  
  override int		OpenFileRead(const String& fname,
     AudioCodec::SoundFormat format);
  override int		OpenFileWrite(const String& fname,
     AudioCodec::SoundFormat format, int sample_rate, int fields);
  override int64_t	ReadData(float* dat, int64_t num);	
  override int64_t	WriteData(const float* dat, int64_t num);	
  override void		CloseFile();
  override int64_t	SeekFile(int64_t frames, SeekCode whence);
  
protected:
  SndfileHandle*	sfh;
};


//////////////////////////////////
//  AudioCodec			//
//////////////////////////////////

AudioCodec::SoundFormat AudioCodec::ExtensionToFormat(String ext) {
  ext.downcase();
  if (ext == "mp3")
    return SF_FORMAT_MP3;
  else if (ext == "wav")
    return SF_FORMAT_WAV;
  else if ((ext == "aiff") || (ext == "aif"))
    return SF_FORMAT_AIFF;
  else if (ext == "au")
    return SF_FORMAT_AU;
    
  return SF_AUTO;
}

AudioCodec* AudioCodec::New(SoundFormat sf) {
  if ((sf >= SF_SNDFILE_MIN) && (sf <= SF_SNDFILE_MAX))
    return new SndfileCodec;
  
  taMisc::Warning("Could not create a codec! for SoundFormat:", String(sf));
  return NULL; 
}

void AudioCodec::Initialize() {
}


//////////////////////////////////
//  SndfileCodec		//
//////////////////////////////////

SndfileCodec::SndfileCodec() {
  sfh = NULL;
}
SndfileCodec::~SndfileCodec() {
  CloseFile();
}

void SndfileCodec::CloseFile() {
  if (sfh) {
    delete sfh;
    sfh = NULL;
  }
}

int SndfileCodec::OpenFileRead(const String& fname,
     AudioCodec::SoundFormat format)
{
  CloseFile();
  int mode = SFM_READ;
  format = (AudioCodec::SoundFormat) (format | SF_ENDIAN_CPU | SF_FORMAT_FLOAT);
  sfh = new SndfileHandle(fname, mode, format);
  // note: issue can arise where sfh->p is null, but the error routine
  // doesn't check for this, so we get a crash
  if (!sfh || !(bool)(*sfh)) {
    TestError(true, "OpenFileRead", "Unknown sndfile error, (no handle)"); 
    return -1;
  }
  int rval = sfh->error();
  TestError((rval != 0), "OpenFileRead", sfh->strError()); 
  return rval;
}

int SndfileCodec::OpenFileWrite(const String& fname,
     AudioCodec::SoundFormat format, int sample_rate, int fields)
{
  CloseFile();
  int mode = SFM_WRITE;
  sfh = new SndfileHandle(fname, mode, format, fields, sample_rate);
  // note: issue can arise where sfh->p is null, but the error routine
  // doesn't check for this, so we get a crash
  // note: the (bool) trick didn't seem to be working, at least on Kubuntu64
  if (!sfh || (sfh->refCount() == 0)) {
    TestError(true, "OpenFileWrite", "Unknown sndfile error, (no handle)"); 
    return -1;
  }
  int rval = sfh->error();
  TestError((rval != 0), "OpenFileWrite", sfh->strError()); 
  return rval;
}


int64_t SndfileCodec::ReadData(float* dat, int64_t num) {
  if (!sfh) return 0;
  return sfh->read(dat, num);
}	

int64_t SndfileCodec::SeekFile(int64_t frames, SeekCode whence_) {
  if (!sfh) return -1;
  int whence = SEEK_SET;
  switch (whence_) {
  case SC_SET: whence = SEEK_SET; break;
  case SC_CUR: whence = SEEK_CUR; break;
  case SC_END: whence = SEEK_END; break;
  }
  return sfh->seek(frames, whence);
}

int64_t SndfileCodec::WriteData(const float* dat, int64_t num) {
  if (!sfh) return 0;
  return sfh->write(dat, num);
}	


//////////////////////////////////
//  FieldSpec			//
//////////////////////////////////

void FieldSpec::Initialize() {
  is_auto = true;
  act = 1;
}

//////////////////////////////////
//  FileInput			//
//////////////////////////////////

void FileInput::Initialize() {
  format = AudioCodec::SF_AUTO;
  loop = false;
  done = true;
}

void FileInput::Destroy() {
  CloseFile();
}

void FileInput::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CloseFile();
  codec = NULL;
  afname = _nilString;
  QFileInfo fi(fname);
  if (fi.exists())
    afname = fi.absoluteFilePath();
}

void FileInput::GetCodec() {
  if (codec) return;
  if (format == 0) {
    String ext = fname;
    while (ext.contains('.'))
      ext = ext.after('.');
    format = AudioCodec::ExtensionToFormat(ext);
  }
  codec = AudioCodec::New(format);
}

void FileInput::DecodeFields(int raw_fields, bool check, bool quiet, bool& ok)
{ //note: some values supposedly shouldn't be possible, but we check robustly...
  if (fields.is_auto) {
    // grab up to 2
    fields.act = MIN(raw_fields, 2);
  }
  if (CheckError((fields.act < 1), quiet, ok,
    "fields.act was <= 0!"))
    return;
  div_t qr = div(raw_fields, fields.act);
  if (CheckError((qr.rem != 0), quiet, ok,
    "fields in the file not evenly divisible by requested fields!"))
    return;
  
  // first speculatively enforce 1 for vals if auto -- it all goes to chans otherwise
  if (vals.is_auto) {
    vals.act = 1;
  }
  if (chans.is_auto) {
    // divide remaining degs of free by requested vals
    qr = div(qr.quot, vals.act);
    if (CheckError((qr.rem != 0), quiet, ok,
      "fields in the file not evenly divisible by requested fields*vals!"))
      return;
    chans.act = qr.quot;
  } else {
    // otherwise, reduce dfs by requested chans
    qr = div(qr.quot, chans.act);
    if (CheckError((qr.rem != 0), quiet, ok,
      "fields in the file not evenly divisible by requested fields*chans!"))
      return;
  }
  // finally, vals
  if (vals.is_auto) {
    // what remains...
    vals.act = qr.quot;
  } else {
    // otherwise, need to verify it is as expected
    qr = div(qr.quot, chans.act);
    if (CheckError((vals.act != qr.quot), quiet, ok,
      "fields in the file not evenly divisible by requested fields*chans*vals!"))
      return;
  }
}
  
void FileInput::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  // if we need_config, then we will need a valid file to open to get it from
  bool need_config = ((fs.fs_val == SampleFreq::SF_AUTO) ||
    fields.is_auto || chans.is_auto || vals.is_auto);
  if (check) {
    // note: we don't do the config yet, just make sure we can
    if (need_config) {
      QFileInfo fi(fname);
      if (CheckError((!fi.exists()), quiet, ok,
        "File '" + fname + "' does not exist -- you need to either set fs and fields,"
        "or have a valid sound file.")) return;
    }
  }
  
  if (check) return;
  
  GetCodec();
  if (need_config) {
    // open the file
    if (CheckError(((OpenFile_int() != 0)), quiet, ok,
        "Could not open codec for reading")) return;
    // get the sample rate etc.
    fs.SetCustom(codec->samplerate());
    DecodeFields(codec->fields(), check, quiet, ok);
    CloseFile();
  }
  if (ok) {
    out_buff.vals = vals.act;
    out_buff.chans = chans.act;
    out_buff.fields = fields.act;
  }
  out_buff.fs = fs;
}

void FileInput::CloseFile() {
  if (codec) {
    codec->CloseFile();
  }
}

int FileInput::OpenFile()
{
  done = true;
  if (!codec) {
    GetCodec();
    if (!codec) return -1;
  }
  int rval = codec->OpenFileRead(afname, format);
  bool specs_ok = ((fs.fs_val == SampleFreq::SF_AUTO) || ((int)fs.fs_act == codec->samplerate()));
  if (TestError((!specs_ok), "OpenFile",
    "Previously set fs does not match this file:", fname, " (use FS_AUTO)")) 
  {
    CloseFile();
    rval = -1;
  } else {
    // check that fields/chans jives with that previously set
    DecodeFields(codec->fields(), false, true, specs_ok);
    if (specs_ok)
      specs_ok = ((out_buff.fields == fields.act) && (out_buff.chans == chans.act)
        && (out_buff.vals == vals.act));
    if (TestError((!specs_ok), "OpenFile",
      "Previously set fields/chans/vals does not match this file:", fname)) 
    {
      CloseFile();
      rval = -1;
    }
  }
  if (rval == 0)
    done = false;
  return rval;
}

int FileInput::OpenFile_int()
{
  GetCodec();
  if (!codec) return -1;
  return codec->OpenFileRead(afname, format);
}

int64_t FileInput::ReadData(float* dat, int64_t num) {
  if (!codec) return 0;
  return codec->ReadData(dat, num);
}	

int64_t FileInput::SeekFile(int64_t frames, AudioCodec::SeekCode whence) {
  if (!codec) return -1;
  return codec->SeekFile(frames, whence);
}	
void FileInput::SetFormat(AudioCodec::SoundFormat format_)
{
  if (format == format_) return;
  format = format_;
  UpdateAfterEdit();
}

void FileInput::SetFile(const String& fname_)
{
  fname = fname_;
  String ext = fname_;
  while (ext.contains('.'))
    ext = ext.after('.');
  AudioCodec::SoundFormat tformat = AudioCodec::ExtensionToFormat(ext);
  // if format not set, just set it, otherwise verify and issue warning
  if (format == 0) {
    format = tformat;
  } else {
    TestError((format != tformat), "SetFile",
      "The request format does not match the filename extension!");
  }
  UpdateAfterEdit();
}

void FileInput::ProcNext_Samples_impl(int n, ProcStatus& ps) {
  const int buff_index = 0;//clarity
  const int fields = out_buff.fields;
  int64_t tot = n * fields;
  float* buf = new float[tot]; 
  bool no_data = false; // have to guard against empty file
  while (tot > 0) {
    // if done (ie not looping) but still more requested, then output 0
    if (done || no_data) {
      for (int i = 0; ((ps == PS_OK) && (i < tot)); ++i, samples += 1)
      for (int f = 0; ((ps == PS_OK) && (f < fields)); ++f) {
        out_buff.mat.Set(0.0f, 0, 0, f, out_buff.item, out_buff.stage);
        if (out_buff.NextIndex()) {
          NotifyClientsBuffStageFull(&out_buff, buff_index, ps);
        }
      }
      tot = 0;
    } else { // not done: read 
      int64_t act = ReadData(buf, tot) / fields; // act in items
      // get all the data, then copy to output buffer
      //chans, vals = 1
      for (int i = 0; ((ps == PS_OK) && (i < act)); ++i, samples += 1)
      for (int f = 0; ((ps == PS_OK) && (f < fields)); ++f) {
        float out_val = buf[i*fields + f] * gain;
        out_buff.mat.Set(out_val, 0, 0, f, out_buff.item, out_buff.stage);
        if (out_buff.NextIndex()) {
          NotifyClientsBuffStageFull(&out_buff, buff_index, ps);
        }
      }
      if (act == 0) {
        no_data = true; // will shunt us to the dummy output next iter
      }
      if (act < tot) {
        if (loop) {
          if (TestError((SeekFile(0, AudioCodec::SC_SET) < 0), "ProcNext_Samples_impl",
            "Seek of audio file failed during looping")) 
          {
            ps = PS_ERROR;
          }
        } else { // no looping, so done
          done = true;
        }
      }
      tot -= act;
    }
  }
  delete[] buf;
  // check if we ran out of data
  if ((ps == PS_OK) && (done))
    ps = PS_STOP;
}


//////////////////////////////////
//  FileOutput			//
//////////////////////////////////

void FileOutput::Initialize() {
  block_flags = BF_0;
  format = AudioCodec::SF_AUTO;
  value_type = AudioCodec::VT_FORMAT_FLOAT;
  fields_eff = 1;
}

void FileOutput::Destroy() {
  CloseFile();
}

void FileOutput::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  CloseFile();
  codec = NULL;
  afname = _nilString;
  if (fname.nonempty()) {
    QFileInfo fi(fname);
    afname = fi.filePath();
  }
}

void FileOutput::AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps)
{
  if (!src_buff) return; // ex. we can't mon InputBlockSet
  float_Matrix* mat = &src_buff->mat;
  if (!mat || !codec) return;
  
  // insure at least one dim, and has some data
  if ((mat->dims() < 1) || (mat->dim(0) == 0)) {return;}
  
  // note: because we write in exact same val/chan/field order, we can just pass
  // our own "unsafe slice" as the data
  for (int i = 0; ((ps == PS_OK) && (i < mat->dim(ITEM_DIM))); ++i) {
    float& dat = mat->FastEl(0, 0, 0, i, stage);
    WriteData(&dat, fields_eff);
  }
} 


void FileOutput::GetCodec() {
  if (codec) return;
  if (format == 0) {
    String ext = fname;
    while (ext.contains('.'))
      ext = ext.after('.');
    format = AudioCodec::ExtensionToFormat(ext);
  }
  codec = AudioCodec::New(format);
}

void FileOutput::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  if (check) return; // nothing else to do on check
  
  //note: no mon_data is not an error, we just do nothing
  DataBuffer* db = in_block.GetBuffer();
  if (!db) return;
  
  
  // set fs
  // if it ain't an integer, it won't work with sndfile!
  float fs_act = db->fs;
  if (CheckError((fs_act <= 0), quiet, ok,
    "sample rate of input is not set")) return;
  if (CheckError((fs_act != (float)((int)fs_act)), quiet, ok,
    "sample rate of input is not an integer")) return;
  
  if (check) return;
  
  fs = db->fs;
  
  // eff fields
  fields_eff = db->fields * db->chans * db->vals;
  
  GetCodec();
}

void FileOutput::CloseFile() {
  if (codec) {
    codec->CloseFile();
  }
}

int FileOutput::OpenFile(FileOutput::OpenMode open_mode)
{
  if (open_mode == OM_AUTO)
    open_mode = (block_flags & BF_APPEND) ? OM_APPEND : OM_OVERWRITE;
  if (!codec) {
    GetCodec();
    if (!codec) return -1;
  }
  int tformat = format;
  if (value_type == 0)
    tformat |= AudioCodec::VT_FORMAT_FLOAT;
  else tformat |= value_type;
  int rval = codec->OpenFileWrite(afname, 
    (AudioCodec::SoundFormat)tformat, (int)fs.fs_act, fields_eff);
  if (TestError((rval != 0), "OpenFile",
    "Could not open:", fname, " (error: ", String(rval), ")")) 
  {
    CloseFile();
    rval = -1;
  }
  int64_t seek_res = -1;
  if (open_mode == OM_APPEND) {
    seek_res = codec->SeekFile(0, AudioCodec::SC_END);
    if (TestError((seek_res < 0), "OpenFile",
      "Could not seek to end to append for:", fname))
      rval = -1;
  } else {
    seek_res = codec->SeekFile(0, AudioCodec::SC_SET);
    if (TestError((seek_res < 0), "OpenFile",
      "Could not seek to 0 to overwrite for:", fname))
      rval = -1;
  }
  return rval;
}

int64_t FileOutput::WriteData(const float* dat, int64_t num) {
  if (!codec) return 0;
  return codec->WriteData(dat, num);
}	

int64_t FileOutput::SeekFile(int64_t frames, AudioCodec::SeekCode whence) {
  if (!codec) return -1;
  return codec->SeekFile(frames, whence);
}	

void FileOutput::SetFile(const String& fname_)
{
  CloseFile();
  fname = fname_;
  UpdateAfterEdit();
}
