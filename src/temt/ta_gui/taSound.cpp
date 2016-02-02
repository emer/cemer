// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "taSound.h"

TA_BASEFUNS_CTORS_DEFN(taSound);


#if (QT_VERSION >= 0x050000)

#include <taFiler>
#include <float_Matrix>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <taSound_QObj>

#ifdef TA_SNDFILE
#include <sndfile.hh>
#endif

void taSound::Initialize() {
  q_obj = NULL;
}

void taSound::Destroy() {
  // should be in cutlinks?
  if(q_obj) {
    delete q_obj;
    q_obj = NULL;
  }
}

void taSound::Copy_(const taSound& cp) {
  q_buf = cp.q_buf;
}

bool taSound::IsValid() const {
  return q_buf.isValid();
}

int taSound::ByteCount() const {
  return q_buf.byteCount();
}

int taSound::FrameCount() const {
  return q_buf.frameCount();
}

int taSound::SampleCount() const {
  return q_buf.sampleCount();
}

int64_t taSound::Duration() const {
  return q_buf.duration();
}

int taSound::SampleRate() const {
  return q_buf.format().sampleRate();
}

int taSound::ChannelCount() const {
  return q_buf.format().channelCount();
}

int taSound::SampleSize() const {
  return q_buf.format().sampleSize();
}

taSound::SoundSampleType taSound::SampleType() const {
  return (SoundSampleType)q_buf.format().sampleType();
}

taSound::Endian taSound::ByteOrder() const {
  return (Endian)q_buf.format().byteOrder();
}

int taSound::BytesForDuration(int64_t duration) const {
  return q_buf.format().bytesForDuration(duration);
}

int taSound::BytesForFrames(int frameCount) const {
  return q_buf.format().bytesForFrames(frameCount);
}
    
int taSound::BytesPerFrame() const {
  return q_buf.format().bytesPerFrame();
}

int64_t taSound::DurationForBytes(int bytes) const {
  return q_buf.format().durationForBytes(bytes);
}

int64_t taSound::DurationForFrames(int frameCount) const {
  return q_buf.format().durationForFrames(frameCount);
}

int taSound::FramesForBytes(int byteCount) const {
  return q_buf.format().framesForBytes(byteCount);
}

int taSound::FramesForDuration(int64_t duration) const {
  return q_buf.format().framesForDuration(duration);
}

bool taSound::InitBuffer(int frame_count, int sample_rate, int channels,
                         int sample_size, SoundSampleType sample_type,
                         Endian byte_order) {
  QAudioFormat fmt;
  fmt.setSampleRate(sample_rate);
  fmt.setChannelCount(channels);
  fmt.setSampleSize(sample_size);
  fmt.setSampleType((QAudioFormat::SampleType)sample_type);
  fmt.setByteOrder((QAudioFormat::Endian)byte_order);
  fmt.setCodec("audio/pcm");    // only format we support..
  if(TestError(!fmt.isValid(), "InitBuffer", "format is not valid!")) {
    return false;
  }
  q_buf = QAudioBuffer(frame_count, fmt);
  if(TestError(!q_buf.isValid(), "InitBuffer", "initialization failed (but format was valid) -- something strange..")) {
    return false;
  }
  return true;
}
  
bool taSound::LoadSound(const String& fname) {
#ifdef TA_SNDFILE
  int read_fmt = SF_ENDIAN_CPU; // force cpu endian
  SndfileHandle* sfh = new SndfileHandle(fname, SFM_READ, read_fmt);
  if (!sfh || !(bool)(*sfh)) {
    TestError(true, "LoadSound", "Unknown sndfile error, (no handle)"); 
    return false;
  }
  int rval = sfh->error();
  if(TestError((rval != 0), "LoadSound", sfh->strError())) {
    delete sfh;
    return false;
  }

  sf_count_t frame_count = sfh->frames();
  if(TestError(frame_count <= 0, "LoadSound", "no frames to load")) {
    delete sfh;
    return false;
  }
  int channels = sfh->channels();
  int sample_rate = sfh->samplerate();
  int fullfmt = sfh->format();
  int subfmt = fullfmt & SF_FORMAT_SUBMASK;

  int sample_size = 16;
  SoundSampleType sample_type = SignedInt;
  switch(subfmt) {
  case SF_FORMAT_PCM_S8:        // smallest value encoded is a short
  case SF_FORMAT_PCM_16:
  case SF_FORMAT_PCM_U8:
    break;
  case SF_FORMAT_PCM_24:        // uses 32bit int
  case SF_FORMAT_PCM_32:
    sample_size = 32;
    break;
  case SF_FORMAT_FLOAT:
    sample_size = 32;
    sample_type = Float;
    break;
  default:
    break;
  }    

  if(!InitBuffer(frame_count, sample_rate, channels, sample_size, sample_type)) {
    delete sfh;
    return false;
  }

  if(sample_size == 16) {
    sfh->readf((short*)q_buf.data(), frame_count);
  }
  else if(sample_type == Float) {
    sfh->readf((float*)q_buf.data(), frame_count);
  }
  else {                        // 24, 32 bit
    sfh->readf((int*)q_buf.data(), frame_count);
  }

  delete sfh;
  
  return true;
#else
  
  if(!q_obj) {
    q_obj = new taSound_QObj(this);
  }
  return q_obj->LoadSound(fname);

#endif
}

