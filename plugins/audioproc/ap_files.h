#ifndef AP_FILES_H
#define AP_FILES_H

#include "audioproc.h"

class AUDIOPROC_API AudioCodec: public taOBase {
  // abstract codec -- .cpp file defines one per library 
INHERITED(taOBase) 
public:
  enum SoundFormat { // format types, based on those from sndfile
    SF_AUTO		= 0, // determine from context, ex. file extension
    SF_FORMAT_WAV	= 0x010000, /* Microsoft WAV format (little endian default) */
    SF_FORMAT_AIFF	= 0x020000, /* Apple/SGI AIFF format (big endian) */
    SF_FORMAT_AU	= 0x030000, /* Sun/NeXT AU format (big endian)   */
#ifndef __MAKETA__
    SF_SNDFILE_MIN	= SF_FORMAT_WAV,
    SF_SNDFILE_MAX	= SF_FORMAT_AU,
#endif
    SF_FORMAT_MP3	= 0x100000, /* mp3 */
  };
  
  enum ValueType { // value types, only used for writing files
    VT_AUTO		= 0, // typically we use FLOAT
    VT_FORMAT_PCM_S8	= 0x0001,       /* Signed 8 bit data */
    VT_FORMAT_PCM_16	= 0x0002,       /* Signed 16 bit data */
    VT_FORMAT_PCM_24	= 0x0003,       /* Signed 24 bit data */
    VT_FORMAT_PCM_32	= 0x0004,       /* Signed 32 bit data */

    VT_FORMAT_PCM_U8	= 0x0005,       /* Unsigned 8 bit data (WAV and RAW only) */

    VT_FORMAT_FLOAT	= 0x0006,       /* 32 bit float data */
    VT_FORMAT_DOUBLE	= 0x0007,       /* 64 bit float data */
  };
  
  static AudioCodec*	New(SoundFormat sf); // make the correct impl, based on format
  static SoundFormat	ExtensionToFormat(String ext);
    // converts an extension into best-guess of format; 0 (auto) means unknown
  static void		MatToFile(float_Matrix* mat, const String& fname,
    AudioCodec::SoundFormat format, int sample_rate, int fields, 
    AudioCodec::ValueType val_type = VT_FORMAT_PCM_16);
    // save the contents of the matrix as a sound file
  static void		TableCellNameToFile(DataTable* tab, const String& col,
    const String& fname,
    AudioCodec::SoundFormat format, int sample_rate, int fields, 
    AudioCodec::ValueType val_type = VT_FORMAT_PCM_16);
    // save the contents of the matrix table cell current row as a sound file
    
  TA_BASEFUNS(AudioCodec)
#ifndef __MAKETA__
public: // interface to implement; props valid after successful open
  enum SeekCode {
    SC_SET,	// relative to beginning
    SC_CUR,	// relative to current
    SC_END	// relative to end
  };
  
  virtual int		fields() const {return 0;} // ie 1 for mono, 2 for stereo
  virtual int		samplerate() const {return 0;}
  
  virtual int		OpenFileRead(const String& fname,
     AudioCodec::SoundFormat format = SF_AUTO) {return -1;}
  virtual int		OpenFileWrite(const String& fname,
     AudioCodec::SoundFormat format, int sample_rate, int fields)
      {return -1;}
  virtual int64_t	ReadData(float* dat, int64_t num) {return 0;}	
  virtual int64_t	WriteData(const float* dat, int64_t num) {return 0;}	
  virtual void		CloseFile() {}
  virtual int64_t	SeekFile(int64_t frames, SeekCode whence) {return -1;}
#endif
private:
  void	Initialize();
  void	Destroy() {}
  SIMPLE_COPY(AudioCodec)
};
TA_SMART_PTRS(AudioCodec)

class AUDIOPROC_API FieldSpec: public taBase {
  // #INLINE defines how file fields get allocated to chans and fields 
INHERITED(taBase) 
public:
  bool			is_auto; // #LABEL_auto determined from file
  short			act; // #CONDEDIT_OFF_auto #MIN_1 actual value
  
  TA_BASEFUNS_LITE(FieldSpec)
private:
  void	Initialize();
  void	Destroy() {}
  SIMPLE_COPY(FieldSpec)
};

