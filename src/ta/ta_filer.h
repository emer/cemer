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

/*
  compression
  Compression is controlled by 3 factors:
    comp_req flag -- when set, it acts as follows:
      Load: shows .gz files
      Save: shows the compression checkbox in file dialog
        shows .gz files in the file dialog
      SaveAs: shows the compression checkbox in file dialog
        if proposed filename has gz suffix, 'compress' check is on
    compression flag -- this is readonly and always indicate that the current
      stream is compressed
    .gz file suffix -- when present, we assume the file is compressed
    
    On Load, compression is SOLELY determined by the suffix of the file.
    On Save, compression is determined as follows:
      a .gz file name on the file
      the 'compress' checkbox is selected

*/

class TA_API taFiler : public taRefN {
  // ##NO_TOKENS associate this with each file that is managed
friend class taRefN; // for deleting
friend class taiStreamArgType; // we let it access the fstrm directly, for fstream arg types
public:
  enum FileOperation {
    foOpen,
    foSave,
    foSaveAs,
    foAppend
  };
  
  enum FilerFlags { // #BITS flags to control operations
    NO_FLAGS	= 0,	// #NO_BIT
    CONFIRM_OVERWRITE		= 0x001, // for Save/Append, warn if file already exists
    FILE_MUST_EXIST		= 0x002, // for Open, file must exist
    COMPRESS_ENABLE		= 0x004, // enables use of compression/compressed files
    COMPRESS_REQ		= 0x008, // if compr enabled, requests it by default
    COMPRESS_REQ_DEF		= 0x100, // use taiMisc default for compression
    
    DEF_FLAGS			= 0x003, // #NO_BIT default flags for no compression
    DEF_FLAGS_COMPRESS		= 0x107 // #NO_BIT default flags for compression w/ autocompress
  };

  static int	buf_size;	// size of the buffer for input operations
  static String	last_dir;	// last directory name processed
  static String	last_fname;	// last filename processed

  static taFiler* 	New(const String& filetype_ = "All",
    const String& ext = "", FilerFlags flags = DEF_FLAGS); // creates instance
  
  String	filter; // the QFileDialog-compatible filter expression
  String	filetype; // the filetype, ex Project
  String	ext; // the default extension (if any), including '.'
  String	dir;
  String	fname;
  bool		select_only;	// file is to be selected only (not opened)
  istream*	istrm;		// #READ_ONLY
  ostream*	ostrm;		// #READ_ONLY
  bool		open_file;	// #READ_ONLY true if there is an open file somewhere
  bool		file_selected;	// #READ_ONLY true if a file was selected last time..

  inline bool		compressEnabled() const {return (flags && COMPRESS_ENABLE);}
    // true if client enabled use of compression
  inline bool		compressReq() const 
    {return ((flags && (COMPRESS_REQ | COMPRESS_ENABLE)) == (COMPRESS_REQ | COMPRESS_ENABLE));}
    // true if client requested compression AND it is enabled)
  bool			isOpen() const {return open_file;}  // true if file is open
  bool			isCompressed() const; 
    // if open, indicates actual compr status, else indicates .gz suffix on fname
  const String		filterText(bool incl_allfiles = true) const; // Qt-compatible filter text
    // the low-level api functions work directly on the fname
  virtual istream*	open_read();
  virtual ostream*	open_write();
  virtual ostream* 	open_append();
  virtual bool		open_write_exist_check(); // returns true if file already exists for writing

  virtual istream*	Open();
  // to open an existing file for reading; starts with curr filename if any
  virtual ostream*	Save();
  // to save to an existing file; requests a new filename if doesn't exist
  virtual ostream*	SaveAs();
  // to save to a new file
  virtual ostream*	Append();
  // to open an existing file for appending
  virtual void		Close();	// close the stream

  virtual bool		GetFileName(String& fname, FileOperation filerOperation); // gui-dependent routine to get filename from user
  virtual void		FixFileName(); // make sure suffix is right
  virtual void		GetDir();      // get directory from file name


  virtual taFiler& operator=(const taFiler& cp); // copy operator used extensively in src/pdp files (copies very few props)
protected:
  FilerFlags 	flags;	
  fstream*	fstrm;		// #READ_ONLY the underlying file
  bool		compressed; // set when file has .gz suffix
  bool		file_exists; // set by GetFileName if the file actually exists
  taFiler(FilerFlags flags);
  virtual ~taFiler();
private:
  taFiler(const taFiler& cp); // #IGNORE copy construction not allowed
  void	Init(FilerFlags flags_); // same as constructor
};


#endif // ta_filer_h
