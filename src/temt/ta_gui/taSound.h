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

#ifndef taSound_h
#define taSound_h 1

// parent includes:
#include <taNBase>

// member includes:
#ifndef __MAKETA__
#if (QT_VERSION >= 0x050000)
#include <QAudioBuffer>
#else
class QAudioBuffer {
};
#endif
#else
class QAudioBuffer; //
#endif

// declare all other types mentioned but not required to include:
class taSound_QObj;
class float_Matrix;


taTypeDef_Of(taSound);

class TA_API taSound : public taNBase {
  // #STEM_BASE ##CAT_Audio represents a sound in TA accessible way (interfaces with Qt5 MultiMedia Audio classes -- not avail in Qt4) -- stores sound as QAudioBuffer, can convert to/from float_Matrix
friend class taSound_QObj;
INHERITED(taNBase)
public:
  enum SoundSampleType { // should follow QAudioFormat::SampleType
    Unknown	= 0,	// Not Set
    SignedInt	= 1,	// Samples are signed integers
    UnSignedInt	= 2,	// Samples are unsigned intergers
    Float	= 3,	// Samples are floats
  };

  enum Endian {
    BigEndian	 = 0,	// Samples are big endian byte order
    LittleEndian = 1,   // Samples are little endian byte order
  };

  bool          IsValid() const;
  // #CAT_Sound is this sound valid??
  
  int           ByteCount() const;
  // #CAT_Sound number of bytes in this sound's buffer's data
  int           FrameCount() const;
  // #CAT_Sound number of frames (samples in each channel) in this sound's buffer's data
  int           SampleCount() const;
  // #CAT_Sound number of total samples (frames * channels) in this sound's buffer's data
  int64_t       Duration() const;
  // #CAT_Sound total duration of sound in microseconds (depends on sample rate and size)
  
  int           SampleRate() const;
  // #CAT_Sound sample rate in Hz (e.g., 16000 (lo-fi), 22050 (speech), 44100 (CD), 48000 (DAT, DVD))
  int           ChannelCount() const;
  // #CAT_Sound number of channels encoded -- these are stored interleaved (inner loop) in the raw data -- mono = 1, stereo = 2, etc
  int           SampleSize() const;
  // #CAT_Sound number of bits per sample (8, 16, 24, 32 typically)
  SoundSampleType SampleType() const;
  // #CAT_Sound how are the samples encoded (int, uint, float)
  Endian        ByteOrder() const;
  // #CAT_Sound byte order of integer values 

  int BytesForDuration(int64_t duration) const;
  // #CAT_Sound Returns the number of bytes required for this audio format for duration microseconds -- Note that some rounding may occur if duration is not an exact fraction of the SampleRate()

  int BytesForFrames(int frameCount) const;
  // #CAT_Sound Returns the number of bytes required for frameCount frames of this format -- a frame is a single sample for each channel
    
  int BytesPerFrame() const;
  // #CAT_Sound Returns the number of bytes required to represent one frame (a sample in each channel) in this format

  int64_t DurationForBytes(int bytes) const;
  // #CAT_Sound Returns the number of microseconds represented by bytes in this format
    
  int64_t DurationForFrames(int frameCount) const;
  // #CAT_Sound Return the number of microseconds represented by frameCount frames (a sample in each channel) in this format.

  int FramesForBytes(int byteCount) const;
  // #CAT_Sound Returns the number of frames represented by byteCount in this format.

  int FramesForDuration(int64_t duration) const;
  // #CAT_Sound Returns the number of frames required to represent duration microseconds in this format -- Note that some rounding may occur if duration is not an exact fraction of the SampleRate().
  
  virtual bool  InitBuffer(int frame_count, int sample_rate = 48000, int channels = 1,
                           int sample_size = 16, SoundSampleType sample_type = SignedInt,
                           Endian byte_order = LittleEndian);
  // #CAT_Sound initialize a new sound buffer for this sound, using given parameters -- number of total frames (samples), sample rate in Hz (e.g., 16000 (lo-fi), 22050 (speech), 44100 (CD), 48000 (DAT, DVD)), number of channels, sample size (8, 16, 24, 32 bits typically), sample type format, and byte order
  
  virtual bool	LoadSound(const String& fname);
  // #BUTTON #CAT_File #FILE_DIALOG_LOAD #EXT_wav,ogg load a sound from given file name (leave file name blank to get a chooser)
  
  virtual bool	SaveSound(const String& fname);
  // #BUTTON #CAT_File #FILE_DIALOG_SAVE #EXT_wav save a sound to given file name (leave file name blank to get a chooser) -- only saves to .wav files, only works for 16 or 32 bit sample size, uses existing sound data to determine format -- this only works with the optional libsndfile library installed

  virtual bool  PlaySound(const String& device_name = "");
  // #BUTTON play the sound to the given device name (empty for default) -- use ListSoundDevices to find a list of valid device names

  virtual String ListSoundDevices();
  // #BUTTON #USE_RVAL provide a comma-separated list of available sound devices
  
  virtual float	GetSample_frame(int frame, int channel = 0);
  // #CAT_Sound get the sample value for given frame, channel as a floating point number in normalized range -1..1 (unless stored as a float natively, in which case it is not guaranteed to be normalized)
  virtual float	GetSample_msec(int64_t microsec, int channel = 0);
  // #CAT_Sound get the sample value for given microsecond position, channel as a floating point number in normalized range -1..1 (unless stored as a float natively, in which case it is not guaranteed to be normalized)

  const void*   SoundData();
  // #IGNORE #CAT_Sound pointer to the raw sound data -- for READ ONLY purposes!  use with care!
  
  virtual bool	SoundToMatrix(float_Matrix& sound_data, int channel = -1);
  // #CAT_Data convert sound data to floating point Matrix with normalized -1..1 values (unless sound is stored as a float natively, in which case it is not guaranteed to be normalized) -- for use in signal processing routines -- can optionally select a specific channel (formats sound_data as a single-dimensional matrix of frames size), and -1 gets all available channels (formats sound_data as two-dimensional matrix with inner dimension as channels and outer dimension frames

  virtual bool	SoundFromMatrix(const float_Matrix& sound_data, int channel = -1,
                                int sample_rate = 44100, int sample_size = 16,
                                SoundSampleType sample_type = SignedInt,
                                Endian byte_order = LittleEndian);
  // #CAT_Data load sound data from floating point Matrix with normalized -1..1 values -- configures a new audio buffer based on size and channels of matrix (for a 2D matrix) -- can optionally select a specific channel from a multi-channel matrix (-1 uses all channels) -- you can also specify all the other parameters of the audio buffer
  
  QAudioBuffer& GetAudioBuffer() { return q_buf; }
  // #IGNORE get the underlying qt audio buffer
  void  	SetAudioBuffer(const QAudioBuffer& buf) { q_buf = buf; }
  // #CAT_Image #IGNORE set the underlying qt audio buffer


  static float GetFloatAtIdx(const void* buf, int idx, SoundSampleType styp,
                             int samp_size);
  // #IGNORE low-level access routine
  static void  WriteFloatAtIdx(const float& val, void* buf, int idx, SoundSampleType styp,
                               int samp_size);
  // #IGNORE low-level access routine
  
  TA_BASEFUNS(taSound);
protected:
  QAudioBuffer	q_buf;          // the underlying Qt audio buffer
  taSound_QObj* q_obj;          // helper QObject class for signals / slots
    
private:
  void	Copy_(const taSound& cp);
  void  Initialize();
  void  Destroy();
};

#endif // taSound_h