bool taSound::SaveSound(const String& fname) {
  if(TestError(!q_buf.isValid(), "SaveSound",
               "Sound is not valid!")) {
    return false;
  }

#ifdef TA_SNDFILE
  int channels = ChannelCount();
  int sample_rate = SampleRate();
  int sample_size = SampleSize();
  SoundSampleType sample_type = SampleType();
  int majorfmt = SF_FORMAT_WAV;
  int subfmt = SF_FORMAT_PCM_16;

  String ext = "wav";
  String use_fname = fname;
  if(fname.contains('.')) {
    ext = fname.after('.',-1);
    ext.downcase();
  }
  if(ext != "wav") {
    TestWarning(true, "SaveSound",
                "only .wav file extensions are supported for saving sound files -- replacing your extension:", ext, "with wav");
    ext = "wav";
    use_fname = fname.before('.',-1) + ext;
  }
  
  if(TestError(!(sample_size == 16 || sample_size == 32), "SaveSound",
               "Only 16 or 32 bit sample size supported for saving")) {
    return false;
  }
  
  if(TestError(!(sample_size == 16 || sample_size == 32), "SaveSound",
               "Only 16 or 32 bit sample size supported for saving")) {
    return false;
  }
  if(TestError(sample_type == UnSignedInt, "SaveSound",
               "unsigned int NOT supported for saving")) {
    return false;
  }
  sf_count_t frame_count = FrameCount();
  if(TestError(frame_count <= 0, "SaveSound", "no frames to save")) {
    return false;
  }
  
  if(sample_size != 16) {
    sample_size = 32;
    if(sample_type == SignedInt) {
      subfmt = SF_FORMAT_PCM_32;
    }
    else {
      subfmt = SF_FORMAT_FLOAT;
    }
  }

  int fmt = majorfmt | subfmt;
  
  SndfileHandle* sfh = new SndfileHandle(use_fname, SFM_WRITE, fmt, channels,
                                         sample_rate);
  if (!sfh || !(bool)(*sfh)) {
    TestError(true, "SaveSound", "Unknown sndfile error, (no handle)"); 
    return false;
  }
  int rval = sfh->error();
  if(TestError((rval != 0), "SaveSound", sfh->strError())) {
    delete sfh;
    return false;
  }

  if(sample_size == 16) {
    sfh->writef((const short*)q_buf.constData(), frame_count);
  }
  else if(sample_type == Float) {
    sfh->writef((const float*)q_buf.constData(), frame_count);
  }
  else {                        // 32 bit int
    sfh->writef((const int*)q_buf.constData(), frame_count);
  }

  delete sfh;
  
  return true;
#else // TA_SNDFILE
  TestError(true, "SaveSound", "Not compiled with the libsndfile library -- not able to save sound files!")) {
  return false;
#endif
}

bool taSound::PlaySound(const String& device_name) {
  if(TestError(!q_buf.isValid(), "PlaySound",
               "Sound is not valid!")) {
    return false;
  }
  if(!q_obj) {
    q_obj = new taSound_QObj(this);
  }
  return q_obj->PlaySound(device_name);
}

String taSound::ListSoundDevices() {
  String rval;
  QList<QAudioDeviceInfo> devs = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  for(int i=0; i<devs.count(); i++) {
    QAudioDeviceInfo ad = devs[i];
    if(i == 0)
      rval = ad.deviceName();
    else
      rval += ", " + ad.deviceName();
  }
  return rval;
}

const void* taSound::SoundData() {
  return q_buf.constData();
}

