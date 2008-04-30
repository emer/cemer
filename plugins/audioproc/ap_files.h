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
  
  static AudioCodec*	New(SoundFormat sf); // make the correct impl, based on format
  static SoundFormat	ExtensionToFormat(String ext);
    // converts an extension into best-guess of format; 0 (auto) means unknown
  
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
  
  virtual int		OpenFile(const String& fname,
     AudioCodec::SoundFormat format = SF_AUTO) {return -1;}
  virtual int64_t	ReadData(float* dat, int64_t num) {return 0;}	
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
  int			act; // #CONDEDIT_OFF_auto #MIN_1 actual value
  
  TA_BASEFUNS(FieldSpec)
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
  FieldSpec		num_fields; // number of fields, typically auto unless using cached Audioproc multi-channel files, in which case you must specify 1 for mono and 2 for stereo files; number of chans are then determined
  
  String		fnameAbs() const {return afname;} // absolute filename
  
  void 			SetFormat(AudioCodec::SoundFormat format
    = AudioCodec::SF_AUTO);
    // #BUTTON set the format to be used; automatically creates the correct codec; set format to be 0 to lookup based on file extension
  void 			SetFile(const String& fname);
    // #BUTTON set the file to be used
  
  virtual int		OpenFile(); // if no format has been set, get the codec based on ext; open the file, confirm specs, return 0 if ok, else error code
  virtual void		CloseFile();
  
  TA_BASEFUNS(FileInput)
#ifndef __MAKETA__  
protected:
  AudioCodecPtr		codec;
  String		afname; // absolute version of fname
  bool			done; // set when all data read and not looping -- further reads return 0
  
  override void		UpdateAfterEdit_impl();
  override void 	InitThisConfig_impl(bool check, bool quiet, bool& ok);
  override void		ProcNext_Samples_impl(int n, ProcStatus& ps);
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



#endif
