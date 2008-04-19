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
  
  override int		OpenFile(const String& fname,
     AudioCodec::SoundFormat format);
  override int64_t	ReadData(float* dat, int64_t num);	
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

int SndfileCodec::OpenFile(const String& fname,
     AudioCodec::SoundFormat format)
{
  CloseFile();
  sfh = new SndfileHandle(fname, SFM_READ, 
    (format | SF_ENDIAN_CPU | SF_FORMAT_FLOAT));
  // note: issue can arise where sfh->p is null, but the error routine
  // doesn't check for this, so we get a crash
  if (!sfh || !(bool)(*sfh)) {
    TestError(true, "OpenFile", "Unknown sndfile error, (no handle)"); 
    return -1;
  }
  int rval = sfh->error();
  TestError((rval != 0), "OpenFile", sfh->strError()); 
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

void FileInput::InitThisConfig_impl(bool check, bool quiet, bool& ok) {
  inherited::InitThisConfig_impl(check, quiet, ok);
  
  // if we need_config, then we will need a valid file to open to get it from
  bool need_config = ((fs.fs_val == SampleFreq::SF_AUTO));
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
    out_buff.fields = codec->fields();
    CloseFile();
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
  int rval = codec->OpenFile(afname, format);
  bool specs_ok = ((fs.fs_val == SampleFreq::SF_AUTO) || ((int)fs.fs_act == codec->samplerate()));
  if (TestError((!specs_ok), "OpenFile",
    "Previously set fs does not match this file:", fname, " (use FS_AUTO)")) 
  {
    CloseFile();
    rval = -1;
  } else {
    specs_ok = (out_buff.fields == codec->fields());
    if (TestError((!specs_ok), "OpenFile",
      "Previously set fields does not match this file:", fname)) 
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
  return codec->OpenFile(afname, format);
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
        out_buff.mat.Set(buf[i*fields + f], 0, 0, f, out_buff.item, out_buff.stage);
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