namespace {
  float GetFloatAtIdx(const void* buf, int idx, taSound::SoundSampleType styp,
                      int samp_size) {
    switch(styp) {
    case taSound::SignedInt: {
      if(samp_size == 32) {
        qint16* dat = (qint16*)buf;
        return ((float)dat[idx] / (float)0x7FFFFFFF);
      }
      else if(samp_size == 24) {
        qint16* dat = (qint16*)buf;
        return ((float)dat[idx] / (float)0x7FFFFF);
      }
      else if(samp_size == 16) {
        const qint16* dat = (qint16*)buf;
        return ((float)dat[idx] / (float)0x7FFF);
      }
      else if(samp_size == 8) {
        const qint8* dat = (qint8*)buf;
        return ((float)dat[idx] / (float)0x7F);
      }
      break;
    }
    case taSound::UnSignedInt: {
      if(samp_size == 16) {
        const quint16* dat = (quint16*)buf;
        return ((float)dat[idx] / (float)0xFFFF);
      }
      else if(samp_size == 8) {
        const quint8* dat = (quint8*)buf;
        return ((float)dat[idx] / (float)0xFF);
      }
      break;
    }
    case taSound::Float: {
      const float* dat = (float*)buf;
      return dat[idx];
      break;
    }
    case taSound::Unknown:
      return 0.0f;
      break;
  }
    return 0.0f;
  }
  
  void WriteFloatAtIdx(const float& val, void* buf, int idx,
                       taSound::SoundSampleType styp, int samp_size) {
    switch(styp) {
    case taSound::SignedInt: {
      if(samp_size == 32) {
        qint16* dat = (qint16*)buf;
        dat[idx] = val * (float)0x7FFFFFFF;
      }
      else if(samp_size == 24) {
        qint16* dat = (qint16*)buf;
        dat[idx] = val * (float)0x7FFFFF;
      }
      else if(samp_size == 16) {
        qint16* dat = (qint16*)buf;
        dat[idx] = val * (float)0x7FFF;
      }
      else if(samp_size == 8) {
        qint8* dat = (qint8*)buf;
        dat[idx] = val * (float)0x7F;
      }
      break;
    }
    case taSound::UnSignedInt: {
      if(samp_size == 16) {
        quint16* dat = (quint16*)buf;
        dat[idx] = val * (float)0xFFFF;
      }
      else if(samp_size == 8) {
        quint8* dat = (quint8*)buf;
        dat[idx] = val * (float)0xFF;
      }
      break;
    }
    case taSound::Float: {
      float* dat = (float*)buf;
      dat[idx] = val;
      break;
    }
    case taSound::Unknown:
      break;
  }
  }    
}

float taSound::GetSample_frame(int frame, int channel) {
  if(TestError(!q_buf.isValid(), "GetSample_frame",
               "Sound is not valid!")) {
    return 0.0f;
  }
  if(TestError(frame >= q_buf.frameCount(), "GetSample_frame",
               "frame index:", String(frame), "exceeds frame count:",
               String(q_buf.frameCount()))) {
    return 0.0f;
  }
  int n_chan = ChannelCount();
  if(TestError(channel >= n_chan, "GetSample_frame",
               "channel index:", String(channel), "exceeds channel count:",
               String(n_chan))) {
    return 0.0f;
  }
  int idx = frame * n_chan + channel;
  SoundSampleType styp = SampleType();
  int samp_size = SampleSize();
  return GetFloatAtIdx(q_buf.constData(), idx, styp, samp_size);
}

float taSound::GetSample_msec(int64_t microsec, int channel) {
  int frame = FramesForDuration(microsec);
  return GetSample_frame(frame, channel);
}

bool taSound::SoundToMatrix(float_Matrix& sound_data, int channel) {
  int n_frm = q_buf.frameCount();
  if(TestError(!q_buf.isValid() || n_frm == 0, "SoundToMatrix",
               "Sound is not valid or has no data!")) {
    return false;
  }
  int n_chan = ChannelCount();
  SoundSampleType styp = SampleType();
  int samp_size = SampleSize();
  const void* buf = q_buf.constData();
  if(channel < 0 && n_chan > 1) {
    sound_data.SetGeom(2, n_chan, n_frm);
    int idx = 0;
    for(int i=0; i < n_frm; i++) {
      for(int c=0; c < n_chan; c++, idx++) {
        sound_data.FastEl2d(c,i) = GetFloatAtIdx(buf, idx, styp, samp_size);
      }
    }
  }
  else {
    sound_data.SetGeom(1, n_frm);
    if(n_chan == 1) {
      for(int i=0; i < n_frm; i++) {
        sound_data.FastEl_Flat(i) = GetFloatAtIdx(buf, i, styp, samp_size);
      }
    }
    else {
      int idx = 0;
      for(int i=0; i < n_frm; i++) {
        sound_data.FastEl_Flat(i) = GetFloatAtIdx(buf, idx + channel, styp, samp_size);
        idx += n_chan;
      }
    }
  }
  return true;
}

