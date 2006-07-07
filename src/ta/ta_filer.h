// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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


#ifndef ta_filer_h
#define ta_filer_h 1

// NOTE: taFiler replaces the duo of taiGetFile and taFile from 3.x
//	the ui portion of taiGetFile has been transferred to taFiler_impl subclass
//	in ta_filer.cc for no gui, and ta_xxdialog.cc for gui
#include "ta_type.h"

class taFiler; //

enum FilerOperation {
  foOpen,
  foSave,
  foSaveAs,
  foAppend
};

#define taFiler_CreateInstance(dir, filt, comp) \
  taFiler::New(dir, filt, comp)

class TA_API taFiler : public taRefN {
  // ##NO_TOKENS associate this with each file that is managed
friend class taRefN; // for deleting
public:
  enum OpenMode {
    NO_AUTO,			// don't automatically open
    READ,			// auto open in READ mode..
    WRITE,
    APPEND
  };
  
  enum FilerFlags { // #BITS flags to control operations
    NO_FLAGS	= 0,	// #NO_BIT
    CONFIRM_OVERWRITE		= 0x001, // for Save/Append, warn if file already exists
    FILE_MUST_EXIST		= 0x002 // for Save/Append, warn if file already exists
  };

  static int	buf_size;	// size of the buffer for input operations
  static String	last_fname;	// last file name processed
  static String	last_dir;	// last directory name processed

  static taFiler* 	New(const String& dir = ".", const String& filter = "", bool compress = false); // creates instance
  
  String	filter;
  String	dir;
  String	fname;
  bool		compress;	// true if file should be auto-compressed
  bool		select_only;	// file is to be selected only (not opened)
  ifstream*	ifstrm;		// #READ_ONLY
  char*		iofbuf;		// #IGNORE buffer for input/output
//obs  pid_t		compress_pid;	// #IGNORE process id of compress child proc
  ofstream*	ofstrm;		// #READ_ONLY
  fstream*	fstrm;		// #READ_ONLY
  istream*	istrm;		// #READ_ONLY
  ostream*	ostrm;		// #READ_ONLY
  bool		open_file;	// #READ_ONLY true if there is an open file somewhere
  bool		file_selected;	// #READ_ONLY true if a file was selected last time..
  OpenMode	mode;		// #HIDDEN auto-open file in this mode (for auto)

  virtual istream*	open_read();
  virtual ostream*	open_write();
  virtual ostream* 	open_append();
  virtual bool		open_write_exist_check(); // returns true if file already exists for writing

  virtual istream*	Open(const char* nm = NULL, bool no_dlg = false);
  // to get a file for reading (already exists)
  virtual ostream*	Save(const char* nm = NULL, bool no_dlg = false);
  // to save to an existing file
  virtual ostream*	SaveAs(const char* nm = NULL, bool no_dlg = false);
  // to save with a new file
  virtual ostream*	Append(const char* nm = NULL, bool no_dlg = false);
  // to save a file for appending (already exists)
  virtual void		Close();	// close the stream

  virtual void		AutoOpen();		// auto-open a file based on mode
  virtual bool		IsOpen();		// check if file is open

  virtual bool		GetFileName(String& fname, FilerOperation filerOperation,
    int filer_flags = 0); // gui-dependent routine to get filename from user
  virtual void		FixFileName(); // make sure suffix is right
  virtual void		GetDir();      // get directory from file name


  virtual taFiler& operator=(const taFiler& cp); // copy operator used extensively in src/pdp files (copies very few props)
protected:
  taFiler(const String& dir_, const String& filter_, bool compress_);
  virtual ~taFiler();
private:
  taFiler(const taFiler& cp); // #IGNORE copy construction not allowed
  void	Init(const String& dir_ = ".", const String& filter_ = "", bool compress_ = false); // same as constructor
};


#endif // ta_filer_h