class AUDIOPROC_API FileInput: public InputBlock
{ /*  get input from a file
  You can either set the sampling rate and fields manually, if you know what they will be,
  or leave fs set to Auto; but you must have a valid file set so that the fs can be
  determined during Init, since later stages need to configure themselves.
*/
INHERITED(InputBlock) 
public:
  String		fname; // #READ_ONLY #SHOW #SAVE filename in use; can be relative to project
  AudioCodec::SoundFormat format; // #READ_ONLY #SHOW #SAVE sound format
  bool			loop; // if set true, then keep looping input, else set PS_STOP at end
  FieldSpec		vals; // number of vals, typically auto unless using cached Audioproc multi-channel files, in which case you must specify 1 for mono and 2 for stereo files; number of chans are then determined
  FieldSpec		chans; // number of chans, typically auto unless using cached Audioproc multi-channel files with multi-vals, in which case you must specify the number of chans; number of vals are then determined
  FieldSpec		fields; // number of fields, typically auto unless using cached Audioproc multi-channel files with multi-chans or vals, in which case you must specify 1 for mono and 2 for stereo files; number of chans/vals are then determined
  
  String		fnameAbs() const {return afname;} // absolute filename
  
  void 			SetFormat(AudioCodec::SoundFormat format
    = AudioCodec::SF_AUTO);
    // #BUTTON set the format to be used; automatically creates the correct codec; set format to be 0 to lookup based on file extension
  void 			SetFile(const String& fname);
    // #BUTTON set the file to be used
  
  virtual int		OpenFile(); // if no format has been set, get the codec based on ext; open the file, confirm specs, return 0 if ok, else error code
  virtual void		CloseFile();
  
  SIMPLE_LINKS(FileInput);
  TA_BASEFUNS(FileInput)
#ifndef __MAKETA__  
protected:
  AudioCodecPtr		codec;
  String		afname; // absolute version of fname
  bool			done; // set when all data read and not looping -- further reads return 0
  
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		ProcNext_Samples_impl(int n, ProcStatus& ps);

  void 			DecodeFields(int raw_fields, bool check, bool quiet, bool& ok);
  void			GetCodec();
  virtual int		OpenFile_int(); // opens the file, return 0 if ok, else error code
  int64_t		ReadData(float* dat, int64_t num);	
  int64_t		SeekFile(int64_t frames, AudioCodec::SeekCode whence);	
#endif

private:
  void	Initialize();
  void	Destroy();
  SIMPLE_COPY(FileInput)
};


class AUDIOPROC_API FileOutput: public OutputBlock
{ /*  write output from previous stage to a file -- file "channels" are used in following row major order: val, chan, field (so "val" changes most quickly, just like in audiproc buffers)
*/
INHERITED(OutputBlock) 
public:
  enum BlockFlags { // #BITS
    BF_0 = 0, // #IGNORE
    BF_APPEND		= 0x001, // #LABEL_Append default mode is Append, otherwise Overwrite
    BF_RESET_ON_CONFIG	= 0x002, // #LABEL_ResetOnConfig reset data on each init config, otherwise not (program must manage data)
  };
  
  enum OpenMode {
    OM_AUTO, // open file according to default mode of block
    OM_APPEND, // append to existing data, if any
    OM_OVERWRITE, // clear any existing data
  };
  
  BlockFlags		block_flags; // flags that control operation
  String		fname; // filename in use; can be relative to project
  AudioCodec::SoundFormat format; // sound format
  AudioCodec::ValueType value_type; // the value type in the file, not all types can be used, typically we use FLOAT for internal audioproc files, or PCM16 for wav files
  SampleFreq		fs; // #READ_ONLY #SHOW #NO_SAVE the sample frequency of output of this buffer
  
  int			fields_eff;  // #HIDDEN  #READ_ONLY #NO_SAVE number of fields*chans*vals
  
  String		fnameAbs() const {return afname;} // absolute filename
  
  void 			SetFile(const String& fname);
    // #BUTTON set the file to be used
  
  virtual int		OpenFile(OpenMode open_mode = OM_AUTO); // #BUTTON get the codec based on format; open the file, return 0 if ok, else error code
  virtual void		CloseFile(); // #BUTTON 
  
  TA_BASEFUNS(FileOutput)
#ifndef __MAKETA__  
protected:
  AudioCodecPtr		codec;
  String		afname; // absolute version of fname
  bool			done; // set when all data read and not looping -- further reads return 0
  
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		AcceptData_impl(SignalProcBlock* src_blk,
    DataBuffer* src_buff, int buff_index, int stage, ProcStatus& ps); 
  
  void			GetCodec();
  int64_t		WriteData(const float* dat, int64_t num);	
  int64_t		SeekFile(int64_t frames, AudioCodec::SeekCode whence);	
#endif

private:
  void	Initialize();
  void	Destroy();
  SIMPLE_COPY(FileOutput)
};




#endif