bool taSound::SoundFromMatrix(const float_Matrix& sound_data, int channel,
                              int sample_rate, int samp_size,
                              SoundSampleType stype,
                              Endian byte_order) {
  if(TestError(sound_data.size == 0, "SoundFromMatrix",
               "no data in the sound data matrix!")) {
    return false;
  }
  int n_chan = 1;
  if(sound_data.dims() == 2) {
    n_chan = sound_data.dim(0);
  }
  if(TestError(sound_data.dims() > 2, "SoundFromMatrix",
               "matrix must only have up to 2 dimensions (channels, frames)")) {
    return false;
  }
  int n_chan_write = n_chan;
  if(channel >= 0)
    n_chan_write = 1;
  
  int n_frm = sound_data.Frames();

  if(!InitBuffer(n_frm, sample_rate, n_chan_write, samp_size, stype, byte_order)) {
    return false;
  }
    
  void* buf = q_buf.data();
  if(channel < 0 && n_chan > 1) {
    int idx = 0;
    for(int i=0; i < n_frm; i++) {
      for(int c=0; c < n_chan; c++, idx++) {
        WriteFloatAtIdx(sound_data.FastEl2d(c,i), buf, idx, stype, samp_size);
      }
    }
  }
  else {
    if(n_chan == 1) {
      for(int i=0; i < n_frm; i++) {
        WriteFloatAtIdx(sound_data.FastEl_Flat(i), buf, i, stype, samp_size);
      }
    }
    else {
      for(int i=0; i < n_frm; i++) {
        WriteFloatAtIdx(sound_data.FastEl2d(i, channel), buf, i, stype, samp_size);
      }
    }
  }
  return true;
}

#else // QT_VERSION >= 0x050000

void taSound::Initialize() {
  q_obj = NULL;
}

void taSound::Destroy() {
}

void taSound::Copy_(const taSound& cp) {
}

bool taSound::IsValid() const {
  return false;
}

int taSound::ByteCount() const {
  return 0;
}

int taSound::FrameCount() const {
  return 0;
}

int taSound::SampleCount() const {
  return 0;
}

int64_t taSound::Duration() const {
  return 0;
}

int taSound::SampleRate() const {
  return -1;
}

int taSound::ChannelCount() const {
  return -1;
}

int taSound::SampleSize() const {
  return -1;
}

taSound::SoundSampleType taSound::SampleType() const {
  return Unknown;
}

taSound::Endian taSound::ByteOrder() const {
  return LittleEndian;
}


int taSound::BytesForDuration(int64_t duration) const {
  return -1;
}

int taSound::BytesForFrames(int frameCount) const {
  return -1;
}
    
int taSound::BytesPerFrame() const {
  return -1;
}

int64_t taSound::DurationForBytes(int bytes) const {
  return -1;
}

int64_t taSound::DurationForFrames(int frameCount) const {
  return -1;
}

int taSound::FramesForBytes(int byteCount) const {
  return -1;
}

int taSound::FramesForDuration(int64_t duration) const {
  return -1;
}

bool taSound::InitBuffer(int frame_count, int sample_rate, int channels,
                         int sample_size, SoundSampleType sample_type,
                         Endian byte_order) {
  return false;
}
  
bool taSound::LoadSound(const String& fname) {
  return false;
}

bool taSound::SaveSound(const String& fname) {
  return false;
}

bool taSound::PlaySound(const String& device_name) {
  return false;
}

String taSound::ListSoundDevices() {
  return "";
}

const void* taSound::SoundData() {
  return NULL;
}

float taSound::GetSample_frame(int frame, int channel) {
  return 0.0f;
}

float taSound::GetSample_msec(int64_t microsec, int channel) {
  return 0.0f;
}

bool taSound::SoundToMatrix(float_Matrix& sound_data, int channel) {
  return false;
}

bool taSound::SoundFromMatrix(const float_Matrix& sound_data, int channel,
                              int sample_rate, int sample_size,
                              SoundSampleType sample_type,
                              Endian byte_order) {
  return false;
}
  
#endif // QT_VERSION
